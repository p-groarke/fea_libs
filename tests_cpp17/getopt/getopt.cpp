#include <algorithm>
#include <array>
#include <chrono>
#include <fea/getopt/getopt.hpp>
#include <fea/utils/platform.hpp>
#include <fea/utils/string.hpp>
#include <gtest/gtest.h>
#include <random>

namespace {
std::string last_printed_string;
std::wstring last_printed_wstring;

constexpr bool do_console_print = false;
constexpr bool print_test_case = false;

int print_to_string(const std::string& message) {
	last_printed_string = message;
	if constexpr (do_console_print) {
		return printf("%s", message.c_str());
	} else {
		return 0;
	}
}
int print_to_wstring(const std::wstring& message) {
	last_printed_wstring = message;
	if constexpr (do_console_print) {
		return wprintf(L"%s", message.c_str());
	} else {
		return 0;
	}
}
int print_to_string16(const std::u16string& message) {
	last_printed_string = fea::utf16_to_utf8(message);
	if constexpr (do_console_print) {
		return printf("%s", last_printed_string.c_str());
	} else {
		return 0;
	}
}
int print_to_string32(const std::u32string& message) {
	last_printed_string = fea::utf32_to_utf8(message);
	if constexpr (do_console_print) {
		return printf("%s", last_printed_string.c_str());
	} else {
		return 0;
	}
}

// Testing framework. Generates an object containing the "called with" options
// to unit test.

template <class CharT>
struct option_tester {
	using string_t = std::basic_string<CharT, std::char_traits<CharT>,
			std::allocator<CharT>>;

	enum class opt_type {
		arg0,
		help,
		raw,
		flag,
		default_arg,
		optional,
		required,
		multi,
		concat,
		count,
	};

	struct test_case {
		opt_type type;
		bool expected = false;
		string_t option;
		std::vector<string_t> arguments;
		string_t expected_data;
		bool was_recieved = false;
		string_t recieved_data;
	};


	// Allways init arg0
	option_tester() {
		_data[size_t(opt_type::arg0)].push_back({
				opt_type::arg0,
				true,
				FEA_ML("tool.exe"),
				{},
				FEA_ML("tool.exe"),
				false,
				FEA_ML(""),
		});
	}

	void add_test(opt_type test_type, string_t&& option_and_expected) {
		assert(test_type != opt_type::count);
		_data[size_t(test_type)].push_back({
				test_type,
				true,
				option_and_expected,
				{},
				std::move(option_and_expected),
				false,
				FEA_ML(""),
		});
	}
	void add_test(
			opt_type test_type, string_t&& option, string_t&& expected_data) {
		assert(test_type != opt_type::count);
		_data[size_t(test_type)].push_back({
				test_type,
				true,
				std::move(option),
				{},
				std::move(expected_data),
				false,
				FEA_ML(""),
		});
	}
	void add_test(opt_type test_type, string_t&& option,
			std::initializer_list<string_t>&& arguments,
			string_t&& expected_data) {
		assert(test_type != opt_type::count);
		_data[size_t(test_type)].push_back({
				test_type,
				true,
				std::move(option),
				std::move(arguments),
				std::move(expected_data),
				false,
				FEA_ML(""),
		});
	}

	void merge(option_tester<CharT>&& other) {
		for (size_t i = 0; i < _data.size(); ++i) {
			opt_type o = opt_type(i);
			if (o == opt_type::arg0) {
				continue;
			}

			_data[i].insert(_data[i].end(),
					std::make_move_iterator(other._data[i].begin()),
					std::make_move_iterator(other._data[i].end()));
		}
	}

	void populate() {
		// Randomize options except for arg0.

		// Only supports 1 arg0.
		assert(_data[size_t(opt_type::arg0)].size() == 1);
		_test_cases.push_back(_data[size_t(opt_type::arg0)].back());

		// Add raw args.
		_test_cases.insert(_test_cases.end(),
				_data[size_t(opt_type::raw)].begin(),
				_data[size_t(opt_type::raw)].end());

		// Gather all the options to be randomized.
		// Except arg0 and raw args, which come first.
		std::vector<test_case> temp;
		for (size_t i = 0; i < size_t(opt_type::count); ++i) {
			opt_type o = opt_type(i);
			if (o == opt_type::arg0 || o == opt_type::raw) {
				continue;
			}

			temp.insert(temp.end(), _data[i].begin(), _data[i].end());
		}

		auto rng = std::mt19937_64{ size_t(
				std::chrono::system_clock::now().time_since_epoch().count()) };
		std::shuffle(temp.begin(), temp.end(), rng);

		// Finally, since help will abort all the other options, we make sure
		// there are no test_cases after a help option.
		auto it = std::find_if(temp.begin(), temp.end(),
				[](const test_case& t) { return t.type == opt_type::help; });

		if (it != temp.end()) {
			++it;
			for (; it != temp.end(); ++it) {
				it->expected = false;
				// it->passed_in_data = FEA_ML("");
			}
			// temp.erase(it + 1, temp.end());
		}

		// Finally, generate the test scenario.
		_test_cases.insert(_test_cases.end(), temp.begin(), temp.end());
	}

	std::vector<const CharT*> get_argv() const {
		// Randomize options except for arg0.
		std::vector<const CharT*> ret;

		for (const test_case& t : _test_cases) {
			ret.push_back(t.option.c_str());

			for (const string_t& arg : t.arguments) {
				ret.push_back(arg.c_str());
			}
		}

		if constexpr (print_test_case) {
			// Just convert everything to utf8
			std::string print_str;
			for (const CharT* cstr : ret) {
				print_str += fea::any_to_utf8(string_t{ cstr }) + " ";
			}

			if (!print_str.empty()) {
				// trailing white space
				print_str.pop_back();
			}

			printf("testing command : %s\n", print_str.c_str());
		}

		return ret;
	}

	void recieved(opt_type test_type, string_t&& recieved_str) {
		// Find first not set.
		auto it = std::find_if(_test_cases.begin(), _test_cases.end(),
				[&](const test_case& t) {
					if (test_type == opt_type::flag
							&& t.type == opt_type::concat) {
						return true;
					} else {
						return t.type == test_type && t.was_recieved == false;
					}
				});

		if (it == _test_cases.end()) {
			throw std::runtime_error{
				"test failed, recieved extra option that wasn't expected"
			};
			return;
		}

		it->was_recieved = true;

		if (it->type == opt_type::concat) {
			it->recieved_data += std::move(recieved_str) + FEA_ML(" ");
		} else {
			it->recieved_data = std::move(recieved_str);
		}
	}

	void testit() const {
		for (const test_case& t : _test_cases) {
			EXPECT_EQ(t.was_recieved, t.expected);
			if (!t.expected) {
				EXPECT_EQ(t.recieved_data, FEA_ML(""));
				continue;
			}

			string_t test_str = t.recieved_data;

			if (!test_str.empty() && fea::ends_with(test_str, FEA_ML(" "))) {
				test_str.pop_back();
			}
			EXPECT_EQ(test_str, t.expected_data);
		}
	}

private:
	std::array<std::vector<test_case>, size_t(opt_type::count)> _data;

	std::vector<test_case> _test_cases;
};


// These must be set by the test, so the callbacks can set the recieved data.
static option_tester<char> char_global_tester;
static option_tester<wchar_t> wchar_global_tester;
static option_tester<char16_t> char16_global_tester;
static option_tester<char32_t> char32_global_tester;

template <class CharT>
static constexpr auto& get_global_tester() {
	if constexpr (std::is_same_v<CharT, char>) {
		return char_global_tester;
	} else if constexpr (std::is_same_v<CharT, wchar_t>) {
		return wchar_global_tester;
	} else if constexpr (std::is_same_v<CharT, char16_t>) {
		return char16_global_tester;
	} else if constexpr (std::is_same_v<CharT, char32_t>) {
		return char32_global_tester;
	}
}


// Encapsulates many random ordered options. Basically a fuzzer.
template <class CharT>
struct test_scenario {
	using opt_test = option_tester<CharT>;

	void merge(test_scenario<CharT>&& other) {
		for (size_t i = 0; i < other.tests.size(); ++i) {
			if (i >= tests.size()) {
				tests.push_back(std::move(other.tests[i]));
			} else {
				tests[i].merge(std::move(other.tests[i]));
			}
		}
	}

	void fuzzit(fea::get_opt<CharT>& opt) {
		for (opt_test& test : tests) {
			test.populate();
		}

		for (const opt_test& test : tests) {
			auto& g_tester = get_global_tester<CharT>();
			g_tester = test;

			std::vector<const CharT*> opts = g_tester.get_argv();
			opt.parse_options(opts.size(), opts.data());

			g_tester.testit();
		}
	}

	std::vector<option_tester<CharT>> tests;
};


// Generate a test for all supported help options.
template <class CharT>
test_scenario<CharT> test_all_help() {
	test_scenario<CharT> ret;

	ret.tests.push_back({});
	ret.tests.back().add_test(
			option_tester<CharT>::opt_type::help, FEA_ML("-h"), FEA_ML(""));

	ret.tests.push_back({});
	ret.tests.back().add_test(
			option_tester<CharT>::opt_type::help, FEA_ML("--help"), FEA_ML(""));

	ret.tests.push_back({});
	ret.tests.back().add_test(
			option_tester<CharT>::opt_type::help, FEA_ML("/h"), FEA_ML(""));

	ret.tests.push_back({});
	ret.tests.back().add_test(
			option_tester<CharT>::opt_type::help, FEA_ML("/help"), FEA_ML(""));

	ret.tests.push_back({});
	ret.tests.back().add_test(
			option_tester<CharT>::opt_type::help, FEA_ML("/?"), FEA_ML(""));

	return ret;
}

// Test 2 raw args.
template <class CharT>
test_scenario<CharT> test_raw() {
	test_scenario<CharT> ret;

	ret.tests.push_back({});
	ret.tests.back().add_test(
			option_tester<CharT>::opt_type::raw, FEA_ML("raw arg 1"));

	ret.tests.push_back({});
	ret.tests.back().add_test(
			option_tester<CharT>::opt_type::raw, FEA_ML("raw arg 2"));

	ret.tests.push_back({});
	ret.tests.back().add_test(
			option_tester<CharT>::opt_type::raw, FEA_ML("raw arg 1"));
	ret.tests.back().add_test(
			option_tester<CharT>::opt_type::raw, FEA_ML("raw arg 2"));

	return ret;
}

template <class CharT>
test_scenario<CharT> test_flags_and_concat() {
	test_scenario<CharT> ret;

	ret.tests.push_back({});
	ret.tests.back().add_test(
			option_tester<CharT>::opt_type::flag, FEA_ML("-f"));

	ret.tests.push_back({});
	ret.tests.back().add_test(
			option_tester<CharT>::opt_type::flag, FEA_ML("--flag2"));

	ret.tests.push_back({});
	ret.tests.back().add_test(
			option_tester<CharT>::opt_type::flag, FEA_ML("--flag3"));

	ret.tests.push_back({});
	ret.tests.back().add_test(
			option_tester<CharT>::opt_type::flag, FEA_ML("--flag4"));

	ret.tests.push_back({});
	ret.tests.back().add_test(
			option_tester<CharT>::opt_type::flag, FEA_ML("--flag5"));

	ret.tests.push_back({});
	ret.tests.back().add_test(
			option_tester<CharT>::opt_type::flag, FEA_ML("-f"));
	ret.tests.back().add_test(
			option_tester<CharT>::opt_type::flag, FEA_ML("--flag2"));
	ret.tests.back().add_test(
			option_tester<CharT>::opt_type::flag, FEA_ML("--flag3"));
	ret.tests.back().add_test(
			option_tester<CharT>::opt_type::flag, FEA_ML("--flag4"));
	ret.tests.back().add_test(
			option_tester<CharT>::opt_type::flag, FEA_ML("--flag5"));
	ret.tests.back().add_test(
			option_tester<CharT>::opt_type::help, FEA_ML("--help"), FEA_ML(""));

	ret.tests.push_back(ret.tests.back());
	ret.tests.push_back(ret.tests.back());
	ret.tests.push_back(ret.tests.back());
	ret.tests.push_back(ret.tests.back());

	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::concat,
			FEA_ML("-faAbB"), FEA_ML("-f --flag2 --flag3 --flag4 --flag5"));

	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::concat,
			FEA_ML("-fab"), FEA_ML("-f --flag2 --flag4"));

	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::concat,
			FEA_ML("-fAB"), FEA_ML("-f --flag3 --flag5"));

	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::concat,
			FEA_ML("-Bb"), FEA_ML("--flag5 --flag4"));

	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::concat,
			FEA_ML("-abAB"), FEA_ML("--flag2 --flag4 --flag3 --flag5"));

	return ret;
}

template <class CharT>
test_scenario<CharT> test_default_arg() {
	test_scenario<CharT> ret;

	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::default_arg,
			FEA_ML("--default1"), FEA_ML("d_val1"));

	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::default_arg,
			FEA_ML("-d"), FEA_ML("d_val1"));

	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::default_arg,
			FEA_ML("--default2"), FEA_ML("d_val2"));

	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::default_arg,
			FEA_ML("--default1"), { FEA_ML("someval") }, FEA_ML("someval"));

	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::default_arg,
			FEA_ML("-d"), { FEA_ML("someval2") }, FEA_ML("someval2"));

	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::default_arg,
			FEA_ML("--default2"), { FEA_ML("someval3") }, FEA_ML("someval3"));

	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::default_arg,
			FEA_ML("--default1"), FEA_ML("d_val1"));
	ret.tests.back().add_test(option_tester<CharT>::opt_type::default_arg,
			FEA_ML("--default2"), FEA_ML("d_val2"));

	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::default_arg,
			FEA_ML("--default1"), { FEA_ML("someval1") }, FEA_ML("someval1"));
	ret.tests.back().add_test(option_tester<CharT>::opt_type::default_arg,
			FEA_ML("--default2"), FEA_ML("d_val2"));

	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::default_arg,
			FEA_ML("--default2"), FEA_ML("d_val2"));
	ret.tests.back().add_test(option_tester<CharT>::opt_type::default_arg,
			FEA_ML("--default1"), { FEA_ML("someval1") }, FEA_ML("someval1"));

	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::default_arg,
			FEA_ML("--default2"), { FEA_ML("someval2") }, FEA_ML("someval2"));
	ret.tests.back().add_test(option_tester<CharT>::opt_type::default_arg,
			FEA_ML("--default1"), { FEA_ML("someval1") }, FEA_ML("someval1"));

	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::default_arg,
			FEA_ML("--default1"), { FEA_ML("someval1") }, FEA_ML("someval1"));
	ret.tests.back().add_test(option_tester<CharT>::opt_type::default_arg,
			FEA_ML("--default2"), { FEA_ML("someval2") }, FEA_ML("someval2"));

	return ret;
}

template <class CharT>
test_scenario<CharT> test_optional_arg() {
	test_scenario<CharT> ret;

	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::optional,
			FEA_ML("--optional1"), { FEA_ML("opt") }, FEA_ML("opt"));

	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::optional,
			FEA_ML("--optional1"), FEA_ML(""));

	ret.tests.push_back({});
	ret.tests.back().add_test(
			option_tester<CharT>::opt_type::optional, FEA_ML("-o"), FEA_ML(""));

	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::optional,
			FEA_ML("-o"), { FEA_ML("optshort") }, FEA_ML("optshort"));

	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::optional,
			FEA_ML("-o"), { FEA_ML("opt1") }, FEA_ML("opt1"));
	ret.tests.back().add_test(option_tester<CharT>::opt_type::optional,
			FEA_ML("--optional2"), { FEA_ML("opt2") }, FEA_ML("opt2"));
	ret.tests.back().add_test(option_tester<CharT>::opt_type::optional,
			FEA_ML("--optional3"), FEA_ML(""));

	ret.tests.push_back(ret.tests.back());
	ret.tests.push_back(ret.tests.back());
	ret.tests.push_back(ret.tests.back());
	ret.tests.push_back(ret.tests.back());
	return ret;
}

template <class CharT>
test_scenario<CharT> test_required_arg() {
	test_scenario<CharT> ret;

	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::required,
			FEA_ML("--required1"), { FEA_ML("req") }, FEA_ML("req"));

	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::required,
			FEA_ML("-r"), { FEA_ML("reqshort") }, FEA_ML("reqshort"));

	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::required,
			FEA_ML("--required2"), { FEA_ML("req2") }, FEA_ML("req2"));

	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::required,
			FEA_ML("--required3"), { FEA_ML("req3") }, FEA_ML("req3"));

	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::required,
			FEA_ML("-r"), { FEA_ML("reqshort2") }, FEA_ML("reqshort2"));
	ret.tests.back().add_test(option_tester<CharT>::opt_type::required,
			FEA_ML("--required2"), { FEA_ML("req2-2") }, FEA_ML("req2-2"));
	ret.tests.back().add_test(option_tester<CharT>::opt_type::required,
			FEA_ML("--required3"), { FEA_ML("req3-3") }, FEA_ML("req3-3"));

	ret.tests.push_back(ret.tests.back());
	ret.tests.push_back(ret.tests.back());
	ret.tests.push_back(ret.tests.back());
	ret.tests.push_back(ret.tests.back());
	return ret;
}

template <class CharT>
test_scenario<CharT> test_multi_arg() {
	test_scenario<CharT> ret;

	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::multi,
			FEA_ML("--multi1"), { FEA_ML("a") }, FEA_ML("a"));

	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::multi,
			FEA_ML("--multi1"), { FEA_ML("a2 b2 c2 d2") },
			FEA_ML("a2 b2 c2 d2"));

	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::multi,
			FEA_ML("--multi1"),
			{ FEA_ML("a3"), FEA_ML("b3"), FEA_ML("c3"), FEA_ML("d3") },
			FEA_ML("a3 b3 c3 d3"));

	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::multi,
			FEA_ML("-m"), { FEA_ML("a4") }, FEA_ML("a4"));

	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::multi,
			FEA_ML("-m"), { FEA_ML("a5 b5 c5 d5") }, FEA_ML("a5 b5 c5 d5"));

	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::multi,
			FEA_ML("-m"),
			{ FEA_ML("a6"), FEA_ML("b6"), FEA_ML("c6"), FEA_ML("d6") },
			FEA_ML("a6 b6 c6 d6"));

	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::multi,
			FEA_ML("--multi2"), { FEA_ML("a7"), FEA_ML("b7"), FEA_ML("c7") },
			FEA_ML("a7 b7 c7"));

	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::multi,
			FEA_ML("--multi2"),
			{ FEA_ML("a8 b8 c8 d8 e8 f8 g8 h8 i8 j8 k8 l8 m8 n8 o8 p8 q8 r8 s8 "
					 "t8 u8 v8 w8 x8 y8 z8") },
			FEA_ML("a8 b8 c8 d8 e8 f8 g8 h8 i8 j8 k8 l8 m8 n8 o8 p8 q8 r8 s8 "
				   "t8 u8 v8 w8 x8 y8 z8"));

	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::multi,
			FEA_ML("--multi2"),
			{ FEA_ML("a8"), FEA_ML("b8"), FEA_ML("c8"), FEA_ML("d8"),
					FEA_ML("e8"), FEA_ML("f8"), FEA_ML("g8"), FEA_ML("h8"),
					FEA_ML("i8"), FEA_ML("j8"), FEA_ML("k8"), FEA_ML("l8"),
					FEA_ML("m8"), FEA_ML("n8"), FEA_ML("o8"), FEA_ML("p8"),
					FEA_ML("q8"), FEA_ML("r8"), FEA_ML("s8"), FEA_ML("t8"),
					FEA_ML("u8"), FEA_ML("v8"), FEA_ML("w8"), FEA_ML("x8"),
					FEA_ML("y8"), FEA_ML("z8") },
			FEA_ML("a8 b8 c8 d8 e8 f8 g8 h8 i8 j8 k8 l8 m8 n8 o8 p8 q8 r8 s8 "
				   "t8 u8 v8 w8 x8 y8 z8"));

	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::multi,
			FEA_ML("--multi1"), { FEA_ML("a") }, FEA_ML("a"));
	ret.tests.back().add_test(option_tester<CharT>::opt_type::multi,
			FEA_ML("--multi2"), { FEA_ML("a7"), FEA_ML("b7"), FEA_ML("c7") },
			FEA_ML("a7 b7 c7"));

	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::multi,
			FEA_ML("--multi1"), { FEA_ML("a2 b2 c2 d2") },
			FEA_ML("a2 b2 c2 d2"));
	ret.tests.back().add_test(option_tester<CharT>::opt_type::multi,
			FEA_ML("--multi2"),
			{ FEA_ML("a8 b8 c8 d8 e8 f8 g8 h8 i8 j8 k8 l8 m8 n8 o8 p8 q8 r8 s8 "
					 "t8 u8 v8 w8 x8 y8 z8") },
			FEA_ML("a8 b8 c8 d8 e8 f8 g8 h8 i8 j8 k8 l8 m8 n8 o8 p8 q8 r8 s8 "
				   "t8 u8 v8 w8 x8 y8 z8"));

	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::multi,
			FEA_ML("--multi1"),
			{ FEA_ML("a3"), FEA_ML("b3"), FEA_ML("c3"), FEA_ML("d3") },
			FEA_ML("a3 b3 c3 d3"));
	ret.tests.back().add_test(option_tester<CharT>::opt_type::multi,
			FEA_ML("--multi2"),
			{ FEA_ML("a8"), FEA_ML("b8"), FEA_ML("c8"), FEA_ML("d8"),
					FEA_ML("e8"), FEA_ML("f8"), FEA_ML("g8"), FEA_ML("h8"),
					FEA_ML("i8"), FEA_ML("j8"), FEA_ML("k8"), FEA_ML("l8"),
					FEA_ML("m8"), FEA_ML("n8"), FEA_ML("o8"), FEA_ML("p8"),
					FEA_ML("q8"), FEA_ML("r8"), FEA_ML("s8"), FEA_ML("t8"),
					FEA_ML("u8"), FEA_ML("v8"), FEA_ML("w8"), FEA_ML("x8"),
					FEA_ML("y8"), FEA_ML("z8") },
			FEA_ML("a8 b8 c8 d8 e8 f8 g8 h8 i8 j8 k8 l8 m8 n8 o8 p8 q8 r8 s8 "
				   "t8 u8 v8 w8 x8 y8 z8"));

	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::multi,
			FEA_ML("-m"), { FEA_ML("a4") }, FEA_ML("a4"));
	ret.tests.back().add_test(option_tester<CharT>::opt_type::multi,
			FEA_ML("--multi2"), { FEA_ML("a7"), FEA_ML("b7"), FEA_ML("c7") },
			FEA_ML("a7 b7 c7"));


	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::multi,
			FEA_ML("-m"), { FEA_ML("a5 b5 c5 d5") }, FEA_ML("a5 b5 c5 d5"));
	ret.tests.back().add_test(option_tester<CharT>::opt_type::multi,
			FEA_ML("--multi2"),
			{ FEA_ML("a8 b8 c8 d8 e8 f8 g8 h8 i8 j8 k8 l8 m8 n8 o8 p8 q8 r8 s8 "
					 "t8 u8 v8 w8 x8 y8 z8") },
			FEA_ML("a8 b8 c8 d8 e8 f8 g8 h8 i8 j8 k8 l8 m8 n8 o8 p8 q8 r8 s8 "
				   "t8 u8 v8 w8 x8 y8 z8"));


	ret.tests.push_back({});
	ret.tests.back().add_test(option_tester<CharT>::opt_type::multi,
			FEA_ML("-m"),
			{ FEA_ML("a6"), FEA_ML("b6"), FEA_ML("c6"), FEA_ML("d6") },
			FEA_ML("a6 b6 c6 d6"));
	ret.tests.back().add_test(option_tester<CharT>::opt_type::multi,
			FEA_ML("--multi2"),
			{ FEA_ML("a8"), FEA_ML("b8"), FEA_ML("c8"), FEA_ML("d8"),
					FEA_ML("e8"), FEA_ML("f8"), FEA_ML("g8"), FEA_ML("h8"),
					FEA_ML("i8"), FEA_ML("j8"), FEA_ML("k8"), FEA_ML("l8"),
					FEA_ML("m8"), FEA_ML("n8"), FEA_ML("o8"), FEA_ML("p8"),
					FEA_ML("q8"), FEA_ML("r8"), FEA_ML("s8"), FEA_ML("t8"),
					FEA_ML("u8"), FEA_ML("v8"), FEA_ML("w8"), FEA_ML("x8"),
					FEA_ML("y8"), FEA_ML("z8") },
			FEA_ML("a8 b8 c8 d8 e8 f8 g8 h8 i8 j8 k8 l8 m8 n8 o8 p8 q8 r8 s8 "
				   "t8 u8 v8 w8 x8 y8 z8"));

	return ret;
}

template <class CharT>
std::vector<test_scenario<CharT>> one_test_to_rule_them_all(
		size_t permutations = 50) {
	std::vector<test_scenario<CharT>> ret;

	for (size_t i = 0; i < permutations; ++i) {
		test_scenario<CharT> scenario;
		scenario.merge(test_raw<CharT>());
		scenario.merge(test_flags_and_concat<CharT>());
		scenario.merge(test_default_arg<CharT>());
		scenario.merge(test_optional_arg<CharT>());
		scenario.merge(test_required_arg<CharT>());
		scenario.merge(test_multi_arg<CharT>());
		ret.push_back(scenario);
	}

	return ret;
}

// Add all options compatible with the option_tester.
template <class CharT, class PrintFunc>
void add_options(fea::get_opt<CharT, PrintFunc>& opts) {
	using string_t = std::basic_string<CharT, std::char_traits<CharT>,
			std::allocator<CharT>>;

	opts.add_arg0_callback([](string_t&& str) {
		using opt_test = option_tester<CharT>;
		opt_test& t = get_global_tester<CharT>();
		t.recieved(opt_test::opt_type::arg0, std::move(str));
		return true;
	});

	opts.add_help_callback([]() {
		using opt_test = option_tester<CharT>;
		opt_test& t = get_global_tester<CharT>();
		t.recieved(opt_test::opt_type::help, FEA_ML(""));
	});

	opts.add_raw_option(
			FEA_ML("filename"),
			[](string_t&& str) {
				static_assert(
						std::is_same_v<typename string_t::value_type, CharT>,
						"unit test failed : Called with string must be same "
						"type as getopt.");

				using opt_test = option_tester<CharT>;
				opt_test& t = get_global_tester<CharT>();
				t.recieved(opt_test::opt_type::raw, std::move(str));

				return true;
			},
			FEA_ML("File to process.\nThis is a second indented string.\nAnd a "
				   "third."));

	opts.add_raw_option(
			FEA_ML("other_raw_opt"),
			[](string_t&& str) {
				static_assert(
						std::is_same_v<typename string_t::value_type, CharT>,
						"unit test failed : Called with string must be same "
						"type as getopt.");

				using opt_test = option_tester<CharT>;
				opt_test& t = get_global_tester<CharT>();
				t.recieved(opt_test::opt_type::raw, std::move(str));

				return true;
			},
			FEA_ML("Some looooooooong string that should be cut off by the "
				   "library and reindented appropriately. Hopefully without "
				   "splitting inside a word and making everything super nice "
				   "for users that can even add backslash n if they want to "
				   "start another sentence at the right indentantation like "
				   "this following sentence.\nI am a sentence that should "
				   "start at a newline, but still be split appropriately if I "
				   "am too long because that would be unfortunate wouldn't it "
				   "now."));

	opts.add_flag_option(
			FEA_ML("flag1"),
			[]() {
				using opt_test = option_tester<CharT>;
				opt_test& t = get_global_tester<CharT>();
				t.recieved(opt_test::opt_type::flag, { FEA_ML("-f") });
				return true;
			},
			FEA_ML("A simple flag."), FEA_CH('f'));

	opts.add_flag_option(
			FEA_ML("flag2"),
			[]() {
				using opt_test = option_tester<CharT>;
				opt_test& t = get_global_tester<CharT>();
				t.recieved(opt_test::opt_type::flag, { FEA_ML("--flag2") });
				return true;
			},
			FEA_ML("A simple flag."), FEA_CH('a'));

	opts.add_flag_option(
			FEA_ML("flag3"),
			[]() {
				using opt_test = option_tester<CharT>;
				opt_test& t = get_global_tester<CharT>();
				t.recieved(opt_test::opt_type::flag, { FEA_ML("--flag3") });
				return true;
			},
			FEA_ML("A simple flag."), FEA_CH('A'));

	opts.add_flag_option(
			FEA_ML("flag4"),
			[]() {
				using opt_test = option_tester<CharT>;
				opt_test& t = get_global_tester<CharT>();
				t.recieved(opt_test::opt_type::flag, { FEA_ML("--flag4") });
				return true;
			},
			FEA_ML("A simple flag."), FEA_CH('b'));

	opts.add_flag_option(
			FEA_ML("flag5"),
			[]() {
				using opt_test = option_tester<CharT>;
				opt_test& t = get_global_tester<CharT>();
				t.recieved(opt_test::opt_type::flag, { FEA_ML("--flag5") });
				return true;
			},
			FEA_ML("A simple flag."), FEA_CH('B'));

	opts.add_default_arg_option(
			FEA_ML("default1"),
			[](string_t&& str) {
				static_assert(
						std::is_same_v<typename string_t::value_type, CharT>,
						"unit test failed : Called with string must be same "
						"type as getopt.");

				using opt_test = option_tester<CharT>;
				opt_test& t = get_global_tester<CharT>();
				t.recieved(opt_test::opt_type::default_arg, std::move(str));
				return true;
			},
			FEA_ML("Some looooooooong string that should be cut off by the "
				   "library and reindented appropriately. Hopefully without "
				   "splitting inside a word and making everything super nice "
				   "for users that can even add backslash n if they want to "
				   "start another sentence at the right indentantation like "
				   "this following sentence.\nI am a sentence that should "
				   "start at a newline, but still be split appropriately if I "
				   "am too long because that would be unfortunate wouldn't it "
				   "now."),
			FEA_ML("d_val1"), FEA_CH('d'));

	opts.add_default_arg_option(
			FEA_ML("default2"),
			[](string_t&& str) {
				using opt_test = option_tester<CharT>;
				opt_test& t = get_global_tester<CharT>();
				t.recieved(opt_test::opt_type::default_arg, std::move(str));
				return true;
			},
			FEA_ML("A default option."), FEA_ML("d_val2"));
	opts.add_optional_arg_option(
			FEA_ML("optional1"),
			[](string_t&& str) {
				using opt_test = option_tester<CharT>;
				opt_test& t = get_global_tester<CharT>();
				t.recieved(opt_test::opt_type::optional, std::move(str));
				return true;
			},
			FEA_ML("An option with optional arg."), FEA_CH('o'));
	opts.add_optional_arg_option(
			FEA_ML("optional2"),
			[](string_t&& str) {
				using opt_test = option_tester<CharT>;
				opt_test& t = get_global_tester<CharT>();
				t.recieved(opt_test::opt_type::optional, std::move(str));
				return true;
			},
			FEA_ML("An option with optional arg."));
	opts.add_optional_arg_option(
			FEA_ML("optional3"),
			[](string_t&& str) {
				using opt_test = option_tester<CharT>;
				opt_test& t = get_global_tester<CharT>();
				t.recieved(opt_test::opt_type::optional, std::move(str));
				return true;
			},
			FEA_ML("An option with optional arg."));
	opts.add_required_arg_option(
			FEA_ML("required1"),
			[](string_t&& str) {
				using opt_test = option_tester<CharT>;
				opt_test& t = get_global_tester<CharT>();
				t.recieved(opt_test::opt_type::required, std::move(str));
				return true;
			},
			FEA_ML("An option with a required arg."), FEA_CH('r'));
	opts.add_required_arg_option(
			FEA_ML("required2"),
			[](string_t&& str) {
				using opt_test = option_tester<CharT>;
				opt_test& t = get_global_tester<CharT>();
				t.recieved(opt_test::opt_type::required, std::move(str));
				return true;
			},
			FEA_ML("An option with a required arg."));
	opts.add_required_arg_option(
			FEA_ML("required3"),
			[](string_t&& str) {
				using opt_test = option_tester<CharT>;
				opt_test& t = get_global_tester<CharT>();
				t.recieved(opt_test::opt_type::required, std::move(str));
				return true;
			},
			FEA_ML("An option with a required arg."));
	opts.add_multi_arg_option(
			FEA_ML("multi1"),
			[](std::vector<string_t>&& vec) {
				using opt_test = option_tester<CharT>;
				opt_test& t = get_global_tester<CharT>();

				string_t recieved;

				for (const string_t& str : vec) {
					recieved += str + FEA_ML(" ");
				}

				if (!recieved.empty()) {
					// Remove trailing space
					recieved.pop_back();
				}

				t.recieved(opt_test::opt_type::multi, std::move(recieved));
				return true;
			},
			FEA_ML("An option wich accepts multiple args."), FEA_CH('m'));
	opts.add_multi_arg_option(
			FEA_ML("multi2"),
			[](std::vector<string_t>&& vec) {
				using opt_test = option_tester<CharT>;
				opt_test& t = get_global_tester<CharT>();
				string_t recieved;

				for (const string_t& str : vec) {
					recieved += str + FEA_ML(" ");
				}

				if (!recieved.empty()) {
					// Remove trailing space
					recieved.pop_back();
				}

				t.recieved(opt_test::opt_type::multi, std::move(recieved));
				return true;
			},
			FEA_ML("An option wich accepts multiple args."));
}

TEST(fea_getopt, printing) {

#if defined(_MSC_VER)
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
#endif

	// std::string u8 = "\u00df\u6c34\U0001f34c";
	// std::wstring u16 = L"\u00df\u6c34\U0001f34c";
	// printf("%s\n", u8.c_str());
	// wprintf(L"%s\n", u16.c_str());
	{
		fea::get_opt<char> opt{ print_to_string };
		const char* test = FEA_MAKE_LITERAL_T(char, "test char\n");
		opt.print(test);
		EXPECT_EQ(last_printed_string, std::string{ test });
	}
	{
		fea::get_opt<wchar_t> opt{ print_to_wstring };
		const wchar_t* test = FEA_MAKE_LITERAL_T(wchar_t, "test wchar\n");
		opt.print(test);
		EXPECT_EQ(last_printed_wstring, std::wstring{ test });
	}
	{
		fea::get_opt<char16_t> opt{ print_to_string16 };
		const char16_t* test = FEA_MAKE_LITERAL_T(char16_t, "test char16\n");
		std::string utf8 = fea::utf16_to_utf8({ test });
		opt.print(test);
		EXPECT_EQ(last_printed_string, utf8);
	}
	{
		fea::get_opt<char32_t> opt{ print_to_string32 };
		const char32_t* test = FEA_MAKE_LITERAL_T(char32_t, "test char32\n");
		std::string utf8 = fea::utf32_to_utf8({ test });
		opt.print(test);
		EXPECT_EQ(last_printed_string, utf8);
	}
}

TEST(fea_getopt, basics) {

	{
		using mchar_t = char;
		fea::get_opt<mchar_t> opt{ print_to_string };
		// fea::get_opt<char> opt{};
		add_options(opt);

		{
			test_scenario<mchar_t> scenario = test_all_help<mchar_t>();
			scenario.fuzzit(opt);
		}
		{
			test_scenario<mchar_t> scenario = test_raw<mchar_t>();
			scenario.fuzzit(opt);
		}
		{
			test_scenario<mchar_t> scenario = test_flags_and_concat<mchar_t>();
			scenario.fuzzit(opt);
		}
		{
			test_scenario<mchar_t> scenario = test_default_arg<mchar_t>();
			scenario.fuzzit(opt);
		}
		{
			test_scenario<mchar_t> scenario = test_optional_arg<mchar_t>();
			scenario.fuzzit(opt);
		}
		{
			test_scenario<mchar_t> scenario = test_required_arg<mchar_t>();
			scenario.fuzzit(opt);
		}
		{
			test_scenario<mchar_t> scenario = test_multi_arg<mchar_t>();
			scenario.fuzzit(opt);
		}
		{
			std::vector<test_scenario<mchar_t>> scenarios
					= one_test_to_rule_them_all<mchar_t>();
			for (test_scenario<mchar_t>& s : scenarios) {
				s.fuzzit(opt);
			}
		}
	}

	{
		using mchar_t = wchar_t;
		fea::get_opt<mchar_t> opt{ print_to_wstring };
		// fea::get_opt<wchar_t> opt{};
		add_options(opt);

		{
			test_scenario<mchar_t> scenario = test_all_help<mchar_t>();
			scenario.fuzzit(opt);
		}
		{
			test_scenario<mchar_t> scenario = test_raw<mchar_t>();
			scenario.fuzzit(opt);
		}
		{
			test_scenario<mchar_t> scenario = test_flags_and_concat<mchar_t>();
			scenario.fuzzit(opt);
		}
		{
			test_scenario<mchar_t> scenario = test_default_arg<mchar_t>();
			scenario.fuzzit(opt);
		}
		{
			test_scenario<mchar_t> scenario = test_optional_arg<mchar_t>();
			scenario.fuzzit(opt);
		}
		{
			test_scenario<mchar_t> scenario = test_required_arg<mchar_t>();
			scenario.fuzzit(opt);
		}
		{
			test_scenario<mchar_t> scenario = test_multi_arg<mchar_t>();
			scenario.fuzzit(opt);
		}
		{
			std::vector<test_scenario<mchar_t>> scenarios
					= one_test_to_rule_them_all<mchar_t>();
			for (test_scenario<mchar_t>& s : scenarios) {
				s.fuzzit(opt);
			}
		}
	}

	{
		using mchar_t = char16_t;
		fea::get_opt<mchar_t> opt{ print_to_string16 };
		// fea::get_opt<char16_t> opt{};
		add_options(opt);

		{
			test_scenario<mchar_t> scenario = test_all_help<mchar_t>();
			scenario.fuzzit(opt);
		}
		{
			test_scenario<mchar_t> scenario = test_raw<mchar_t>();
			scenario.fuzzit(opt);
		}
		{
			test_scenario<mchar_t> scenario = test_flags_and_concat<mchar_t>();
			scenario.fuzzit(opt);
		}
		{
			test_scenario<mchar_t> scenario = test_default_arg<mchar_t>();
			scenario.fuzzit(opt);
		}
		{
			test_scenario<mchar_t> scenario = test_optional_arg<mchar_t>();
			scenario.fuzzit(opt);
		}
		{
			test_scenario<mchar_t> scenario = test_required_arg<mchar_t>();
			scenario.fuzzit(opt);
		}
		{
			test_scenario<mchar_t> scenario = test_multi_arg<mchar_t>();
			scenario.fuzzit(opt);
		}
		{
			std::vector<test_scenario<mchar_t>> scenarios
					= one_test_to_rule_them_all<mchar_t>();
			for (test_scenario<mchar_t>& s : scenarios) {
				s.fuzzit(opt);
			}
		}
	}

	{
		using mchar_t = char32_t;
		fea::get_opt<mchar_t> opt{ print_to_string32 };
		// fea::get_opt<char32_t> opt{};
		add_options(opt);

		{
			test_scenario<mchar_t> scenario = test_all_help<mchar_t>();
			scenario.fuzzit(opt);
		}
		{
			test_scenario<mchar_t> scenario = test_raw<mchar_t>();
			scenario.fuzzit(opt);
		}
		{
			test_scenario<mchar_t> scenario = test_flags_and_concat<mchar_t>();
			scenario.fuzzit(opt);
		}
		{
			test_scenario<mchar_t> scenario = test_default_arg<mchar_t>();
			scenario.fuzzit(opt);
		}
		{
			test_scenario<mchar_t> scenario = test_optional_arg<mchar_t>();
			scenario.fuzzit(opt);
		}
		{
			test_scenario<mchar_t> scenario = test_required_arg<mchar_t>();
			scenario.fuzzit(opt);
		}
		{
			test_scenario<mchar_t> scenario = test_multi_arg<mchar_t>();
			scenario.fuzzit(opt);
		}
		{
			std::vector<test_scenario<mchar_t>> scenarios
					= one_test_to_rule_them_all<mchar_t>();
			for (test_scenario<mchar_t>& s : scenarios) {
				s.fuzzit(opt);
			}
		}
	}
}
} // namespace

#include <array>
#include <fea/macros/literals.hpp>
#include <fea/meta/static_for.hpp>
#include <fea/meta/tuple.hpp>
#include <fea/string/split.hpp>
#include <gtest/gtest.h>
#include <tuple>

namespace {
#define gen_constants(tupname, str) \
	static const auto tupname = std::make_tuple(std::string_view{ str }, \
			std::wstring_view{ L##str }, std::u16string_view{ u##str }, \
			std::u32string_view{ U##str }, std::string{ str }, \
			std::wstring{ L##str }, std::u16string{ u##str }, \
			std::u32string{ U##str }, str, L##str, u##str, U##str)


#define gen_tests(tupname, ...) \
	static const auto tupname = std::make_tuple( \
			std::array{ FEA_VA_SV(__VA_ARGS__) }, \
			std::array{ FEA_VA_WSV(__VA_ARGS__) }, \
			std::array{ FEA_VA_U16SV(__VA_ARGS__) }, \
			std::array{ FEA_VA_U32SV(__VA_ARGS__) }, \
			std::array{ FEA_VA_S(__VA_ARGS__) }, \
			std::array{ FEA_VA_WS(__VA_ARGS__) }, \
			std::array{ FEA_VA_U16S(__VA_ARGS__) }, \
			std::array{ FEA_VA_U32S(__VA_ARGS__) }, std::array{ __VA_ARGS__ }, \
			std::array{ FEA_VA_WPREFIX(__VA_ARGS__) }, \
			std::array{ FEA_VA_U16PREFIX(__VA_ARGS__) }, \
			std::array{ FEA_VA_U32PREFIX(__VA_ARGS__) })

gen_constants(alice, R"xx(CHAPTER I.
Down the Rabbit-Hole

Alice was beginning to get very tired of sitting
by her sister on the bank,
and of having nothing to do:
once or twice she had peeped into the book her sister
was reading,
but it had no pictures or conversations in it,
“and what is the use of a book,”
thought Alice “without pictures or conversations?”
)xx");

template <class T1, class T2, class T3, class T4>
void test_all_splits(const T1& src, const T2& rem_results,
		const T3& app_results, const T4& pre_results) {
	static constexpr size_t s = std::tuple_size_v<std::decay_t<decltype(src)>>;
	fea::static_for<s>([&](auto const_i) {
		static constexpr size_t i = const_i;
		const auto& str = std::get<i>(src);

		// No delims.
		{
			const auto& answer_arr = std::get<i>(rem_results);
			using CharT = std::decay_t<decltype(str[0])>;

			auto split_vec = fea::split(str, FEA_LIT("."));
			auto split_str_vec = fea::split_to_str(str, FEA_LIT("."));

			if (answer_arr.size() == 1 && answer_arr[0] == FEA_STR("")) {
				// Empty case.
				EXPECT_EQ(split_vec.size(), 0);
				EXPECT_EQ(split_str_vec.size(), 0);
			} else {
				EXPECT_EQ(split_vec.size(), answer_arr.size());
				for (size_t j = 0; j < split_vec.size(); ++j) {
					const auto& answer_str = answer_arr[j];
					const auto& test_str = split_vec[j];
					EXPECT_EQ(answer_str, test_str);

					const auto& test_str2 = split_str_vec[j];
					EXPECT_EQ(answer_str, test_str2);
				}
			}

			auto split_vec2 = fea::split(str, FEA_STRV("."));
			EXPECT_EQ(split_vec, split_vec2);
			auto split_vec3 = fea::split(str, FEA_STR("."));
			EXPECT_EQ(split_vec, split_vec3);
			auto split_vec4 = fea::split(str, FEA_CH('.'));
			EXPECT_EQ(split_vec, split_vec4);
		}

		// Append delims.
		{
			const auto& answer_arr = std::get<i>(app_results);
			using CharT = std::decay_t<decltype(str[0])>;

			auto split_vec = fea::split<fea::split_delim_opt::append>(
					str, FEA_LIT("."));
			auto split_str_vec = fea::split<fea::split_delim_opt::append>(
					str, FEA_LIT("."));

			if (answer_arr.size() == 1 && answer_arr[0] == FEA_STR("")) {
				// Empty case.
				EXPECT_EQ(split_vec.size(), 0);
				EXPECT_EQ(split_str_vec.size(), 0);
			} else {
				EXPECT_EQ(split_vec.size(), answer_arr.size());
				for (size_t j = 0; j < split_vec.size(); ++j) {
					const auto& answer_str = answer_arr[j];
					const auto& test_str = split_vec[j];
					EXPECT_EQ(answer_str, test_str);

					const auto& test_str2 = split_str_vec[j];
					EXPECT_EQ(answer_str, test_str2);
				}
			}

			auto split_vec2 = fea::split<fea::split_delim_opt::append>(
					str, FEA_STRV("."));
			EXPECT_EQ(split_vec, split_vec2);
			auto split_vec3 = fea::split<fea::split_delim_opt::append>(
					str, FEA_STR("."));
			EXPECT_EQ(split_vec, split_vec3);
			auto split_vec4 = fea::split<fea::split_delim_opt::append>(
					str, FEA_CH('.'));
			EXPECT_EQ(split_vec, split_vec4);
		}

		// Prepend delims.
		{
			const auto& answer_arr = std::get<i>(pre_results);
			using CharT = std::decay_t<decltype(str[0])>;

			auto split_vec = fea::split<fea::split_delim_opt::prepend>(
					str, FEA_LIT("."));
			auto split_str_vec = fea::split<fea::split_delim_opt::prepend>(
					str, FEA_LIT("."));

			if (answer_arr.size() == 1 && answer_arr[0] == FEA_STR("")) {
				// Empty case.
				EXPECT_EQ(split_vec.size(), 0);
				EXPECT_EQ(split_str_vec.size(), 0);
			} else {
				EXPECT_EQ(split_vec.size(), answer_arr.size());
				for (size_t j = 0; j < split_vec.size(); ++j) {
					const auto& answer_str = answer_arr[j];
					const auto& test_str = split_vec[j];
					EXPECT_EQ(answer_str, test_str);

					const auto& test_str2 = split_str_vec[j];
					EXPECT_EQ(answer_str, test_str2);
				}
			}

			auto split_vec2 = fea::split<fea::split_delim_opt::prepend>(
					str, FEA_STRV("."));
			EXPECT_EQ(split_vec, split_vec2);
			auto split_vec3 = fea::split<fea::split_delim_opt::prepend>(
					str, FEA_STR("."));
			EXPECT_EQ(split_vec, split_vec3);
			auto split_vec4 = fea::split<fea::split_delim_opt::prepend>(
					str, FEA_CH('.'));
			EXPECT_EQ(split_vec, split_vec4);
		}
	});
}

TEST(string_split, basics) {
	{
		gen_tests(line_test, "CHAPTER I.", "Down the Rabbit-Hole",
				"Alice was beginning to get very tired of sitting",
				"by her sister on the bank,", "and of having nothing to do:",
				"once or twice she had peeped into the book her sister",
				"was reading,",
				"but it had no pictures or conversations in it,",
				"“and what is the use of a book,”",
				"thought Alice “without pictures or conversations?”");

		static constexpr size_t s = std::tuple_size_v<decltype(alice)>;
		fea::static_for<s>([](auto const_i) {
			static constexpr size_t i = const_i;
			const auto& str = std::get<i>(alice);
			const auto& answer_arr = std::get<i>(line_test);

			using CharT = std::decay_t<decltype(str[0])>;

			auto split_vec = fea::split(str, FEA_LIT("\n"));
			EXPECT_EQ(split_vec.size(), answer_arr.size());
			for (size_t j = 0; j < split_vec.size(); ++j) {
				const auto& answer_str = answer_arr[j];
				const auto& test_str = split_vec[j];
				EXPECT_EQ(answer_str, test_str);
			}
		});
	}

	{
		gen_constants(split_src, ".Please.split.this.string.");

		gen_tests(rem, "Please", "split", "this", "string");
		gen_tests(app, ".", "Please.", "split.", "this.", "string.");
		gen_tests(pre, ".Please", ".split", ".this", ".string", ".");

		test_all_splits(split_src, rem, app, pre);
	}
	{
		gen_constants(split_src, "..Please.split.this.string..");

		gen_tests(rem, "Please", "split", "this", "string");
		gen_tests(app, ".", ".", "Please.", "split.", "this.", "string.", ".");
		gen_tests(pre, ".", ".Please", ".split", ".this", ".string", ".", ".");

		test_all_splits(split_src, rem, app, pre);
	}
	{
		gen_constants(split_src, "...Please.split.this.string...");

		gen_tests(rem, "Please", "split", "this", "string");
		gen_tests(app, ".", ".", ".", "Please.", "split.", "this.", "string.",
				".", ".");
		gen_tests(pre, ".", ".", ".Please", ".split", ".this", ".string", ".",
				".", ".");

		test_all_splits(split_src, rem, app, pre);
	}
	{
		gen_constants(split_src, "word");

		gen_tests(rem, "word");
		gen_tests(app, "word");
		gen_tests(pre, "word");

		test_all_splits(split_src, rem, app, pre);
	}
	{
		gen_constants(split_src, "...");

		gen_tests(rem, "");
		gen_tests(app, ".", ".", ".");
		gen_tests(pre, ".", ".", ".");

		test_all_splits(split_src, rem, app, pre);
	}
	{
		gen_constants(split_src, "");

		gen_tests(rem, "");
		gen_tests(app, "");
		gen_tests(pre, "");

		test_all_splits(split_src, rem, app, pre);
	}
	{
		gen_constants(split_src, ".");

		gen_tests(rem, "");
		gen_tests(app, ".");
		gen_tests(pre, ".");

		test_all_splits(split_src, rem, app, pre);
	}
}
} // namespace

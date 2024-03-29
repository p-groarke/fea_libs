﻿#include <array>
#include <fea/meta/static_for.hpp>
#include <fea/meta/traits.hpp>
#include <fea/meta/tuple.hpp>
#include <fea/string/string.hpp>
#include <gtest/gtest.h>
#include <tuple>

#define gen_constants(tupname, str) \
	const auto tupname = std::make_tuple(std::string_view{ str }, \
			std::wstring_view{ L##str }, std::u16string_view{ u##str }, \
			std::u32string_view{ U##str }, std::string{ str }, \
			std::wstring{ L##str }, std::u16string{ u##str }, \
			std::u32string{ U##str }, str, L##str, u##str, U##str)


#define gen_tests(tupname, ...) \
	const auto tupname = std::make_tuple(std::array{ FEA_VA_SV(__VA_ARGS__) }, \
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


namespace {
TEST(string, basics) {
	{
		std::string str = "a string weeee, bang, ding, ow";
		EXPECT_TRUE(fea::contains(str, "ding"));
		EXPECT_FALSE(fea::contains(str, "dong"));

		std::string caps = "NOT SCREAMING";
		EXPECT_EQ(fea::to_lower_ascii(caps), "not screaming");
		EXPECT_NE(fea::to_lower_ascii(caps), "NOT SCREAMING");

		std::string capscpy = caps;
		fea::to_lower_ascii_inplace(capscpy);
		EXPECT_EQ(capscpy, "not screaming");
		EXPECT_NE(capscpy, "NOT SCREAMING");

		auto vec = fea::split(str, ",");
		std::vector<std::string_view> vec_answer{ "a string weeee", " bang",
			" ding", " ow" };
		EXPECT_EQ(vec.size(), 4u);
		EXPECT_EQ(vec, vec_answer);

		vec = fea::split(str, ", ");
		vec_answer = { "a", "string", "weeee", "bang", "ding", "ow" };
		EXPECT_EQ(vec.size(), 6u);
		EXPECT_EQ(vec, vec_answer);

		EXPECT_EQ(fea::replace_all(caps, "NOT", "is"), "is SCREAMING");

		capscpy = caps;
		fea::replace_all_inplace(capscpy, "NOT", "is");
		EXPECT_EQ(capscpy, "is SCREAMING");

		fea::replace_all_inplace(capscpy, "is", "is not");
		EXPECT_EQ(capscpy, "is not SCREAMING");

		fea::replace_all_inplace(capscpy, "is", "is");
		EXPECT_EQ(capscpy, "is not SCREAMING");
	}
	{
		const std::string test_str = "test";
		const std::wstring wtest_str = L"test";

		EXPECT_FALSE(fea::starts_with(test_str, "test1"));
		EXPECT_FALSE(fea::starts_with(test_str, "1test"));
		EXPECT_TRUE(fea::starts_with(test_str, "\0"));
		EXPECT_TRUE(fea::starts_with(test_str, "test"));
		EXPECT_TRUE(fea::starts_with(test_str, "tes"));
		EXPECT_TRUE(fea::starts_with(test_str, "t"));
		EXPECT_TRUE(fea::starts_with(test_str, 't'));
		EXPECT_FALSE(fea::starts_with(test_str, 'a'));
		EXPECT_FALSE(fea::starts_with(test_str, std::string{ "test1" }));
		EXPECT_FALSE(fea::starts_with(test_str, std::string{ "1test" }));
		EXPECT_TRUE(fea::starts_with(test_str, std::string{ "\0" }));
		EXPECT_TRUE(fea::starts_with(test_str, std::string{ "test" }));
		EXPECT_TRUE(fea::starts_with(test_str, std::string{ "tes" }));
		EXPECT_TRUE(fea::starts_with(test_str, std::string{ "t" }));

		EXPECT_FALSE(fea::starts_with(wtest_str, L"test1"));
		EXPECT_FALSE(fea::starts_with(wtest_str, L"1test"));
		EXPECT_TRUE(fea::starts_with(wtest_str, L"\0"));
		EXPECT_TRUE(fea::starts_with(wtest_str, L"test"));
		EXPECT_TRUE(fea::starts_with(wtest_str, L"tes"));
		EXPECT_TRUE(fea::starts_with(wtest_str, L"t"));
		EXPECT_TRUE(fea::starts_with(wtest_str, L't'));
		EXPECT_FALSE(fea::starts_with(wtest_str, L'a'));
		EXPECT_FALSE(fea::starts_with(wtest_str, std::wstring{ L"test1" }));
		EXPECT_FALSE(fea::starts_with(wtest_str, std::wstring{ L"1test" }));
		EXPECT_TRUE(fea::starts_with(wtest_str, std::wstring{ L"\0" }));
		EXPECT_TRUE(fea::starts_with(wtest_str, std::wstring{ L"test" }));
		EXPECT_TRUE(fea::starts_with(wtest_str, std::wstring{ L"tes" }));
		EXPECT_TRUE(fea::starts_with(wtest_str, std::wstring{ L"t" }));

		EXPECT_FALSE(fea::ends_with(test_str, "test1"));
		EXPECT_FALSE(fea::ends_with(test_str, "1test"));
		EXPECT_TRUE(fea::ends_with(test_str, "\0"));
		EXPECT_TRUE(fea::ends_with(test_str, "test"));
		EXPECT_TRUE(fea::ends_with(test_str, "est"));
		EXPECT_TRUE(fea::ends_with(test_str, "t"));
		EXPECT_TRUE(fea::ends_with(test_str, 't'));
		EXPECT_FALSE(fea::ends_with(test_str, 'e'));
		EXPECT_FALSE(fea::ends_with(test_str, std::string{ "test1" }));
		EXPECT_FALSE(fea::ends_with(test_str, std::string{ "1test" }));
		EXPECT_TRUE(fea::ends_with(test_str, std::string{ "\0" }));
		EXPECT_TRUE(fea::ends_with(test_str, std::string{ "test" }));
		EXPECT_TRUE(fea::ends_with(test_str, std::string{ "est" }));
		EXPECT_TRUE(fea::ends_with(test_str, std::string{ "t" }));

		EXPECT_FALSE(fea::ends_with(wtest_str, L"test1"));
		EXPECT_FALSE(fea::ends_with(wtest_str, L"1test"));
		EXPECT_TRUE(fea::ends_with(wtest_str, L"\0"));
		EXPECT_TRUE(fea::ends_with(wtest_str, L"test"));
		EXPECT_TRUE(fea::ends_with(wtest_str, L"est"));
		EXPECT_TRUE(fea::ends_with(wtest_str, L"t"));
		EXPECT_TRUE(fea::ends_with(wtest_str, L't'));
		EXPECT_FALSE(fea::ends_with(wtest_str, L'e'));
		EXPECT_FALSE(fea::ends_with(wtest_str, std::wstring{ L"test1" }));
		EXPECT_FALSE(fea::ends_with(wtest_str, std::wstring{ L"1test" }));
		EXPECT_TRUE(fea::ends_with(wtest_str, std::wstring{ L"\0" }));
		EXPECT_TRUE(fea::ends_with(wtest_str, std::wstring{ L"test" }));
		EXPECT_TRUE(fea::ends_with(wtest_str, std::wstring{ L"est" }));
		EXPECT_TRUE(fea::ends_with(wtest_str, std::wstring{ L"t" }));
	}
}

TEST(string, size) {
	{
		gen_constants(tests, "");
		constexpr size_t empty_strs_size = std::tuple_size_v<decltype(tests)>;

		fea::static_for<empty_strs_size>([&](auto const_i) {
			constexpr size_t i = const_i;

			const auto& test_str = std::get<i>(tests);
			EXPECT_EQ(0u, fea::size(test_str));
		});
	}
	{
		gen_constants(tests, "\n");
		constexpr size_t empty_strs_size = std::tuple_size_v<decltype(tests)>;

		fea::static_for<empty_strs_size>([&](auto const_i) {
			constexpr size_t i = const_i;

			const auto& test_str = std::get<i>(tests);
			EXPECT_EQ(1u, fea::size(test_str));
		});
	}
	{
		gen_constants(tests, "a-t\nest");
		constexpr size_t empty_strs_size = std::tuple_size_v<decltype(tests)>;

		fea::static_for<empty_strs_size>([&](auto const_i) {
			constexpr size_t i = const_i;

			const auto& test_str = std::get<i>(tests);
			EXPECT_EQ(7u, fea::size(test_str));
		});
	}
}

gen_constants(alice, R"xx(CHAPTER I.
Down the Rabbit-Hole

Alice was beginning to get very tired of sitting by her sister on the bank, and
of having nothing to do: once or twice she had peeped into the book her sister
was reading, but it had no pictures or conversations in it, “and what is the
use of a book,” thought Alice “without pictures or conversations?”

So she was considering in her own mind (as well as she could, for the hot day
made her feel very sleepy and stupid), whether the pleasure of making a
daisy-chain would be worth the trouble of getting up and picking the daisies,
when suddenly a White Rabbit with pink eyes ran close by her.

There was nothing so very remarkable in that; nor did Alice think it so very
much out of the way to hear the Rabbit say to itself, “Oh dear! Oh dear! I
shall be late!” (when she thought it over afterwards, it occurred to her that
she ought to have wondered at this, but at the time it all seemed quite
natural); but when the Rabbit actually took a watch out of its
waistcoat-pocket, and looked at it, and then hurried on, Alice started to her
feet, for it flashed across her mind that she had never before seen a rabbit
with either a waistcoat-pocket, or a watch to take out of it, and burning with
curiosity, she ran across the field after it, and fortunately was just in time
to see it pop down a large rabbit-hole under the hedge.
)xx");

constexpr size_t alice_size = std::tuple_size_v<decltype(alice)>;

TEST(string, contains) {
	gen_tests(valid_search, "I.", "”", "Alice", "Rabbit-Hole",
			"under the hedge.", "\n");

	gen_tests(invalid_search, ">test", ">", "1potato", "/I'm a sentence");


	fea::static_for<alice_size>([&](auto const_i) {
		constexpr size_t i = const_i;

		const auto& test_str = std::get<i>(alice);
		using str_t = std::decay_t<decltype(test_str)>;

		const auto& valid_arr = std::get<i>(valid_search);
		for (const auto& search : valid_arr) {
			EXPECT_TRUE(fea::contains(test_str, search));
			EXPECT_TRUE(fea::contains(test_str, search[0]));
			if constexpr (!std::is_pointer_v<str_t>) {
				EXPECT_TRUE(fea::contains(test_str, search.data()));
			}
		}

		const auto& invalid_arr = std::get<i>(invalid_search);
		for (const auto& search : invalid_arr) {
			EXPECT_FALSE(fea::contains(test_str, search));
			EXPECT_FALSE(fea::contains(test_str, search[0]));
			if constexpr (!std::is_pointer_v<str_t>) {
				EXPECT_FALSE(fea::contains(test_str, search.data()));
			}
		}
	});
}

gen_tests(alice_chunks, R"xx(CHAPTER I.)xx", R"xx(Down the Rabbit-Hole)xx",
		R"xx(Alice was beginning)xx", R"xx(to get very tired of)xx",
		R"xx(sitting by her sister on the bank,)xx",
		R"xx(and of having nothing to do)xx",
		R"xx(: once or twice she had peeped into)xx",
		R"xx(the book her sister)xx", R"xx(was reading, )xx",
		R"xx(but it had no pictures or conversations in it, )xx",
		R"xx(“and what is the)xx", R"xx(use of a book,” )xx",
		R"xx(thought Alice )xx", R"xx(“without pictures or conversations?”)xx");

TEST(string, starts_with) {
	gen_tests(valid_searches, "CHAP", "Down the Rabbit-", "Alice was", "to",
			"sitting by", "and of having nothing to do", ":", "t", "was ",
			"but", "“", "use", "thought Alice ", "“without");

	gen_tests(invalid_searches, "x", " D", "Alice  was", "\n", "Sitting by",
			"test", "!", "a", "wAs ", "butxxxx", "”", "usE", "Thought\n Alice ",
			"without");

	static_assert(std::get<0>(alice_chunks).size()
					== std::get<0>(valid_searches).size(),
			"Sanity check failed, you have gone off the deep end.");
	static_assert(std::get<0>(alice_chunks).size()
					== std::get<0>(invalid_searches).size(),
			"Sanity check failed, you have gone off the deep end.");

	fea::static_for<alice_size>([&](auto const_i) {
		constexpr size_t i = const_i;

		const auto& alice_arr = std::get<i>(alice_chunks);
		const auto& valid_arr = std::get<i>(valid_searches);
		const auto& invalid_arr = std::get<i>(invalid_searches);

		for (size_t j = 0; j < alice_arr.size(); ++j) {
			using str_t = std::decay_t<decltype(alice_arr[i])>;
			EXPECT_TRUE(fea::starts_with(alice_arr[j], valid_arr[j]));
			EXPECT_TRUE(fea::starts_with(alice_arr[j], valid_arr[j][0]));
			if constexpr (!std::is_pointer_v<str_t>) {
				EXPECT_TRUE(
						fea::starts_with(alice_arr[j], valid_arr[j].data()));
			}
		}

		for (size_t j = 0; j < alice_arr.size(); ++j) {
			using str_t = std::decay_t<decltype(alice_arr[i])>;
			EXPECT_FALSE(fea::starts_with(alice_arr[j], invalid_arr[j]));

			if (fea::size(invalid_arr[j]) <= 2) {
				EXPECT_FALSE(fea::starts_with(alice_arr[j], invalid_arr[j][0]));
			}

			if constexpr (!std::is_pointer_v<str_t>) {
				// Only test chars for small cases, because we want other
				// scenarios.
				EXPECT_FALSE(
						fea::starts_with(alice_arr[j], invalid_arr[j].data()));
			}
		}
	});
}

TEST(string, ends_with) {
	gen_tests(valid_searches, " I.", "Rabbit-Hole", "ng", " of", ",",
			"and of having nothing to do", "peeped into", "sister", ", ",
			"it, ", "is the", ",” ", "thought Alice ", " or conversations?”");

	gen_tests(invalid_searches, "I,", "LE", "Alice was", "\n", "!", "test",
			"tO", "a", "  ,", "ittttt", "”", ",“", "Thought\n Alice ", "!!");

	static_assert(std::get<0>(alice_chunks).size()
					== std::get<0>(valid_searches).size(),
			"Sanity check failed, you have gone off the deep end.");
	static_assert(std::get<0>(alice_chunks).size()
					== std::get<0>(invalid_searches).size(),
			"Sanity check failed, you have gone off the deep end.");

	fea::static_for<alice_size>([&](auto const_i) {
		constexpr size_t i = const_i;

		const auto& alice_arr = std::get<i>(alice_chunks);
		const auto& valid_arr = std::get<i>(valid_searches);
		const auto& invalid_arr = std::get<i>(invalid_searches);

		for (size_t j = 0; j < alice_arr.size(); ++j) {
			using str_t = std::decay_t<decltype(alice_arr[i])>;
			// using char_t = std::decay_t<std::remove_pointer_t<str_t>>;
			EXPECT_TRUE(fea::ends_with(alice_arr[j], valid_arr[j]));

			size_t size = fea::size(valid_arr[j]);
			EXPECT_TRUE(fea::ends_with(alice_arr[j], valid_arr[j][size - 1]));

			if constexpr (!std::is_pointer_v<str_t>) {
				EXPECT_TRUE(fea::ends_with(alice_arr[j], valid_arr[j].data()));
			}
		}

		for (size_t j = 0; j < alice_arr.size(); ++j) {
			using str_t = std::decay_t<decltype(alice_arr[i])>;
			EXPECT_FALSE(fea::ends_with(alice_arr[j], invalid_arr[j]));

			// Only test chars for small cases, because we want other
			// scenarios.
			size_t size = fea::size(invalid_arr[j]);
			if (size <= 2) {
				EXPECT_FALSE(
						fea::ends_with(alice_arr[j], invalid_arr[j][size - 1]));
			}

			if constexpr (!std::is_pointer_v<str_t>) {
				EXPECT_FALSE(
						fea::ends_with(alice_arr[j], invalid_arr[j].data()));
			}
		}
	});
}

TEST(string, to_lower_to_upper_ascii) {
	gen_tests(
			testcases, "A", "AA", "BcD", "BoBBy", "+-/", "\naaaAAbbBB", "bla");

	gen_tests(lower_answers, "a", "aa", "bcd", "bobby", "+-/", "\naaaaabbbb",
			"bla");

	gen_tests(upper_answers, "A", "AA", "BCD", "BOBBY", "+-/", "\nAAAAABBBB",
			"BLA");

	constexpr size_t s = std::tuple_size_v<decltype(testcases)>;
	fea::static_for<s>([&](auto const_i) {
		constexpr size_t i = const_i;
		const auto& test_arr = std::get<i>(testcases);
		const auto& loanswer_arr = std::get<i>(lower_answers);
		const auto& hianswer_arr = std::get<i>(upper_answers);
		ASSERT_EQ(loanswer_arr.size(), hianswer_arr.size());

		for (size_t j = 0; j < loanswer_arr.size(); ++j) {
			const auto& str = test_arr[j];
			{
				auto new_str = fea::to_lower_ascii(str);
				EXPECT_EQ(loanswer_arr[j], new_str);

				new_str = fea::to_upper_ascii(str);
				EXPECT_EQ(hianswer_arr[j], new_str);

				new_str = fea::to_lower_ascii(str);
				EXPECT_EQ(loanswer_arr[j], new_str);
				new_str = fea::to_upper_ascii(str);
				EXPECT_EQ(hianswer_arr[j], new_str);
			}

			{
				using char_t = std::decay_t<decltype(str[0])>;
				auto temp_str = std::basic_string<char_t>{ str };
				fea::to_lower_ascii_inplace(temp_str);
				EXPECT_EQ(loanswer_arr[j], temp_str);

				temp_str = std::basic_string<char_t>{ str };
				fea::to_upper_ascii_inplace(temp_str);
				EXPECT_EQ(hianswer_arr[j], temp_str);

				fea::to_lower_ascii_inplace(temp_str);
				EXPECT_EQ(loanswer_arr[j], temp_str);
				fea::to_upper_ascii_inplace(temp_str);
				EXPECT_EQ(hianswer_arr[j], temp_str);
			}
		}
	});
}

TEST(string, capitalize_ascii) {
	gen_tests(testcases, "A B C", "bla bla bla", "bLA Bla blA", "a", "AA",
			"BcD", "BoBBy", "+-/", "\naaaAAbbBB", "bla");

	gen_tests(capitalize_answers, "A b c", "Bla bla bla", "Bla bla bla", "A",
			"Aa", "Bcd", "Bobby", "+-/", "\naaaaabbbb", "Bla");

	gen_tests(capitalize_words_answers, "A B C", "Bla Bla Bla", "Bla Bla Bla",
			"A", "Aa", "Bcd", "Bobby", "+-/", "\nAaaaabbbb", "Bla");

	constexpr size_t s = std::tuple_size_v<decltype(testcases)>;
	fea::static_for<s>([&](auto const_i) {
		constexpr size_t i = const_i;
		const auto& test_arr = std::get<i>(testcases);
		const auto& cap_arr = std::get<i>(capitalize_answers);
		const auto& cap_words_arr = std::get<i>(capitalize_words_answers);
		ASSERT_EQ(cap_arr.size(), cap_words_arr.size());

		for (size_t j = 0; j < cap_arr.size(); ++j) {
			const auto& str = test_arr[j];
			{
				auto new_str = fea::capitalize_ascii(str);
				EXPECT_EQ(cap_arr[j], new_str);

				new_str = fea::capitalize_words_ascii(str);
				EXPECT_EQ(cap_words_arr[j], new_str);

				new_str = fea::capitalize_ascii(str);
				EXPECT_EQ(cap_arr[j], new_str);
				new_str = fea::capitalize_words_ascii(str);
				EXPECT_EQ(cap_words_arr[j], new_str);
			}

			{
				using char_t = std::decay_t<decltype(str[0])>;
				auto temp_str = std::basic_string<char_t>{ str };
				fea::capitalize_ascii_inplace(temp_str);
				EXPECT_EQ(cap_arr[j], temp_str);

				temp_str = std::basic_string<char_t>{ str };
				fea::capitalize_words_ascii_inplace(temp_str);
				EXPECT_EQ(cap_words_arr[j], temp_str);

				fea::capitalize_ascii_inplace(temp_str);
				EXPECT_EQ(cap_arr[j], temp_str);
				fea::capitalize_words_ascii_inplace(temp_str);
				EXPECT_EQ(cap_words_arr[j], temp_str);
			}
		}
	});
}

TEST(string, is_letter) {
	gen_tests(valid_search, "a", "B", "c", "Z", "A", "z", "C");
	fea::tuple_for_each(
			[](const auto& str_tup) {
				for (const auto& str : str_tup) {
					EXPECT_TRUE(fea::is_letter_ascii(str[0]));
				}
			},
			valid_search);

	gen_tests(lower_search, "a", "b", "c", "z");
	fea::tuple_for_each(
			[](const auto& str_tup) {
				for (const auto& str : str_tup) {
					EXPECT_TRUE(fea::is_letter_ascii(str[0]));
					EXPECT_TRUE(fea::is_lower_letter_ascii(str[0]));
					EXPECT_FALSE(fea::is_upper_letter_ascii(str[0]));
				}
			},
			lower_search);

	gen_tests(upper_search, "A", "B", "C", "Z");
	fea::tuple_for_each(
			[](const auto& str_tup) {
				for (const auto& str : str_tup) {
					EXPECT_TRUE(fea::is_letter_ascii(str[0]));
					EXPECT_FALSE(fea::is_lower_letter_ascii(str[0]));
					EXPECT_TRUE(fea::is_upper_letter_ascii(str[0]));
				}
			},
			upper_search);

	gen_tests(invalid_search, "0", "9", "!", "-", " ", "/", "$", "#", "\n",
			"\r", "\t");
	fea::tuple_for_each(
			[](const auto& str_tup) {
				for (const auto& str : str_tup) {
					EXPECT_FALSE(fea::is_letter_ascii(str[0]));
					EXPECT_FALSE(fea::is_lower_letter_ascii(str[0]));
					EXPECT_FALSE(fea::is_upper_letter_ascii(str[0]));
				}
			},
			invalid_search);
}

TEST(string, is_number) {
	gen_tests(valid_search, "0", "12", "1234", "5555", "4242", "69");
	fea::tuple_for_each(
			[](const auto& str_tup) {
				for (const auto& str : str_tup) {
					EXPECT_TRUE(fea::is_number_ascii(str));
				}
			},
			valid_search);

	gen_tests(invalid_search, "\na", "b", "1cc", "0c1", "000000a", "+1", "-1-");
	fea::tuple_for_each(
			[](const auto& str_tup) {
				for (const auto& str : str_tup) {
					EXPECT_FALSE(fea::is_number_ascii(str));
				}
			},
			invalid_search);
}

TEST(string, line_funcs) {
	gen_tests(lines, "0\n\n1\n2\n3", "0\r\n\r\n1\r\n2\r\n3");
	gen_tests(answers1, "1", "1\r");
	gen_tests(answers2, "", "1");
	std::array<size_t, 2> expected_lines{ 5, 5 };

	constexpr size_t s = std::tuple_size_v<decltype(lines)>;
	fea::static_for<s>([&](auto const_i) {
		constexpr size_t i = const_i;
		const auto& test_arr = std::get<i>(lines);
		const auto& ans1_arr = std::get<i>(answers1);
		const auto& ans2_arr = std::get<i>(answers2);

		for (size_t j = 0; j < test_arr.size(); ++j) {
			{
				auto str_v = fea::get_line(test_arr[j], 2);
				auto ans1 = ans1_arr[j];
				EXPECT_EQ(str_v, ans1);
			}

			{
				using CharT = std::decay_t<decltype(test_arr[j][0])>;
				auto str_v = fea::get_line(test_arr[j], 2, FEA_LIT("\r\n"));
				auto ans = ans2_arr[j];
				EXPECT_EQ(str_v, ans);
			}

			{
				using CharT = std::decay_t<decltype(test_arr[j][0])>;

				size_t num_lines = 0;
				fea::for_each_line(test_arr[j], [&](auto sv) {
					++num_lines;
					if (num_lines == expected_lines[j]) {
						EXPECT_EQ(sv.size(), 1u);
						EXPECT_FALSE(fea::contains(sv, FEA_CH('\n')));
						return;
					}

					EXPECT_LE(sv.size(), 2u);
					EXPECT_FALSE(fea::contains(sv, FEA_CH('\n')));
				});
				EXPECT_EQ(num_lines, expected_lines[j]);

				num_lines = 0;
				fea::for_each_line<false>(test_arr[j], [&](auto sv) {
					++num_lines;
					if (num_lines == expected_lines[j]) {
						EXPECT_EQ(sv.size(), 1u);
						EXPECT_FALSE(fea::contains(sv, FEA_CH('\n')));
						return;
					}

					EXPECT_GE(sv.size(), 1u);
					EXPECT_TRUE(fea::contains(sv, FEA_CH('\n')));
				});
				EXPECT_EQ(num_lines, expected_lines[j]);
			}
		}
	});
}


#if FEA_CPP20
struct str {
	str(const char* s)
			: data(s) {
	}
	str() = default;

	friend std::strong_ordering operator<=>(const str& lhs, const str& rhs) {
		return fea::lexicographical_compare(lhs.data.begin(), lhs.data.end(),
				rhs.data.begin(), rhs.data.end());
	}
	friend bool operator==(const str& lhs, const str& rhs) {
		// Don't use operator<=>
		return lhs.data == rhs.data;
	}

	std::string data;
};

TEST(string, lexicographical_compare) {
	std::vector<str> vec{ "abc", "abcd", "Benefit", "Abc", "ABC", "ABCD", "Bob",
		"bobby", "0", "1", "aBc", "2", "As", "Aster", "Astrolabe", "Astronomy",
		"10", "astrophysics", "Ataman", "Baa", "Barnacle", "Attack", "Be",
		"been", "22", "At", "Bent" };

	const std::vector<str> answer{ "0", "1", "10", "2", "22", "ABC", "ABCD",
		"Abc", "As", "Aster", "Astrolabe", "Astronomy", "At", "Ataman",
		"Attack", "aBc", "abc", "abcd", "astrophysics", "Baa", "Barnacle", "Be",
		"Benefit", "Bent", "Bob", "been", "bobby" };

	std::sort(vec.begin(), vec.end());
	// for (const auto& str : vec) {
	//	printf("%s\n", str.data.c_str());
	//}
	EXPECT_EQ(answer, vec);
}
#endif
} // namespace

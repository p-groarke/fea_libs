#include <fea/language/language.hpp>
#include <gtest/gtest.h>

namespace {
#define FAIL_MSG "language.cpp : Unit test failed."

TEST(language, basics) {
	for (size_t i = 0; i < fea::detail::iso_639_languages.size(); ++i) {
		const fea::iso_639_lang& l = fea::detail::iso_639_languages[i];
		EXPECT_EQ(fea::iso_639_id_t(i), l.runtime_id);

		if (l.parent_macro_id != (std::numeric_limits<uint16_t>::max)()) {
			EXPECT_FALSE(l.parent_macro_code_3.empty());
		}

		{
			const fea::iso_639_lang& l3 = fea::iso_639_3_lookup(l.code_3);
			EXPECT_EQ(l.name, l3.name);
			EXPECT_EQ(l.runtime_id, l3.runtime_id);
		}
		if (!l.code_2b.empty()) {
			const fea::iso_639_lang& l2b = fea::iso_639_2b_lookup(l.code_2b);
			EXPECT_EQ(l.name, l2b.name);
			EXPECT_EQ(l.runtime_id, l2b.runtime_id);
		}
		if (!l.code_2t.empty()) {
			const fea::iso_639_lang& l2t = fea::iso_639_2t_lookup(l.code_2t);
			EXPECT_EQ(l.name, l2t.name);
			EXPECT_EQ(l.runtime_id, l2t.runtime_id);
		}
		if (!l.code_1.empty()) {
			const fea::iso_639_lang& l1 = fea::iso_639_1_lookup(l.code_1);
			EXPECT_EQ(l.name, l1.name);
			EXPECT_EQ(l.runtime_id, l1.runtime_id);
		}
	}

	EXPECT_TRUE(fea::iso_639_3_valid("eng"));
	EXPECT_TRUE(fea::iso_639_2b_valid("eng"));
	EXPECT_TRUE(fea::iso_639_2t_valid("eng"));
	EXPECT_TRUE(fea::iso_639_1_valid("en"));

	EXPECT_FALSE(fea::iso_639_3_valid("en"));
	EXPECT_FALSE(fea::iso_639_2b_valid("en"));
	EXPECT_FALSE(fea::iso_639_2t_valid("en"));
	EXPECT_FALSE(fea::iso_639_1_valid("e"));

	EXPECT_FALSE(fea::iso_639_3_valid("cn"));
	EXPECT_FALSE(fea::iso_639_2b_valid("cn"));
	EXPECT_FALSE(fea::iso_639_2t_valid("cn"));
	EXPECT_FALSE(fea::iso_639_1_valid("cn"));
}

TEST(language, english) {
	// Letters
	{
		constexpr std::span<const char> letters = fea::en::letters();
		static_assert(letters.size() == 26u, FAIL_MSG);

		std::vector<char> expected(letters.begin(), letters.end());
		std::vector<char> test = expected;
		std::sort(test.begin(), test.end());
		auto new_end = std::unique(test.begin(), test.end());
		test.erase(new_end, test.end());
		EXPECT_EQ(expected.size(), test.size());
		EXPECT_NE(expected, test);
	}

	// Letter freq
	{
		// Test constexpr access.
		[[maybe_unused]] constexpr double t = fea::en::letter_frequency('E');
		[[maybe_unused]] constexpr double t2 = fea::en::letter_frequency('e');

		// Make sure percentages add up.
		double total = 0.0;
		for (char c = 65; c < 91; ++c) {
			total += fea::en::letter_frequency(c);
		}
		EXPECT_NEAR(total, 1.0, 0.000001);

		total = 0.0;
		for (char c = 97; c < 123; ++c) {
			total += fea::en::letter_frequency(c);
		}
		EXPECT_NEAR(total, 1.0, 0.000001);

		total = 0.0;
		for (uint8_t c = 0; c < 255u; ++c) {
			total += fea::en::letter_frequency(c);
		}
		EXPECT_NEAR(total, 2.0, 0.000001);
	}

	// Dictionary
	{
		constexpr std::span<const std::string_view> dic = fea::en::dictionary();
		static_assert(dic.size() != 0, FAIL_MSG);

		std::vector<std::string_view> expected(dic.begin(), dic.end());
		std::vector<std::string_view> test = expected;
		std::sort(test.begin(), test.end());
		auto new_end = std::unique(test.begin(), test.end());
		test.erase(new_end, test.end());
		EXPECT_EQ(expected.size(), test.size());
		EXPECT_NE(expected, test);
	}

	// Bigrams
	{
		constexpr std::span<const std::string_view> bigrams
				= fea::en::bigrams();
		static_assert(bigrams.size() == 26u * 26u, FAIL_MSG);

		std::vector<std::string_view> expected(bigrams.begin(), bigrams.end());
		std::vector<std::string_view> test = expected;
		std::sort(test.begin(), test.end());
		auto new_end = std::unique(test.begin(), test.end());
		test.erase(new_end, test.end());
		EXPECT_EQ(expected.size(), test.size());
		EXPECT_NE(expected, test);

		[[maybe_unused]] constexpr double f = fea::en::bigram_frequency("th");

		// Make sure percentages add up.
		double total = 0.0;
		for (char c = 'a'; c <= 'z'; ++c) {
			for (char c2 = 'a'; c2 <= 'z'; ++c2) {
				std::array<char, 2> b{ c, c2 };
				total += fea::en::bigram_frequency({ b.data(), b.size() });
			}
		}
		EXPECT_NEAR(total, 1.0, 0.000001);
	}
}
} // namespace

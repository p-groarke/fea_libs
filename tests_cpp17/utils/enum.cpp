#include <fea/utils/enum.hpp>
#include <fea/utils/unused.hpp>
#include <gtest/gtest.h>

namespace {
TEST(enum, traits) {
	enum class e {
		one,
		two,
		three,
		four,
		count,
	};

	fea::pack_nt<e, e::one, e::two, e::three, e::four> p = fea::explode_enum<e>(
			[](auto... cs) {
				using pack_t = fea::pack_nt<e, decltype(cs)::value...>;
				constexpr size_t idx = fea::pack_idx_nt_v<e, e::three, pack_t>;
				static_assert(idx == 2, "traits.cpp : test failed");
				return fea::pack_nt<e, decltype(cs)::value...>{};
			});
	fea::unused(p);

	static_assert(!fea::is_same_nt_v<e, e::one, e::two>,
			"type_map.cpp : test failed");
	static_assert(
			fea::is_same_nt_v<e, e::one, e::one>, "type_map.cpp : test failed");
}

TEST(enum, safe_switch) {
	enum class e {
		one,
		two,
		three,
		four,
		count,
	};

	int result = 0;
	auto switcher = fea::safe_switch<e>()
							.case_<e::one>([&]() { result = 1; })
							.case_<e::three>([&]() { result = 3; })
							.case_<e::two>([&]() { result = 2; })
							.case_<e::four>([&]() { result = 4; });
	switcher(e::one);
	EXPECT_EQ(result, 1);

	switcher(e::three);
	EXPECT_EQ(result, 3);

	switcher(e::two);
	EXPECT_EQ(result, 2);

	switcher(e::four);
	EXPECT_EQ(result, 4);

#if FEA_DEBUG
	EXPECT_DEATH(switcher(e::count), "");
#endif

	// Using operator () example.
	fea::safe_switch<e>()
			.case_<e::one>([&]() { result = 1; })
			.case_<e::three>([&]() { result = 3; })
			.case_<e::two>([&]() { result = 2; })
			.case_<e::four>([&]() { result = 4; })(e::three);

	EXPECT_EQ(result, 3);
}

namespace espace_all {

FEA_STRING_ENUM(e, unsigned, zero, one, two, three, four, five, count);

TEST(enum_macros, basics) {

	EXPECT_EQ(size_t(e::zero), 0u);
	EXPECT_EQ(size_t(e::one), 1u);
	EXPECT_EQ(size_t(e::two), 2u);
	EXPECT_EQ(size_t(e::three), 3u);
	EXPECT_EQ(size_t(e::four), 4u);
	EXPECT_EQ(size_t(e::five), 5u);
	EXPECT_EQ(size_t(e::count), 6u);

	EXPECT_EQ(enu::literals<e>(), enu::e_literals);
	EXPECT_EQ(enu::wliterals<e>(), enu::e_wliterals);
	EXPECT_EQ(enu::u16literals<e>(), enu::e_u16literals);
	EXPECT_EQ(enu::u32literals<e>(), enu::e_u32literals);

	EXPECT_EQ(enu::strings<e>(), enu::e_strings);
	EXPECT_EQ(enu::wstrings<e>(), enu::e_wstrings);
	EXPECT_EQ(enu::u16strings<e>(), enu::e_u16strings);
	EXPECT_EQ(enu::u32strings<e>(), enu::e_u32strings);

	EXPECT_EQ(enu::e_literals.size(), size_t(e::count) + 1);
	EXPECT_EQ(enu::e_wliterals.size(), size_t(e::count) + 1);
	EXPECT_EQ(enu::e_u16literals.size(), size_t(e::count) + 1);
	EXPECT_EQ(enu::e_u32literals.size(), size_t(e::count) + 1);

	EXPECT_EQ(enu::e_strings.size(), size_t(e::count) + 1);
	EXPECT_EQ(enu::e_wstrings.size(), size_t(e::count) + 1);
	EXPECT_EQ(enu::e_u16strings.size(), size_t(e::count) + 1);
	EXPECT_EQ(enu::e_u32strings.size(), size_t(e::count) + 1);

	EXPECT_STREQ(enu::e_literals[e::zero], "zero");
	EXPECT_STREQ(enu::e_literals[e::one], "one");
	EXPECT_STREQ(enu::e_literals[e::two], "two");
	EXPECT_STREQ(enu::e_literals[e::three], "three");
	EXPECT_STREQ(enu::e_literals[e::four], "four");
	EXPECT_STREQ(enu::e_literals[e::five], "five");
	EXPECT_STREQ(enu::e_literals[e::count], "count");

	EXPECT_STREQ(enu::e_wliterals[e::zero], L"zero");
	EXPECT_STREQ(enu::e_wliterals[e::one], L"one");
	EXPECT_STREQ(enu::e_wliterals[e::two], L"two");
	EXPECT_STREQ(enu::e_wliterals[e::three], L"three");
	EXPECT_STREQ(enu::e_wliterals[e::four], L"four");
	EXPECT_STREQ(enu::e_wliterals[e::five], L"five");
	EXPECT_STREQ(enu::e_wliterals[e::count], L"count");

	EXPECT_EQ(enu::e_u16literals[e::zero], std::u16string{ u"zero" });
	EXPECT_EQ(enu::e_u16literals[e::one], std::u16string{ u"one" });
	EXPECT_EQ(enu::e_u16literals[e::two], std::u16string{ u"two" });
	EXPECT_EQ(enu::e_u16literals[e::three], std::u16string{ u"three" });
	EXPECT_EQ(enu::e_u16literals[e::four], std::u16string{ u"four" });
	EXPECT_EQ(enu::e_u16literals[e::five], std::u16string{ u"five" });
	EXPECT_EQ(enu::e_u16literals[e::count], std::u16string{ u"count" });

	EXPECT_EQ(enu::e_u32literals[e::zero], std::u32string{ U"zero" });
	EXPECT_EQ(enu::e_u32literals[e::one], std::u32string{ U"one" });
	EXPECT_EQ(enu::e_u32literals[e::two], std::u32string{ U"two" });
	EXPECT_EQ(enu::e_u32literals[e::three], std::u32string{ U"three" });
	EXPECT_EQ(enu::e_u32literals[e::four], std::u32string{ U"four" });
	EXPECT_EQ(enu::e_u32literals[e::five], std::u32string{ U"five" });
	EXPECT_EQ(enu::e_u32literals[e::count], std::u32string{ U"count" });


	EXPECT_EQ(enu::e_strings[e::zero], "zero");
	EXPECT_EQ(enu::e_strings[e::one], "one");
	EXPECT_EQ(enu::e_strings[e::two], "two");
	EXPECT_EQ(enu::e_strings[e::three], "three");
	EXPECT_EQ(enu::e_strings[e::four], "four");
	EXPECT_EQ(enu::e_strings[e::five], "five");
	EXPECT_EQ(enu::e_strings[e::count], "count");

	EXPECT_EQ(enu::e_wstrings[e::zero], L"zero");
	EXPECT_EQ(enu::e_wstrings[e::one], L"one");
	EXPECT_EQ(enu::e_wstrings[e::two], L"two");
	EXPECT_EQ(enu::e_wstrings[e::three], L"three");
	EXPECT_EQ(enu::e_wstrings[e::four], L"four");
	EXPECT_EQ(enu::e_wstrings[e::five], L"five");
	EXPECT_EQ(enu::e_wstrings[e::count], L"count");

	EXPECT_EQ(enu::e_u16strings[e::zero], u"zero");
	EXPECT_EQ(enu::e_u16strings[e::one], u"one");
	EXPECT_EQ(enu::e_u16strings[e::two], u"two");
	EXPECT_EQ(enu::e_u16strings[e::three], u"three");
	EXPECT_EQ(enu::e_u16strings[e::four], u"four");
	EXPECT_EQ(enu::e_u16strings[e::five], u"five");
	EXPECT_EQ(enu::e_u16strings[e::count], u"count");

	EXPECT_EQ(enu::e_u32strings[e::zero], U"zero");
	EXPECT_EQ(enu::e_u32strings[e::one], U"one");
	EXPECT_EQ(enu::e_u32strings[e::two], U"two");
	EXPECT_EQ(enu::e_u32strings[e::three], U"three");
	EXPECT_EQ(enu::e_u32strings[e::four], U"four");
	EXPECT_EQ(enu::e_u32strings[e::five], U"five");
	EXPECT_EQ(enu::e_u32strings[e::count], U"count");


	EXPECT_STREQ(enu::to_literal(e::zero), "zero");
	EXPECT_STREQ(enu::to_literal(e::one), "one");
	EXPECT_STREQ(enu::to_literal(e::two), "two");
	EXPECT_STREQ(enu::to_literal(e::three), "three");
	EXPECT_STREQ(enu::to_literal(e::four), "four");
	EXPECT_STREQ(enu::to_literal(e::five), "five");
	EXPECT_STREQ(enu::to_literal(e::count), "count");

	EXPECT_STREQ(enu::to_wliteral(e::zero), L"zero");
	EXPECT_STREQ(enu::to_wliteral(e::one), L"one");
	EXPECT_STREQ(enu::to_wliteral(e::two), L"two");
	EXPECT_STREQ(enu::to_wliteral(e::three), L"three");
	EXPECT_STREQ(enu::to_wliteral(e::four), L"four");
	EXPECT_STREQ(enu::to_wliteral(e::five), L"five");
	EXPECT_STREQ(enu::to_wliteral(e::count), L"count");

	EXPECT_EQ(enu::to_u16literal(e::zero), std::u16string{ u"zero" });
	EXPECT_EQ(enu::to_u16literal(e::one), std::u16string{ u"one" });
	EXPECT_EQ(enu::to_u16literal(e::two), std::u16string{ u"two" });
	EXPECT_EQ(enu::to_u16literal(e::three), std::u16string{ u"three" });
	EXPECT_EQ(enu::to_u16literal(e::four), std::u16string{ u"four" });
	EXPECT_EQ(enu::to_u16literal(e::five), std::u16string{ u"five" });
	EXPECT_EQ(enu::to_u16literal(e::count), std::u16string{ u"count" });

	EXPECT_EQ(enu::to_u32literal(e::zero), std::u32string{ U"zero" });
	EXPECT_EQ(enu::to_u32literal(e::one), std::u32string{ U"one" });
	EXPECT_EQ(enu::to_u32literal(e::two), std::u32string{ U"two" });
	EXPECT_EQ(enu::to_u32literal(e::three), std::u32string{ U"three" });
	EXPECT_EQ(enu::to_u32literal(e::four), std::u32string{ U"four" });
	EXPECT_EQ(enu::to_u32literal(e::five), std::u32string{ U"five" });
	EXPECT_EQ(enu::to_u32literal(e::count), std::u32string{ U"count" });


	EXPECT_EQ(enu::to_string(e::zero), "zero");
	EXPECT_EQ(enu::to_string(e::one), "one");
	EXPECT_EQ(enu::to_string(e::two), "two");
	EXPECT_EQ(enu::to_string(e::three), "three");
	EXPECT_EQ(enu::to_string(e::four), "four");
	EXPECT_EQ(enu::to_string(e::five), "five");
	EXPECT_EQ(enu::to_string(e::count), "count");

	EXPECT_EQ(enu::to_wstring(e::zero), L"zero");
	EXPECT_EQ(enu::to_wstring(e::one), L"one");
	EXPECT_EQ(enu::to_wstring(e::two), L"two");
	EXPECT_EQ(enu::to_wstring(e::three), L"three");
	EXPECT_EQ(enu::to_wstring(e::four), L"four");
	EXPECT_EQ(enu::to_wstring(e::five), L"five");
	EXPECT_EQ(enu::to_wstring(e::count), L"count");

	EXPECT_EQ(enu::to_u16string(e::zero), u"zero");
	EXPECT_EQ(enu::to_u16string(e::one), u"one");
	EXPECT_EQ(enu::to_u16string(e::two), u"two");
	EXPECT_EQ(enu::to_u16string(e::three), u"three");
	EXPECT_EQ(enu::to_u16string(e::four), u"four");
	EXPECT_EQ(enu::to_u16string(e::five), u"five");
	EXPECT_EQ(enu::to_u16string(e::count), u"count");

	EXPECT_EQ(enu::to_u32string(e::zero), U"zero");
	EXPECT_EQ(enu::to_u32string(e::one), U"one");
	EXPECT_EQ(enu::to_u32string(e::two), U"two");
	EXPECT_EQ(enu::to_u32string(e::three), U"three");
	EXPECT_EQ(enu::to_u32string(e::four), U"four");
	EXPECT_EQ(enu::to_u32string(e::five), U"five");
	EXPECT_EQ(enu::to_u32string(e::count), U"count");


	EXPECT_STREQ(enu::to_literal<e::zero>(), "zero");
	EXPECT_STREQ(enu::to_literal<e::one>(), "one");
	EXPECT_STREQ(enu::to_literal<e::two>(), "two");
	EXPECT_STREQ(enu::to_literal<e::three>(), "three");
	EXPECT_STREQ(enu::to_literal<e::four>(), "four");
	EXPECT_STREQ(enu::to_literal<e::five>(), "five");
	EXPECT_STREQ(enu::to_literal<e::count>(), "count");

	EXPECT_STREQ(enu::to_wliteral<e::zero>(), L"zero");
	EXPECT_STREQ(enu::to_wliteral<e::one>(), L"one");
	EXPECT_STREQ(enu::to_wliteral<e::two>(), L"two");
	EXPECT_STREQ(enu::to_wliteral<e::three>(), L"three");
	EXPECT_STREQ(enu::to_wliteral<e::four>(), L"four");
	EXPECT_STREQ(enu::to_wliteral<e::five>(), L"five");
	EXPECT_STREQ(enu::to_wliteral<e::count>(), L"count");

	EXPECT_EQ(enu::to_u16literal<e::zero>(), std::u16string{ u"zero" });
	EXPECT_EQ(enu::to_u16literal<e::one>(), std::u16string{ u"one" });
	EXPECT_EQ(enu::to_u16literal<e::two>(), std::u16string{ u"two" });
	EXPECT_EQ(enu::to_u16literal<e::three>(), std::u16string{ u"three" });
	EXPECT_EQ(enu::to_u16literal<e::four>(), std::u16string{ u"four" });
	EXPECT_EQ(enu::to_u16literal<e::five>(), std::u16string{ u"five" });
	EXPECT_EQ(enu::to_u16literal<e::count>(), std::u16string{ u"count" });

	EXPECT_EQ(enu::to_u32literal<e::zero>(), std::u32string{ U"zero" });
	EXPECT_EQ(enu::to_u32literal<e::one>(), std::u32string{ U"one" });
	EXPECT_EQ(enu::to_u32literal<e::two>(), std::u32string{ U"two" });
	EXPECT_EQ(enu::to_u32literal<e::three>(), std::u32string{ U"three" });
	EXPECT_EQ(enu::to_u32literal<e::four>(), std::u32string{ U"four" });
	EXPECT_EQ(enu::to_u32literal<e::five>(), std::u32string{ U"five" });
	EXPECT_EQ(enu::to_u32literal<e::count>(), std::u32string{ U"count" });


	EXPECT_EQ(enu::to_string<e::zero>(), "zero");
	EXPECT_EQ(enu::to_string<e::one>(), "one");
	EXPECT_EQ(enu::to_string<e::two>(), "two");
	EXPECT_EQ(enu::to_string<e::three>(), "three");
	EXPECT_EQ(enu::to_string<e::four>(), "four");
	EXPECT_EQ(enu::to_string<e::five>(), "five");
	EXPECT_EQ(enu::to_string<e::count>(), "count");

	EXPECT_EQ(enu::to_wstring<e::zero>(), L"zero");
	EXPECT_EQ(enu::to_wstring<e::one>(), L"one");
	EXPECT_EQ(enu::to_wstring<e::two>(), L"two");
	EXPECT_EQ(enu::to_wstring<e::three>(), L"three");
	EXPECT_EQ(enu::to_wstring<e::four>(), L"four");
	EXPECT_EQ(enu::to_wstring<e::five>(), L"five");
	EXPECT_EQ(enu::to_wstring<e::count>(), L"count");

	EXPECT_EQ(enu::to_u16string<e::zero>(), std::u16string{ u"zero" });
	EXPECT_EQ(enu::to_u16string<e::one>(), std::u16string{ u"one" });
	EXPECT_EQ(enu::to_u16string<e::two>(), std::u16string{ u"two" });
	EXPECT_EQ(enu::to_u16string<e::three>(), std::u16string{ u"three" });
	EXPECT_EQ(enu::to_u16string<e::four>(), std::u16string{ u"four" });
	EXPECT_EQ(enu::to_u16string<e::five>(), std::u16string{ u"five" });
	EXPECT_EQ(enu::to_u16string<e::count>(), std::u16string{ u"count" });

	EXPECT_EQ(enu::to_u32string<e::zero>(), std::u32string{ U"zero" });
	EXPECT_EQ(enu::to_u32string<e::one>(), std::u32string{ U"one" });
	EXPECT_EQ(enu::to_u32string<e::two>(), std::u32string{ U"two" });
	EXPECT_EQ(enu::to_u32string<e::three>(), std::u32string{ U"three" });
	EXPECT_EQ(enu::to_u32string<e::four>(), std::u32string{ U"four" });
	EXPECT_EQ(enu::to_u32string<e::five>(), std::u32string{ U"five" });
	EXPECT_EQ(enu::to_u32string<e::count>(), std::u32string{ U"count" });
}
} // namespace espace_all

namespace espace2 {
FEA_STRING_ENUM_WITH_COUNT(e, unsigned, zero, one, two, three, four, five);
TEST(enum_macros, basics) {
	EXPECT_EQ(size_t(e::zero), 0u);
	EXPECT_EQ(size_t(e::one), 1u);
	EXPECT_EQ(size_t(e::two), 2u);
	EXPECT_EQ(size_t(e::three), 3u);
	EXPECT_EQ(size_t(e::four), 4u);
	EXPECT_EQ(size_t(e::five), 5u);
	EXPECT_EQ(size_t(e::count), 6u);

	EXPECT_EQ(enu::literals<e>(), enu::e_literals);
	EXPECT_EQ(enu::wliterals<e>(), enu::e_wliterals);
	EXPECT_EQ(enu::u16literals<e>(), enu::e_u16literals);
	EXPECT_EQ(enu::u32literals<e>(), enu::e_u32literals);

	EXPECT_EQ(enu::strings<e>(), enu::e_strings);
	EXPECT_EQ(enu::wstrings<e>(), enu::e_wstrings);
	EXPECT_EQ(enu::u16strings<e>(), enu::e_u16strings);
	EXPECT_EQ(enu::u32strings<e>(), enu::e_u32strings);

	EXPECT_EQ(enu::e_literals.size(), size_t(e::count));
	EXPECT_EQ(enu::e_wliterals.size(), size_t(e::count));
	EXPECT_EQ(enu::e_u16literals.size(), size_t(e::count));
	EXPECT_EQ(enu::e_u32literals.size(), size_t(e::count));

	EXPECT_EQ(enu::e_strings.size(), size_t(e::count));
	EXPECT_EQ(enu::e_wstrings.size(), size_t(e::count));
	EXPECT_EQ(enu::e_u16strings.size(), size_t(e::count));
	EXPECT_EQ(enu::e_u32strings.size(), size_t(e::count));

	EXPECT_STREQ(enu::e_literals[e::zero], "zero");
	EXPECT_STREQ(enu::e_literals[e::one], "one");
	EXPECT_STREQ(enu::e_literals[e::two], "two");
	EXPECT_STREQ(enu::e_literals[e::three], "three");
	EXPECT_STREQ(enu::e_literals[e::four], "four");
	EXPECT_STREQ(enu::e_literals[e::five], "five");

	EXPECT_STREQ(enu::e_wliterals[e::zero], L"zero");
	EXPECT_STREQ(enu::e_wliterals[e::one], L"one");
	EXPECT_STREQ(enu::e_wliterals[e::two], L"two");
	EXPECT_STREQ(enu::e_wliterals[e::three], L"three");
	EXPECT_STREQ(enu::e_wliterals[e::four], L"four");
	EXPECT_STREQ(enu::e_wliterals[e::five], L"five");

	EXPECT_EQ(enu::e_u16literals[e::zero], std::u16string{ u"zero" });
	EXPECT_EQ(enu::e_u16literals[e::one], std::u16string{ u"one" });
	EXPECT_EQ(enu::e_u16literals[e::two], std::u16string{ u"two" });
	EXPECT_EQ(enu::e_u16literals[e::three], std::u16string{ u"three" });
	EXPECT_EQ(enu::e_u16literals[e::four], std::u16string{ u"four" });
	EXPECT_EQ(enu::e_u16literals[e::five], std::u16string{ u"five" });

	EXPECT_EQ(enu::e_u32literals[e::zero], std::u32string{ U"zero" });
	EXPECT_EQ(enu::e_u32literals[e::one], std::u32string{ U"one" });
	EXPECT_EQ(enu::e_u32literals[e::two], std::u32string{ U"two" });
	EXPECT_EQ(enu::e_u32literals[e::three], std::u32string{ U"three" });
	EXPECT_EQ(enu::e_u32literals[e::four], std::u32string{ U"four" });
	EXPECT_EQ(enu::e_u32literals[e::five], std::u32string{ U"five" });

	EXPECT_EQ(enu::e_strings[e::zero], "zero");
	EXPECT_EQ(enu::e_strings[e::one], "one");
	EXPECT_EQ(enu::e_strings[e::two], "two");
	EXPECT_EQ(enu::e_strings[e::three], "three");
	EXPECT_EQ(enu::e_strings[e::four], "four");
	EXPECT_EQ(enu::e_strings[e::five], "five");

	EXPECT_EQ(enu::e_wstrings[e::zero], L"zero");
	EXPECT_EQ(enu::e_wstrings[e::one], L"one");
	EXPECT_EQ(enu::e_wstrings[e::two], L"two");
	EXPECT_EQ(enu::e_wstrings[e::three], L"three");
	EXPECT_EQ(enu::e_wstrings[e::four], L"four");
	EXPECT_EQ(enu::e_wstrings[e::five], L"five");

	EXPECT_EQ(enu::e_u16strings[e::zero], u"zero");
	EXPECT_EQ(enu::e_u16strings[e::one], u"one");
	EXPECT_EQ(enu::e_u16strings[e::two], u"two");
	EXPECT_EQ(enu::e_u16strings[e::three], u"three");
	EXPECT_EQ(enu::e_u16strings[e::four], u"four");
	EXPECT_EQ(enu::e_u16strings[e::five], u"five");

	EXPECT_EQ(enu::e_u32strings[e::zero], U"zero");
	EXPECT_EQ(enu::e_u32strings[e::one], U"one");
	EXPECT_EQ(enu::e_u32strings[e::two], U"two");
	EXPECT_EQ(enu::e_u32strings[e::three], U"three");
	EXPECT_EQ(enu::e_u32strings[e::four], U"four");
	EXPECT_EQ(enu::e_u32strings[e::five], U"five");


	EXPECT_STREQ(enu::to_literal(e::zero), "zero");
	EXPECT_STREQ(enu::to_literal(e::one), "one");
	EXPECT_STREQ(enu::to_literal(e::two), "two");
	EXPECT_STREQ(enu::to_literal(e::three), "three");
	EXPECT_STREQ(enu::to_literal(e::four), "four");
	EXPECT_STREQ(enu::to_literal(e::five), "five");

	EXPECT_STREQ(enu::to_wliteral(e::zero), L"zero");
	EXPECT_STREQ(enu::to_wliteral(e::one), L"one");
	EXPECT_STREQ(enu::to_wliteral(e::two), L"two");
	EXPECT_STREQ(enu::to_wliteral(e::three), L"three");
	EXPECT_STREQ(enu::to_wliteral(e::four), L"four");
	EXPECT_STREQ(enu::to_wliteral(e::five), L"five");

	EXPECT_EQ(enu::to_u16literal(e::zero), std::u16string{ u"zero" });
	EXPECT_EQ(enu::to_u16literal(e::one), std::u16string{ u"one" });
	EXPECT_EQ(enu::to_u16literal(e::two), std::u16string{ u"two" });
	EXPECT_EQ(enu::to_u16literal(e::three), std::u16string{ u"three" });
	EXPECT_EQ(enu::to_u16literal(e::four), std::u16string{ u"four" });
	EXPECT_EQ(enu::to_u16literal(e::five), std::u16string{ u"five" });

	EXPECT_EQ(enu::to_u32literal(e::zero), std::u32string{ U"zero" });
	EXPECT_EQ(enu::to_u32literal(e::one), std::u32string{ U"one" });
	EXPECT_EQ(enu::to_u32literal(e::two), std::u32string{ U"two" });
	EXPECT_EQ(enu::to_u32literal(e::three), std::u32string{ U"three" });
	EXPECT_EQ(enu::to_u32literal(e::four), std::u32string{ U"four" });
	EXPECT_EQ(enu::to_u32literal(e::five), std::u32string{ U"five" });


	EXPECT_EQ(enu::to_string(e::zero), "zero");
	EXPECT_EQ(enu::to_string(e::one), "one");
	EXPECT_EQ(enu::to_string(e::two), "two");
	EXPECT_EQ(enu::to_string(e::three), "three");
	EXPECT_EQ(enu::to_string(e::four), "four");
	EXPECT_EQ(enu::to_string(e::five), "five");

	EXPECT_EQ(enu::to_wstring(e::zero), L"zero");
	EXPECT_EQ(enu::to_wstring(e::one), L"one");
	EXPECT_EQ(enu::to_wstring(e::two), L"two");
	EXPECT_EQ(enu::to_wstring(e::three), L"three");
	EXPECT_EQ(enu::to_wstring(e::four), L"four");
	EXPECT_EQ(enu::to_wstring(e::five), L"five");

	EXPECT_EQ(enu::to_u16string(e::zero), u"zero");
	EXPECT_EQ(enu::to_u16string(e::one), u"one");
	EXPECT_EQ(enu::to_u16string(e::two), u"two");
	EXPECT_EQ(enu::to_u16string(e::three), u"three");
	EXPECT_EQ(enu::to_u16string(e::four), u"four");
	EXPECT_EQ(enu::to_u16string(e::five), u"five");

	EXPECT_EQ(enu::to_u32string(e::zero), U"zero");
	EXPECT_EQ(enu::to_u32string(e::one), U"one");
	EXPECT_EQ(enu::to_u32string(e::two), U"two");
	EXPECT_EQ(enu::to_u32string(e::three), U"three");
	EXPECT_EQ(enu::to_u32string(e::four), U"four");
	EXPECT_EQ(enu::to_u32string(e::five), U"five");


	EXPECT_STREQ(enu::to_literal<e::zero>(), "zero");
	EXPECT_STREQ(enu::to_literal<e::one>(), "one");
	EXPECT_STREQ(enu::to_literal<e::two>(), "two");
	EXPECT_STREQ(enu::to_literal<e::three>(), "three");
	EXPECT_STREQ(enu::to_literal<e::four>(), "four");
	EXPECT_STREQ(enu::to_literal<e::five>(), "five");

	EXPECT_STREQ(enu::to_wliteral<e::zero>(), L"zero");
	EXPECT_STREQ(enu::to_wliteral<e::one>(), L"one");
	EXPECT_STREQ(enu::to_wliteral<e::two>(), L"two");
	EXPECT_STREQ(enu::to_wliteral<e::three>(), L"three");
	EXPECT_STREQ(enu::to_wliteral<e::four>(), L"four");
	EXPECT_STREQ(enu::to_wliteral<e::five>(), L"five");

	EXPECT_EQ(enu::to_u16literal<e::zero>(), std::u16string{ u"zero" });
	EXPECT_EQ(enu::to_u16literal<e::one>(), std::u16string{ u"one" });
	EXPECT_EQ(enu::to_u16literal<e::two>(), std::u16string{ u"two" });
	EXPECT_EQ(enu::to_u16literal<e::three>(), std::u16string{ u"three" });
	EXPECT_EQ(enu::to_u16literal<e::four>(), std::u16string{ u"four" });
	EXPECT_EQ(enu::to_u16literal<e::five>(), std::u16string{ u"five" });

	EXPECT_EQ(enu::to_u32literal<e::zero>(), std::u32string{ U"zero" });
	EXPECT_EQ(enu::to_u32literal<e::one>(), std::u32string{ U"one" });
	EXPECT_EQ(enu::to_u32literal<e::two>(), std::u32string{ U"two" });
	EXPECT_EQ(enu::to_u32literal<e::three>(), std::u32string{ U"three" });
	EXPECT_EQ(enu::to_u32literal<e::four>(), std::u32string{ U"four" });
	EXPECT_EQ(enu::to_u32literal<e::five>(), std::u32string{ U"five" });


	EXPECT_EQ(enu::to_string<e::zero>(), "zero");
	EXPECT_EQ(enu::to_string<e::one>(), "one");
	EXPECT_EQ(enu::to_string<e::two>(), "two");
	EXPECT_EQ(enu::to_string<e::three>(), "three");
	EXPECT_EQ(enu::to_string<e::four>(), "four");
	EXPECT_EQ(enu::to_string<e::five>(), "five");

	EXPECT_EQ(enu::to_wstring<e::zero>(), L"zero");
	EXPECT_EQ(enu::to_wstring<e::one>(), L"one");
	EXPECT_EQ(enu::to_wstring<e::two>(), L"two");
	EXPECT_EQ(enu::to_wstring<e::three>(), L"three");
	EXPECT_EQ(enu::to_wstring<e::four>(), L"four");
	EXPECT_EQ(enu::to_wstring<e::five>(), L"five");

	EXPECT_EQ(enu::to_u16string<e::zero>(), std::u16string{ u"zero" });
	EXPECT_EQ(enu::to_u16string<e::one>(), std::u16string{ u"one" });
	EXPECT_EQ(enu::to_u16string<e::two>(), std::u16string{ u"two" });
	EXPECT_EQ(enu::to_u16string<e::three>(), std::u16string{ u"three" });
	EXPECT_EQ(enu::to_u16string<e::four>(), std::u16string{ u"four" });
	EXPECT_EQ(enu::to_u16string<e::five>(), std::u16string{ u"five" });

	EXPECT_EQ(enu::to_u32string<e::zero>(), std::u32string{ U"zero" });
	EXPECT_EQ(enu::to_u32string<e::one>(), std::u32string{ U"one" });
	EXPECT_EQ(enu::to_u32string<e::two>(), std::u32string{ U"two" });
	EXPECT_EQ(enu::to_u32string<e::three>(), std::u32string{ U"three" });
	EXPECT_EQ(enu::to_u32string<e::four>(), std::u32string{ U"four" });
	EXPECT_EQ(enu::to_u32string<e::five>(), std::u32string{ U"five" });
}
} // namespace espace2
} // namespace

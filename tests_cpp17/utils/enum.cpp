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

	fea::pack_nt<e::one, e::two, e::three, e::four> p
			= fea::explode_enum<e>([](auto... cs) {
				  using pack_t = fea::pack_nt<decltype(cs)::value...>;
				  constexpr size_t idx = fea::pack_idx_nt_v<e::three, pack_t>;
				  static_assert(idx == 2, "traits.cpp : test failed");
				  return fea::pack_nt<decltype(cs)::value...>{};
			  });
	fea::unused(p);

	static_assert(!fea::is_same_nt_v<e, e::one, e::two>,
			"type_map.cpp : test failed");
	static_assert(
			!fea::is_same_nt_v2<e::one, e::two>, "type_map.cpp : test failed");
	static_assert(
			fea::is_same_nt_v<e, e::one, e::one>, "type_map.cpp : test failed");
	static_assert(
			fea::is_same_nt_v2<e::one, e::one>, "type_map.cpp : test failed");
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

FEA_ALLSTRINGS_ENUM(e, unsigned, zero, one, two, three, four, five, count)

TEST(enum_macros, basics) {

	EXPECT_EQ(size_t(e::zero), 0u);
	EXPECT_EQ(size_t(e::one), 1u);
	EXPECT_EQ(size_t(e::two), 2u);
	EXPECT_EQ(size_t(e::three), 3u);
	EXPECT_EQ(size_t(e::four), 4u);
	EXPECT_EQ(size_t(e::five), 5u);
	EXPECT_EQ(size_t(e::count), 6u);

	EXPECT_EQ(strings<e>(), e_strings);
	EXPECT_EQ(wstrings<e>(), e_wstrings);
	EXPECT_EQ(u16strings<e>(), e_u16strings);
	EXPECT_EQ(u32strings<e>(), e_u32strings);

	EXPECT_EQ(e_strings.size(), size_t(e::count) + 1);
	EXPECT_EQ(e_wstrings.size(), size_t(e::count) + 1);
	EXPECT_EQ(e_u16strings.size(), size_t(e::count) + 1);
	EXPECT_EQ(e_u32strings.size(), size_t(e::count) + 1);


	EXPECT_EQ(e_strings[e::zero], "zero");
	EXPECT_EQ(e_strings[e::one], "one");
	EXPECT_EQ(e_strings[e::two], "two");
	EXPECT_EQ(e_strings[e::three], "three");
	EXPECT_EQ(e_strings[e::four], "four");
	EXPECT_EQ(e_strings[e::five], "five");
	EXPECT_EQ(e_strings[e::count], "count");

	EXPECT_EQ(e_wstrings[e::zero], L"zero");
	EXPECT_EQ(e_wstrings[e::one], L"one");
	EXPECT_EQ(e_wstrings[e::two], L"two");
	EXPECT_EQ(e_wstrings[e::three], L"three");
	EXPECT_EQ(e_wstrings[e::four], L"four");
	EXPECT_EQ(e_wstrings[e::five], L"five");
	EXPECT_EQ(e_wstrings[e::count], L"count");

	EXPECT_EQ(e_u16strings[e::zero], u"zero");
	EXPECT_EQ(e_u16strings[e::one], u"one");
	EXPECT_EQ(e_u16strings[e::two], u"two");
	EXPECT_EQ(e_u16strings[e::three], u"three");
	EXPECT_EQ(e_u16strings[e::four], u"four");
	EXPECT_EQ(e_u16strings[e::five], u"five");
	EXPECT_EQ(e_u16strings[e::count], u"count");

	EXPECT_EQ(e_u32strings[e::zero], U"zero");
	EXPECT_EQ(e_u32strings[e::one], U"one");
	EXPECT_EQ(e_u32strings[e::two], U"two");
	EXPECT_EQ(e_u32strings[e::three], U"three");
	EXPECT_EQ(e_u32strings[e::four], U"four");
	EXPECT_EQ(e_u32strings[e::five], U"five");
	EXPECT_EQ(e_u32strings[e::count], U"count");


	EXPECT_EQ(to_string(e::zero), "zero");
	EXPECT_EQ(to_string(e::one), "one");
	EXPECT_EQ(to_string(e::two), "two");
	EXPECT_EQ(to_string(e::three), "three");
	EXPECT_EQ(to_string(e::four), "four");
	EXPECT_EQ(to_string(e::five), "five");
	EXPECT_EQ(to_string(e::count), "count");

	EXPECT_EQ(to_wstring(e::zero), L"zero");
	EXPECT_EQ(to_wstring(e::one), L"one");
	EXPECT_EQ(to_wstring(e::two), L"two");
	EXPECT_EQ(to_wstring(e::three), L"three");
	EXPECT_EQ(to_wstring(e::four), L"four");
	EXPECT_EQ(to_wstring(e::five), L"five");
	EXPECT_EQ(to_wstring(e::count), L"count");

	EXPECT_EQ(to_u16string(e::zero), u"zero");
	EXPECT_EQ(to_u16string(e::one), u"one");
	EXPECT_EQ(to_u16string(e::two), u"two");
	EXPECT_EQ(to_u16string(e::three), u"three");
	EXPECT_EQ(to_u16string(e::four), u"four");
	EXPECT_EQ(to_u16string(e::five), u"five");
	EXPECT_EQ(to_u16string(e::count), u"count");

	EXPECT_EQ(to_u32string(e::zero), U"zero");
	EXPECT_EQ(to_u32string(e::one), U"one");
	EXPECT_EQ(to_u32string(e::two), U"two");
	EXPECT_EQ(to_u32string(e::three), U"three");
	EXPECT_EQ(to_u32string(e::four), U"four");
	EXPECT_EQ(to_u32string(e::five), U"five");
	EXPECT_EQ(to_u32string(e::count), U"count");


	EXPECT_EQ(to_string<e::zero>(), "zero");
	EXPECT_EQ(to_string<e::one>(), "one");
	EXPECT_EQ(to_string<e::two>(), "two");
	EXPECT_EQ(to_string<e::three>(), "three");
	EXPECT_EQ(to_string<e::four>(), "four");
	EXPECT_EQ(to_string<e::five>(), "five");
	EXPECT_EQ(to_string<e::count>(), "count");

	EXPECT_EQ(to_wstring<e::zero>(), L"zero");
	EXPECT_EQ(to_wstring<e::one>(), L"one");
	EXPECT_EQ(to_wstring<e::two>(), L"two");
	EXPECT_EQ(to_wstring<e::three>(), L"three");
	EXPECT_EQ(to_wstring<e::four>(), L"four");
	EXPECT_EQ(to_wstring<e::five>(), L"five");
	EXPECT_EQ(to_wstring<e::count>(), L"count");

	EXPECT_EQ(to_u16string<e::zero>(), std::u16string{ u"zero" });
	EXPECT_EQ(to_u16string<e::one>(), std::u16string{ u"one" });
	EXPECT_EQ(to_u16string<e::two>(), std::u16string{ u"two" });
	EXPECT_EQ(to_u16string<e::three>(), std::u16string{ u"three" });
	EXPECT_EQ(to_u16string<e::four>(), std::u16string{ u"four" });
	EXPECT_EQ(to_u16string<e::five>(), std::u16string{ u"five" });
	EXPECT_EQ(to_u16string<e::count>(), std::u16string{ u"count" });

	EXPECT_EQ(to_u32string<e::zero>(), std::u32string{ U"zero" });
	EXPECT_EQ(to_u32string<e::one>(), std::u32string{ U"one" });
	EXPECT_EQ(to_u32string<e::two>(), std::u32string{ U"two" });
	EXPECT_EQ(to_u32string<e::three>(), std::u32string{ U"three" });
	EXPECT_EQ(to_u32string<e::four>(), std::u32string{ U"four" });
	EXPECT_EQ(to_u32string<e::five>(), std::u32string{ U"five" });
	EXPECT_EQ(to_u32string<e::count>(), std::u32string{ U"count" });
}
} // namespace espace_all

namespace espace2 {
FEA_ALLSTRINGS_ENUM_WITH_COUNT(e, unsigned, zero, one, two, three, four, five)
TEST(enum_macros, basics) {
	EXPECT_EQ(size_t(e::zero), 0u);
	EXPECT_EQ(size_t(e::one), 1u);
	EXPECT_EQ(size_t(e::two), 2u);
	EXPECT_EQ(size_t(e::three), 3u);
	EXPECT_EQ(size_t(e::four), 4u);
	EXPECT_EQ(size_t(e::five), 5u);
	EXPECT_EQ(size_t(e::count), 6u);

	EXPECT_EQ(strings<e>(), e_strings);
	EXPECT_EQ(wstrings<e>(), e_wstrings);
	EXPECT_EQ(u16strings<e>(), e_u16strings);
	EXPECT_EQ(u32strings<e>(), e_u32strings);

	EXPECT_EQ(e_strings.size(), size_t(e::count));
	EXPECT_EQ(e_wstrings.size(), size_t(e::count));
	EXPECT_EQ(e_u16strings.size(), size_t(e::count));
	EXPECT_EQ(e_u32strings.size(), size_t(e::count));


	// Direct array access.
	EXPECT_EQ(e_strings[e::zero], "zero");
	EXPECT_EQ(e_strings[e::one], "one");
	EXPECT_EQ(e_strings[e::two], "two");
	EXPECT_EQ(e_strings[e::three], "three");
	EXPECT_EQ(e_strings[e::four], "four");
	EXPECT_EQ(e_strings[e::five], "five");

	EXPECT_EQ(e_wstrings[e::zero], L"zero");
	EXPECT_EQ(e_wstrings[e::one], L"one");
	EXPECT_EQ(e_wstrings[e::two], L"two");
	EXPECT_EQ(e_wstrings[e::three], L"three");
	EXPECT_EQ(e_wstrings[e::four], L"four");
	EXPECT_EQ(e_wstrings[e::five], L"five");

	EXPECT_EQ(e_u16strings[e::zero], u"zero");
	EXPECT_EQ(e_u16strings[e::one], u"one");
	EXPECT_EQ(e_u16strings[e::two], u"two");
	EXPECT_EQ(e_u16strings[e::three], u"three");
	EXPECT_EQ(e_u16strings[e::four], u"four");
	EXPECT_EQ(e_u16strings[e::five], u"five");

	EXPECT_EQ(e_u32strings[e::zero], U"zero");
	EXPECT_EQ(e_u32strings[e::one], U"one");
	EXPECT_EQ(e_u32strings[e::two], U"two");
	EXPECT_EQ(e_u32strings[e::three], U"three");
	EXPECT_EQ(e_u32strings[e::four], U"four");
	EXPECT_EQ(e_u32strings[e::five], U"five");


	// Runtime getter.
	EXPECT_EQ(to_string(e::zero), "zero");
	EXPECT_EQ(to_string(e::one), "one");
	EXPECT_EQ(to_string(e::two), "two");
	EXPECT_EQ(to_string(e::three), "three");
	EXPECT_EQ(to_string(e::four), "four");
	EXPECT_EQ(to_string(e::five), "five");

	EXPECT_EQ(to_wstring(e::zero), L"zero");
	EXPECT_EQ(to_wstring(e::one), L"one");
	EXPECT_EQ(to_wstring(e::two), L"two");
	EXPECT_EQ(to_wstring(e::three), L"three");
	EXPECT_EQ(to_wstring(e::four), L"four");
	EXPECT_EQ(to_wstring(e::five), L"five");

	EXPECT_EQ(to_u16string(e::zero), u"zero");
	EXPECT_EQ(to_u16string(e::one), u"one");
	EXPECT_EQ(to_u16string(e::two), u"two");
	EXPECT_EQ(to_u16string(e::three), u"three");
	EXPECT_EQ(to_u16string(e::four), u"four");
	EXPECT_EQ(to_u16string(e::five), u"five");

	EXPECT_EQ(to_u32string(e::zero), U"zero");
	EXPECT_EQ(to_u32string(e::one), U"one");
	EXPECT_EQ(to_u32string(e::two), U"two");
	EXPECT_EQ(to_u32string(e::three), U"three");
	EXPECT_EQ(to_u32string(e::four), U"four");
	EXPECT_EQ(to_u32string(e::five), U"five");


	// Non-type getter.
	EXPECT_EQ(to_string<e::zero>(), "zero");
	EXPECT_EQ(to_string<e::one>(), "one");
	EXPECT_EQ(to_string<e::two>(), "two");
	EXPECT_EQ(to_string<e::three>(), "three");
	EXPECT_EQ(to_string<e::four>(), "four");
	EXPECT_EQ(to_string<e::five>(), "five");

	EXPECT_EQ(to_wstring<e::zero>(), L"zero");
	EXPECT_EQ(to_wstring<e::one>(), L"one");
	EXPECT_EQ(to_wstring<e::two>(), L"two");
	EXPECT_EQ(to_wstring<e::three>(), L"three");
	EXPECT_EQ(to_wstring<e::four>(), L"four");
	EXPECT_EQ(to_wstring<e::five>(), L"five");

	EXPECT_EQ(to_u16string<e::zero>(), std::u16string{ u"zero" });
	EXPECT_EQ(to_u16string<e::one>(), std::u16string{ u"one" });
	EXPECT_EQ(to_u16string<e::two>(), std::u16string{ u"two" });
	EXPECT_EQ(to_u16string<e::three>(), std::u16string{ u"three" });
	EXPECT_EQ(to_u16string<e::four>(), std::u16string{ u"four" });
	EXPECT_EQ(to_u16string<e::five>(), std::u16string{ u"five" });

	EXPECT_EQ(to_u32string<e::zero>(), std::u32string{ U"zero" });
	EXPECT_EQ(to_u32string<e::one>(), std::u32string{ U"one" });
	EXPECT_EQ(to_u32string<e::two>(), std::u32string{ U"two" });
	EXPECT_EQ(to_u32string<e::three>(), std::u32string{ U"three" });
	EXPECT_EQ(to_u32string<e::four>(), std::u32string{ U"four" });
	EXPECT_EQ(to_u32string<e::five>(), std::u32string{ U"five" });
}
} // namespace espace2
} // namespace

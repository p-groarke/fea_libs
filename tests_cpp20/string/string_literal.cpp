#include <fea/string/string_literal.hpp>
#include <fea/utils/platform.hpp>
#include <gtest/gtest.h>

namespace {
#define TEST_FAIL_MSG "string_literal.cpp : Unit test failed."

template <fea::string_literal _lit>
struct test {
	static constexpr auto lit = _lit;
	static constexpr size_t hash = _lit.hash();
};
template <fea::wstring_literal _lit>
struct wtest {
	static constexpr auto lit = _lit;
	static constexpr size_t hash = _lit.hash();
};
template <fea::u8string_literal _lit>
struct u8test {
	static constexpr auto lit = _lit;
	static constexpr size_t hash = _lit.hash();
};
template <fea::u16string_literal _lit>
struct u16test {
	static constexpr auto lit = _lit;
	static constexpr size_t hash = _lit.hash();
};
template <fea::u32string_literal _lit>
struct u32test {
	static constexpr auto lit = _lit;
	static constexpr size_t hash = _lit.hash();
};

TEST(string_literal, basics) {
	// Answers, taking into account size of wchar_t and char8_t.
#if FEA_64BIT
	constexpr std::array<size_t, 5> answers{
		0u, // sizeof(T) == 0 invalid
		20863340834690462u,
		11903599094407814618u,
		0u,
		11576784882793927306u,
	};
#else
	constexpr std::array<size_t, 5> answers{
		0u, // sizeof(T) == 0 invalid
		1640245246u,
		879236538u,
		0u,
		1194068202u,
	};
#endif

	{
		constexpr test<"bla"> t;
		constexpr std::string_view t2{ "bla" };
		static_assert(t.lit.size() == 3, TEST_FAIL_MSG);
		static_assert(t.lit.sv() == t2, TEST_FAIL_MSG);
		static_assert(t.lit.hash() == answers[sizeof(char)], TEST_FAIL_MSG);

#if FEA_WINDOWS
		// Windows uses fnv1a.
		size_t std_hash = std::hash<std::string_view>{}(t2);
		EXPECT_EQ(t.hash, std_hash);
#endif
	}

	{
		constexpr wtest<L"bla"> t;
		constexpr std::wstring_view t2{ L"bla" };
		static_assert(t.lit.size() == 3, TEST_FAIL_MSG);
		static_assert(t.lit.sv() == t2, TEST_FAIL_MSG);
		static_assert(t.lit.hash() == answers[sizeof(wchar_t)], TEST_FAIL_MSG);

#if FEA_WINDOWS
		size_t std_hash = std::hash<std::wstring_view>{}(t2);
		EXPECT_EQ(t.hash, std_hash);
#endif
	}

	{
		constexpr u8test<u8"bla"> t;
		constexpr std::u8string_view t2{ u8"bla" };
		static_assert(t.lit.size() == 3, TEST_FAIL_MSG);
		static_assert(t.lit.sv() == t2, TEST_FAIL_MSG);
		static_assert(t.lit.hash() == answers[sizeof(char8_t)], TEST_FAIL_MSG);

#if FEA_WINDOWS
		size_t std_hash = std::hash<std::u8string_view>{}(t2);
		EXPECT_EQ(t.hash, std_hash);
#endif
	}

	{
		constexpr u16test<u"bla"> t;
		constexpr std::u16string_view t2{ u"bla" };
		static_assert(t.lit.size() == 3, TEST_FAIL_MSG);
		static_assert(t.lit.sv() == t2, TEST_FAIL_MSG);
		static_assert(t.lit.hash() == answers[sizeof(char16_t)], TEST_FAIL_MSG);

#if FEA_WINDOWS
		size_t std_hash = std::hash<std::u16string_view>{}(t2);
		EXPECT_EQ(t.hash, std_hash);
#endif
	}

	{
		constexpr u32test<U"bla"> t;
		constexpr std::u32string_view t2{ U"bla" };
		static_assert(t.lit.size() == 3, TEST_FAIL_MSG);
		static_assert(t.lit.sv() == t2, TEST_FAIL_MSG);
		static_assert(t.lit.hash() == answers[sizeof(char32_t)], TEST_FAIL_MSG);

#if FEA_WINDOWS
		size_t std_hash = std::hash<std::u32string_view>{}(t2);
		EXPECT_EQ(t.hash, std_hash);
#endif
	}
}

TEST(string_literal, cexpr_hash) {
#if FEA_64BIT
	constexpr std::array<size_t, 5> answers{
		0u, // sizeof(T) == 0 invalid
		1080464581000616832u,
		7933071513771995546u,
		0u,
		18137240241279183490u,
	};
#else
	constexpr std::array<size_t, 5> answers{
		0u, // sizeof(T) == 0 invalid
		2786919424u,
		123405690u,
		0u,
		310081570u,
	};
#endif

	{
		constexpr std::string_view sv = "a test";
		constexpr size_t hash = fea::cexpr_make_hash(sv);
		static_assert(hash == answers[sizeof(char)], TEST_FAIL_MSG);

#if FEA_WINDOWS
		size_t std_hash = std::hash<std::string_view>{}(sv);
		EXPECT_EQ(hash, std_hash);
#endif
	}
	{
		constexpr std::wstring_view sv = L"a test";
		constexpr size_t hash = fea::cexpr_make_hash(sv);
		static_assert(hash == answers[sizeof(wchar_t)], TEST_FAIL_MSG);

#if FEA_WINDOWS
		size_t std_hash = std::hash<std::wstring_view>{}(sv);
		EXPECT_EQ(hash, std_hash);
#endif
	}
	{
		constexpr std::u16string_view sv = u"a test";
		constexpr size_t hash = fea::cexpr_make_hash(sv);
		static_assert(hash == answers[sizeof(char16_t)], TEST_FAIL_MSG);

#if FEA_WINDOWS
		size_t std_hash = std::hash<std::u16string_view>{}(sv);
		EXPECT_EQ(hash, std_hash);
#endif
	}
	{
		constexpr std::u32string_view sv = U"a test";
		constexpr size_t hash = fea::cexpr_make_hash(sv);
		static_assert(hash == answers[sizeof(char32_t)], TEST_FAIL_MSG);

#if FEA_WINDOWS
		size_t std_hash = std::hash<std::u32string_view>{}(sv);
		EXPECT_EQ(hash, std_hash);
#endif
	}
	{
		constexpr std::u8string_view sv = u8"a test";
		constexpr size_t hash = fea::cexpr_make_hash(sv);
		static_assert(hash == answers[sizeof(char8_t)], TEST_FAIL_MSG);

#if FEA_WINDOWS
		size_t std_hash = std::hash<std::u8string_view>{}(sv);
		EXPECT_EQ(hash, std_hash);
#endif
	}

	// Null terminated vs. not.
	{
		constexpr const char c[5]{ 'h', 'e', 'l', 'l', 'o' };
		constexpr std::string_view sv = "hello";
		constexpr std::array<char, 5> arr{ 'h', 'e', 'l', 'l', 'o' };
		constexpr const char c2[6]{ 'h', 'e', 'l', 'l', 'o', '\0' };
		constexpr std::array<char, 6> arr2{ 'h', 'e', 'l', 'l', 'o', '\0' };
		constexpr size_t hash = fea::cexpr_make_hash(c);
		constexpr size_t hash2 = fea::cexpr_make_hash(sv);
		constexpr size_t hash3 = fea::cexpr_make_hash("hello");
		constexpr size_t hash4 = fea::cexpr_make_hash(arr);
		constexpr size_t hash5 = fea::cexpr_make_hash(c2);
		constexpr size_t hash6 = fea::cexpr_make_hash(arr2);
		// Simulate mistake.
		constexpr size_t hash5_miss
				= fea::cexpr_make_hash(std::string_view{ c2, 6 });
		constexpr size_t hash6_miss = fea::cexpr_make_hash(
				std::string_view{ arr2.data(), arr2.size() });

		static_assert(hash == hash2, TEST_FAIL_MSG);
		static_assert(hash == hash3, TEST_FAIL_MSG);
		static_assert(hash == hash4, TEST_FAIL_MSG);
		static_assert(hash == hash5, TEST_FAIL_MSG);
		static_assert(hash == hash6, TEST_FAIL_MSG);
		static_assert(hash == hash5_miss, TEST_FAIL_MSG);
		static_assert(hash == hash6_miss, TEST_FAIL_MSG);

#if FEA_WINDOWS
		size_t std_hash1
				= std::hash<std::string_view>{}(std::string_view{ c, 5 });
		EXPECT_EQ(hash, std_hash1);
		size_t std_hash2 = std::hash<std::string_view>{}(sv);
		EXPECT_EQ(hash, std_hash2);
		size_t std_hash3 = std::hash<std::string>{}("hello");
		EXPECT_EQ(hash, std_hash3);
		size_t std_hash4 = std::hash<std::string_view>{}(
				std::string_view{ arr.data(), arr.size() });
		EXPECT_EQ(hash, std_hash4);

		size_t std_hash5
				= std::hash<std::string_view>{}(std::string_view{ c2 });
		EXPECT_EQ(hash, std_hash5);
		size_t std_hash6 = std::hash<std::string_view>{}(
				std::string_view{ arr2.data() });
		EXPECT_EQ(hash, std_hash6);

		// std::hash disagrees and includes null character in hash.
		size_t std_hash5_miss
				= std::hash<std::string_view>{}(std::string_view{ c2, 6 });
		EXPECT_NE(hash, std_hash5_miss);
		size_t std_hash6_miss = std::hash<std::string_view>{}(
				std::string_view{ arr2.data(), arr2.size() });
		EXPECT_NE(hash, std_hash6_miss);
#endif
	}
}

TEST(string_literal, make) {
	constexpr std::string_view t = "test";
	constexpr auto test_sl = fea::make_string_literal<t.size()>(t);
	constexpr const char arr[5] = "test";
	static_assert(std::equal(test_sl.data, test_sl.data + test_sl.size(),
						  std::begin(arr)),
			TEST_FAIL_MSG);

	constexpr fea::string_literal sl = "test";
	static_assert(sl == test_sl, TEST_FAIL_MSG);
}

TEST(string_literal, equality) {
	constexpr fea::string_literal t1 = "test";
	constexpr fea::string_literal t2 = "test\0\0\0";
	static_assert(t1 == t2, TEST_FAIL_MSG);
	static_assert(!(t1 != t2), TEST_FAIL_MSG);

	constexpr fea::string_literal t3 = "Test\0\0\0";
	static_assert(t1 != t3, TEST_FAIL_MSG);
	static_assert(!(t1 == t3), TEST_FAIL_MSG);

	constexpr fea::string_literal t4 = "test";
	static_assert(t1 == t4, TEST_FAIL_MSG);
	static_assert(!(t1 != t4), TEST_FAIL_MSG);

	constexpr fea::string_literal t5 = "tesT";
	static_assert(t1 != t5, TEST_FAIL_MSG);
	static_assert(!(t1 == t5), TEST_FAIL_MSG);
}

TEST(string_literal, concat) {
	constexpr fea::string_literal sl1 = "string ";
	constexpr fea::string_literal sl2 = "literal ";
	constexpr fea::string_literal sl3 = "concat";

	constexpr fea::string_literal result = fea::cexpr_concat<sl1, sl2, sl3>();
	using result_t = std::decay_t<decltype(result)>;
	static_assert(
			std::is_same_v<result_t, fea::string_literal<22>>, TEST_FAIL_MSG);

	constexpr fea::string_literal expected = "string literal concat";
	static_assert(result == expected, TEST_FAIL_MSG);
}
} // namespace

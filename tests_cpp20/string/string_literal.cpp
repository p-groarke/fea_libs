#include <fea/string/string_literal.hpp>
#include <fea/utils/platform.hpp>
#include <gtest/gtest.h>

namespace {
#define ERROR_MSG "string_literal.cpp : Unit test failed."

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
		static_assert(t.lit.size() == 3, ERROR_MSG);
		static_assert(t.lit.sv() == t2, ERROR_MSG);
		static_assert(t.lit.hash() == answers[sizeof(char)], ERROR_MSG);

#if FEA_WINDOWS
		// Windows uses fnv1a.
		size_t std_hash = std::hash<std::string_view>{}(t2);
		EXPECT_EQ(t.hash, std_hash);
#endif
	}

	{
		constexpr wtest<L"bla"> t;
		constexpr std::wstring_view t2{ L"bla" };
		static_assert(t.lit.size() == 3, ERROR_MSG);
		static_assert(t.lit.sv() == t2, ERROR_MSG);
		static_assert(t.lit.hash() == answers[sizeof(wchar_t)], ERROR_MSG);

#if FEA_WINDOWS
		size_t std_hash = std::hash<std::wstring_view>{}(t2);
		EXPECT_EQ(t.hash, std_hash);
#endif
	}

	{
		constexpr u8test<u8"bla"> t;
		constexpr std::u8string_view t2{ u8"bla" };
		static_assert(t.lit.size() == 3, ERROR_MSG);
		static_assert(t.lit.sv() == t2, ERROR_MSG);
		static_assert(t.lit.hash() == answers[sizeof(char8_t)], ERROR_MSG);

#if FEA_WINDOWS
		size_t std_hash = std::hash<std::u8string_view>{}(t2);
		EXPECT_EQ(t.hash, std_hash);
#endif
	}

	{
		constexpr u16test<u"bla"> t;
		constexpr std::u16string_view t2{ u"bla" };
		static_assert(t.lit.size() == 3, ERROR_MSG);
		static_assert(t.lit.sv() == t2, ERROR_MSG);
		static_assert(t.lit.hash() == answers[sizeof(char16_t)], ERROR_MSG);

#if FEA_WINDOWS
		size_t std_hash = std::hash<std::u16string_view>{}(t2);
		EXPECT_EQ(t.hash, std_hash);
#endif
	}

	{
		constexpr u32test<U"bla"> t;
		constexpr std::u32string_view t2{ U"bla" };
		static_assert(t.lit.size() == 3, ERROR_MSG);
		static_assert(t.lit.sv() == t2, ERROR_MSG);
		static_assert(t.lit.hash() == answers[sizeof(char32_t)], ERROR_MSG);

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
		0u,
		0u,
		0u,
		0u,
	};
#endif

	{
		constexpr std::string_view sv = "a test";
		constexpr size_t hash = fea::make_cexpr_hash(sv);
		static_assert(hash == answers[sizeof(char)], ERROR_MSG);

#if FEA_WINDOWS
		size_t std_hash = std::hash<std::string_view>{}(sv);
		EXPECT_EQ(hash, std_hash);
#endif
	}
	{
		constexpr std::wstring_view sv = L"a test";
		constexpr size_t hash = fea::make_cexpr_hash(sv);
		static_assert(hash == answers[sizeof(wchar_t)], ERROR_MSG);

#if FEA_WINDOWS
		size_t std_hash = std::hash<std::wstring_view>{}(sv);
		EXPECT_EQ(hash, std_hash);
#endif
	}
	{
		constexpr std::u16string_view sv = u"a test";
		constexpr size_t hash = fea::make_cexpr_hash(sv);
		static_assert(hash == answers[sizeof(char16_t)], ERROR_MSG);

#if FEA_WINDOWS
		size_t std_hash = std::hash<std::u16string_view>{}(sv);
		EXPECT_EQ(hash, std_hash);
#endif
	}
	{
		constexpr std::u32string_view sv = U"a test";
		constexpr size_t hash = fea::make_cexpr_hash(sv);
		static_assert(hash == answers[sizeof(char32_t)], ERROR_MSG);

#if FEA_WINDOWS
		size_t std_hash = std::hash<std::u32string_view>{}(sv);
		EXPECT_EQ(hash, std_hash);
#endif
	}
	{
		constexpr std::u8string_view sv = u8"a test";
		constexpr size_t hash = fea::make_cexpr_hash(sv);
		static_assert(hash == answers[sizeof(char8_t)], ERROR_MSG);

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
		constexpr size_t hash = fea::make_cexpr_hash(c);
		constexpr size_t hash2 = fea::make_cexpr_hash(sv);
		constexpr size_t hash3 = fea::make_cexpr_hash("hello");
		constexpr size_t hash4 = fea::make_cexpr_hash(arr);
		constexpr size_t hash5 = fea::make_cexpr_hash(c2);
		constexpr size_t hash6 = fea::make_cexpr_hash(arr2);
		// Simulate mistake.
		constexpr size_t hash5_miss
				= fea::make_cexpr_hash(std::string_view{ c2, 6 });
		constexpr size_t hash6_miss = fea::make_cexpr_hash(
				std::string_view{ arr2.data(), arr2.size() });

		static_assert(hash == hash2, ERROR_MSG);
		static_assert(hash == hash3, ERROR_MSG);
		static_assert(hash == hash4, ERROR_MSG);
		static_assert(hash == hash5, ERROR_MSG);
		static_assert(hash == hash6, ERROR_MSG);
		static_assert(hash == hash5_miss, ERROR_MSG);
		static_assert(hash == hash6_miss, ERROR_MSG);

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
} // namespace

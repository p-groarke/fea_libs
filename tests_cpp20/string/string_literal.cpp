#include <fea/string/string_literal.hpp>
#include <gtest/gtest.h>

namespace {
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
	{
		constexpr test<"bla"> t;
		constexpr std::string_view t2{ "bla" };
		static_assert(t.lit.size() == 3, "string_literal.cpp : test failed");
		static_assert(t.lit.sv() == t2, "string_literal.cpp : test failed");

#if FEA_64BIT
		static_assert(t.lit.hash() == 20863340834690462u,
				"string_literal.cpp : test failed");
#else
		static_assert(t.lit.hash() == 1640245246u,
				"string_literal.cpp : test failed");
#endif
	}

	{
		constexpr wtest<L"bla"> t;
		constexpr std::wstring_view t2{ L"bla" };
		static_assert(t.lit.size() == 3, "string_literal.cpp : test failed");
		static_assert(t.lit.sv() == t2, "string_literal.cpp : test failed");

#if FEA_WINDOWS
		// wchar_t 2 bytes on windows
#if FEA_64BIT
		static_assert(t.lit.hash() == 11903599094407814618u,
				"string_literal.cpp : test failed");
#else
		static_assert(
				t.lit.hash() == 879236538u, "string_literal.cpp : test failed");
#endif
#else
		// wchar_t 4 bytes on gcc/clang
#if FEA_64BIT
		static_assert(t.lit.hash() == 11576784882793927306u,
				"string_literal.cpp : test failed");
#else
		static_assert(t.lit.hash() == 1194068202u,
				"string_literal.cpp : test failed");
#endif
#endif
	}

	{
		constexpr u8test<u8"bla"> t;
		constexpr std::u8string_view t2{ u8"bla" };
		static_assert(t.lit.size() == 3, "string_literal.cpp : test failed");
		static_assert(t.lit.sv() == t2, "string_literal.cpp : test failed");

#if FEA_64BIT
		static_assert(t.lit.hash() == 20863340834690462u,
				"string_literal.cpp : test failed");
#else
		static_assert(t.lit.hash() == 1640245246u,
				"string_literal.cpp : test failed");
#endif
	}

	{
		constexpr u16test<u"bla"> t;
		constexpr std::u16string_view t2{ u"bla" };
		static_assert(t.lit.size() == 3, "string_literal.cpp : test failed");
		static_assert(t.lit.sv() == t2, "string_literal.cpp : test failed");

#if FEA_64BIT
		static_assert(t.lit.hash() == 11903599094407814618u,
				"string_literal.cpp : test failed");
#else
		static_assert(
				t.lit.hash() == 879236538u, "string_literal.cpp : test failed");
#endif
	}

	{
		constexpr u32test<U"bla"> t;
		constexpr std::u32string_view t2{ U"bla" };
		static_assert(t.lit.size() == 3, "string_literal.cpp : test failed");
		static_assert(t.lit.sv() == t2, "string_literal.cpp : test failed");

#if FEA_64BIT
		static_assert(t.lit.hash() == 11576784882793927306u,
				"string_literal.cpp : test failed");
#else
		static_assert(t.lit.hash() == 1194068202u,
				"string_literal.cpp : test failed");
#endif
	}
}
} // namespace

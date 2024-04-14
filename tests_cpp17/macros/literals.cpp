#include <array>
#include <fea/macros/literals.hpp>
#include <fea/string/conversions.hpp>
#include <gtest/gtest.h>

#define testme(...) \
	std::array<std::string, 6> arr \
			= { FEA_FOR_EACH(FEA_STRINGIFY_COMMA, __VA_ARGS__) }

namespace {
TEST(macros_literals, basics) {
	EXPECT_STREQ(FEA_STRINGIFY(test), "test");
	EXPECT_STREQ(FEA_WSTRINGIFY(test), L"test");
	EXPECT_EQ(std::u16string{ FEA_U16STRINGIFY(test) }, u"test");
	EXPECT_EQ(std::u32string{ FEA_U32STRINGIFY(test) }, U"test");
#if FEA_CPP20
	EXPECT_EQ(std::u8string{ FEA_U8STRINGIFY(test) }, u8"test");
#endif

	[](std::string str1, std::string str2) { EXPECT_EQ(str1, str2); }(
			FEA_STRINGIFY_COMMA(test) "test");
	[](std::wstring str1, std::wstring str2) { EXPECT_EQ(str1, str2); }(
			FEA_WSTRINGIFY_COMMA(test) L"test");
	[](std::u16string str1, std::u16string str2) { EXPECT_EQ(str1, str2); }(
			FEA_U16STRINGIFY_COMMA(test) u"test");
	[](std::u32string str1, std::u32string str2) { EXPECT_EQ(str1, str2); }(
			FEA_U32STRINGIFY_COMMA(test) U"test");
#if FEA_CPP20
	[](std::u8string str1, std::u8string str2) { EXPECT_EQ(str1, str2); }(
			FEA_U8STRINGIFY_COMMA(test) u8"test");
#endif

	EXPECT_EQ(FEA_SIZEOF_VAARGS(test, test, test), 3);
	EXPECT_EQ(FEA_SIZEOF_VAARGS(test, test, test, test, test, test), 6);

	{
		testme(0, 1, 2, 3, 4, 5);

		for (size_t i = 0; i < arr.size(); ++i) {
			EXPECT_EQ(arr[i], std::to_string(i));
		}
	}
}

TEST(macros_literals, prefix) {
	// If this compiles the literals are well formed.
	std::string t1{ FEA_PREFIX("test") };
	std::wstring t2{ FEA_WPREFIX("test") };
	std::u16string t3{ FEA_U16PREFIX("test") };
	std::u32string t4{ FEA_U32PREFIX("test") };
#if FEA_CPP20
	std::u8string t5{ FEA_U8PREFIX("test") };
#endif

	EXPECT_EQ("test", t1);
	EXPECT_EQ(L"test", t2);
	EXPECT_EQ(u"test", t3);
	EXPECT_EQ(U"test", t4);
#if FEA_CPP20
	EXPECT_EQ(u8"test", t5);
#endif

	std::array<const char*, 4> arr1{ FEA_VA_PREFIX("0", "1", "2", "3") };
	std::array<const wchar_t*, 4> arr2{ FEA_VA_WPREFIX("0", "1", "2", "3") };
	std::array<const char16_t*, 4> arr3{ FEA_VA_U16PREFIX("0", "1", "2", "3") };
	std::array<const char32_t*, 4> arr4{ FEA_VA_U32PREFIX("0", "1", "2", "3") };
#if FEA_CPP20
	std::array<const char8_t*, 4> arr5{ FEA_VA_U8PREFIX("0", "1", "2", "3") };
#endif

	for (size_t i = 0; i < arr1.size(); ++i) {
		EXPECT_EQ(std::to_string(i), arr1[i]);
		EXPECT_EQ(std::to_wstring(i), arr2[i]);
		EXPECT_EQ(fea::utf8_to_any<char16_t>(std::to_string(i)), arr3[i]);
		EXPECT_EQ(fea::any_to_utf32(std::to_string(i)), arr4[i]);
#if FEA_CPP20
		EXPECT_EQ(fea::utf8_to_any<char8_t>(std::to_string(i)), arr5[i]);
#endif
	}
}
} // namespace

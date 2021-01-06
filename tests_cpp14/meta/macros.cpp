#include <array>
#include <fea/meta/macros.hpp>
#include <gtest/gtest.h>
#include <string>
#include <utility>


#define testme(...) \
	std::array<std::string, 6> arr \
			= { FEA_FOR_EACH(FEA_STRINGIFY_COMMA, __VA_ARGS__) }

namespace {
TEST(macros, basics) {

	EXPECT_STREQ(FEA_STRINGIFY(test), "test");
	EXPECT_STREQ(FEA_WSTRINGIFY(test), L"test");
	EXPECT_EQ(std::u16string{ FEA_U16STRINGIFY(test) }, u"test");
	EXPECT_EQ(std::u32string{ FEA_U32STRINGIFY(test) }, U"test");

	[](std::string str1, std::string str2) { EXPECT_EQ(str1, str2); }(
			FEA_STRINGIFY_COMMA(test) "test");
	[](std::wstring str1, std::wstring str2) { EXPECT_EQ(str1, str2); }(
			FEA_WSTRINGIFY_COMMA(test) L"test");
	[](std::u16string str1, std::u16string str2) { EXPECT_EQ(str1, str2); }(
			FEA_U16STRINGIFY_COMMA(test) u"test");
	[](std::u32string str1, std::u32string str2) { EXPECT_EQ(str1, str2); }(
			FEA_U32STRINGIFY_COMMA(test) U"test");

	EXPECT_EQ(FEA_SIZEOF_VAARGS(test, test, test), 3);
	EXPECT_EQ(FEA_SIZEOF_VAARGS(test, test, test, test, test, test), 6);

	testme(0, 1, 2, 3, 4, 5);

	for (size_t i = 0; i < arr.size(); ++i) {
		EXPECT_EQ(arr[i], std::to_string(i));
	}
}
} // namespace

#include <array>
#include <fea/macros/foreach.hpp>
#include <fea/macros/literals.hpp>
#include <fea/macros/macros.hpp>
#include <fea/utils/unused.hpp>
#include <gtest/gtest.h>
#include <string>
#include <utility>


#define testme(...) \
	std::array<std::string, 6> arr \
			= { FEA_FOR_EACH(FEA_STRINGIFY_COMMA, __VA_ARGS__) }

#define num(x) x,
#define num_last(x) x

#define nums_va(x, ...) std::array<int, 4> FEA_PASTE(arr, x){ __VA_ARGS__ };

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

	{
		testme(0, 1, 2, 3, 4, 5);

		for (size_t i = 0; i < arr.size(); ++i) {
			EXPECT_EQ(arr[i], std::to_string(i));
		}
	}

	{
		std::array<int, 4> arr{ FEA_FOR_EACH(num, 0, 1, 2, 3) };
		for (int i = 0; i < int(arr.size()); ++i) {
			EXPECT_EQ(arr[i], i);
		}
	}

	{
		FEA_FOR_EACH_VA(nums_va, 0, 1, 2, 3)
		for (int i = 0; i < 4; ++i) {
			EXPECT_EQ(arr0[i], i);
			EXPECT_EQ(arr1[i], i);
			EXPECT_EQ(arr2[i], i);
			EXPECT_EQ(arr3[i], i);
		}
	}

	{
		FEA_FOR_EACH_PARAM(nums_va, FEA_VA_LIST(0, 1, 2, 3), 0, 1, 2, 3)
		for (int i = 0; i < 4; ++i) {
			EXPECT_EQ(arr0[i], i);
			EXPECT_EQ(arr1[i], i);
			EXPECT_EQ(arr2[i], i);
			EXPECT_EQ(arr3[i], i);
		}
	}

	{
		int FEA_FOR_EACH_LAST(num, num_last, i0, i1, i2, i3);
		i0 = 0;
		i1 = 0;
		i2 = 0;
		i3 = 0;
		fea::unused(i0, i1, i2, i3);
	}
}
} // namespace

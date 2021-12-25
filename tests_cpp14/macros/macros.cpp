#include <array>
#include <fea/macros/foreach.hpp>
#include <fea/macros/macros.hpp>
#include <fea/utils/unused.hpp>
#include <gtest/gtest.h>
#include <string>
#include <utility>


#define num(x) x,
#define num_last(x) x

#define nums_va(x, ...) std::array<int, 4> FEA_PASTE(arr, x){ __VA_ARGS__ };

namespace {
TEST(macros, basics) {

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

#include <algorithm>
#include <fea/algorithm/sort.hpp>
#include <fea/numerics/random.hpp>
#include <gtest/gtest.h>
#include <vector>

namespace {

TEST(sort, basics) {
	{
		std::vector<uint8_t> vals{ 54, 18, 2, 128, 3 };
		std::vector<uint8_t> cmp = vals;

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);
	}

	{
		std::vector<size_t> vals{ 0, 54, 100'000, 18, 100'042, 0, 2, 128, 3, 0,
			128, 100'000, 3, 54, 54, 54, 0, 100'042, 100'042, 1, 100'000 };
		std::vector<size_t> cmp = vals;

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);
	}

	{
		std::vector<uint8_t> vals(42);
		fea::random_fill(vals.begin(), vals.end());
		std::vector<uint8_t> cmp = vals;

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);
	}

	{
		std::vector<size_t> vals(1000);
		fea::random_fill(vals.begin(), vals.end());
		std::vector<size_t> cmp = vals;

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);
	}
}
} // namespace
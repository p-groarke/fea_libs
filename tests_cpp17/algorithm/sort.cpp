#include <algorithm>
#include <fea/algorithm/sort.hpp>
#include <fea/numerics/random.hpp>
#include <gtest/gtest.h>
#include <vector>

namespace {

TEST(sort, basics) {
	// Simplest.
	{
		using t = uint8_t;
		std::vector<t> vals{ 54, 18, 2, 128, 3 };
		std::vector<t> cmp = vals;

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);
	}

	// Test passes.
	{
		using t = size_t;
		std::vector<t> vals{ 0, 54, 100'000, 18, 100'042, 0, 2, 128, 3, 0, 128,
			100'000, 3, 54, 54, 54, 0, 100'042, 100'042, 1, 100'000 };
		std::vector<t> cmp = vals;

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);
	}

	// Simple fuzz.
	{
		using t = uint8_t;
		std::vector<t> vals(42);
		fea::random_fill(vals.begin(), vals.end());
		std::vector<t> cmp = vals;

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);
	}

	// Simple fuzz.
	{
		using t = size_t;
		std::vector<t> vals(1000);
		fea::random_fill(vals.begin(), vals.end());
		std::vector<t> cmp = vals;

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);
	}

	// High collision count fuzz.
	{
		using t = uint16_t;
		std::vector<t> vals(1000);
		fea::random_fill(vals.begin(), vals.end(), 0, 5);
		std::vector<t> cmp = vals;

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);
	}

	// Test the caches are reset.
	{
		using t = uint16_t;
		std::vector<t> vals(1000);
		fea::random_fill(vals.begin(), vals.end(), 0, 5);
		std::vector<t> cmp = vals;

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);
	}
}

// TEST(sort, signedints) {
//	// Simplest.
//	{
//		using t = char;
//		std::vector<t> vals{ -54, 18, 2, 127, -3 };
//		std::vector<t> cmp = vals;
//
//		fea::radix_sort(vals.begin(), vals.end());
//		std::sort(cmp.begin(), cmp.end());
//		EXPECT_EQ(vals, cmp);
//	}
//
////	// Test passes.
////	{
////		using t = int64_t;
////		std::vector<t> vals{ 0, 54, 100'000, 18, 100'042, 0, 2, 128, 3, 0,
///128, /			100'000, 3, 54, 54, 54, 0, 100'042, 100'042, 1, 100'000 };
////		std::vector<t> cmp = vals;
////
////		fea::radix_sort(vals.begin(), vals.end());
////		std::sort(cmp.begin(), cmp.end());
////		EXPECT_EQ(vals, cmp);
////	}
////
////	// Simple fuzz.
////	{
////		using t = int8_t;
////		std::vector<t> vals(42);
////		fea::random_fill(vals.begin(), vals.end());
////		std::vector<t> cmp = vals;
////
////		fea::radix_sort(vals.begin(), vals.end());
////		std::sort(cmp.begin(), cmp.end());
////		EXPECT_EQ(vals, cmp);
////	}
////
////	// Simple fuzz.
////	{
////		using t = int64_t;
////		std::vector<t> vals(1000);
////		fea::random_fill(vals.begin(), vals.end());
////		std::vector<t> cmp = vals;
////
////		fea::radix_sort(vals.begin(), vals.end());
////		std::sort(cmp.begin(), cmp.end());
////		EXPECT_EQ(vals, cmp);
////	}
////
////	// High collision count fuzz.
////	{
////		using t = int16_t;
////		std::vector<t> vals(1000);
////		fea::random_fill(vals.begin(), vals.end(), 0, 5);
////		std::vector<t> cmp = vals;
////
////		fea::radix_sort(vals.begin(), vals.end());
////		std::sort(cmp.begin(), cmp.end());
////		EXPECT_EQ(vals, cmp);
////	}
////
////	// Test the caches are reset.
////	{
////		using t = int16_t;
////		std::vector<t> vals(1000);
////		fea::random_fill(vals.begin(), vals.end(), 0, 5);
////		std::vector<t> cmp = vals;
////
////		fea::radix_sort(vals.begin(), vals.end());
////		std::sort(cmp.begin(), cmp.end());
////		EXPECT_EQ(vals, cmp);
////	}
//}
} // namespace
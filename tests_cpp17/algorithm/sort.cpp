#include <algorithm>
#include <fea/algorithm/sort.hpp>
#include <fea/benchmark/benchmark.hpp>
#include <fea/numerics/random.hpp>
#include <fea/utility/platform.hpp>
#include <gtest/gtest.h>
#include <vector>

namespace {

TEST(sort, radix_basics) {
	// Simplest.
	{
		using t = uint8_t;
		std::vector<t> vals{ 54, 18, 2, 128, 3 };
		std::vector<t> cmp = vals;

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);
	}

	// Pre-sorted.
	{
		using t = uint8_t;
		std::vector<t> vals{ 2, 3, 18, 54, 128 };
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

TEST(sort, radix_signed_ints) {
	// Simplest.
	{
		using t = char;
		std::vector<t> vals{ -101, -54, 18, 2, 127, -3, -2 };
		std::vector<t> cmp = vals;

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);
	}

	// Pre-sorted
	{
		using t = char;
		std::vector<t> vals{ -101, -54, -2, -3, 2, 18, 127 };
		std::vector<t> cmp = vals;

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);
	}

	// Test passes.
	{
#if FEA_32BIT
		using t = int32_t;
#else
		using t = int64_t;
#endif
		std::vector<t> vals{ 0, 54, -128, -100'000, 18, -100'042, 0, 2, 100'042,
			128, 3, 0, 128, -128, 100'000, 3, 54, 54, 54, -128, 100'042, -54,
			-54, -54, 0, 100'042, -128, 100'000, -100'042, 1, -128, -100'000 };
		std::vector<t> cmp = vals;

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);
	}

	// Simple fuzz.
	{
		using t = int8_t;
		std::vector<t> vals(42);
		fea::random_fill(vals.begin(), vals.end());
		std::vector<t> cmp = vals;

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);
	}

	// Simple fuzz.
	{
#if FEA_32BIT
		using t = int32_t;
#else
		using t = int64_t;
#endif
		std::vector<t> vals(1000);
		fea::random_fill(vals.begin(), vals.end());
		std::vector<t> cmp = vals;

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);
	}

	// High collision count fuzz.
	{
		using t = int16_t;
		std::vector<t> vals(1000);
		fea::random_fill(vals.begin(), vals.end(), -5, 5);
		std::vector<t> cmp = vals;

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);
	}

	// Test the caches are reset.
	{
		using t = int16_t;
		std::vector<t> vals(1000);
		fea::random_fill(vals.begin(), vals.end(), -5, 5);
		std::vector<t> cmp = vals;

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);
	}
}

TEST(sort, radix_floats) {
	// Simplest.
	{
		using t = float;
		std::vector<t> vals{ -101.f, -54.f, 18.f, 2.f, 127.f, -3.f, -2.f };
		std::vector<t> cmp = vals;

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);
	}

	// Pre-sorted
	{
#if FEA_32BIT
		using t = float;
#else
		using t = double;
#endif
		std::vector<t> vals{ -101.0, -54.0, -2.0, -3.0, 2.0, 18.0, 127.0 };
		std::vector<t> cmp = vals;

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);
	}

	// Test passes.
	{
		using t = float;
		std::vector<t> vals{ 0.f, 54.f, -128.f, -100'000.f, 18.f, -100'042.f,
			0.f, 2.f, 100'042.f, 128.f, 3.f, 0.f, 128.f, -128.f, 100'000.f, 3.f,
			54.f, 54.f, 54.f, -128.f, 100'042.f, -54.f, -54.f, -54.f, 0.f,
			100'042.f, -128.f, 100'000.f, -100'042.f, 1.f, -128.f, -100'000.f };
		std::vector<t> cmp = vals;

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);
	}

	// Simple fuzz.
	{
#if FEA_32BIT
		using t = float;
#else
		using t = double;
#endif
		std::vector<t> vals(42);
		fea::random_fill(vals.begin(), vals.end());
		std::vector<t> cmp = vals;

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);
	}

	// Simple fuzz.
	{
#if FEA_32BIT
		using t = float;
#else
		using t = double;
#endif
		std::vector<t> vals(1000);
		fea::random_fill(vals.begin(), vals.end());
		std::vector<t> cmp = vals;

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);
	}

	// High collision count fuzz.
	{
		using t = float;
		std::vector<t> vals(1000);
		fea::random_fill(vals.begin(), vals.end(), -1.f, 1.f);
		std::vector<t> cmp = vals;

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);
	}

	// Test the caches are reset.
	{
		using t = float;
		std::vector<t> vals(1000);
		fea::random_fill(vals.begin(), vals.end(), -1.f, 1.f);
		std::vector<t> cmp = vals;

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);
	}
}

#if 0 && FEA_RELEASE
TEST(sort, radix_benchmarks) {
	using t = float;
	std::vector<t> vals(100'000'000);
	fea::random_fill(vals.begin(), vals.end(), -1000.f, 1000.f);

	fea::bench::suite suite;
	suite.title("Radix Sort Validation");
	suite.average(5);
	// suite.sleep_between(std::chrono::milliseconds{ 500 });
	suite.benchmark("The Big Short",
			[&]() { fea::radix_sort(vals.begin(), vals.end()); });

	fea::random_fill(vals.begin(), vals.end(), -1000.f, 1000.f);
	suite.benchmark("Second run",
			[&]() { fea::radix_sort(vals.begin(), vals.end()); });

	suite.print();
}
#endif

} // namespace
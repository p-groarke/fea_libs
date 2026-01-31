#include <algorithm>
#include <fea/algorithm/sort.hpp>
#include <fea/benchmark/benchmark.hpp>
#include <fea/numerics/random.hpp>
#include <fea/utility/error.hpp>
#include <fea/utility/platform.hpp>
#include <gtest/gtest.h>
#include <numeric>
#include <vector>

namespace {
// Sort input values using sorted indexes.
template <class IdxT, class T>
void sort_vals(const std::vector<IdxT>& idx_vec, std::vector<T>* val_vec_ptr) {
	auto& val_vec = *val_vec_ptr;
	assert(idx_vec.size() == val_vec.size());
	auto cpy = val_vec;
	for (size_t i = 0; i < idx_vec.size(); ++i) {
		val_vec[i] = cpy[size_t(idx_vec[i])];
	}
}

// Dev test, index sort is sorted with everything else.
TEST(sort, radix_idxes) {
	// Simplest.
	{
		using t = uint8_t;
		const std::vector<t> in{ 54, 18, 2, 128, 3 };
		std::vector<t> vals = in;
		std::vector<t> cmp = vals;
		std::vector<size_t> idxes(vals.size());
		std::iota(idxes.begin(), idxes.end(), 0);

		fea::radix_sort_idxes(
				vals.begin(), vals.end(), idxes.begin(), idxes.end());
		EXPECT_EQ(vals, cmp);

		std::sort(cmp.begin(), cmp.end());

		ASSERT_EQ(vals.size(), idxes.size());
		for (size_t i = 0; i < idxes.size(); ++i) {
			// Deref at sorted index the answer, and check its the same value.
			EXPECT_EQ(vals[idxes[i]], cmp[i]);
		}

		sort_vals(idxes, &vals);
		EXPECT_EQ(vals, cmp);
	}
	{
		using t = uint32_t;
		const std::vector<t> in{ 54, 18, 2, 128, 3 };
		std::vector<t> vals = in;
		std::vector<t> cmp = vals;
		std::vector<size_t> idxes(vals.size());
		std::iota(idxes.begin(), idxes.end(), 0);

		fea::radix_sort_idxes(
				vals.begin(), vals.end(), idxes.begin(), idxes.end());
		EXPECT_EQ(vals, cmp);
		std::sort(cmp.begin(), cmp.end());
		sort_vals(idxes, &vals);
		EXPECT_EQ(vals, cmp);
	}
	{
		using t = int;
		const std::vector<t> in{ -101, -54, 18, 2, 127, -3, -2 };
		std::vector<t> vals = in;
		std::vector<t> cmp = vals;
		std::vector<size_t> idxes(vals.size());
		std::iota(idxes.begin(), idxes.end(), 0);

		fea::radix_sort_idxes(
				vals.begin(), vals.end(), idxes.begin(), idxes.end());
		EXPECT_EQ(vals, cmp);
		std::sort(cmp.begin(), cmp.end());
		sort_vals(idxes, &vals);
		EXPECT_EQ(vals, cmp);
	}
	{
		using t = float;
		const std::vector<t> in{ -101.f, -54.f, 18.f, 2.f, 127.f, -3.f, -2.f };
		std::vector<t> vals = in;
		std::vector<t> cmp = vals;
		std::vector<size_t> idxes(vals.size());
		std::iota(idxes.begin(), idxes.end(), 0);

		fea::radix_sort_idxes(
				vals.begin(), vals.end(), idxes.begin(), idxes.end());
		EXPECT_EQ(vals, cmp);
		std::sort(cmp.begin(), cmp.end());
		sort_vals(idxes, &vals);
		EXPECT_EQ(vals, cmp);
	}
}

TEST(sort, radix_basics) {
	// Simplest.
	{
		using t = uint8_t;
		const std::vector<t> in{ 54, 18, 2, 128, 3 };
		std::vector<t> vals = in;
		std::vector<t> vals2 = in;
		std::vector<t> cmp = in;
		std::vector<t> cmp2 = in;
		std::vector<size_t> idxes(in.size());
		std::iota(idxes.begin(), idxes.end(), 0);

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);

		fea::radix_sort_idxes(
				vals2.begin(), vals2.end(), idxes.begin(), idxes.end());
		EXPECT_EQ(vals2, cmp2);
		std::sort(cmp2.begin(), cmp2.end());
		sort_vals(idxes, &vals2);
		EXPECT_EQ(vals2, cmp2);
	}

	// Multipass.
	{
		using t = uint32_t;
		const std::vector<t> in{ 54, 18, 2, 128, 3 };
		std::vector<t> vals = in;
		std::vector<t> vals2 = in;
		std::vector<t> cmp = in;
		std::vector<t> cmp2 = in;
		std::vector<size_t> idxes(in.size());
		std::iota(idxes.begin(), idxes.end(), 0);

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);

		fea::radix_sort_idxes(
				vals2.begin(), vals2.end(), idxes.begin(), idxes.end());
		EXPECT_EQ(vals2, cmp2);
		std::sort(cmp2.begin(), cmp2.end());
		sort_vals(idxes, &vals2);
		EXPECT_EQ(vals2, cmp2);
	}

	// Pre-sorted.
	{
		using t = uint8_t;
		const std::vector<t> in{ 2, 3, 18, 54, 128 };
		std::vector<t> vals = in;
		std::vector<t> vals2 = in;
		std::vector<t> cmp = in;
		std::vector<t> cmp2 = in;
		std::vector<size_t> idxes(in.size());
		std::iota(idxes.begin(), idxes.end(), 0);

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);

		fea::radix_sort_idxes(
				vals2.begin(), vals2.end(), idxes.begin(), idxes.end());
		EXPECT_EQ(vals2, cmp2);
		std::sort(cmp2.begin(), cmp2.end());
		sort_vals(idxes, &vals2);
		EXPECT_EQ(vals2, cmp2);
	}

	// Test passes.
	{
		using t = size_t;
		const std::vector<t> in{ 0, 54, 100'000, 18, 100'042, 0, 2, 128, 3, 0,
			128, 100'000, 3, 54, 54, 54, 0, 100'042, 100'042, 1, 100'000 };
		std::vector<t> vals = in;
		std::vector<t> vals2 = in;
		std::vector<t> cmp = in;
		std::vector<t> cmp2 = in;
		std::vector<size_t> idxes(in.size());
		std::iota(idxes.begin(), idxes.end(), 0);

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);

		fea::radix_sort_idxes(
				vals2.begin(), vals2.end(), idxes.begin(), idxes.end());
		EXPECT_EQ(vals2, cmp2);
		std::sort(cmp2.begin(), cmp2.end());
		sort_vals(idxes, &vals2);
		EXPECT_EQ(vals2, cmp2);
	}

	// Simple fuzz.
	{
		using t = uint8_t;
		std::vector<t> in(42);
		fea::random_fill(in.begin(), in.end());
		std::vector<t> vals = in;
		std::vector<t> vals2 = in;
		std::vector<t> cmp = in;
		std::vector<t> cmp2 = in;
		std::vector<size_t> idxes(in.size());
		std::iota(idxes.begin(), idxes.end(), 0);

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);

		fea::radix_sort_idxes(
				vals2.begin(), vals2.end(), idxes.begin(), idxes.end());
		EXPECT_EQ(vals2, cmp2);
		std::sort(cmp2.begin(), cmp2.end());
		sort_vals(idxes, &vals2);
		EXPECT_EQ(vals2, cmp2);
	}

	// Simple fuzz.
	{
		using t = size_t;
		std::vector<t> in(1000);
		fea::random_fill(in.begin(), in.end());
		std::vector<t> vals = in;
		std::vector<t> vals2 = in;
		std::vector<t> cmp = in;
		std::vector<t> cmp2 = in;
		std::vector<size_t> idxes(in.size());
		std::iota(idxes.begin(), idxes.end(), 0);

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);

		fea::radix_sort_idxes(
				vals2.begin(), vals2.end(), idxes.begin(), idxes.end());
		EXPECT_EQ(vals2, cmp2);
		std::sort(cmp2.begin(), cmp2.end());
		sort_vals(idxes, &vals2);
		EXPECT_EQ(vals2, cmp2);
	}

	// High collision count fuzz.
	{
		using t = uint16_t;
		std::vector<t> in(1000);
		fea::random_fill(in.begin(), in.end(), 0, 5);
		std::vector<t> vals = in;
		std::vector<t> vals2 = in;
		std::vector<t> cmp = in;
		std::vector<t> cmp2 = in;
		std::vector<size_t> idxes(in.size());
		std::iota(idxes.begin(), idxes.end(), 0);

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);

		fea::radix_sort_idxes(
				vals2.begin(), vals2.end(), idxes.begin(), idxes.end());
		EXPECT_EQ(vals2, cmp2);
		std::sort(cmp2.begin(), cmp2.end());
		sort_vals(idxes, &vals2);
		EXPECT_EQ(vals2, cmp2);
	}

	// Test the caches are reset.
	{
		using t = uint16_t;
		std::vector<t> in(1000);
		fea::random_fill(in.begin(), in.end(), 0, 5);
		std::vector<t> vals = in;
		std::vector<t> vals2 = in;
		std::vector<t> cmp = in;
		std::vector<t> cmp2 = in;
		std::vector<size_t> idxes(in.size());
		std::iota(idxes.begin(), idxes.end(), 0);

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);

		fea::radix_sort_idxes(
				vals2.begin(), vals2.end(), idxes.begin(), idxes.end());
		EXPECT_EQ(vals2, cmp2);
		std::sort(cmp2.begin(), cmp2.end());
		sort_vals(idxes, &vals2);
		EXPECT_EQ(vals2, cmp2);
	}
}

TEST(sort, radix_signed_ints) {
	// Simplest.
	{
		using t = char;
		const std::vector<t> in{ -101, -54, 18, 2, 127, -3, -2 };
		std::vector<t> vals = in;
		std::vector<t> vals2 = in;
		std::vector<t> cmp = in;
		std::vector<t> cmp2 = in;
		std::vector<size_t> idxes(in.size());
		std::iota(idxes.begin(), idxes.end(), 0);

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);

		fea::radix_sort_idxes(
				vals2.begin(), vals2.end(), idxes.begin(), idxes.end());
		EXPECT_EQ(vals2, cmp2);
		std::sort(cmp2.begin(), cmp2.end());
		sort_vals(idxes, &vals2);
		EXPECT_EQ(vals2, cmp2);
	}

	// Multipass.
	{
		using t = int;
		const std::vector<t> in{ -101, -54, 18, 2, 127, -3, -2 };
		std::vector<t> vals = in;
		std::vector<t> vals2 = in;
		std::vector<t> cmp = in;
		std::vector<t> cmp2 = in;
		std::vector<size_t> idxes(in.size());
		std::iota(idxes.begin(), idxes.end(), 0);

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);

		fea::radix_sort_idxes(
				vals2.begin(), vals2.end(), idxes.begin(), idxes.end());
		EXPECT_EQ(vals2, cmp2);
		std::sort(cmp2.begin(), cmp2.end());
		sort_vals(idxes, &vals2);
		EXPECT_EQ(vals2, cmp2);
	}

	// Pre-sorted
	{
		using t = char;
		const std::vector<t> in{ -101, -54, -2, -3, 2, 18, 127 };
		std::vector<t> vals = in;
		std::vector<t> vals2 = in;
		std::vector<t> cmp = in;
		std::vector<t> cmp2 = in;
		std::vector<size_t> idxes(in.size());
		std::iota(idxes.begin(), idxes.end(), 0);

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);

		fea::radix_sort_idxes(
				vals2.begin(), vals2.end(), idxes.begin(), idxes.end());
		EXPECT_EQ(vals2, cmp2);
		std::sort(cmp2.begin(), cmp2.end());
		sort_vals(idxes, &vals2);
		EXPECT_EQ(vals2, cmp2);
	}

	// Test passes.
	{
#if FEA_32BIT
		using t = int32_t;
#else
		using t = int64_t;
#endif
		const std::vector<t> in{ 0, 54, -128, -100'000, 18, -100'042, 0, 2,
			100'042, 128, 3, 0, 128, -128, 100'000, 3, 54, 54, 54, -128,
			100'042, -54, -54, -54, 0, 100'042, -128, 100'000, -100'042, 1,
			-128, -100'000 };
		std::vector<t> vals = in;
		std::vector<t> vals2 = in;
		std::vector<t> cmp = in;
		std::vector<t> cmp2 = in;
		std::vector<size_t> idxes(in.size());
		std::iota(idxes.begin(), idxes.end(), 0);

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);

		fea::radix_sort_idxes(
				vals2.begin(), vals2.end(), idxes.begin(), idxes.end());
		EXPECT_EQ(vals2, cmp2);
		std::sort(cmp2.begin(), cmp2.end());
		sort_vals(idxes, &vals2);
		EXPECT_EQ(vals2, cmp2);
	}

	// Simple fuzz.
	{
		using t = int8_t;
		std::vector<t> in(42);
		fea::random_fill(in.begin(), in.end());
		std::vector<t> vals = in;
		std::vector<t> vals2 = in;
		std::vector<t> cmp = in;
		std::vector<t> cmp2 = in;
		std::vector<size_t> idxes(in.size());
		std::iota(idxes.begin(), idxes.end(), 0);

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);

		fea::radix_sort_idxes(
				vals2.begin(), vals2.end(), idxes.begin(), idxes.end());
		EXPECT_EQ(vals2, cmp2);
		std::sort(cmp2.begin(), cmp2.end());
		sort_vals(idxes, &vals2);
		EXPECT_EQ(vals2, cmp2);
	}

	// Simple fuzz.
	{
#if FEA_32BIT
		using t = int32_t;
#else
		using t = int64_t;
#endif
		std::vector<t> in(1000);
		fea::random_fill(in.begin(), in.end());
		std::vector<t> vals = in;
		std::vector<t> vals2 = in;
		std::vector<t> cmp = in;
		std::vector<t> cmp2 = in;
		std::vector<size_t> idxes(in.size());
		std::iota(idxes.begin(), idxes.end(), 0);

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);

		fea::radix_sort_idxes(
				vals2.begin(), vals2.end(), idxes.begin(), idxes.end());
		EXPECT_EQ(vals2, cmp2);
		std::sort(cmp2.begin(), cmp2.end());
		sort_vals(idxes, &vals2);
		EXPECT_EQ(vals2, cmp2);
	}

	// High collision count fuzz.
	{
		using t = int16_t;
		std::vector<t> in(1000);
		fea::random_fill(in.begin(), in.end(), -5, 5);
		std::vector<t> vals = in;
		std::vector<t> vals2 = in;
		std::vector<t> cmp = in;
		std::vector<t> cmp2 = in;
		std::vector<size_t> idxes(in.size());
		std::iota(idxes.begin(), idxes.end(), 0);

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);

		fea::radix_sort_idxes(
				vals2.begin(), vals2.end(), idxes.begin(), idxes.end());
		EXPECT_EQ(vals2, cmp2);
		std::sort(cmp2.begin(), cmp2.end());
		sort_vals(idxes, &vals2);
		EXPECT_EQ(vals2, cmp2);
	}

	// Test the caches are reset.
	{
		using t = int16_t;
		std::vector<t> in(1000);
		fea::random_fill(in.begin(), in.end(), -5, 5);
		std::vector<t> vals = in;
		std::vector<t> vals2 = in;
		std::vector<t> cmp = in;
		std::vector<t> cmp2 = in;
		std::vector<size_t> idxes(in.size());
		std::iota(idxes.begin(), idxes.end(), 0);

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);

		fea::radix_sort_idxes(
				vals2.begin(), vals2.end(), idxes.begin(), idxes.end());
		EXPECT_EQ(vals2, cmp2);
		std::sort(cmp2.begin(), cmp2.end());
		sort_vals(idxes, &vals2);
		EXPECT_EQ(vals2, cmp2);
	}
}

TEST(sort, radix_floats) {
	// Simplest.
	{
		using t = float;
		const std::vector<t> in{ -101.f, -54.f, 18.f, 2.f, 127.f, -3.f, -2.f };
		std::vector<t> vals = in;
		std::vector<t> vals2 = in;
		std::vector<t> cmp = in;
		std::vector<t> cmp2 = in;
		std::vector<size_t> idxes(in.size());
		std::iota(idxes.begin(), idxes.end(), 0);

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);

		fea::radix_sort_idxes(
				vals2.begin(), vals2.end(), idxes.begin(), idxes.end());
		EXPECT_EQ(vals2, cmp2);
		std::sort(cmp2.begin(), cmp2.end());
		sort_vals(idxes, &vals2);
		EXPECT_EQ(vals2, cmp2);
	}

	// Pre-sorted
	{
#if FEA_32BIT
		using t = float;
#else
		using t = double;
#endif
		const std::vector<t> in{ -101.0, -54.0, -2.0, -3.0, 2.0, 18.0, 127.0 };
		std::vector<t> vals = in;
		std::vector<t> vals2 = in;
		std::vector<t> cmp = in;
		std::vector<t> cmp2 = in;
		std::vector<size_t> idxes(in.size());
		std::iota(idxes.begin(), idxes.end(), 0);

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);

		fea::radix_sort_idxes(
				vals2.begin(), vals2.end(), idxes.begin(), idxes.end());
		EXPECT_EQ(vals2, cmp2);
		std::sort(cmp2.begin(), cmp2.end());
		sort_vals(idxes, &vals2);
		EXPECT_EQ(vals2, cmp2);
	}

	// Test passes.
	{
		using t = float;
		const std::vector<t> in{ 0.f, 54.f, -128.f, -100'000.f, 18.f,
			-100'042.f, 0.f, 2.f, 100'042.f, 128.f, 3.f, 0.f, 128.f, -128.f,
			100'000.f, 3.f, 54.f, 54.f, 54.f, -128.f, 100'042.f, -54.f, -54.f,
			-54.f, 0.f, 100'042.f, -128.f, 100'000.f, -100'042.f, 1.f, -128.f,
			-100'000.f };
		std::vector<t> vals = in;
		std::vector<t> vals2 = in;
		std::vector<t> cmp = in;
		std::vector<t> cmp2 = in;
		std::vector<size_t> idxes(in.size());
		std::iota(idxes.begin(), idxes.end(), 0);

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);

		fea::radix_sort_idxes(
				vals2.begin(), vals2.end(), idxes.begin(), idxes.end());
		EXPECT_EQ(vals2, cmp2);
		std::sort(cmp2.begin(), cmp2.end());
		sort_vals(idxes, &vals2);
		EXPECT_EQ(vals2, cmp2);
	}

	// Simple fuzz.
	{
#if FEA_32BIT
		using t = float;
#else
		using t = double;
#endif
		std::vector<t> in(42);
		fea::random_fill(in.begin(), in.end());
		std::vector<t> vals = in;
		std::vector<t> vals2 = in;
		std::vector<t> cmp = in;
		std::vector<t> cmp2 = in;
		std::vector<size_t> idxes(in.size());
		std::iota(idxes.begin(), idxes.end(), 0);

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);

		fea::radix_sort_idxes(
				vals2.begin(), vals2.end(), idxes.begin(), idxes.end());
		EXPECT_EQ(vals2, cmp2);
		std::sort(cmp2.begin(), cmp2.end());
		sort_vals(idxes, &vals2);
		EXPECT_EQ(vals2, cmp2);
	}

	// Simple fuzz.
	{
#if FEA_32BIT
		using t = float;
#else
		using t = double;
#endif
		std::vector<t> in(1000);
		fea::random_fill(in.begin(), in.end());
		std::vector<t> vals = in;
		std::vector<t> vals2 = in;
		std::vector<t> cmp = in;
		std::vector<t> cmp2 = in;
		std::vector<size_t> idxes(in.size());
		std::iota(idxes.begin(), idxes.end(), 0);

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);

		fea::radix_sort_idxes(
				vals2.begin(), vals2.end(), idxes.begin(), idxes.end());
		EXPECT_EQ(vals2, cmp2);
		std::sort(cmp2.begin(), cmp2.end());
		sort_vals(idxes, &vals2);
		EXPECT_EQ(vals2, cmp2);
	}

	// High collision count fuzz.
	{
		using t = float;
		std::vector<t> in(1000);
		fea::random_fill(in.begin(), in.end(), -1.f, 1.f);
		std::vector<t> vals = in;
		std::vector<t> vals2 = in;
		std::vector<t> cmp = in;
		std::vector<t> cmp2 = in;
		std::vector<size_t> idxes(in.size());
		std::iota(idxes.begin(), idxes.end(), 0);

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);

		fea::radix_sort_idxes(
				vals2.begin(), vals2.end(), idxes.begin(), idxes.end());
		EXPECT_EQ(vals2, cmp2);
		std::sort(cmp2.begin(), cmp2.end());
		sort_vals(idxes, &vals2);
		EXPECT_EQ(vals2, cmp2);
	}

	// Test the caches are reset.
	{
		using t = float;
		std::vector<t> in(1000);
		fea::random_fill(in.begin(), in.end(), -1.f, 1.f);
		std::vector<t> vals = in;
		std::vector<t> vals2 = in;
		std::vector<t> cmp = in;
		std::vector<t> cmp2 = in;
		std::vector<size_t> idxes(in.size());
		std::iota(idxes.begin(), idxes.end(), 0);

		fea::radix_sort(vals.begin(), vals.end());
		std::sort(cmp.begin(), cmp.end());
		EXPECT_EQ(vals, cmp);

		fea::radix_sort_idxes(
				vals2.begin(), vals2.end(), idxes.begin(), idxes.end());
		EXPECT_EQ(vals2, cmp2);
		std::sort(cmp2.begin(), cmp2.end());
		sort_vals(idxes, &vals2);
		EXPECT_EQ(vals2, cmp2);
	}
}

#if 1 && FEA_RELEASE
TEST(sort, radix_benchmark_values) {
	using t = float;
	std::vector<t> in(100'000'000);
	fea::random_fill(in.begin(), in.end(), -1000.f, 1000.f);
	std::vector<t> vals = in;

	fea::bench::suite suite;
	suite.title("Radix Sort Value Based");
	suite.average(5);
	suite.sleep_between(std::chrono::milliseconds{ 100 });
	suite.benchmark("100 million floats",
			[&]() { fea::radix_sort(vals.begin(), vals.end()); });
	suite.print();

	if (!std::is_sorted(vals.begin(), vals.end())) {
		fea::maybe_throw<std::invalid_argument>(
				__FUNCTION__, __LINE__, "Failed to sort.");
	}
}

TEST(sort, radix_benchmark_indexes) {
	using t = float;
	std::vector<t> in(100'000'000);
	fea::random_fill(in.begin(), in.end(), -1000.f, 1000.f);
	std::vector<t> vals = in;
	std::vector<size_t> idxes(vals.size());
	std::iota(idxes.begin(), idxes.end(), 0);

	fea::bench::suite suite;
	suite.title("Radix Sort Index Based");
	suite.average(5);
	suite.sleep_between(std::chrono::milliseconds{ 100 });
	suite.benchmark("100 million floats", [&]() {
		fea::radix_sort_idxes(
				vals.begin(), vals.end(), idxes.begin(), idxes.end());
	});

	suite.print();

	sort_vals(idxes, &vals);
	if (!std::is_sorted(vals.begin(), vals.end())) {
		fea::maybe_throw<std::invalid_argument>(
				__FUNCTION__, __LINE__, "Failed to sort.");
	}
}
#endif

// TODO : Investigate w3c version
// myArray = [170, 45, 75, 90, 802, 24, 2, 66]
// print("Original array:", myArray)
// radixArray = [[], [], [], [], [], [], [], [], [], []]
// maxVal = max(myArray)
// exp = 1
//
// while maxVal // exp > 0:
//
//    while len(myArray) > 0:
//        val = myArray.pop()
//        radixIndex = (val // exp) % 10
//        radixArray[radixIndex].append(val)
//
//    for bucket in radixArray:
//        while len(bucket) > 0:
//            val = bucket.pop()
//            myArray.append(val)
//
//    exp *= 10
//
// print("Sorted array:", myArray)
} // namespace
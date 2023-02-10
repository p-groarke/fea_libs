#include <cstdio>
#include <fea/math/math.hpp>
#include <fea/numerics/random.hpp>
#include <fea/utils/unused.hpp>
#include <gtest/gtest.h>

namespace {
enum class e {
	a,
	b,
	c,
	count,
};
TEST(random, basics) {
	constexpr size_t num_fuzz = 1000u;
	{
		uint8_t mmin = 0;
		uint8_t mmax = fea::random_val<uint8_t>();

		for (size_t i = 0; i < num_fuzz; ++i) {
			uint8_t v = fea::random_val(mmin, mmax);
			EXPECT_LE(v, mmax);
			EXPECT_GE(v, mmin);
		}
	}

	{
		for (size_t i = 0; i < num_fuzz; ++i) {
			size_t count = fea::random_idx(10u);
			EXPECT_LE(count, 9u);
			EXPECT_GE(count, 0u);
		}
	}

	{
		std::vector<int> v(100);
		fea::random_fill(v.begin(), v.end(), 1, 42);
		for (int i : v) {
			EXPECT_GT(i, 0);
			EXPECT_LT(i, 43);
		}
	}

	{
		std::array<uint8_t, 4> bytes = fea::random_bytes<4>();
		fea::unused(bytes); // how to test?
		std::vector<uint8_t> vbytes = fea::random_bytes(4);
		EXPECT_EQ(vbytes.size(), 4u);
	}

	for (size_t i = 0; i < num_fuzz; ++i) {
		// Basically just confirm it compiles on all platforms and configs.
		bool v = fea::random_val<bool>();
		EXPECT_TRUE(v || !v);
	}

	{
		for (size_t i = 0; i < num_fuzz; ++i) {
			e v = fea::random_val<e>();
			EXPECT_NE(v, e::count);
		}

		for (size_t i = 0; i < num_fuzz; ++i) {
			e v = fea::random_val(e::a, e::b);
			EXPECT_TRUE(v == e::a || v == e::b);
		}
	}

	{
		for (size_t i = 0; i < num_fuzz; ++i) {
			double v = fea::random_val(0.0, 42.0);
			EXPECT_LE(v, 42.0);
			EXPECT_GE(v, 0.0);
		}
		for (size_t i = 0; i < num_fuzz; ++i) {
			float v = fea::random_val(0.f, 42.f);
			EXPECT_LE(v, 42.f);
			EXPECT_GE(v, 0.f);
		}
	}

	{
		const size_t num_v = 100;
		std::vector<double> v1(num_v);
		for (size_t i = 0; i < num_fuzz; ++i) {
			fea::random_fixed_sum(v1.begin(), v1.end(), double(num_v));
			double ans = fea::sum(v1);

			EXPECT_NEAR(ans, double(num_v), 0.00000000001);
		}
	}

	{
		for (size_t i = 0; i < num_fuzz; ++i) {
			std::array<size_t, 10> v1 = fea::random_fixed_sum<10>(size_t(100));
			size_t ans = fea::sum(v1);
			EXPECT_EQ(ans, 100u);
		}
	}
}
} // namespace

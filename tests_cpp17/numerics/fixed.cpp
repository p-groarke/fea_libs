#include <fea/numerics/fixed.hpp>
#include <gtest/gtest.h>

namespace {
#define FAIL_MSG "fixed.cpp : failed test"

TEST(fixed, basics) {
	// ctors and simple conversions
	{
		const double val = 42.0;
		fea::fixed t = val;
		t = val;
		EXPECT_EQ(double(t), val);

		t = 1.0;
		EXPECT_EQ(double(t), 1.0);

		t = fea::fixed(int64_t(1));
		EXPECT_EQ(int64_t(t), 1);

		t = 1.f;
		EXPECT_EQ(float(t), 1.f);

		fea::fixed cpy = t;
		EXPECT_EQ(float(t), 1.f);
		EXPECT_EQ(double(t), 1.0);
		EXPECT_EQ(int64_t(t), 1);

		cpy = std::move(t);
		EXPECT_EQ(float(t), 1.f);
		EXPECT_EQ(double(t), 1.0);
		EXPECT_EQ(int64_t(t), 1);
		t = cpy;

		cpy = fea::fixed(t);
		EXPECT_EQ(float(t), 1.f);
		EXPECT_EQ(double(t), 1.0);
		EXPECT_EQ(int64_t(t), 1);

		cpy = fea::fixed(std::move(t));
		EXPECT_EQ(float(t), 1.f);
		EXPECT_EQ(double(t), 1.0);
		EXPECT_EQ(int64_t(t), 1);
		t = cpy;
	}

	// Comparison operators.
	{
		fea::fixed f1 = 2.0;
		fea::fixed f2 = 2.0;
		EXPECT_TRUE(f1 == f2);
		EXPECT_FALSE(f1 != f2);
		EXPECT_FALSE(f1 < f2);
		EXPECT_FALSE(f1 > f2);
		EXPECT_TRUE(f1 <= f2);
		EXPECT_TRUE(f1 >= f2);

		f1 = 0.0;
		f2 = 2.0;
		EXPECT_FALSE(f1 == f2);
		EXPECT_TRUE(f1 != f2);
		EXPECT_TRUE(f1 < f2);
		EXPECT_FALSE(f1 > f2);
		EXPECT_TRUE(f1 <= f2);
		EXPECT_FALSE(f1 >= f2);

		f1 = 2.0;
		f2 = 0.0;
		EXPECT_FALSE(f1 == f2);
		EXPECT_TRUE(f1 != f2);
		EXPECT_FALSE(f1 < f2);
		EXPECT_TRUE(f1 > f2);
		EXPECT_FALSE(f1 <= f2);
		EXPECT_TRUE(f1 >= f2);

		fea::currency c1 = 2.0;
		fea::currency c2 = 2.0;
		EXPECT_TRUE(c1 == c2);
		EXPECT_FALSE(c1 != c2);
		EXPECT_FALSE(c1 < c2);
		EXPECT_FALSE(c1 > c2);
		EXPECT_TRUE(c1 <= c2);
		EXPECT_TRUE(c1 >= c2);

		c1 = 0.0;
		c2 = 2.0;
		EXPECT_FALSE(c1 == c2);
		EXPECT_TRUE(c1 != c2);
		EXPECT_TRUE(c1 < c2);
		EXPECT_FALSE(c1 > c2);
		EXPECT_TRUE(c1 <= c2);
		EXPECT_FALSE(c1 >= c2);

		c1 = 2.0;
		c2 = 0.0;
		EXPECT_FALSE(c1 == c2);
		EXPECT_TRUE(c1 != c2);
		EXPECT_FALSE(c1 < c2);
		EXPECT_TRUE(c1 > c2);
		EXPECT_FALSE(c1 <= c2);
		EXPECT_TRUE(c1 >= c2);
	}

	// Math operators.
	{
		fea::fixed f1 = 2.0;
		fea::fixed f2 = 2.0;
		fea::fixed ans = f1 + f2;
		EXPECT_EQ(ans, fea::fixed(4.0));

		ans = f1 - f2;
		EXPECT_EQ(ans, fea::fixed(0.0));

		ans = f1 * f2;
		EXPECT_EQ(ans, fea::fixed(4.0));

		ans = f1 / f2;
		EXPECT_EQ(ans, fea::fixed(1.0));

		ans = f1 % f2;
		EXPECT_EQ(ans, fea::fixed(0.0));

		f1 = 8.0;
		f2 = 2.0;
		ans = f1 + f2;
		EXPECT_EQ(ans, fea::fixed(10.0));

		ans = f1 - f2;
		EXPECT_EQ(ans, fea::fixed(6.0));

		ans = f1 * f2;
		EXPECT_EQ(ans, fea::fixed(16.0));

		ans = f1 / f2;
		EXPECT_EQ(ans, fea::fixed(4.0));

		ans = f1 % f2;
		EXPECT_EQ(ans, fea::fixed(0.0));

		f1 = 2.0;
		f2 = 8.0;
		ans = f1 + f2;
		EXPECT_EQ(ans, fea::fixed(10.0));

		ans = f1 - f2;
		EXPECT_EQ(ans, fea::fixed(-6.0));

		ans = f1 * f2;
		EXPECT_EQ(ans, fea::fixed(16.0));

		ans = f1 / f2;
		EXPECT_EQ(ans, fea::fixed(0.25));

		ans = f1 % f2;
		EXPECT_EQ(ans, fea::fixed(2.0));

		f1 = -2.0;
		f2 = 8.0;
		ans = f1 + f2;
		EXPECT_EQ(ans, fea::fixed(6.0));

		ans = f1 - f2;
		EXPECT_EQ(ans, fea::fixed(-10.0));

		ans = f1 * f2;
		EXPECT_EQ(ans, fea::fixed(-16.0));

		ans = f1 / f2;
		EXPECT_EQ(ans, fea::fixed(-0.25));

		ans = f1 % f2;
		EXPECT_EQ(ans, fea::fixed(-2.0));

		f1 = 2.0;
		f2 = -8.0;
		ans = f1 + f2;
		EXPECT_EQ(ans, fea::fixed(-6.0));

		ans = f1 - f2;
		EXPECT_EQ(ans, fea::fixed(10.0));

		ans = f1 * f2;
		EXPECT_EQ(ans, fea::fixed(-16.0));

		ans = f1 / f2;
		EXPECT_EQ(ans, fea::fixed(-0.25));

		ans = f1 % f2;
		EXPECT_EQ(ans, fea::fixed(2.0));

		// A few more modulo
		f1 = 1.0;
		f2 = 5.0;
		ans = f1 % f2;
		EXPECT_EQ(ans, fea::fixed(1.0));
		f1 = 5.0;
		f2 = 1.0;
		ans = f1 % f2;
		EXPECT_EQ(ans, fea::fixed(0.0));
	}

	{
		fea::currency f1 = 2.0;
		fea::currency f2 = 2.0;
		fea::currency ans = f1 + f2;
		EXPECT_EQ(ans, fea::currency(4.0));

		ans = f1 - f2;
		EXPECT_EQ(ans, fea::currency(0.0));

		ans = f1 * f2;
		EXPECT_EQ(ans, fea::currency(4.0));

		ans = f1 / f2;
		EXPECT_EQ(ans, fea::currency(1.0));

		ans = f1 % f2;
		EXPECT_EQ(ans, fea::currency(0.0));

		f1 = 8.0;
		f2 = 2.0;
		ans = f1 + f2;
		EXPECT_EQ(ans, fea::currency(10.0));

		ans = f1 - f2;
		EXPECT_EQ(ans, fea::currency(6.0));

		ans = f1 * f2;
		EXPECT_EQ(ans, fea::currency(16.0));

		ans = f1 / f2;
		EXPECT_EQ(ans, fea::currency(4.0));

		ans = f1 % f2;
		EXPECT_EQ(ans, fea::currency(0.0));

		f1 = 2.0;
		f2 = 8.0;
		ans = f1 + f2;
		EXPECT_EQ(ans, fea::currency(10.0));

		ans = f1 - f2;
		EXPECT_EQ(ans, fea::currency(-6.0));

		ans = f1 * f2;
		EXPECT_EQ(ans, fea::currency(16.0));

		ans = f1 / f2;
		EXPECT_EQ(ans, fea::currency(0.25));

		ans = f1 % f2;
		EXPECT_EQ(ans, fea::currency(2.0));

		f1 = -2.0;
		f2 = 8.0;
		ans = f1 + f2;
		EXPECT_EQ(ans, fea::currency(6.0));

		ans = f1 - f2;
		EXPECT_EQ(ans, fea::currency(-10.0));

		ans = f1 * f2;
		EXPECT_EQ(ans, fea::currency(-16.0));

		ans = f1 / f2;
		EXPECT_EQ(ans, fea::currency(-0.25));

		ans = f1 % f2;
		EXPECT_EQ(ans, fea::currency(-2.0));

		f1 = 2.0;
		f2 = -8.0;
		ans = f1 + f2;
		EXPECT_EQ(ans, fea::currency(-6.0));

		ans = f1 - f2;
		EXPECT_EQ(ans, fea::currency(10.0));

		ans = f1 * f2;
		EXPECT_EQ(ans, fea::currency(-16.0));

		ans = f1 / f2;
		EXPECT_EQ(ans, fea::currency(-0.25));

		ans = f1 % f2;
		EXPECT_EQ(ans, fea::currency(2.0));

		// A few more modulo
		f1 = 1.0;
		f2 = 5.0;
		ans = f1 % f2;
		EXPECT_EQ(ans, fea::currency(1.0));
		f1 = 5.0;
		f2 = 1.0;
		ans = f1 % f2;
		EXPECT_EQ(ans, fea::currency(0.0));
	}

	{
		fea::fixed f1(int64_t(2));
		fea::fixed f2(int64_t(2));
		fea::fixed ans = f1 + f2;
		EXPECT_EQ(int64_t(ans), 4);

		ans = f1 - f2;
		EXPECT_EQ(int64_t(ans), 0);

		ans = f1 * f2;
		EXPECT_EQ(int64_t(ans), 4);

		ans = f1 / f2;
		EXPECT_EQ(int64_t(ans), 1);

		ans = f1 % f2;
		EXPECT_EQ(int64_t(ans), 0);

		f1 = 8.0;
		f2 = 2.0;
		ans = f1 + f2;
		EXPECT_EQ(int64_t(ans), 10);

		ans = f1 - f2;
		EXPECT_EQ(int64_t(ans), 6);

		ans = f1 * f2;
		EXPECT_EQ(int64_t(ans), 16);

		ans = f1 / f2;
		EXPECT_EQ(int64_t(ans), 4);

		ans = f1 % f2;
		EXPECT_EQ(int64_t(ans), 0);
	}

	// Sanity checks.
	{
		using mfixed1 = fea::detail::fixed<(size_t(1) << 23), int64_t>;
		using mfixed2 = fea::detail::fixed<100, int64_t>;
		using mfixed3 = fea::detail::fixed<3, int64_t>;
		using mfixed4 = fea::detail::fixed<2, int>;
		using mfixed5 = fea::detail::fixed<4, int>;
		using mfixed6 = fea::detail::fixed<(size_t(1) << 62), int64_t>;

		static_assert(mfixed1::scaling_is_pow2_v, FAIL_MSG);
		static_assert(!mfixed2::scaling_is_pow2_v, FAIL_MSG);
		static_assert(!mfixed3::scaling_is_pow2_v, FAIL_MSG);
		static_assert(mfixed4::scaling_is_pow2_v, FAIL_MSG);
		static_assert(mfixed5::scaling_is_pow2_v, FAIL_MSG);
		static_assert(mfixed6::scaling_is_pow2_v, FAIL_MSG);

		static_assert(mfixed1::scaling_sqrt_v == 23, FAIL_MSG);
		static_assert(mfixed2::scaling_sqrt_v == 0, FAIL_MSG);
		static_assert(mfixed3::scaling_sqrt_v == 0, FAIL_MSG);
		static_assert(mfixed4::scaling_sqrt_v == 1, FAIL_MSG);
		static_assert(mfixed5::scaling_sqrt_v == 2, FAIL_MSG);
		static_assert(mfixed6::scaling_sqrt_v == 62, FAIL_MSG);
	}

	// numeric_limits specialization.
	{
		constexpr fea::fixed mmin = (std::numeric_limits<fea::fixed>::min)();
		constexpr fea::fixed mlowest
				= std::numeric_limits<fea::fixed>::lowest();
		constexpr fea::fixed mmax = (std::numeric_limits<fea::fixed>::max)();
		constexpr fea::fixed mepsilon
				= std::numeric_limits<fea::fixed>::epsilon();
		constexpr fea::fixed mround_err
				= std::numeric_limits<fea::fixed>::round_error();
		constexpr fea::fixed minfinity
				= std::numeric_limits<fea::fixed>::infinity();
		constexpr fea::fixed mquiet_nan
				= std::numeric_limits<fea::fixed>::quiet_NaN();
		constexpr fea::fixed msignaling_nan
				= std::numeric_limits<fea::fixed>::signaling_NaN();
		constexpr fea::fixed mdenorm_min
				= std::numeric_limits<fea::fixed>::denorm_min();
	}

	// At 23 bits of fractional digits, we should have the same epsilon
	// precision as a float32.
	{
		static_assert(sizeof(float) == 4, "Float isn't 32bits :o");
		constexpr float float_eps = std::numeric_limits<float>::epsilon();

		using mfixed = fea::detail::fixed<(size_t(1) << 23), int64_t>;
		constexpr float fixed_eps
				= float(std::numeric_limits<mfixed>::epsilon());

		static_assert(float_eps == fixed_eps, FAIL_MSG);
	}
}
} // namespace
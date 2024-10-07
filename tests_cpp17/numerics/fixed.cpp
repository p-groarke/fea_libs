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

		t = fea::fixed(std::intmax_t(1));
		EXPECT_EQ(std::intmax_t(t), 1);

		t = 1.f;
		EXPECT_EQ(float(t), 1.f);

		fea::fixed cpy = t;
		EXPECT_EQ(float(t), 1.f);
		EXPECT_EQ(double(t), 1.0);
		EXPECT_EQ(std::intmax_t(t), 1);

		cpy = std::move(t);
		EXPECT_EQ(float(t), 1.f);
		EXPECT_EQ(double(t), 1.0);
		EXPECT_EQ(std::intmax_t(t), 1);
		t = cpy;

		cpy = fea::fixed(t);
		EXPECT_EQ(float(t), 1.f);
		EXPECT_EQ(double(t), 1.0);
		EXPECT_EQ(std::intmax_t(t), 1);

		cpy = fea::fixed(std::move(t));
		EXPECT_EQ(float(t), 1.f);
		EXPECT_EQ(double(t), 1.0);
		EXPECT_EQ(std::intmax_t(t), 1);
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
		fea::fixed f1(std::intmax_t(2));
		fea::fixed f2(std::intmax_t(2));
		fea::fixed ans = f1 + f2;
		EXPECT_EQ(std::intmax_t(ans), 4);

		ans = f1 - f2;
		EXPECT_EQ(std::intmax_t(ans), 0);

		ans = f1 * f2;
		EXPECT_EQ(std::intmax_t(ans), 4);

		ans = f1 / f2;
		EXPECT_EQ(std::intmax_t(ans), 1);

		ans = f1 % f2;
		EXPECT_EQ(std::intmax_t(ans), 0);

		f1 = 8.0;
		f2 = 2.0;
		ans = f1 + f2;
		EXPECT_EQ(std::intmax_t(ans), 10);

		ans = f1 - f2;
		EXPECT_EQ(std::intmax_t(ans), 6);

		ans = f1 * f2;
		EXPECT_EQ(std::intmax_t(ans), 16);

		ans = f1 / f2;
		EXPECT_EQ(std::intmax_t(ans), 4);

		ans = f1 % f2;
		EXPECT_EQ(std::intmax_t(ans), 0);
	}

	// Sanity checks.
	{
		using mfixed1 = fea::detail::fixed<(size_t(1) << 23), int64_t>;
		using mfixed2 = fea::detail::fixed<100, int64_t>;
		using mfixed3 = fea::detail::fixed<3, int64_t>;
		using mfixed4 = fea::detail::fixed<2, int>;
		using mfixed5 = fea::detail::fixed<4, int>;

		static_assert(mfixed1::scaling_is_pow2_v, FAIL_MSG);
		static_assert(!mfixed2::scaling_is_pow2_v, FAIL_MSG);
		static_assert(!mfixed3::scaling_is_pow2_v, FAIL_MSG);
		static_assert(mfixed4::scaling_is_pow2_v, FAIL_MSG);
		static_assert(mfixed5::scaling_is_pow2_v, FAIL_MSG);

		static_assert(mfixed1::scaling_sqrt_v == 23, FAIL_MSG);
		static_assert(mfixed2::scaling_sqrt_v == 0, FAIL_MSG);
		static_assert(mfixed3::scaling_sqrt_v == 0, FAIL_MSG);
		static_assert(mfixed4::scaling_sqrt_v == 1, FAIL_MSG);
		static_assert(mfixed5::scaling_sqrt_v == 2, FAIL_MSG);

#if FEA_ARCH >= 64
		using mfixed6 = fea::detail::fixed<(size_t(1) << 62), int64_t>;
		static_assert(mfixed6::scaling_is_pow2_v, FAIL_MSG);
		static_assert(mfixed6::scaling_sqrt_v == 62, FAIL_MSG);
#endif
	}

	// numeric_limits specialization.
	{
#if FEA_ARCH >= 64
		// Q : min and lowest behave like int or float?
		constexpr fea::fixed mmin = (std::numeric_limits<fea::fixed>::min)();
		constexpr fea::fixed mlowest
				= std::numeric_limits<fea::fixed>::lowest();

		constexpr std::intmax_t mmax
				= std::intmax_t((std::numeric_limits<fea::fixed>::max)());
		constexpr std::intmax_t mmax_expected
				= (std::intmax_t(1) << (64 - 23 - 1)) - 1;
		static_assert(mmax == mmax_expected, FAIL_MSG);

		// At 23 bits of fractional digits, we should have the same epsilon
		// precision as a float32.
		constexpr fea::fixed mepsilon
				= std::numeric_limits<fea::fixed>::epsilon();
		static_assert(
				mepsilon == std::numeric_limits<float>::epsilon(), FAIL_MSG);
#else
		constexpr std::intmax_t mmax
				= std::intmax_t((std::numeric_limits<fea::fixed>::max)());
		constexpr std::intmax_t mmax_expected
				= (std::intmax_t(1) << (32 - 15 - 1)) - 1;
		static_assert(mmax == mmax_expected, FAIL_MSG);
#endif

		constexpr fea::fixed mround_err
				= std::numeric_limits<fea::fixed>::round_error();
		static_assert(mround_err == 0.5, FAIL_MSG);

		constexpr fea::fixed minfinity
				= std::numeric_limits<fea::fixed>::infinity();
		static_assert(minfinity == 0.0, FAIL_MSG);

		constexpr fea::fixed mquiet_nan
				= std::numeric_limits<fea::fixed>::quiet_NaN();
		static_assert(mquiet_nan == 0.0, FAIL_MSG);

		constexpr fea::fixed msignaling_nan
				= std::numeric_limits<fea::fixed>::signaling_NaN();
		static_assert(msignaling_nan == 0.0, FAIL_MSG);

		constexpr fea::fixed mdenorm_min
				= std::numeric_limits<fea::fixed>::denorm_min();
		static_assert(mdenorm_min == 0.0, FAIL_MSG);
	}
}

TEST(fixed, precision) {
	{
		constexpr fea::fixed eps = std::numeric_limits<fea::fixed>::epsilon();
		constexpr fea::fixed f1 = eps + eps;
		constexpr fea::fixed f2 = eps * 2.0;
		static_assert(f1 == f2, FAIL_MSG);

		constexpr fea::fixed f22 = 2.0 * eps;
		static_assert(f2 == f22, FAIL_MSG);

		constexpr fea::fixed f3 = eps - eps;
		static_assert(f3 == 0.0, FAIL_MSG);

		constexpr fea::fixed f4 = eps / eps;
		static_assert(f4 == 1.0, FAIL_MSG);
	}

	{
		fea::fixed eps = std::numeric_limits<fea::fixed>::epsilon();
		fea::fixed f1 = eps + eps;
		fea::fixed f2 = eps * 2.0;
		EXPECT_EQ(f1, f2);

		fea::fixed f22 = 2.0 * eps;
		EXPECT_EQ(f2, f22);

		fea::fixed f3 = eps - eps;
		EXPECT_EQ(f3, 0.0);

		fea::fixed f4 = eps / eps;
		EXPECT_EQ(f4, 1.0);
	}

	{
		constexpr fea::fixed eps = std::numeric_limits<fea::fixed>::epsilon();
		constexpr fea::fixed f1 = 1.0 + eps;
		static_assert(f1 != 1.0, FAIL_MSG);

		constexpr fea::fixed f12 = eps + 1.0;
		static_assert(f12 != 1.0, FAIL_MSG);

		constexpr fea::fixed f2 = 1.0 * eps;
		static_assert(f2 != 1.0, FAIL_MSG);

		constexpr fea::fixed f22 = eps * 1.0;
		static_assert(f22 != 1.0, FAIL_MSG);

		constexpr fea::fixed f3 = 1.0 - eps;
		static_assert(f3 != 1.0, FAIL_MSG);

		constexpr fea::fixed f32 = eps - 1.0;
		static_assert(f32 != 1.0, FAIL_MSG);

		constexpr fea::fixed f4 = 1.0 / eps;
		static_assert(f4 != 1.0, FAIL_MSG);

		constexpr fea::fixed f42 = eps / 1.0;
		static_assert(f42 != 1.0, FAIL_MSG);
	}

	{
		fea::fixed eps = std::numeric_limits<fea::fixed>::epsilon();
		fea::fixed f1 = 1.0 + eps;
		EXPECT_NE(f1, 1.0);

		fea::fixed f12 = eps + 1.0;
		EXPECT_NE(f12, 1.0);

		fea::fixed f2 = 1.0 * eps;
		EXPECT_NE(f2, 1.0);

		fea::fixed f22 = eps * 1.0;
		EXPECT_NE(f22, 1.0);

		fea::fixed f3 = 1.0 - eps;
		EXPECT_NE(f3, 1.0);

		fea::fixed f32 = eps - 1.0;
		EXPECT_NE(f32, 1.0);

		fea::fixed f4 = 1.0 / eps;
		EXPECT_NE(f4, 1.0);

		fea::fixed f42 = eps / 1.0;
		EXPECT_NE(f42, 1.0);
	}
}
} // namespace
#include <fea/numerics/fixed.hpp>
#include <fea/utility/platform.hpp>
#include <gtest/gtest.h>

namespace {
#define FAIL_MSG "fixed.cpp : failed test"

using mint_t = typename fea::fixed::value_t;

TEST(fixed, basics) {
	// ctors and simple conversions
	{
		const double val = 42.0;
		fea::fixed t = val;
		t = val;
		EXPECT_EQ(double(t), val);

		t = 1.0;
		EXPECT_EQ(double(t), 1.0);

		t = fea::fixed(mint_t(1));
		EXPECT_EQ(mint_t(t), 1);

		t = 1.f;
		EXPECT_EQ(float(t), 1.f);

		fea::fixed cpy = t;
		EXPECT_EQ(float(t), 1.f);
		EXPECT_EQ(double(t), 1.0);
		EXPECT_EQ(mint_t(t), 1);

		cpy = std::move(t);
		EXPECT_EQ(float(t), 1.f);
		EXPECT_EQ(double(t), 1.0);
		EXPECT_EQ(mint_t(t), 1);
		t = cpy;

		cpy = fea::fixed(t);
		EXPECT_EQ(float(t), 1.f);
		EXPECT_EQ(double(t), 1.0);
		EXPECT_EQ(mint_t(t), 1);

		cpy = fea::fixed(std::move(t));
		EXPECT_EQ(float(t), 1.f);
		EXPECT_EQ(double(t), 1.0);
		EXPECT_EQ(mint_t(t), 1);
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

		ans = ~f1;
#if FEA_32BIT
		EXPECT_EQ(ans, fea::fixed(-2.00048828125));
#else
		EXPECT_EQ(ans, fea::fixed(-2.0000001192092896));
#endif

		ans = f1 & f2;
		EXPECT_EQ(ans, fea::fixed(2.0));

		ans = f1 | f2;
		EXPECT_EQ(ans, fea::fixed(2.0));

		ans = f1 ^ f2;
		EXPECT_EQ(ans, fea::fixed(0.0));

		ans = f1 >> f2;
		EXPECT_EQ(ans, fea::fixed(0.5));

		ans = f1 >> size_t(f2);
		EXPECT_EQ(ans, fea::fixed(0.5));

		ans = f1 << f2;
		EXPECT_EQ(ans, fea::fixed(8.0));

		ans = f1 << size_t(f2);
		EXPECT_EQ(ans, fea::fixed(8.0));


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

		ans = ~f1;
#if FEA_32BIT
		EXPECT_EQ(ans, fea::fixed(-8.00048828125));
#else
		EXPECT_EQ(ans, fea::fixed(-8.0000001192092896));
#endif

		ans = f1 & f2;
		EXPECT_EQ(ans, fea::fixed(0.0));

		ans = f1 | f2;
		EXPECT_EQ(ans, fea::fixed(10.0));

		ans = f1 ^ f2;
		EXPECT_EQ(ans, fea::fixed(10.0));

		ans = f1 >> f2;
		EXPECT_EQ(ans, fea::fixed(2.0));

		ans = f1 >> size_t(f2);
		EXPECT_EQ(ans, fea::fixed(2.0));

		ans = f1 << f2;
		EXPECT_EQ(ans, fea::fixed(32.0));

		ans = f1 << size_t(f2);
		EXPECT_EQ(ans, fea::fixed(32.0));


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

		ans = ~f1;
#if FEA_32BIT
		EXPECT_EQ(ans, fea::fixed(-2.00048828125));
#else
		EXPECT_EQ(ans, fea::fixed(-2.0000001192092896));
#endif

		ans = f1 & f2;
		EXPECT_EQ(ans, fea::fixed(0.0));

		ans = f1 | f2;
		EXPECT_EQ(ans, fea::fixed(10.0));

		ans = f1 ^ f2;
		EXPECT_EQ(ans, fea::fixed(10.0));

		ans = f1 >> f2;
		EXPECT_EQ(ans, fea::fixed(0.0078125));

		ans = f1 >> size_t(f2);
		EXPECT_EQ(ans, fea::fixed(0.0078125));

		ans = f1 << f2;
		EXPECT_EQ(ans, fea::fixed(512.0));

		ans = f1 << size_t(f2);
		EXPECT_EQ(ans, fea::fixed(512.0));


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

		ans = ~f1;
#if FEA_32BIT
		EXPECT_EQ(ans, fea::fixed(1.99951171875));
#else
		EXPECT_EQ(ans, fea::fixed(1.9999998807907104));
#endif

		ans = f1 & f2;
		EXPECT_EQ(ans, fea::fixed(8.0));

		ans = f1 | f2;
		EXPECT_EQ(ans, fea::fixed(-2.0));

		ans = f1 ^ f2;
		EXPECT_EQ(ans, fea::fixed(-10.0));

		ans = f1 >> f2;
		EXPECT_EQ(ans, fea::fixed(-0.0078125));

		ans = f1 >> size_t(f2);
		EXPECT_EQ(ans, fea::fixed(-0.0078125));

		ans = f1 << f2;
		EXPECT_EQ(ans, fea::fixed(-512.0));

		ans = f1 << size_t(f2);
		EXPECT_EQ(ans, fea::fixed(-512.0));


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

		ans = ~f1;
#if FEA_32BIT
		EXPECT_EQ(ans, fea::fixed(-2.00048828125));
#else
		EXPECT_EQ(ans, fea::fixed(-2.0000001192092896));
#endif

		ans = f1 & f2;
		EXPECT_EQ(ans, fea::fixed(0.0));

		ans = f1 | f2;
		EXPECT_EQ(ans, fea::fixed(-6.0));

		ans = f1 ^ f2;
		EXPECT_EQ(ans, fea::fixed(-6.0));

#if FEA_DEBUG
		EXPECT_DEATH(f1 >> f2, "");
		EXPECT_DEATH(f1 << f2, "");
#endif


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
		fea::fixed f1(mint_t(2));
		fea::fixed f2(mint_t(2));
		fea::fixed ans = f1 + f2;
		EXPECT_EQ(mint_t(ans), 4);

		ans = f1 - f2;
		EXPECT_EQ(mint_t(ans), 0);

		ans = f1 * f2;
		EXPECT_EQ(mint_t(ans), 4);

		ans = f1 / f2;
		EXPECT_EQ(mint_t(ans), 1);

		ans = f1 % f2;
		EXPECT_EQ(mint_t(ans), 0);

		f1 = 8.0;
		f2 = 2.0;
		ans = f1 + f2;
		EXPECT_EQ(mint_t(ans), 10);

		ans = f1 - f2;
		EXPECT_EQ(mint_t(ans), 6);

		ans = f1 * f2;
		EXPECT_EQ(mint_t(ans), 16);

		ans = f1 / f2;
		EXPECT_EQ(mint_t(ans), 4);

		ans = f1 % f2;
		EXPECT_EQ(mint_t(ans), 0);
	}

	// Sanity checks.
	{
		using mfixed1 = fea::basic_fixed<int64_t, (size_t(1) << 23)>;
		using mfixed2 = fea::basic_fixed<int64_t, 100>;
		using mfixed3 = fea::basic_fixed<int64_t, 3>;
		using mfixed4 = fea::basic_fixed<int, 2>;
		using mfixed5 = fea::basic_fixed<int, 4>;

		static_assert(mfixed1::is_scaling_pow2_v, FAIL_MSG);
		static_assert(!mfixed2::is_scaling_pow2_v, FAIL_MSG);
		static_assert(!mfixed3::is_scaling_pow2_v, FAIL_MSG);
		static_assert(mfixed4::is_scaling_pow2_v, FAIL_MSG);
		static_assert(mfixed5::is_scaling_pow2_v, FAIL_MSG);

		static_assert(mfixed1::scaling_sqrt_v == 23, FAIL_MSG);
		static_assert(mfixed2::scaling_sqrt_v == 0, FAIL_MSG);
		static_assert(mfixed3::scaling_sqrt_v == 0, FAIL_MSG);
		static_assert(mfixed4::scaling_sqrt_v == 1, FAIL_MSG);
		static_assert(mfixed5::scaling_sqrt_v == 2, FAIL_MSG);

#if FEA_ARCH >= 64
		using mfixed6 = fea::basic_fixed<int64_t, (size_t(1) << 62)>;
		static_assert(mfixed6::is_scaling_pow2_v, FAIL_MSG);
		static_assert(mfixed6::scaling_sqrt_v == 62, FAIL_MSG);
#endif
	}

	// numeric_limits specialization.
	{
		// We just check that all these are implemented and working.
		static_assert(std::numeric_limits<fea::fixed>::is_specialized == true);
		static_assert(std::numeric_limits<fea::fixed>::is_signed
					  == std::is_signed_v<mint_t>);
		static_assert(std::numeric_limits<fea::fixed>::is_integer == false);
		static_assert(std::numeric_limits<fea::fixed>::is_exact == true);
		static_assert(std::numeric_limits<fea::fixed>::has_infinity == false);
		static_assert(std::numeric_limits<fea::fixed>::has_quiet_NaN == false);
		static_assert(
				std::numeric_limits<fea::fixed>::has_signaling_NaN == false);
		static_assert(std::numeric_limits<fea::fixed>::has_denorm
					  == std::denorm_absent);
		static_assert(
				std::numeric_limits<fea::fixed>::has_denorm_loss == false);
		static_assert(std::numeric_limits<fea::fixed>::round_style
					  == std::round_toward_zero);
		static_assert(std::numeric_limits<fea::fixed>::is_iec559 == false);
		static_assert(std::numeric_limits<fea::fixed>::is_bounded == true);
		static_assert(std::numeric_limits<fea::fixed>::is_modulo
					  == std::numeric_limits<mint_t>::is_modulo);
		static_assert(std::numeric_limits<fea::fixed>::digits
					  == std::numeric_limits<mint_t>::digits);
		static_assert(std::numeric_limits<fea::fixed>::digits10
					  == std::numeric_limits<mint_t>::digits10);
		static_assert(std::numeric_limits<fea::fixed>::max_digits10
					  == std::numeric_limits<mint_t>::max_digits10);
		static_assert(std::numeric_limits<fea::fixed>::radix == 2);
		static_assert(std::numeric_limits<fea::fixed>::min_exponent == 0);
		static_assert(std::numeric_limits<fea::fixed>::min_exponent10 == 0);
		static_assert(std::numeric_limits<fea::fixed>::max_exponent == 0);
		static_assert(std::numeric_limits<fea::fixed>::max_exponent10 == 0);
		static_assert(std::numeric_limits<fea::fixed>::traps
					  == std::numeric_limits<mint_t>::traps);
		static_assert(
				std::numeric_limits<fea::fixed>::tinyness_before == false);

		// We behave like floats, to be interchangeable.
		// min() closest to zero, lowest() lowest negative value.
		constexpr fea::fixed mmin = (std::numeric_limits<fea::fixed>::min)();
		static_assert(mmin == 0.0, FAIL_MSG);
		static_assert(mint_t(mmin) == mint_t(0), FAIL_MSG);

		constexpr fea::fixed mlowest
				= std::numeric_limits<fea::fixed>::lowest();
		static_assert(mlowest < 0.0, FAIL_MSG);
		static_assert(mint_t(mlowest) < mint_t(0), FAIL_MSG);
#if FEA_ARCH == 64
		constexpr mint_t mlowest_expected = -(mint_t(1) << (64 - 23 - 1));
#else
		constexpr mint_t mlowest_expected = -(mint_t(1) << (32 - 11 - 1));
#endif
		static_assert(mint_t(mlowest) == mlowest_expected, FAIL_MSG);

		constexpr fea::fixed mmax = (std::numeric_limits<fea::fixed>::max)();
#if FEA_ARCH == 64
		constexpr mint_t mmax_expected = (mint_t(1) << (64 - 23 - 1)) - 1;
#else
		constexpr mint_t mmax_expected = (mint_t(1) << (32 - 11 - 1)) - 1;
#endif
		static_assert(mint_t(mmax) == mmax_expected, FAIL_MSG);

#if FEA_ARCH == 64
		constexpr fea::fixed mepsilon
				= std::numeric_limits<fea::fixed>::epsilon();

		// At 23 bits of fractional digits, we should have the same epsilon
		// precision as a float32.
		// Only applicable to 64bits.
		static_assert(
				mepsilon == std::numeric_limits<float>::epsilon(), FAIL_MSG);
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

TEST(fixed, assignement_ops) {
	fea::fixed f = 42.0;

	f += 1.0;
	EXPECT_EQ(43.0, f);

	f -= 1.0;
	EXPECT_EQ(42.0, f);

	f = f * 2.0;
	EXPECT_EQ(84.0, f);

	f *= 0.5;
	EXPECT_EQ(42.0, f);

	f /= 0.5;
	EXPECT_EQ(84.0, f);

	f /= 2.0;
	EXPECT_EQ(42.0, f);

	f %= 8.0;
	EXPECT_EQ(2.0, f);

	f %= 2.0;
	EXPECT_EQ(0.0, f);

	f = fea::fixed(15.0);
	f &= 3.0;
	EXPECT_EQ(3.0, f);

	f = fea::fixed(15.0);
	f &= 6.0;
	EXPECT_EQ(6.0, f);

	f &= 1.0;
	EXPECT_EQ(0.0, f);

	f = 6.0;
	f |= 1.0;
	EXPECT_EQ(7.0, f);

	f |= 8.0;
	EXPECT_EQ(15.0, f);

	f ^= 8.0;
	EXPECT_EQ(7.0, f);

	f ^= 4.0;
	EXPECT_EQ(3.0, f);

	f <<= 1;
	EXPECT_EQ(6.0, f);

	f >>= 2;
	EXPECT_EQ(1.5, f);

	f >>= 1;
	EXPECT_EQ(0.75, f);

	f <<= 2;
	EXPECT_EQ(3.0, f);

	f <<= 1;
	EXPECT_EQ(6.0, f);

	f = 3.0;
	f <<= fea::fixed(1.0);
	EXPECT_EQ(6.0, f);

	f >>= fea::fixed(2.0);
	EXPECT_EQ(1.5, f);

	f >>= fea::fixed(1.0);
	EXPECT_EQ(0.75, f);

	f <<= fea::fixed(2.0);
	EXPECT_EQ(3.0, f);

	f <<= fea::fixed(1.0);
	EXPECT_EQ(6.0, f);

	f = 1.0;
	EXPECT_EQ(2.0, ++f);
	EXPECT_EQ(3.0, ++f);
	EXPECT_EQ(3.0, f++);
	EXPECT_EQ(4.0, f++);
	EXPECT_EQ(5.0, f);

	EXPECT_EQ(4.0, --f);
	EXPECT_EQ(3.0, --f);
	EXPECT_EQ(3.0, f--);
	EXPECT_EQ(2.0, f--);
	EXPECT_EQ(1.0, f);
}
} // namespace
#include <cstdint>
#include <fea/math/moving_average.hpp>
#include <gtest/gtest.h>

namespace fea {
using namespace fea::moving_average::abbrev;
}

namespace {
#define TEST_FAIL_MSG "average.cpp : Failed unit test."

TEST(moving_average, ca_unbounded) {
	static_assert(fea::ca<double>::is_int_v == false, TEST_FAIL_MSG);
	static_assert(fea::ca<float>::is_int_v == false, TEST_FAIL_MSG);
	static_assert(fea::ca<int64_t>::is_int_v == true, TEST_FAIL_MSG);
	static_assert(fea::ca<int>::is_int_v == true, TEST_FAIL_MSG);

	static_assert(std::is_same_v<typename fea::ca<double>::mfloat_t, double>,
			TEST_FAIL_MSG);
	static_assert(std::is_same_v<typename fea::ca<float>::mfloat_t, float>,
			TEST_FAIL_MSG);
	static_assert(std::is_same_v<typename fea::ca<int64_t>::mfloat_t, double>,
			TEST_FAIL_MSG);
	static_assert(std::is_same_v<typename fea::ca<int>::mfloat_t, float>,
			TEST_FAIL_MSG);

	// unbounded
	{
		fea::cumulative_average<double> ca;
		EXPECT_EQ(ca.get(), 0.0);
		EXPECT_EQ(ca(0.5), 0.5);
		EXPECT_EQ(ca(0.5), 0.5);
		EXPECT_EQ(ca(0.5), 0.5);
		EXPECT_EQ(ca(0.5), 0.5);
		EXPECT_EQ(ca(0.5), 0.5);
		EXPECT_EQ(ca(0.5), 0.5);
		EXPECT_EQ(ca(0.5), 0.5);
		EXPECT_EQ(ca(0.5), 0.5);
		EXPECT_EQ(ca(0.5), 0.5);
		EXPECT_EQ(ca(0.5), 0.5);
		EXPECT_EQ(ca(0.5), 0.5);
		EXPECT_EQ(ca(0.5), 0.5);
		EXPECT_EQ(ca(0.5), 0.5);
		EXPECT_EQ(ca(0.5), 0.5);
		EXPECT_EQ(ca(0.5), 0.5);
		EXPECT_EQ(ca(0.5), 0.5);
		EXPECT_EQ(ca.get(), 0.5);
	}

	{
		fea::ca<double> ca;
		EXPECT_EQ(ca.get(), 0.0);
		EXPECT_EQ(ca(1.0), 1.0);
		EXPECT_EQ(ca(0.5), 0.75);
		EXPECT_EQ(ca(0.5), 2.0 / 3.0);
		EXPECT_EQ(ca(0.5), 0.625);
		EXPECT_EQ(ca(0.5), 0.6);
		ca(0.5);
		ca(0.5);
		EXPECT_EQ(ca(0.5), 0.5625);
		EXPECT_EQ(ca.get(), 0.5625);
	}

	{
		fea::ca<int> ca;
		EXPECT_EQ(ca.get(), 0);
		EXPECT_EQ(ca(1000), 1000);
		EXPECT_EQ(ca(500), 750);
		EXPECT_EQ(ca(500), 667);
		EXPECT_EQ(ca(500), 625);
		EXPECT_EQ(ca(500), 600);
		EXPECT_EQ(ca(500), 583);
		EXPECT_EQ(ca.get(), 583);
	}

	// bounded
	{
		fea::cumulative_average<double> ca(2);
		EXPECT_EQ(ca.get(), 0.0);
		EXPECT_EQ(ca(0.5), 0.5);
		EXPECT_EQ(ca(0.5), 0.5);
		EXPECT_EQ(ca(0.5), 0.5);
		EXPECT_EQ(ca(0.5), 0.5);
		EXPECT_EQ(ca.get(), 0.5);
	}

	{
		fea::ca<double> ca(2);
		EXPECT_EQ(ca.get(), 0.0);
		EXPECT_EQ(ca(1.0), 1.0);
		EXPECT_EQ(ca(0.5), 0.75);
		EXPECT_EQ(ca(0.5), 0.625);
		EXPECT_EQ(ca(0.5), 0.5625);
		EXPECT_EQ(ca(0.5), 0.53125);
		EXPECT_EQ(ca(0.5), 0.515625);
		EXPECT_EQ(ca.get(), 0.515625);
	}

	{
		fea::ca<double> ca(5);
		EXPECT_EQ(ca.get(), 0.0);
		EXPECT_EQ(ca(1.0), 1.0);
		EXPECT_EQ(ca(0.5), 0.75);
		EXPECT_EQ(ca(0.5), 2.0 / 3.0);
		EXPECT_EQ(ca(0.5), 0.625);
		EXPECT_EQ(ca(0.5), 0.6);
		EXPECT_EQ(ca(0.5), 0.58);
		EXPECT_EQ(ca.get(), 0.58);
	}

	{
		fea::ca<int> ca(2);
		EXPECT_EQ(ca.get(), 0);
		EXPECT_EQ(ca(1000), 1000);
		EXPECT_EQ(ca(500), 750);
		EXPECT_EQ(ca(500), 625);
		EXPECT_EQ(ca(500), 563);
		EXPECT_EQ(ca(500), 531);
		EXPECT_EQ(ca(500), 516);
		EXPECT_EQ(ca.get(), 516);
	}

	{
		fea::ca<int> ca(5);
		EXPECT_EQ(ca.get(), 0);
		EXPECT_EQ(ca(1000), 1000);
		EXPECT_EQ(ca(500), 750);
		EXPECT_EQ(ca(500), 667);
		EXPECT_EQ(ca(500), 625);
		EXPECT_EQ(ca(500), 600);
		EXPECT_EQ(ca(500), 580);
		EXPECT_EQ(ca.get(), 580);
	}
}


TEST(moving_average, sma) {
	static_assert(fea::sma<double, 10>::is_int_v == false, TEST_FAIL_MSG);
	static_assert(fea::sma<float, 10>::is_int_v == false, TEST_FAIL_MSG);
	static_assert(fea::sma<int64_t, 10>::is_int_v == true, TEST_FAIL_MSG);
	static_assert(fea::sma<int, 10>::is_int_v == true, TEST_FAIL_MSG);

	static_assert(
			std::is_same_v<typename fea::sma<double, 10>::mfloat_t, double>,
			TEST_FAIL_MSG);
	static_assert(std::is_same_v<typename fea::sma<float, 10>::mfloat_t, float>,
			TEST_FAIL_MSG);
	static_assert(
			std::is_same_v<typename fea::sma<int64_t, 10>::mfloat_t, double>,
			TEST_FAIL_MSG);
	static_assert(std::is_same_v<typename fea::sma<int, 10>::mfloat_t, float>,
			TEST_FAIL_MSG);

	{
		fea::simple_moving_average<double, 2> sma;
		EXPECT_EQ(sma.get(), 0.0);
		EXPECT_EQ(sma(0.5), 0.5);
		EXPECT_EQ(sma(0.5), 0.5);
		EXPECT_EQ(sma(0.5), 0.5);
		EXPECT_EQ(sma(0.5), 0.5);
		EXPECT_EQ(sma.get(), 0.5);
	}

	{
		fea::simple_moving_average<double, 2> sma;
		EXPECT_EQ(sma.get(), 0.0);
		EXPECT_EQ(sma(1.0), 1.0);
		EXPECT_EQ(sma(0.5), 0.75);
		EXPECT_EQ(sma(0.5), 0.5);
		EXPECT_EQ(sma(0.5), 0.5);
		EXPECT_EQ(sma(0.5), 0.5);
		EXPECT_EQ(sma(0.5), 0.5);
		EXPECT_EQ(sma(1.0), 0.75);
		EXPECT_EQ(sma(1.0), 1.0);
		EXPECT_EQ(sma(0.5), 0.75);
		EXPECT_EQ(sma(0.5), 0.5);
		EXPECT_EQ(sma(0.5), 0.5);
		EXPECT_EQ(sma.get(), 0.5);
	}

	{
		fea::simple_moving_average<double, 5> sma;
		EXPECT_EQ(sma.get(), 0.0);
		EXPECT_EQ(sma(1.0), 1.0);
		EXPECT_EQ(sma(0.5), 0.75);
		EXPECT_EQ(sma(0.5), 2.0 / 3.0);
		EXPECT_EQ(sma(0.5), 0.625);
		EXPECT_EQ(sma(0.5), 0.6);
		EXPECT_EQ(sma(0.5), 0.5);
		EXPECT_EQ(sma.get(), 0.5);
	}

	{
		fea::sma<int, 2> sma;
		EXPECT_EQ(sma.get(), 0);
		EXPECT_EQ(sma(1000), 1000);
		EXPECT_EQ(sma(500), 750);
		EXPECT_EQ(sma(500), 500);
		EXPECT_EQ(sma(500), 500);
		EXPECT_EQ(sma(500), 500);
		EXPECT_EQ(sma(500), 500);
		EXPECT_EQ(sma(500), 500);
		EXPECT_EQ(sma(1000), 750);
		EXPECT_EQ(sma(1000), 1000);
		EXPECT_EQ(sma(500), 750);
		EXPECT_EQ(sma(500), 500);
		EXPECT_EQ(sma(500), 500);
		EXPECT_EQ(sma(500), 500);
		EXPECT_EQ(sma(500), 500);
		EXPECT_EQ(sma.get(), 500);
	}

	{
		fea::sma<int, 5> sma;
		EXPECT_EQ(sma.get(), 0);
		EXPECT_EQ(sma(1000), 1000);
		EXPECT_EQ(sma(500), 750);
		EXPECT_EQ(sma(500), 667);
		EXPECT_EQ(sma(500), 625);
		EXPECT_EQ(sma(500), 600);
		EXPECT_EQ(sma(500), 500);
		EXPECT_EQ(sma.get(), 500);
	}
}

TEST(moving_average, ema) {
	static_assert(fea::ema<double>::is_int_v == false, TEST_FAIL_MSG);
	static_assert(fea::ema<float>::is_int_v == false, TEST_FAIL_MSG);
	static_assert(fea::ema<int64_t>::is_int_v == true, TEST_FAIL_MSG);
	static_assert(fea::ema<int>::is_int_v == true, TEST_FAIL_MSG);

	static_assert(std::is_same_v<typename fea::ema<double>::mfloat_t, double>,
			TEST_FAIL_MSG);
	static_assert(std::is_same_v<typename fea::ema<float>::mfloat_t, float>,
			TEST_FAIL_MSG);
	static_assert(std::is_same_v<typename fea::ema<int64_t>::mfloat_t, double>,
			TEST_FAIL_MSG);
	static_assert(std::is_same_v<typename fea::ema<int>::mfloat_t, float>,
			TEST_FAIL_MSG);

	{
		// Alpha 0.5, init avg 0.0.
		fea::exponential_moving_average<double> ema;
		EXPECT_EQ(ema.get(), 0.0);
		EXPECT_EQ(ema(0.5), 0.25);
		EXPECT_EQ(ema(0.5), 0.375);
		EXPECT_EQ(ema(0.5), 0.4375);
		EXPECT_EQ(ema(0.5), 0.46875);
		EXPECT_EQ(ema(0.5), 0.484375);
		EXPECT_EQ(ema(0.5), 0.4921875);
		EXPECT_EQ(ema.get(), 0.4921875);
	}

	{
		fea::ema<double> ema{ 0.5 };
		ema.prime(1.0);
		EXPECT_EQ(ema.get(), 1.0);
		EXPECT_EQ(ema(1.0), 1.0);
		EXPECT_EQ(ema(1.0), 1.0);
		EXPECT_EQ(ema(0.5), 0.75);
		EXPECT_EQ(ema(0.5), 0.625);
		EXPECT_EQ(ema(0.5), 0.5625);
		EXPECT_EQ(ema(0.5), 0.53125);
		EXPECT_EQ(ema(0.5), 0.515625);
		EXPECT_EQ(ema(0.5), 0.5078125);
		EXPECT_EQ(ema.get(), 0.5078125);
	}

	{
		fea::ema<int> ema{ 0.5 };
		ema.prime(1000);
		EXPECT_EQ(ema.get(), 1000);
		EXPECT_EQ(ema(1000), 1000);
		EXPECT_EQ(ema(500), 750);
		EXPECT_EQ(ema(500), 625);
		EXPECT_EQ(ema(500), 563);
		EXPECT_EQ(ema(500), 531);
		EXPECT_EQ(ema(500), 516);
		EXPECT_EQ(ema(500), 508);
		EXPECT_EQ(ema.get(), 508);
	}
}

TEST(moving_average, wma) {
	static_assert(fea::wma<double, 10>::is_int_v == false, TEST_FAIL_MSG);
	static_assert(fea::wma<float, 10>::is_int_v == false, TEST_FAIL_MSG);
	static_assert(fea::wma<int64_t, 10>::is_int_v == true, TEST_FAIL_MSG);
	static_assert(fea::wma<int, 10>::is_int_v == true, TEST_FAIL_MSG);

	static_assert(
			std::is_same_v<typename fea::wma<double, 10>::mfloat_t, double>,
			TEST_FAIL_MSG);
	static_assert(std::is_same_v<typename fea::wma<float, 10>::mfloat_t, float>,
			TEST_FAIL_MSG);
	static_assert(
			std::is_same_v<typename fea::wma<int64_t, 10>::mfloat_t, double>,
			TEST_FAIL_MSG);
	static_assert(std::is_same_v<typename fea::wma<int, 10>::mfloat_t, float>,
			TEST_FAIL_MSG);

	{
		fea::weighted_moving_average<double, 2> wma;
		EXPECT_EQ(wma.get(), 0.0);
		EXPECT_EQ(wma(0.5), 0.5);
		EXPECT_EQ(wma(0.5), 0.5);
		EXPECT_EQ(wma(0.5), 0.5);
		EXPECT_EQ(wma(0.5), 0.5);
		EXPECT_EQ(wma.get(), 0.5);
	}

	{
		fea::wma<double, 2> wma;
		EXPECT_EQ(wma.get(), 0.0);
		EXPECT_EQ(wma(1.0), 1.0);
		EXPECT_EQ(wma(0.5), 2.0 / 3.0);
		EXPECT_EQ(wma(0.5), 0.5);
		EXPECT_EQ(wma(0.5), 0.5);
		EXPECT_EQ(wma(0.5), 0.5);
		EXPECT_EQ(wma(0.5), 0.5);
		EXPECT_EQ(wma(1.0), 5.0 / 6.0);
		EXPECT_EQ(wma(1.0), 1.0);
		EXPECT_EQ(wma(0.5), 2.0 / 3.0);
		EXPECT_EQ(wma(0.5), 0.5);
		EXPECT_EQ(wma(0.5), 0.5);
		EXPECT_EQ(wma.get(), 0.5);
	}

	{
		fea::wma<double, 5> wma;
		EXPECT_EQ(wma.get(), 0.0);
		EXPECT_EQ(wma(1.0), 1.0);
		EXPECT_EQ(wma(0.5), 2.0 / 3.0);
		EXPECT_EQ(wma(0.5), 7.0 / 12.0);
		EXPECT_EQ(wma(0.5), 0.55);
		EXPECT_EQ(wma(0.5), 8.0 / 15.0);
		EXPECT_EQ(wma(0.5), 0.5);
		EXPECT_EQ(wma(0.5), 0.5);
		EXPECT_EQ(wma(1.0), 2.0 / 3.0);
		EXPECT_EQ(wma(1.0), 0.8);
		EXPECT_EQ(wma(1.0), 0.9);
		EXPECT_EQ(wma(1.0), 29.0 / 30.0);
		EXPECT_EQ(wma(1.0), 1.0);
		EXPECT_EQ(wma(1.0), 1.0);
		EXPECT_EQ(wma.get(), 1.0);
	}

	{
		fea::wma<int, 2> wma;
		EXPECT_EQ(wma.get(), 0);
		EXPECT_EQ(wma(1000), 1000);
		EXPECT_EQ(wma(500), 667);
		EXPECT_EQ(wma(500), 500);
		EXPECT_EQ(wma(500), 500);
		EXPECT_EQ(wma(500), 500);
		EXPECT_EQ(wma(500), 500);
		EXPECT_EQ(wma(500), 500);
		EXPECT_EQ(wma(1000), 833);
		EXPECT_EQ(wma(1000), 1000);
		EXPECT_EQ(wma(500), 667);
		EXPECT_EQ(wma(500), 500);
		EXPECT_EQ(wma(500), 500);
		EXPECT_EQ(wma(500), 500);
		EXPECT_EQ(wma(500), 500);
		EXPECT_EQ(wma.get(), 500);
	}

	{
		fea::wma<int, 5> wma;
		EXPECT_EQ(wma.get(), 0);
		EXPECT_EQ(wma(1000), 1000);
		EXPECT_EQ(wma(500), 667);
		EXPECT_EQ(wma(500), 583);
		EXPECT_EQ(wma(500), 550);
		EXPECT_EQ(wma(500), 533);
		EXPECT_EQ(wma(500), 500);
		EXPECT_EQ(wma(500), 500);
		EXPECT_EQ(wma(1000), 667);
		EXPECT_EQ(wma(1000), 800);
		EXPECT_EQ(wma(1000), 900);
		EXPECT_EQ(wma(1000), 967);
		EXPECT_EQ(wma(1000), 1000);
		EXPECT_EQ(wma(1000), 1000);
		EXPECT_EQ(wma.get(), 1000);
	}
}

TEST(moving_average, mm) {
	static_assert(fea::mm<double, 10>::is_int_v == false, TEST_FAIL_MSG);
	static_assert(fea::mm<float, 10>::is_int_v == false, TEST_FAIL_MSG);
	static_assert(fea::mm<int64_t, 10>::is_int_v == true, TEST_FAIL_MSG);
	static_assert(fea::mm<int, 10>::is_int_v == true, TEST_FAIL_MSG);

	static_assert(
			std::is_same_v<typename fea::mm<double, 10>::mfloat_t, double>,
			TEST_FAIL_MSG);
	static_assert(std::is_same_v<typename fea::mm<float, 10>::mfloat_t, float>,
			TEST_FAIL_MSG);
	static_assert(
			std::is_same_v<typename fea::mm<int64_t, 10>::mfloat_t, double>,
			TEST_FAIL_MSG);
	static_assert(std::is_same_v<typename fea::mm<int, 10>::mfloat_t, float>,
			TEST_FAIL_MSG);

	{
		fea::moving_median<double, 2> mm;
		EXPECT_EQ(mm.get(), 0.0);
		EXPECT_EQ(mm(0.5), 0.5);
		EXPECT_EQ(mm(0.5), 0.5);
		EXPECT_EQ(mm(0.5), 0.5);
		EXPECT_EQ(mm(0.5), 0.5);
		EXPECT_EQ(mm.get(), 0.5);
	}

	{
		fea::mm<double, 2> mm;
		EXPECT_EQ(mm.get(), 0.0);
		EXPECT_EQ(mm(1.0), 1.0);
		EXPECT_EQ(mm(0.5), 0.75);
		EXPECT_EQ(mm(0.5), 0.5);
		EXPECT_EQ(mm(0.5), 0.5);
		EXPECT_EQ(mm(0.5), 0.5);
		EXPECT_EQ(mm(0.5), 0.5);
		EXPECT_EQ(mm(1.0), 0.75);
		EXPECT_EQ(mm(1.0), 1.0);
		EXPECT_EQ(mm(0.5), 0.75);
		EXPECT_EQ(mm(0.5), 0.5);
		EXPECT_EQ(mm(0.5), 0.5);
		EXPECT_EQ(mm.get(), 0.5);
	}

	{
		fea::mm<double, 5> mm;
		EXPECT_EQ(mm.get(), 0.0);
		EXPECT_EQ(mm(1.0), 1.0);
		EXPECT_EQ(mm(0.5), 0.75);
		EXPECT_EQ(mm(0.5), 0.5);
		EXPECT_EQ(mm(0.5), 0.5);
		EXPECT_EQ(mm(0.5), 0.5);
		EXPECT_EQ(mm(0.5), 0.5);
		EXPECT_EQ(mm(0.5), 0.5);
		EXPECT_EQ(mm(1.0), 0.5);
		EXPECT_EQ(mm(1.0), 0.5);
		EXPECT_EQ(mm(1.0), 1.0);
		EXPECT_EQ(mm(1.0), 1.0);
		EXPECT_EQ(mm(1.0), 1.0);
		EXPECT_EQ(mm(1.0), 1.0);
		EXPECT_EQ(mm.get(), 1.0);
	}

	{
		fea::mm<int, 2> mm;
		EXPECT_EQ(mm.get(), 0);
		EXPECT_EQ(mm(1000), 1000);
		EXPECT_EQ(mm(500), 750);
		EXPECT_EQ(mm(500), 500);
		EXPECT_EQ(mm(500), 500);
		EXPECT_EQ(mm(500), 500);
		EXPECT_EQ(mm(500), 500);
		EXPECT_EQ(mm(500), 500);
		EXPECT_EQ(mm(1000), 750);
		EXPECT_EQ(mm(1000), 1000);
		EXPECT_EQ(mm(500), 750);
		EXPECT_EQ(mm(500), 500);
		EXPECT_EQ(mm(500), 500);
		EXPECT_EQ(mm(500), 500);
		EXPECT_EQ(mm(500), 500);
		EXPECT_EQ(mm.get(), 500);
	}

	{
		fea::mm<int, 5> mm;
		EXPECT_EQ(mm.get(), 0);
		EXPECT_EQ(mm(1000), 1000);
		EXPECT_EQ(mm(500), 750);
		EXPECT_EQ(mm(500), 500);
		EXPECT_EQ(mm(500), 500);
		EXPECT_EQ(mm(500), 500);
		EXPECT_EQ(mm(500), 500);
		EXPECT_EQ(mm(500), 500);
		EXPECT_EQ(mm(1000), 500);
		EXPECT_EQ(mm(1000), 500);
		EXPECT_EQ(mm(1000), 1000);
		EXPECT_EQ(mm(1000), 1000);
		EXPECT_EQ(mm(1000), 1000);
		EXPECT_EQ(mm(1000), 1000);
		EXPECT_EQ(mm.get(), 1000);
	}
}
} // namespace
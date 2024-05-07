#include <array>
#include <fea/math/statistics.hpp>
#include <fea/utils/platform.hpp>
#include <gtest/gtest.h>
#include <vector>

namespace {
TEST(statistics, basics) {
	static_assert(fea::fact(0) == 1, "math.cpp : unit test failed");
	static_assert(fea::fact(1) == 1, "math.cpp : unit test failed");
	static_assert(fea::fact(2) == 2, "math.cpp : unit test failed");
	static_assert(fea::fact(3) == 6, "math.cpp : unit test failed");
	static_assert(fea::fact(7) == 5040, "math.cpp : unit test failed");
	EXPECT_EQ(fea::fact(7.0), 5040.0);

	static_assert(
			fea::binomial_coeff(4, 2) == 6, "math.cpp : unit test failed");
	static_assert(
			fea::binomial_coeff(10, 3) == 120, "math.cpp : unit test failed");

	for (size_t i = 1; i < 10; ++i) {
		EXPECT_EQ(fea::binomial_coeff(i, i), 1u);
	}

	static_assert(fea::stars_and_bars_pos(10, 4) == 84,
			"math.cpp : unit test failed");
	static_assert(
			fea::stars_and_bars_pos(7, 3) == 15, "math.cpp : unit test failed");

#if FEA_64BIT
	// Overflows on 32 bits.
	EXPECT_EQ(fea::stars_and_bars_zero(size_t(10), size_t(4)), size_t(286));
#endif

	static_assert(fea::stars_and_bars_zero(5, 4) == 56,
			"math.cpp : unit test failed");

	{
		std::array<int, 4> arr{ 0, 1, 2, 3 };
		EXPECT_EQ(fea::sum(arr), 6);
	}

	{
		double revenue = 29.06;
		double cost = 8.74;
		double op_costs = 24.19;
		double net_costs = 24.86;

		double gross = fea::profit_margin(revenue, cost);
		EXPECT_NEAR(gross, 0.6992, 0.0001);

		double operating = fea::profit_margin(revenue, op_costs);
		EXPECT_NEAR(operating, 0.1676, 0.0001);

		double net = fea::profit_margin(revenue, net_costs);
		EXPECT_NEAR(net, 0.1445, 0.0001);
	}

	{
		std::vector<int> v{ 20, 70, 10, 40, 10 };
		EXPECT_EQ(fea::mean(v.begin(), v.end()), 30);
		EXPECT_EQ(fea::median(v.begin(), v.end()), 20);

		std::vector<int> v2{ 20, 70, 10, 40, 10 };
		EXPECT_EQ(v, v2);

		v = { 10, 8, 6, 9, 2, 3, 4, 5, 8 };
		EXPECT_EQ(fea::median(v.begin(), v.end()), 6);

		std::vector<double> vd = { 7.0, 4.0, 9.0, 3.0 };
		EXPECT_EQ(fea::mean(vd.begin(), vd.end()), 5.75);
		EXPECT_EQ(fea::median(vd.begin(), vd.end()), 5.5);

		vd = { 16, 15, 18, 20, 17, 19 };
		EXPECT_EQ(fea::median(vd.begin(), vd.end()), 17.5);

		vd = { 3, 15, 9, 2, 27, 24, 38, 26, 45, 21, 56, 16, 11, 55, 29, 22,
			60 };
		EXPECT_EQ(fea::median(vd.begin(), vd.end()), 24.0);

		vd = { 12, 3, 5, 9, 22, 37, 44, 51, 32, 2, 10, 25 };
		EXPECT_EQ(fea::median(vd.begin(), vd.end()), 17.0);
	}

	{
		std::vector<int> v{ 16, 3, 16, 6, 9, 27, 3, 27, 37, 16, 48 };

		std::vector<int> answer = fea::mode(v.begin(), v.end());
		EXPECT_EQ(answer.size(), 1u);
		EXPECT_EQ(answer.front(), 16);

		v = { 16, 3, 16, 6, 9, 27, 3, 27, 37, 16, 48, 0, 5, 3 };
		answer = fea::mode(v.begin(), v.end());
		EXPECT_EQ(answer.size(), 2u);
		for (auto val : answer) {
			EXPECT_TRUE(val == 16 || val == 3);
		}

		v = { 0, 1, 2, 3, 4 };
		answer = fea::mode(v.begin(), v.end());
		EXPECT_EQ(answer.size(), 0u);

		v = { 1, 4, 1, 2, 7, 1, 2, 5, 3, 6 };
		answer = fea::mode(v.begin(), v.end());
		EXPECT_EQ(answer.size(), 1u);
		EXPECT_EQ(answer.front(), 1);
	}

	// simple linear regression
	{
		std::vector<std::pair<float, float>> v{
			{ 0.f, 0.f },
			{ 1.f, 1.f },
			{ 2.f, 2.f },
		};
		std::pair<float, float> ab
				= fea::simple_linear_regression(v.begin(), v.end());
		EXPECT_EQ(ab.first, 0.f);
		EXPECT_EQ(ab.second, 1.f);

		v = {
			{ 0.f, 0.f },
			{ 1.f, 1.f },
			{ 5.f, 5.f },
		};
		ab = fea::simple_linear_regression(v.begin(), v.end());
		EXPECT_EQ(ab.first, 0.f);
		EXPECT_EQ(ab.second, 1.f);

		v = {
			{ 0.f, 0.f },
			{ 1.f, 1.f },
			{ 2.f, -2.f },
		};
		ab = fea::simple_linear_regression(v.begin(), v.end());
		EXPECT_NEAR(ab.first, 2.f / 3.f, 0.000001f);
		EXPECT_EQ(ab.second, -1.f);

		v = {
			{ 0.f, 0.f },
			{ 1.f, 1.f },
			{ -2.f, -2.f },
		};
		ab = fea::simple_linear_regression(v.begin(), v.end());
		EXPECT_EQ(ab.first, 0.f);
		EXPECT_EQ(ab.second, 1.f);

		v = {
			{ 0.f, 0.f },
			{ -1.f, 1.f },
			{ 2.f, -2.f },
		};
		ab = fea::simple_linear_regression(v.begin(), v.end());
		EXPECT_EQ(ab.first, 0.f);
		EXPECT_EQ(ab.second, -1.f);

		v = {
			{ 0.f, 0.f },
			{ -1.f, 22.f },
			{ 2.f, -2.f },
		};
		ab = fea::simple_linear_regression(v.begin(), v.end());
		EXPECT_EQ(ab.first, 9.f);
		EXPECT_EQ(ab.second, -7.f);
	}
}
} // namespace

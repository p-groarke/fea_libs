#include <array>
#include <fea/math/statistics.hpp>
#include <fea/utils/platform.hpp>
#include <gtest/gtest.h>
#include <vector>

namespace {
// TODO
// struct vec2 {
//	float x = 0.f;
//	float y = 0.f;
// };

TEST(statistics, basics) {
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

	// variance
	{
		std::vector<float> v(6);
		std::iota(v.begin(), v.end(), 1.f);
		float answer = fea::variance(v.begin(), v.end());
		EXPECT_NEAR(answer, 2.91666675f, 0.000001f);
		answer = fea::variance(v.begin(), v.end(), [](float f) { return f; });
		EXPECT_NEAR(answer, 2.91666675f, 0.000001f);

		answer = fea::sample_variance(v.begin(), v.end());
		EXPECT_EQ(answer, 3.5f);
		answer = fea::sample_variance(
				v.begin(), v.end(), [](float f) { return f; });
		EXPECT_EQ(answer, 3.5f);

		std::vector<int> v2(6);
		std::iota(v2.begin(), v2.end(), 1);
		int answer2 = fea::variance(v2.begin(), v2.end());
		EXPECT_EQ(answer2, 3);
		answer2 = fea::variance(v2.begin(), v2.end(), [](int i) { return i; });
		EXPECT_EQ(answer2, 3);

		answer2 = fea::sample_variance(v2.begin(), v2.end());
		EXPECT_EQ(answer2, 4);
		answer2 = fea::sample_variance(
				v2.begin(), v2.end(), [](int i) { return i; });
		EXPECT_EQ(answer2, 4);
	}

	// standard deviation
	{
		std::vector<float> v{ 2.f, 4.f, 4.f, 4.f, 5.f, 5.f, 7.f, 9.f };
		float answer = fea::std_deviation(v.begin(), v.end());
		EXPECT_EQ(answer, 2.f);
		answer = fea::std_deviation(
				v.begin(), v.end(), [](float f) { return f; });
		EXPECT_EQ(answer, 2.f);

		answer = fea::sample_std_deviation(v.begin(), v.end());
		EXPECT_NEAR(answer, 2.13808990f, 0.000001f);
		answer = fea::sample_std_deviation(
				v.begin(), v.end(), [](float f) { return f; });
		EXPECT_NEAR(answer, 2.13808990f, 0.000001f);
	}
	{
		std::vector<int> v{ 2, 4, 4, 4, 5, 5, 7, 9 };
		int answer = fea::std_deviation(v.begin(), v.end());
		EXPECT_EQ(answer, 2);
		answer = fea::std_deviation(
				v.begin(), v.end(), [](int i) { return i; });
		EXPECT_EQ(answer, 2);

		answer = fea::sample_std_deviation(v.begin(), v.end());
		EXPECT_EQ(answer, 2);
		float answer2 = fea::sample_std_deviation(
				v.begin(), v.end(), [](int i) { return float(i); });
		EXPECT_NEAR(answer2, 2.13808990f, 0.000001f);
	}

	// sigma filter
	{
		std::vector<float> v{ 2.f, 4.f, 4.f, 4.f, 5.f, 5.f, 7.f, 9.f };
		const std::vector<float> expected{ 2.f, 4.f, 4.f, 4.f, 5.f, 5.f, 7.f };
		std::vector<float> answers;
		fea::sigma_filter(v.begin(), v.end(), 2.f,
				[&](float val) { answers.push_back(val); });
		EXPECT_EQ(answers.size(), 7u);
		EXPECT_EQ(answers, expected);

		float std_dev = fea::std_deviation(v.begin(), v.end());
		float avg = fea::mean(v.begin(), v.end());
		float low = avg - 2.f * std_dev;
		float high = avg + 2.f * std_dev;
		for (float val : answers) {
			EXPECT_TRUE(low < val && val < high);
		}

		answers.clear();
		const std::vector<float> sample_expected{ 2.f, 4.f, 4.f, 4.f, 5.f, 5.f,
			7.f, 9.f };
		fea::sample_sigma_filter(v.begin(), v.end(), 2.f,
				[&](float val) { answers.push_back(val); });
		EXPECT_EQ(answers.size(), 8u);
		EXPECT_EQ(answers, sample_expected);

		std_dev = fea::sample_std_deviation(v.begin(), v.end());
		avg = fea::mean(v.begin(), v.end());
		low = avg - 2.f * std_dev;
		high = avg + 2.f * std_dev;
		for (float val : answers) {
			EXPECT_TRUE(low < val && val < high);
		}
	}
	{
		std::vector<int> v{ 2, 4, 4, 4, 5, 5, 7, 9 };
		const std::vector<int> expected{ 2, 4, 4, 4, 5, 5, 7 };
		std::vector<int> answers;
		fea::sigma_filter(v.begin(), v.end(), 2.f,
				[&](int val) { answers.push_back(val); });
		EXPECT_EQ(answers.size(), 7u);
		EXPECT_EQ(answers, expected);

		float std_dev = fea::std_deviation(
				v.begin(), v.end(), [](int val) { return float(val); });
		float avg = fea::mean(
				v.begin(), v.end(), [](int val) { return float(val); });
		float low = avg - 2.f * std_dev;
		float high = avg + 2.f * std_dev;
		for (int val : answers) {
			EXPECT_TRUE(low < float(val) && float(val) < high);
		}

		answers.clear();
		const std::vector<int> sample_expected{ 2, 4, 4, 4, 5, 5, 7, 9 };
		fea::sample_sigma_filter(v.begin(), v.end(), 2.f,
				[&](int val) { answers.push_back(val); });
		EXPECT_EQ(answers.size(), 8u);
		EXPECT_EQ(answers, sample_expected);

		std_dev = fea::sample_std_deviation(
				v.begin(), v.end(), [](int val) { return float(val); });
		avg = fea::mean(v.begin(), v.end(), [](int val) { return float(val); });
		low = avg - 2.f * std_dev;
		high = avg + 2.f * std_dev;
		for (int val : answers) {
			EXPECT_TRUE(low < float(val) && float(val) < high);
		}
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

	// extra
	{
		static_assert(fea::fact(0) == 1, "math.cpp : unit test failed");
		static_assert(fea::fact(1) == 1, "math.cpp : unit test failed");
		static_assert(fea::fact(2) == 2, "math.cpp : unit test failed");
		static_assert(fea::fact(3) == 6, "math.cpp : unit test failed");
		static_assert(fea::fact(7) == 5040, "math.cpp : unit test failed");
		EXPECT_EQ(fea::fact(7.0), 5040.0);

		static_assert(
				fea::binomial_coeff(4, 2) == 6, "math.cpp : unit test failed");
		static_assert(fea::binomial_coeff(10, 3) == 120,
				"math.cpp : unit test failed");

		for (size_t i = 1; i < 10; ++i) {
			EXPECT_EQ(fea::binomial_coeff(i, i), 1u);
		}
	}

	// stars and bars
	{
		static_assert(fea::stars_and_bars_pos(10, 4) == 84,
				"math.cpp : unit test failed");
		static_assert(fea::stars_and_bars_pos(7, 3) == 15,
				"math.cpp : unit test failed");

#if FEA_64BIT
		// Overflows on 32 bits.
		EXPECT_EQ(fea::stars_and_bars_zero(size_t(10), size_t(4)), size_t(286));
#endif

		static_assert(fea::stars_and_bars_zero(5, 4) == 56,
				"math.cpp : unit test failed");
	}
}
} // namespace

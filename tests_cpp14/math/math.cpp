#include <fea/math/math.hpp>
#include <gtest/gtest.h>

namespace {
TEST(math, basics) {
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
		EXPECT_EQ(fea::binomial_coeff(i, i), 1);
	}

	static_assert(fea::stars_and_bars_pos(10, 4) == 84,
			"math.cpp : unit test failed");
	static_assert(
			fea::stars_and_bars_pos(7, 3) == 15, "math.cpp : unit test failed");

	EXPECT_EQ(fea::stars_and_bars_zero(size_t(10), size_t(4)), size_t(286));
	static_assert(fea::stars_and_bars_zero(5, 4) == 56,
			"math.cpp : unit test failed");
}
} // namespace

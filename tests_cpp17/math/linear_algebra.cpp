#include <fea/math/linear_algebra.hpp>
#include <gtest/gtest.h>

namespace {
TEST(linear_algebra, basics) {
	EXPECT_EQ(fea::magnitude(0.0, 1.0), 1.0);
	EXPECT_EQ(fea::magnitude(1.0, 0.0), 1.0);
	EXPECT_NEAR(fea::magnitude(1.0, 1.0), 1.4142135623730950488016887242097,
			0.000001);

	double x = 1.0;
	double y = 0.0;
	fea::normalize(x, y);
	EXPECT_EQ(x, 1.0);
	EXPECT_EQ(y, 0.0);

	x = 0.0;
	y = 1.0;
	fea::normalize(x, y);
	EXPECT_EQ(x, 0.0);
	EXPECT_EQ(y, 1.0);

	x = 1.0;
	y = 1.0;
	fea::normalize(x, y);
	EXPECT_NEAR(x, 0.70710678118654752440084436210485, 0.000001);
	EXPECT_NEAR(y, 0.70710678118654752440084436210485, 0.000001);

	EXPECT_EQ(fea::divide_round(10u, 10u), 1u);
	EXPECT_EQ(fea::divide_round(10u, 9u), 1u);
	EXPECT_EQ(fea::divide_round(10u, 8u), 1u);
	EXPECT_EQ(fea::divide_round(10u, 7u), 1u);
	EXPECT_EQ(fea::divide_round(10u, 6u), 2u);
	EXPECT_EQ(fea::divide_round(10u, 5u), 2u);
	EXPECT_EQ(fea::divide_round(10u, 4u), 3u); // 2.5
	EXPECT_EQ(fea::divide_round(10u, 3u), 3u);
	EXPECT_EQ(fea::divide_round(10u, 2u), 5u);
	EXPECT_EQ(fea::divide_round(10u, 1u), 10u);
}
} // namespace
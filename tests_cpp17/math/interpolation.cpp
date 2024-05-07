#include <fea/math/interpolation.hpp>
#include <gtest/gtest.h>

namespace {
TEST(interpolation, basics) {
	EXPECT_EQ(fea::percentage(0.5, 0.0, 1.0), 0.5);
	EXPECT_EQ(fea::percentage(0.5, 0.0, 0.0), 0.0);
	EXPECT_EQ(fea::percentage(0.5, 1.0, 2.0), -0.5);

	EXPECT_EQ(fea::lerp(0.0, 2.0, 4.0), 2.0);
	EXPECT_EQ(fea::lerp(0.5, 2.0, 4.0), 3.0);
	EXPECT_EQ(fea::lerp(1.0, 2.0, 4.0), 4.0);

	EXPECT_EQ(fea::lerp(1.0, 1.0, 2.0, 2.0, 3.0), 2.0);
	EXPECT_EQ(fea::lerp(1.5, 1.0, 2.0, 2.0, 3.0), 2.5);
	EXPECT_EQ(fea::lerp(2.0, 1.0, 2.0, 2.0, 3.0), 3.0);

	EXPECT_EQ(fea::lerp_linquadlog(0.0, 0.0, 0.0, 1.0), 0.0);
	EXPECT_EQ(fea::lerp_linquadlog(0.5, 0.0, 0.0, 1.0), 0.5);
	EXPECT_EQ(fea::lerp_linquadlog(1.0, 0.0, 0.0, 1.0), 1.0);
	EXPECT_EQ(fea::lerp_linquadlog(0.0, 1.0, 0.0, 1.0), 0.0);
	EXPECT_NEAR(fea::lerp_linquadlog(0.5, 1.0, 0.0, 1.0), 0.37754066879814552,
			0.000001);
	EXPECT_EQ(fea::lerp_linquadlog(1.0, 1.0, 0.0, 1.0), 1.0);
	EXPECT_EQ(fea::lerp_linquadlog(0.0, -1.0, 0.0, 1.0), 0.0);
	EXPECT_NEAR(fea::lerp_linquadlog(0.5, -1.0, 0.0, 1.0), 0.62245933120185459,
			0.000001);
	EXPECT_EQ(fea::lerp_linquadlog(1.0, -1.0, 0.0, 1.0), 1.0);
}
} // namespace
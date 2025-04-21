#include <fea/utility/bitmask.hpp>
#include <gtest/gtest.h>

namespace {
enum class bm {
	zero = 0b0000,
	one = 0b0001,
	two = 0b0010,
	three = 0b0100,
	four = 0b1000,
	all_set = 0b1111,
};
} // namespace
namespace fea {
FEA_REGISTER_BITMASK(bm);
}

namespace {
static_assert(fea::register_bitmask<bm>::value,
		"utils : fea::register_bitmask type-trait should be true");
static_assert(fea::is_bitmask_v<bm>,
		"utils : fea::register_bitmask type-trait should be true");

TEST(utils, bitmask) {

	EXPECT_EQ(size_t(bm::one | bm::one), 0b0001u);
	EXPECT_EQ(size_t(bm::one | bm::two), 0b0011u);
	EXPECT_EQ(bm::one | bm::two | bm::three | bm::four, bm::all_set);
	EXPECT_EQ(size_t(bm::one & bm::two), 0b0000u);
	EXPECT_EQ(size_t(bm::one & bm::one), 0b0001u);
	EXPECT_EQ(size_t((bm::one | bm::two) & bm::all_set), 0b0011u);
	EXPECT_EQ(size_t(bm::one ^ bm::two), 0b0011u);
	EXPECT_EQ(size_t(bm::one ^ bm::one), 0b0000u);
	EXPECT_EQ(size_t(~bm::all_set & bm::all_set), 0b0000u);
	EXPECT_EQ(size_t(~bm::one & bm::all_set), 0b1110u);
	EXPECT_EQ(size_t(bm::one >> 1), 0b0000u);
	EXPECT_EQ(size_t(bm::one << 1), 0b0010u);
	EXPECT_EQ(size_t(bm::all_set >> 1), 0b0111u);
	EXPECT_EQ(size_t(bm::all_set << 1), 0b11110u);
	EXPECT_TRUE(!bm::zero);
	EXPECT_FALSE(!!bm::zero);
	EXPECT_FALSE(!bm::one);
	EXPECT_TRUE(!!bm::one);

	bm t = bm::one;

	t |= bm::two;
	EXPECT_EQ(size_t(t), 0b0011u);
	t |= bm::three;
	t |= bm::four;
	EXPECT_EQ(t, bm::all_set);
	t &= bm::all_set;
	EXPECT_EQ(t, bm::all_set);
	t &= bm::one;
	EXPECT_EQ(t, bm::one);
	t ^= bm::two;
	EXPECT_EQ(size_t(t), 0b0011u);
	t ^= bm::one;
	EXPECT_EQ(size_t(t), 0b0010u);
	t = bm::all_set;
	t <<= 1;
	EXPECT_EQ(size_t(t), 0b11110u);
	t >>= 2;
	EXPECT_EQ(size_t(t), 0b0111u);
}

} // namespace

﻿#include <algorithm>
#include <fea/enum/utility.hpp>
#include <fea/numerics/literals.hpp>
#include <gtest/gtest.h>

namespace {
#define FAILMSG "utility.cpp : Unit test failed."

enum class e1 : uint8_t {
	bla,
	blee,
};
enum class e2 : uint16_t {
	bla,
	blee,
};
enum class e3 : int {
	bla,
	blee,
};

TEST(enum_utility, basics) {
	using namespace fea::literals;

	using ret1_t = decltype(fea::to_underlying(e1{}));
	static_assert(std::is_same_v<ret1_t, uint8_t>, FAILMSG);

	using ret2_t = decltype(fea::to_underlying(e2{}));
	static_assert(std::is_same_v<ret2_t, uint16_t>, FAILMSG);

	using ret3_t = decltype(fea::to_underlying(e3{}));
	static_assert(std::is_same_v<ret3_t, int>, FAILMSG);

	EXPECT_EQ(fea::to_underlying(e1::bla), 0_u8);
	EXPECT_EQ(fea::to_underlying(e1::blee), 1_u8);
	EXPECT_EQ(fea::to_underlying(e2::bla), 0_u16);
	EXPECT_EQ(fea::to_underlying(e2::blee), 1_u16);
	EXPECT_EQ(fea::to_underlying(e3::bla), 0_i32);
	EXPECT_EQ(fea::to_underlying(e3::blee), 1_i32);
}
} // namespace

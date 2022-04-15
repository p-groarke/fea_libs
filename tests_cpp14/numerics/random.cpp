#include <fea/numerics/random.hpp>
#include <fea/utils/unused.hpp>
#include <gtest/gtest.h>

namespace {
enum class e {
	a,
	b,
	c,
	count,
};
TEST(random, basics) {
	{
		uint8_t mmin = 0;
		uint8_t mmax = fea::random_int<uint8_t>();

		for (size_t i = 0; i < 100; ++i) {
			uint8_t v = fea::random_int(mmin, mmax);
			EXPECT_LE(v, mmax);
			EXPECT_GE(v, mmin);
		}
	}

	{
		for (size_t i = 0; i < 100; ++i) {
			size_t count = fea::random_idx(10u);
			EXPECT_LE(count, 9u);
			EXPECT_GE(count, 0u);
		}
	}

	{
		std::array<uint8_t, 4> bytes = fea::random_bytes<4>();
		fea::unused(bytes); // how to test?
		std::vector<uint8_t> vbytes = fea::random_bytes(4);
		EXPECT_EQ(vbytes.size(), 4u);
	}

	{
		// fuzz
		for (size_t i = 0; i < 100; ++i) {
			e v = fea::random_enum<e>();
			EXPECT_NE(v, e::count);
		}

		for (size_t i = 0; i < 100; ++i) {
			e v = fea::random_enum(e::a, e::b);
			EXPECT_TRUE(v == e::a || v == e::b);
		}
	}
}
} // namespace

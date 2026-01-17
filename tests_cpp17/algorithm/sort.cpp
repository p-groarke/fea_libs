#include <algorithm>
#include <fea/algorithm/sort.hpp>
#include <fea/numerics/random.hpp>
#include <gtest/gtest.h>
#include <vector>

namespace {

TEST(sort, basics) {
	std::vector<uint8_t> vals(42);
	fea::random_fill(vals.begin(), vals.end());
	std::vector<uint8_t> cmp = vals;

	fea::radix_sort(vals.begin(), vals.end());
	std::sort(cmp.begin(), cmp.end());

	EXPECT_EQ(vals, cmp);
}
} // namespace
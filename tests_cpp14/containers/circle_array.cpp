#include <fea/containers/circle_array.hpp>
#include <gtest/gtest.h>

namespace {

TEST(circle_array, basics) {
	constexpr size_t arr_size = 10;
	fea::circle_array<int, arr_size> carr{};
	for (int v = 0; v < arr_size * 3; ++v) {
		carr.push(v);

		if (v < arr_size) {
			EXPECT_EQ(carr.get(), 0);
		} else {
			EXPECT_EQ(carr.get(), v - arr_size);
		}
	}
}
} // namespace

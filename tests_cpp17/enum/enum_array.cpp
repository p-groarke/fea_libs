#include <algorithm>
#include <fea/enum/enum_array.hpp>
#include <gtest/gtest.h>

namespace {
TEST(enum_array, basics) {
	enum class e { zero, one, two, three, four, five, count };
	{
		fea::enum_array<size_t, e> arr{};
		EXPECT_EQ(arr.size(), size_t(e::count));
		EXPECT_TRUE(std::all_of(
				arr.begin(), arr.end(), [](size_t s) { return s == 0; }));

		for (size_t i = 0; i < arr.size(); ++i) {
			arr[e(i)] = i;
		}

		EXPECT_EQ(arr[e::zero], 0u);
		EXPECT_EQ(arr[e::one], 1u);
		EXPECT_EQ(arr[e::two], 2u);
		EXPECT_EQ(arr[e::three], 3u);
		EXPECT_EQ(arr[e::four], 4u);
		EXPECT_EQ(arr[e::five], 5u);

		EXPECT_EQ(arr.at(e::zero), 0u);
		EXPECT_EQ(arr.at(e::one), 1u);
		EXPECT_EQ(arr.at(e::two), 2u);
		EXPECT_EQ(arr.at(e::three), 3u);
		EXPECT_EQ(arr.at(e::four), 4u);
		EXPECT_EQ(arr.at(e::five), 5u);

		EXPECT_EQ(arr.at<e::zero>(), 0u);
		EXPECT_EQ(arr.at<e::one>(), 1u);
		EXPECT_EQ(arr.at<e::two>(), 2u);
		EXPECT_EQ(arr.at<e::three>(), 3u);
		EXPECT_EQ(arr.at<e::four>(), 4u);
		EXPECT_EQ(arr.at<e::five>(), 5u);

#if FEA_CPP17
		EXPECT_EQ(fea::get<e::zero>(arr), 0u);
		EXPECT_EQ(fea::get<e::one>(arr), 1u);
		EXPECT_EQ(fea::get<e::two>(arr), 2u);
		EXPECT_EQ(fea::get<e::three>(arr), 3u);
		EXPECT_EQ(fea::get<e::four>(arr), 4u);
		EXPECT_EQ(fea::get<e::five>(arr), 5u);
#endif
	}

	{
		fea::enum_array<size_t, e, size_t(e::count)> arr{};
		EXPECT_EQ(arr.size(), size_t(e::count));
		EXPECT_TRUE(std::all_of(
				arr.begin(), arr.end(), [](size_t s) { return s == 0; }));

		for (size_t i = 0; i < arr.size(); ++i) {
			arr[e(i)] = i;
		}

		EXPECT_EQ(arr[e::zero], 0u);
		EXPECT_EQ(arr[e::one], 1u);
		EXPECT_EQ(arr[e::two], 2u);
		EXPECT_EQ(arr[e::three], 3u);
		EXPECT_EQ(arr[e::four], 4u);
		EXPECT_EQ(arr[e::five], 5u);

		EXPECT_EQ(arr.at(e::zero), 0u);
		EXPECT_EQ(arr.at(e::one), 1u);
		EXPECT_EQ(arr.at(e::two), 2u);
		EXPECT_EQ(arr.at(e::three), 3u);
		EXPECT_EQ(arr.at(e::four), 4u);
		EXPECT_EQ(arr.at(e::five), 5u);

		EXPECT_EQ(arr.at<e::zero>(), 0u);
		EXPECT_EQ(arr.at<e::one>(), 1u);
		EXPECT_EQ(arr.at<e::two>(), 2u);
		EXPECT_EQ(arr.at<e::three>(), 3u);
		EXPECT_EQ(arr.at<e::four>(), 4u);
		EXPECT_EQ(arr.at<e::five>(), 5u);

#if FEA_CPP17
		EXPECT_EQ(fea::get<e::zero>(arr), 0u);
		EXPECT_EQ(fea::get<e::one>(arr), 1u);
		EXPECT_EQ(fea::get<e::two>(arr), 2u);
		EXPECT_EQ(fea::get<e::three>(arr), 3u);
		EXPECT_EQ(fea::get<e::four>(arr), 4u);
		EXPECT_EQ(fea::get<e::five>(arr), 5u);
#endif
	}
}
} // namespace

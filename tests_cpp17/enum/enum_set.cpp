#include <algorithm>
#include <fea/enum/enum_set.hpp>
#include <gtest/gtest.h>

namespace {
enum class e { zero, one, two, three, four, five, count };

TEST(enum_set, basics) {
	{
		fea::enum_set<e> set{};
		EXPECT_EQ(set.size(), size_t(e::count));
		EXPECT_TRUE(std::all_of(
				set.begin(), set.end(), [](bool b) { return b == false; }));

		for (size_t i = 0; i < set.size(); ++i) {
			set[e(i)] = i % 2 == 0;
		}

		EXPECT_EQ(set[e::zero], true);
		EXPECT_EQ(set[e::one], false);
		EXPECT_EQ(set[e::two], true);
		EXPECT_EQ(set[e::three], false);
		EXPECT_EQ(set[e::four], true);
		EXPECT_EQ(set[e::five], false);

		EXPECT_EQ(set.at(e::zero), true);
		EXPECT_EQ(set.at(e::one), false);
		EXPECT_EQ(set.at(e::two), true);
		EXPECT_EQ(set.at(e::three), false);
		EXPECT_EQ(set.at(e::four), true);
		EXPECT_EQ(set.at(e::five), false);

		EXPECT_EQ(set.at<e::zero>(), true);
		EXPECT_EQ(set.at<e::one>(), false);
		EXPECT_EQ(set.at<e::two>(), true);
		EXPECT_EQ(set.at<e::three>(), false);
		EXPECT_EQ(set.at<e::four>(), true);
		EXPECT_EQ(set.at<e::five>(), false);

		EXPECT_EQ(fea::get<e::zero>(set), true);
		EXPECT_EQ(fea::get<e::one>(set), false);
		EXPECT_EQ(fea::get<e::two>(set), true);
		EXPECT_EQ(fea::get<e::three>(set), false);
		EXPECT_EQ(fea::get<e::four>(set), true);
		EXPECT_EQ(fea::get<e::five>(set), false);
	}

	{
		fea::enum_set<e, size_t(e::count)> set{};
		EXPECT_EQ(set.size(), size_t(e::count));
		EXPECT_TRUE(std::all_of(
				set.begin(), set.end(), [](bool b) { return b == false; }));

		for (size_t i = 0; i < set.size(); ++i) {
			set[e(i)] = i % 2 == 0;
		}

		EXPECT_EQ(set[e::zero], true);
		EXPECT_EQ(set[e::one], false);
		EXPECT_EQ(set[e::two], true);
		EXPECT_EQ(set[e::three], false);
		EXPECT_EQ(set[e::four], true);
		EXPECT_EQ(set[e::five], false);

		EXPECT_EQ(set.at(e::zero), true);
		EXPECT_EQ(set.at(e::one), false);
		EXPECT_EQ(set.at(e::two), true);
		EXPECT_EQ(set.at(e::three), false);
		EXPECT_EQ(set.at(e::four), true);
		EXPECT_EQ(set.at(e::five), false);

		EXPECT_EQ(set.at<e::zero>(), true);
		EXPECT_EQ(set.at<e::one>(), false);
		EXPECT_EQ(set.at<e::two>(), true);
		EXPECT_EQ(set.at<e::three>(), false);
		EXPECT_EQ(set.at<e::four>(), true);
		EXPECT_EQ(set.at<e::five>(), false);

		EXPECT_EQ(fea::get<e::zero>(set), true);
		EXPECT_EQ(fea::get<e::one>(set), false);
		EXPECT_EQ(fea::get<e::two>(set), true);
		EXPECT_EQ(fea::get<e::three>(set), false);
		EXPECT_EQ(fea::get<e::four>(set), true);
		EXPECT_EQ(fea::get<e::five>(set), false);
	}
}

TEST(enum_set, ctor) {
	// fancy ctor
	{
		constexpr fea::enum_set<e, size_t(e::count)> set{ e::zero, e::two,
			e::four };

		EXPECT_EQ(set.size(), size_t(e::count));

		EXPECT_EQ(set[e::zero], true);
		EXPECT_EQ(set[e::one], false);
		EXPECT_EQ(set[e::two], true);
		EXPECT_EQ(set[e::three], false);
		EXPECT_EQ(set[e::four], true);
		EXPECT_EQ(set[e::five], false);

		EXPECT_EQ(set.at(e::zero), true);
		EXPECT_EQ(set.at(e::one), false);
		EXPECT_EQ(set.at(e::two), true);
		EXPECT_EQ(set.at(e::three), false);
		EXPECT_EQ(set.at(e::four), true);
		EXPECT_EQ(set.at(e::five), false);

		EXPECT_EQ(set.at<e::zero>(), true);
		EXPECT_EQ(set.at<e::one>(), false);
		EXPECT_EQ(set.at<e::two>(), true);
		EXPECT_EQ(set.at<e::three>(), false);
		EXPECT_EQ(set.at<e::four>(), true);
		EXPECT_EQ(set.at<e::five>(), false);

		EXPECT_EQ(fea::get<e::zero>(set), true);
		EXPECT_EQ(fea::get<e::one>(set), false);
		EXPECT_EQ(fea::get<e::two>(set), true);
		EXPECT_EQ(fea::get<e::three>(set), false);
		EXPECT_EQ(fea::get<e::four>(set), true);
		EXPECT_EQ(fea::get<e::five>(set), false);
	}

	// old ctor
	{
		constexpr fea::enum_set<e, size_t(e::count)> set{ true, false, true,
			false, true, false };

		EXPECT_EQ(set.size(), size_t(e::count));

		EXPECT_EQ(set[e::zero], true);
		EXPECT_EQ(set[e::one], false);
		EXPECT_EQ(set[e::two], true);
		EXPECT_EQ(set[e::three], false);
		EXPECT_EQ(set[e::four], true);
		EXPECT_EQ(set[e::five], false);

		EXPECT_EQ(set.at(e::zero), true);
		EXPECT_EQ(set.at(e::one), false);
		EXPECT_EQ(set.at(e::two), true);
		EXPECT_EQ(set.at(e::three), false);
		EXPECT_EQ(set.at(e::four), true);
		EXPECT_EQ(set.at(e::five), false);

		EXPECT_EQ(set.at<e::zero>(), true);
		EXPECT_EQ(set.at<e::one>(), false);
		EXPECT_EQ(set.at<e::two>(), true);
		EXPECT_EQ(set.at<e::three>(), false);
		EXPECT_EQ(set.at<e::four>(), true);
		EXPECT_EQ(set.at<e::five>(), false);

		EXPECT_EQ(fea::get<e::zero>(set), true);
		EXPECT_EQ(fea::get<e::one>(set), false);
		EXPECT_EQ(fea::get<e::two>(set), true);
		EXPECT_EQ(fea::get<e::three>(set), false);
		EXPECT_EQ(fea::get<e::four>(set), true);
		EXPECT_EQ(fea::get<e::five>(set), false);
	}
}
} // namespace

﻿#include <fea/maps/unsigned_map.hpp>
#include <fea/utils/platform.hpp>
#include <gtest/gtest.h>
#include <memory>
#include <unordered_map>

namespace {
struct test {
	test() = default;
	~test() = default;
	test(const test&) = default;
	test(test&&) = default;
	test& operator=(const test&) = default;
	test& operator=(test&&) = default;

	test(size_t v)
			: val(v) {
	}

	size_t val = 42;
};
bool operator==(const test& lhs, const test& rhs) {
	return lhs.val == rhs.val;
}
bool operator!=(const test& lhs, const test& rhs) {
	return !operator==(lhs, rhs);
}

TEST(unsigned_map, basics) {
	constexpr size_t small_num = 10;

	fea::unsigned_map<size_t, test> map1{ small_num };
	map1.reserve(100);
	EXPECT_EQ(map1.capacity(), 100u);
	map1.shrink_to_fit();
	EXPECT_EQ(map1.capacity(), 0u);
	EXPECT_TRUE(map1.empty());
	EXPECT_EQ(map1.size(), 0u);
	EXPECT_FALSE(map1.contains(1));
	EXPECT_EQ(map1.count(1), 0u);

	map1.clear();
	EXPECT_TRUE(map1.empty());
	EXPECT_EQ(map1.size(), 0u);
	EXPECT_FALSE(map1.contains(1));
	EXPECT_EQ(map1.count(1), 0u);

	for (size_t i = 0; i < small_num; ++i) {
		auto ret_pair = map1.insert({ i, { i } });
		EXPECT_TRUE(ret_pair.second);
		EXPECT_EQ(ret_pair.first->second, test{ i });
	}
	for (size_t i = 0; i < small_num; ++i) {
		auto ret_pair = map1.insert({ i, { i } });
		EXPECT_FALSE(ret_pair.second);
		EXPECT_EQ(ret_pair.first->second, test{ i });
	}
	for (size_t i = 0; i < small_num; ++i) {
		test t{ i };
		auto ret_pair = map1.insert({ i, t });
		EXPECT_FALSE(ret_pair.second);
		EXPECT_EQ(ret_pair.first->second, t);
	}

	fea::unsigned_map<size_t, test> map2{ map1 };
	fea::unsigned_map<size_t, test> map_ded{ map1 };
	fea::unsigned_map<size_t, test> map3{ std::move(map_ded) };

	EXPECT_EQ(map1, map2);
	EXPECT_EQ(map1, map3);

	EXPECT_EQ(map1.max_size(), map2.max_size());
	EXPECT_EQ(map1.max_size(), map3.max_size());

	EXPECT_EQ(map1.size(), small_num);
	EXPECT_EQ(map2.size(), small_num);
	EXPECT_EQ(map3.size(), small_num);

	EXPECT_FALSE(map1.empty());
	EXPECT_FALSE(map2.empty());
	EXPECT_FALSE(map3.empty());

	map1.clear();
	EXPECT_TRUE(map1.empty());
	EXPECT_EQ(map1.size(), 0u);

	auto it = map1.find(1);
	EXPECT_EQ(it, map1.end());

#if FEA_DEBUG || defined(FEA_NOTHROW)
	EXPECT_DEATH(map1.at(1), "");
#else
	EXPECT_THROW(map1.at(1), std::out_of_range);
#endif

	EXPECT_FALSE(map1.contains(1));
	EXPECT_EQ(map1.count(1), 0u);

	EXPECT_EQ(map1[1], test{});

	map1.at(1) = test{ 1 };
	EXPECT_NE(map1[1], test{});


	map1 = map2;

	for (size_t i = 0; i < small_num; ++i) {
		EXPECT_EQ(map1[i], test{ i });
		EXPECT_EQ(map1.at(i), test{ i });
		EXPECT_EQ(map1.at_unchecked(i), test{ i });
		EXPECT_EQ(map1.find(i)->second, test{ i });
		EXPECT_TRUE(map1.contains(i));
		EXPECT_EQ(map1.count(i), 1u);

		EXPECT_EQ(map2[i], test{ i });
		EXPECT_EQ(map2.at(i), test{ i });
		EXPECT_EQ(map2.at_unchecked(i), test{ i });
		EXPECT_EQ(map2.find(i)->second, test{ i });
		EXPECT_TRUE(map2.contains(i));
		EXPECT_EQ(map2.count(i), 1u);

		EXPECT_EQ(map3[i], test{ i });
		EXPECT_EQ(map3.at(i), test{ i });
		EXPECT_EQ(map3.at_unchecked(i), test{ i });
		EXPECT_EQ(map3.find(i)->second, test{ i });
		EXPECT_TRUE(map2.contains(i));
		EXPECT_EQ(map2.count(i), 1u);
	}

	map1.erase(1);
	EXPECT_EQ(map1.size(), small_num - 1);
	EXPECT_NE(map1, map2);
	EXPECT_NE(map1, map3);
	EXPECT_FALSE(map1.contains(1));
	EXPECT_EQ(map1.count(1), 0u);

	map1.insert({ 1, { 1 } });
	EXPECT_EQ(map1.size(), small_num);
	EXPECT_EQ(map1, map2);
	EXPECT_EQ(map1, map3);
	EXPECT_TRUE(map1.contains(1));
	EXPECT_EQ(map1.count(1), 1u);

	map1.erase(map1.begin(), map1.end());
	EXPECT_TRUE(map1.empty());
	EXPECT_EQ(map1.size(), 0u);

	it = map1.find(1);
	EXPECT_EQ(it, map1.end());

#if FEA_DEBUG || defined(FEA_NOTHROW)
	EXPECT_DEATH(map1.at(1), "");
#else
	EXPECT_THROW(map1.at(1), std::out_of_range);
#endif

	EXPECT_FALSE(map1.contains(1));
	EXPECT_EQ(map1.count(1), 0u);

	map_ded = map2;
	map1 = std::move(map_ded);

	map1.erase(map1.begin());
	EXPECT_EQ(map1.size(), small_num - 1);
	EXPECT_NE(map1, map2);
	EXPECT_NE(map1, map3);
	EXPECT_FALSE(map1.contains(0));
	EXPECT_EQ(map1.count(0), 0u);

#if FEA_DEBUG || defined(FEA_NOTHROW)
	EXPECT_DEATH(map1.at(0), "");
#else
	EXPECT_THROW(map1.at(0), std::out_of_range);
#endif

	map1 = map2;

	for (it = map1.begin(); it != map1.end();) {
		if (it->second.val % 2 == 1)
			it = map1.erase(it);
		else
			++it;
	}
	EXPECT_EQ(map1.size(), small_num / 2);

	for (auto t : map1) {
		EXPECT_EQ(t.second.val % 2, 0u);
	}

	map1 = map2;

	for (it = map1.begin() + 1; it != map1.end();) {
		if (it->second.val % 2 == 0)
			it = map1.erase(it, std::next(it, 2));
		else
			++it;
	}
	EXPECT_EQ(map1.size(), 4u);
	EXPECT_TRUE(map1.contains(0));
	EXPECT_TRUE(map1.contains(1));
	EXPECT_TRUE(map1.contains(9));
	EXPECT_TRUE(map1.contains(7));

	map1 = map2;

	{
		auto ret_pair1 = map1.insert({ 19, { 19 } });
		EXPECT_TRUE(ret_pair1.second);

		auto ret_pair2 = map1.insert({ 19, { 42 } });
		EXPECT_FALSE(ret_pair2.second);
		EXPECT_EQ(ret_pair2.first, ret_pair1.first);
		EXPECT_EQ(map1.at(19), test{ 19 });
		EXPECT_EQ(map1.at_unchecked(19), test{ 19 });

		ret_pair2 = map1.insert_or_assign(19, test{ 42 });
		EXPECT_FALSE(ret_pair2.second);
		EXPECT_EQ(ret_pair2.first, ret_pair1.first);
		EXPECT_EQ(map1.at(19), test{ 42 });
		EXPECT_EQ(map1.at_unchecked(19), test{ 42 });
		ret_pair2 = map1.insert_or_assign(19, test{ 19 });
	}

	map2.insert({ 20, { 20 } });
	map3.insert({ 20, { 20 } });
	EXPECT_NE(map1, map2);
	EXPECT_NE(map1, map3);

	{
		auto ret_pair = map1.equal_range(19);
		EXPECT_EQ(std::distance(ret_pair.first, ret_pair.second), 1);
		EXPECT_EQ(ret_pair.first->second, test{ 19 });

		ret_pair = map1.equal_range(20);
		EXPECT_EQ(std::distance(ret_pair.first, ret_pair.second), 0);
		EXPECT_EQ(ret_pair.first, map1.end());
		EXPECT_EQ(ret_pair.first, ret_pair.second);
	}

	{
		map1.emplace(20, test{ 20 });
		test t{ 21 };
		map1.emplace(21, t);
	}

	map1 = map2;
	map3 = map2;

	map1 = fea::unsigned_map<size_t, test>(
			{ { 0, { 0 } }, { 1, { 1 } }, { 2, { 2 } } });
	map2 = fea::unsigned_map<size_t, test>(
			{ { 3, { 3 } }, { 4, { 4 } }, { 5, { 5 } } });
	map3 = fea::unsigned_map<size_t, test>(
			{ { 6, { 6 } }, { 7, { 7 } }, { 8, { 8 } } });

	EXPECT_EQ(map1.size(), 3u);
	EXPECT_TRUE(map1.contains(0));
	EXPECT_TRUE(map1.contains(1));
	EXPECT_TRUE(map1.contains(2));
	EXPECT_EQ(map1.at(0), test{ 0 });
	EXPECT_EQ(map1.at_unchecked(0), test{ 0 });
	EXPECT_EQ(map1[1], test{ 1 });
	EXPECT_EQ(map1.find(2)->second, test{ 2 });

	EXPECT_EQ(map2.size(), 3u);
	EXPECT_TRUE(map2.contains(3));
	EXPECT_TRUE(map2.contains(4));
	EXPECT_TRUE(map2.contains(5));
	EXPECT_EQ(map2.at(3), test{ 3 });
	EXPECT_EQ(map2.at_unchecked(3), test{ 3 });
	EXPECT_EQ(map2[4], test{ 4 });
	EXPECT_EQ(map2.find(5)->second, test{ 5 });

	EXPECT_EQ(map3.size(), 3u);
	EXPECT_TRUE(map3.contains(6));
	EXPECT_TRUE(map3.contains(7));
	EXPECT_TRUE(map3.contains(8));
	EXPECT_EQ(map3.at_unchecked(6), test{ 6 });
	EXPECT_EQ(map3[7], test{ 7 });
	EXPECT_EQ(map3.find(8)->second, test{ 8 });

	{
		fea::unsigned_map<size_t, test> map1_back = map1;
		fea::unsigned_map<size_t, test> map2_back{ map2 };
		fea::unsigned_map<size_t, test> map3_back{ map3 };

		map1.swap(map2);
		EXPECT_EQ(map1, map2_back);
		EXPECT_EQ(map2, map1_back);

		using std::swap;
		swap(map1, map3);

		EXPECT_EQ(map1, map3_back);
		EXPECT_EQ(map3, map2_back);

		map1.swap(map2);
		EXPECT_EQ(map1, map1_back);
	}

	map1.insert({ { 3, { 3 } }, { 4, { 4 } }, { 5, { 5 } } });

	EXPECT_EQ(map1.size(), 6u);
	EXPECT_TRUE(map1.contains(0));
	EXPECT_TRUE(map1.contains(1));
	EXPECT_TRUE(map1.contains(2));
	EXPECT_TRUE(map1.contains(3));
	EXPECT_TRUE(map1.contains(4));
	EXPECT_TRUE(map1.contains(5));

	EXPECT_EQ(map1.at(0), test{ 0 });
	EXPECT_EQ(map1.at_unchecked(0), test{ 0 });
	EXPECT_EQ(map1[1], test{ 1 });
	EXPECT_EQ(map1.find(2)->second, test{ 2 });
	EXPECT_EQ(map1.at(3), test{ 3 });
	EXPECT_EQ(map1.at_unchecked(3), test{ 3 });
	EXPECT_EQ(map1[4], test{ 4 });
	EXPECT_EQ(map1.find(5)->second, test{ 5 });

	map2 = fea::unsigned_map<size_t, test>(map1.begin(), map1.end());
	EXPECT_EQ(map1.size(), map2.size());
	EXPECT_EQ(map1, map2);

	map3.clear();
	map3.insert(map1.begin(), map1.end());
	EXPECT_EQ(map1.size(), map3.size());
	EXPECT_EQ(map1, map3);
	EXPECT_EQ(map2.size(), map3.size());
	EXPECT_EQ(map2, map3);
}

TEST(unsigned_map, random) {
}

TEST(unsigned_map, uniqueptr) {
	fea::unsigned_map<size_t, std::unique_ptr<unsigned>> map;

	{
		std::unique_ptr<unsigned> test = std::make_unique<unsigned>(0);
		map[0] = std::move(test);
	}
	{
		std::unique_ptr<unsigned> test = std::make_unique<unsigned>(1);
		map.emplace(1, std::move(test));
	}
	{
		std::unique_ptr<unsigned> test = std::make_unique<unsigned>(2);
		map.insert({ 2, std::move(test) });
	}

	for (size_t i = 3; i < 10; ++i) {
		map.emplace(i, std::make_unique<unsigned>(unsigned(i)));
	}

	EXPECT_EQ(map.size(), 10u);
	for (size_t i = 0; i < 10; ++i) {
		EXPECT_EQ(*map.at(i), i);
	}

	EXPECT_TRUE(map.contains(5));
	EXPECT_EQ(map.count(5), 1u);
	map.erase(5);
	EXPECT_FALSE(map.contains(5));
	EXPECT_EQ(map.count(5), 0u);
	map.clear();
	EXPECT_EQ(map.size(), 0u);
}

} // namespace

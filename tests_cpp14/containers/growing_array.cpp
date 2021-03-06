//#include <fea/containers/growing_array.hpp>
//#include <gtest/gtest.h>
//
// namespace {
//
// TEST(growing_array, basics) {
//	fea::growing_array<size_t, 2> v;
//	v.push_back(0u);
//	v.push_back(1u);
//	v.push_back(2u);
//
//	EXPECT_EQ(v.size(), 3u);
//	EXPECT_EQ(v.max_size(), 128u);
//	EXPECT_FALSE(v.empty());
//
//	EXPECT_EQ(std::distance(v.begin(), v.end()), 3u);
//	EXPECT_EQ(std::distance(v.cbegin(), v.cend()), 3u);
//	EXPECT_EQ(std::distance(v.rbegin(), v.rend()), 3u);
//	EXPECT_EQ(std::distance(v.crbegin(), v.crend()), 3u);
//
//	EXPECT_EQ(*v.begin(), 0u);
//	EXPECT_EQ(*v.cbegin(), 0u);
//	EXPECT_EQ(*(v.end() - 1), 2u);
//	EXPECT_EQ(*(v.cend() - 1), 2u);
//	EXPECT_EQ(*v.rbegin(), 2u);
//	EXPECT_EQ(*(v.rend() - 1), 0u);
//	EXPECT_EQ(*v.crbegin(), 2u);
//	EXPECT_EQ(*(v.crend() - 1), 0u);
//
//	EXPECT_EQ(v.front(), 0u);
//	EXPECT_EQ(v.back(), 2u);
//
//	EXPECT_EQ(v[0], 0u);
//	EXPECT_EQ(v.at(0), 0u);
//	EXPECT_EQ(*v.data(), 0u);
//
//	fea::growing_array<size_t, 2> v2;
//	v2.resize(3, 3u);
//
//	using std::swap;
//	swap(v, v2);
//	EXPECT_EQ(v[0], 3u);
//	EXPECT_EQ(v[1], 3u);
//	EXPECT_EQ(v[2], 3u);
//
//	swap(v, v2);
//	EXPECT_EQ(v[0], 0u);
//	EXPECT_EQ(v[1], 1u);
//	EXPECT_EQ(v[2], 2u);
//
//	v.push_back(3);
//	EXPECT_EQ(v.size(), 4u);
//	EXPECT_EQ(std::distance(v.begin(), v.end()), 4u);
//	EXPECT_EQ(std::distance(v.cbegin(), v.cend()), 4u);
//	EXPECT_EQ(std::distance(v.rbegin(), v.rend()), 4u);
//	EXPECT_EQ(std::distance(v.crbegin(), v.crend()), 4u);
//
//	EXPECT_EQ(*(v.end() - 1), 3u);
//	EXPECT_EQ(*(v.cend() - 1), 3u);
//	EXPECT_EQ(*v.rbegin(), 3u);
//	EXPECT_EQ(*v.crbegin(), 3u);
//
//	EXPECT_EQ(v.back(), 3u);
//
//	EXPECT_EQ(v[3], 3u);
//	EXPECT_EQ(v.at(3), 3u);
//	EXPECT_EQ(*(v.data() + 3), 3u);
//
//	v.pop_back();
//	EXPECT_EQ(v.size(), 3u);
//
//	EXPECT_EQ(std::distance(v.begin(), v.end()), 3u);
//	EXPECT_EQ(std::distance(v.cbegin(), v.cend()), 3u);
//	EXPECT_EQ(std::distance(v.rbegin(), v.rend()), 3u);
//	EXPECT_EQ(std::distance(v.crbegin(), v.crend()), 3u);
//
//	EXPECT_EQ(*(v.end() - 1), 2u);
//	EXPECT_EQ(*(v.cend() - 1), 2u);
//	EXPECT_EQ(*v.rbegin(), 2u);
//	EXPECT_EQ(*v.crbegin(), 2u);
//
//	EXPECT_EQ(v.back(), 2u);
//
//	EXPECT_EQ(v[2], 2u);
//	EXPECT_EQ(v.at(2), 2u);
//	EXPECT_EQ(*(v.data() + 2), 2u);
//
//	v.resize(5);
//	EXPECT_EQ(v.size(), 5u);
//	EXPECT_EQ(v.back(), 0u);
//
//	EXPECT_EQ(std::distance(v.begin(), v.end()), 5u);
//	EXPECT_EQ(std::distance(v.cbegin(), v.cend()), 5u);
//	EXPECT_EQ(std::distance(v.rbegin(), v.rend()), 5u);
//	EXPECT_EQ(std::distance(v.crbegin(), v.crend()), 5u);
//
//	v.resize(3);
//	EXPECT_EQ(v.size(), 3);
//
//	v.resize(5, 5u);
//	EXPECT_EQ(v.size(), 5u);
//	EXPECT_EQ(v.back(), 5u);
//	EXPECT_EQ(v[3], 5u);
//	EXPECT_EQ(v[4], 5u);
//
//	v.clear();
//	EXPECT_EQ(v.size(), 0u);
//	EXPECT_TRUE(v.empty());
//}
//} // namespace

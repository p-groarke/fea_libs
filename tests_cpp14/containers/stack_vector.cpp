#include <fea/containers/stack_vector.hpp>
#include <gtest/gtest.h>

namespace fea {
TEST(stack_vector, basics) {
	fea::stack_vector<size_t, 128> v{ std::array<size_t, 3>{ 0u, 1u, 2u } };
	EXPECT_EQ(v.size(), 3u);
	EXPECT_EQ(v.max_size(), 128u);
	EXPECT_FALSE(v.empty());

	EXPECT_EQ(size_t(std::distance(v.begin(), v.end())), 3u);
	EXPECT_EQ(size_t(std::distance(v.cbegin(), v.cend())), 3u);
	EXPECT_EQ(size_t(std::distance(v.rbegin(), v.rend())), 3u);
	EXPECT_EQ(size_t(std::distance(v.crbegin(), v.crend())), 3u);

	EXPECT_EQ(*v.begin(), 0u);
	EXPECT_EQ(*v.cbegin(), 0u);
	EXPECT_EQ(*(v.end()--), 2u);
	EXPECT_EQ(*(v.cend()--), 2u);
	EXPECT_EQ(*v.rbegin(), 2u);
	EXPECT_EQ(*(v.rend()--), 0u);
	EXPECT_EQ(*v.crbegin(), 2u);
	EXPECT_EQ(*(v.crend()--), 0u);

	EXPECT_EQ(v.front(), 0u);
	EXPECT_EQ(v.back(), 2u);

	EXPECT_EQ(v[0], 0u);
	EXPECT_EQ(v.at(0), 0u);
	EXPECT_EQ(*v.data(), 0u);

	fea::stack_vector<size_t, 128> v2{ std::array<size_t, 3>{ 3u, 3u, 3u } };
	using std::swap;
	swap(v, v2);
	EXPECT_EQ(v[0], 3u);
	EXPECT_EQ(v[1], 3u);
	EXPECT_EQ(v[2], 3u);

	swap(v, v2);
	EXPECT_EQ(v[0], 0u);
	EXPECT_EQ(v[1], 1u);
	EXPECT_EQ(v[2], 2u);

	v.push_back(3);
	EXPECT_EQ(v.size(), 4u);
	EXPECT_EQ(size_t(std::distance(v.begin(), v.end())), 4u);
	EXPECT_EQ(size_t(std::distance(v.cbegin(), v.cend())), 4u);
	EXPECT_EQ(size_t(std::distance(v.rbegin(), v.rend())), 4u);
	EXPECT_EQ(size_t(std::distance(v.crbegin(), v.crend())), 4u);

	EXPECT_EQ(*(v.end()--), 3u);
	EXPECT_EQ(*(v.cend()--), 3u);
	EXPECT_EQ(*v.rbegin(), 3u);
	EXPECT_EQ(*v.crbegin(), 3u);

	EXPECT_EQ(v.back(), 3u);

	EXPECT_EQ(v[3], 3u);
	EXPECT_EQ(v.at(3), 3u);
	EXPECT_EQ(*(v.data() + 3), 3u);

	v.pop_back();
	EXPECT_EQ(v.size(), 3u);

	EXPECT_EQ(size_t(std::distance(v.begin(), v.end())), 3u);
	EXPECT_EQ(size_t(std::distance(v.cbegin(), v.cend())), 3u);
	EXPECT_EQ(size_t(std::distance(v.rbegin(), v.rend())), 3u);
	EXPECT_EQ(size_t(std::distance(v.crbegin(), v.crend())), 3u);

	EXPECT_EQ(*(v.end()--), 2u);
	EXPECT_EQ(*(v.cend()--), 2u);
	EXPECT_EQ(*v.rbegin(), 2u);
	EXPECT_EQ(*v.crbegin(), 2u);

	EXPECT_EQ(v.back(), 2u);

	EXPECT_EQ(v[2], 2u);
	EXPECT_EQ(v.at(2), 2u);
	EXPECT_EQ(*(v.data() + 2), 2u);

	v.resize(5);
	EXPECT_EQ(v.size(), 5u);
	EXPECT_EQ(v.back(), 0u);

	EXPECT_EQ(size_t(std::distance(v.begin(), v.end())), 5u);
	EXPECT_EQ(size_t(std::distance(v.cbegin(), v.cend())), 5u);
	EXPECT_EQ(size_t(std::distance(v.rbegin(), v.rend())), 5u);
	EXPECT_EQ(size_t(std::distance(v.crbegin(), v.crend())), 5u);

	v.resize(3);
	EXPECT_EQ(v.size(), 3u);

	v.resize(5, 5u);
	EXPECT_EQ(v.size(), 5u);
	EXPECT_EQ(v.back(), 5u);
	EXPECT_EQ(v[3], 5u);
	EXPECT_EQ(v[4], 5u);

	v.clear();
	EXPECT_EQ(v.size(), 0u);
	EXPECT_TRUE(v.empty());
}

TEST(stack_vector, ctors) {
	fea::stack_vector<int, 4> v1(2, 42);
	EXPECT_EQ(v1.size(), 2u);
	EXPECT_EQ(v1[0], 42);
	EXPECT_EQ(v1[1], 42);

	//{
	//	auto v2(v1);
	//	EXPECT_EQ(v2.size(), 2);
	//	EXPECT_EQ(v2[0], 42);
	//	EXPECT_EQ(v2[1], 42);
	//}

	//{
	//	auto v2 = v1;
	//	EXPECT_EQ(v2.size(), 2);
	//	EXPECT_EQ(v2[0], 42);
	//	EXPECT_EQ(v2[1], 42);
	//}

	//{
	//	auto v2(std::move(v1));
	//	EXPECT_EQ(v2.size(), 2);
	//	EXPECT_EQ(v2[0], 42);
	//	EXPECT_EQ(v2[1], 42);
	//}

	//{
	//	auto v2 = std::move(v1);
	//	EXPECT_EQ(v2.size(), 2);
	//	EXPECT_EQ(v2[0], 42);
	//	EXPECT_EQ(v2[1], 42);
	//}
}
} // namespace fea

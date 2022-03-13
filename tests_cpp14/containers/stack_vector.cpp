#include <fea/containers/stack_vector.hpp>
#include <gtest/gtest.h>
#include <numeric>

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
	EXPECT_EQ(*(v.end() - 1), 2u);
	EXPECT_EQ(*(v.cend() - 1), 2u);
	EXPECT_EQ(*v.rbegin(), 2u);
	EXPECT_EQ(*(v.rend() - 1), 0u);
	EXPECT_EQ(*v.crbegin(), 2u);
	EXPECT_EQ(*(v.crend() - 1), 0u);

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

	EXPECT_EQ(*(v.end() - 1), 3u);
	EXPECT_EQ(*(v.cend() - 1), 3u);
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

	EXPECT_EQ(*(v.end() - 1), 2u);
	EXPECT_EQ(*(v.cend() - 1), 2u);
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

	const fea::stack_vector<int, 5> v2{ 0, 1, 2 };
	EXPECT_EQ(v2.size(), 3u);
	EXPECT_EQ(v2[0], 0);
	EXPECT_EQ(v2[1], 1);
	EXPECT_EQ(v2[2], 2);

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

TEST(stack_vector, insert) {
	fea::stack_vector<int, 5> v(5);

	{
		const fea::stack_vector<int, 5> answer{ 0, 1, 2, 3, 4 };
		std::iota(v.begin(), v.end(), 0);
		EXPECT_EQ(v, answer);
	}

	v.pop_back();
	EXPECT_EQ(v.size(), 4u);

	auto it = v.insert(v.begin() + 1, 42);
	EXPECT_EQ(v.size(), 5u);
	EXPECT_EQ(*it, 42);
	EXPECT_EQ(std::distance(v.begin(), it), 1);
	{
		const fea::stack_vector<int, 5> answer{ 0, 42, 1, 2, 3 };
		EXPECT_EQ(v, answer);
	}

	std::iota(v.begin(), v.end(), 0);
	v.pop_back();
	v.pop_back();
	v.pop_back();
	EXPECT_EQ(v.size(), 2u);
	it = v.insert(v.begin() + 1, 3, 42);
	EXPECT_EQ(v.size(), 5u);
	EXPECT_EQ(*it, 42);
	EXPECT_EQ(std::distance(v.begin(), it), 1);
	{
		const fea::stack_vector<int, 5> answer{ 0, 42, 42, 42, 1 };
		EXPECT_EQ(v, answer);
	}

	std::vector<int> cpy{ 42, -42, 42 };
	std::iota(v.begin(), v.end(), 0);
	v.resize(2);
	EXPECT_EQ(v.size(), 2u);
	it = v.insert(v.begin() + 1, cpy.begin(), cpy.end());
	EXPECT_EQ(v.size(), 5u);
	EXPECT_EQ(*it, 42);
	EXPECT_EQ(std::distance(v.begin(), it), 1);
	{
		const fea::stack_vector<int, 5> answer{ 0, 42, -42, 42, 1 };
		EXPECT_EQ(v, answer);
	}

	std::iota(v.begin(), v.end(), 0);
	v.resize(2);
	EXPECT_EQ(v.size(), 2u);
	it = v.insert(v.begin() + 1, { 42, -42, 101 });
	EXPECT_EQ(v.size(), 5u);
	EXPECT_EQ(*it, 42);
	EXPECT_EQ(std::distance(v.begin(), it), 1);
	{
		const fea::stack_vector<int, 5> answer{ 0, 42, -42, 101, 1 };
		EXPECT_EQ(v, answer);
	}
}
} // namespace fea

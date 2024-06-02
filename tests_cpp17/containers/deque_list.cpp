#include <fea/containers/deque_list.hpp>
#include <gtest/gtest.h>

namespace {
TEST(deque_list, basics) {
	fea::deque_list<int> dl;

	EXPECT_EQ(dl.cbegin(), dl.cend());
	EXPECT_FALSE(dl.cbegin() != dl.cend());
	EXPECT_EQ(dl.begin(), dl.end());
	EXPECT_FALSE(dl.begin() != dl.end());
	EXPECT_EQ(std::distance(dl.cbegin(), dl.cend()), 0);
	EXPECT_EQ(std::distance(dl.begin(), dl.end()), 0);
	EXPECT_TRUE(dl.empty());
	EXPECT_EQ(dl.size(), 0u);

	dl.clear();
	EXPECT_EQ(dl.cbegin(), dl.cend());
	EXPECT_FALSE(dl.cbegin() != dl.cend());
	EXPECT_EQ(dl.begin(), dl.end());
	EXPECT_FALSE(dl.begin() != dl.end());
	EXPECT_EQ(std::distance(dl.cbegin(), dl.cend()), 0);
	EXPECT_EQ(std::distance(dl.begin(), dl.end()), 0);
	EXPECT_TRUE(dl.empty());
	EXPECT_EQ(dl.size(), 0u);

	dl.shrink_to_fit();
	EXPECT_EQ(dl.cbegin(), dl.cend());
	EXPECT_FALSE(dl.cbegin() != dl.cend());
	EXPECT_EQ(dl.begin(), dl.end());
	EXPECT_FALSE(dl.begin() != dl.end());
	EXPECT_EQ(std::distance(dl.cbegin(), dl.cend()), 0);
	EXPECT_EQ(std::distance(dl.begin(), dl.end()), 0);
	EXPECT_TRUE(dl.empty());
	EXPECT_EQ(dl.size(), 0u);

	{
		int v = 42;
		dl.push_back(v);
	}
	EXPECT_NE(dl.cbegin(), dl.cend());
	EXPECT_TRUE(dl.cbegin() != dl.cend());
	EXPECT_NE(dl.begin(), dl.end());
	EXPECT_TRUE(dl.begin() != dl.end());
	EXPECT_EQ(std::distance(dl.cbegin(), dl.cend()), 1);
	EXPECT_EQ(std::distance(dl.begin(), dl.end()), 1);
	EXPECT_FALSE(dl.empty());
	EXPECT_EQ(dl.size(), 1u);

	EXPECT_EQ(dl.front(), 42);
	EXPECT_EQ(dl.back(), 42);
	EXPECT_EQ(*dl.cbegin(), 42);
	EXPECT_EQ(*dl.begin(), 42);
	EXPECT_EQ(++dl.cbegin(), dl.end());
	EXPECT_EQ(++dl.begin(), dl.end());
	EXPECT_EQ(dl.cbegin(), --dl.end());
	EXPECT_EQ(dl.begin(), --dl.end());

	dl.push_back(-42);
	EXPECT_EQ(dl.front(), 42);
	EXPECT_EQ(dl.back(), -42);

	dl.pop_back();
	dl.pop_back();
	EXPECT_EQ(dl.cbegin(), dl.cend());
	EXPECT_FALSE(dl.cbegin() != dl.cend());
	EXPECT_EQ(dl.begin(), dl.end());
	EXPECT_FALSE(dl.begin() != dl.end());
	EXPECT_EQ(std::distance(dl.cbegin(), dl.cend()), 0);
	EXPECT_EQ(std::distance(dl.begin(), dl.end()), 0);
	EXPECT_TRUE(dl.empty());
	EXPECT_EQ(dl.size(), 0u);

	dl.push_back(42);
	dl.push_back(42);
	EXPECT_NE(dl.cbegin(), dl.cend());
	EXPECT_TRUE(dl.cbegin() != dl.cend());
	EXPECT_NE(dl.begin(), dl.end());
	EXPECT_TRUE(dl.begin() != dl.end());
	EXPECT_EQ(std::distance(dl.cbegin(), dl.cend()), 2);
	EXPECT_EQ(std::distance(dl.begin(), dl.end()), 2);
	EXPECT_FALSE(dl.empty());
	EXPECT_EQ(dl.size(), 2u);

	EXPECT_EQ(dl.front(), 42);
	EXPECT_EQ(dl.back(), 42);
	EXPECT_EQ(*dl.cbegin(), 42);
	EXPECT_EQ(*dl.begin(), 42);
	EXPECT_NE(++dl.cbegin(), dl.end());
	EXPECT_NE(++dl.begin(), dl.end());

	dl.shrink_to_fit();
	EXPECT_NE(dl.cbegin(), dl.cend());
	EXPECT_TRUE(dl.cbegin() != dl.cend());
	EXPECT_NE(dl.begin(), dl.end());
	EXPECT_TRUE(dl.begin() != dl.end());
	EXPECT_EQ(std::distance(dl.cbegin(), dl.cend()), 2);
	EXPECT_EQ(std::distance(dl.begin(), dl.end()), 2);
	EXPECT_FALSE(dl.empty());
	EXPECT_EQ(dl.size(), 2u);

	EXPECT_EQ(dl.front(), 42);
	EXPECT_EQ(dl.back(), 42);
	EXPECT_EQ(*dl.cbegin(), 42);
	EXPECT_EQ(*dl.begin(), 42);
	EXPECT_NE(++dl.cbegin(), dl.end());
	EXPECT_NE(++dl.begin(), dl.end());

	size_t num_iter = 0u;
	for (int v : dl) {
		EXPECT_EQ(v, 42);
		++num_iter;
	}
	EXPECT_EQ(num_iter, 2u);

	dl.clear();
	for (int i = 0; i < 1000; ++i) {
		dl.push_back(i);
	}
	EXPECT_FALSE(dl.empty());
	EXPECT_EQ(dl.size(), 1000u);
	EXPECT_EQ(dl.front(), 0);
	EXPECT_EQ(dl.back(), 999);
	auto it = std::find(dl.begin(), dl.end(), 999);
	EXPECT_NE(it, dl.end());
	EXPECT_EQ(*it, 999);

	dl.shrink_to_fit();
	EXPECT_FALSE(dl.empty());
	EXPECT_EQ(dl.size(), 1000u);
	EXPECT_EQ(dl.front(), 0);
	EXPECT_EQ(dl.back(), 999);
	it = std::find(dl.begin(), dl.end(), 999);
	EXPECT_NE(it, dl.end());
	EXPECT_EQ(*it, 999);

	dl.clear();
	dl.shrink_to_fit();
	EXPECT_TRUE(dl.empty());
	EXPECT_EQ(dl.size(), 0u);
	it = std::find(dl.begin(), dl.end(), 999);
	EXPECT_EQ(it, dl.end());
}

TEST(deque_list, constructors) {
	fea::deque_list<int> dl;
	{
		fea::deque_list<int> dl2 = dl;
		EXPECT_EQ(std::distance(dl2.begin(), dl2.end()),
				std::distance(dl.begin(), dl.end()));
		EXPECT_EQ(dl2.empty(), dl.empty());
		EXPECT_EQ(dl2.size(), dl.size());
		EXPECT_TRUE(std::equal(dl2.begin(), dl2.end(), dl.begin(), dl.end()));
	}
	{
		fea::deque_list<int> dl3 = dl;
		fea::deque_list<int> dl2 = std::move(dl3);
		EXPECT_EQ(std::distance(dl2.begin(), dl2.end()),
				std::distance(dl.begin(), dl.end()));
		EXPECT_EQ(dl2.empty(), dl.empty());
		EXPECT_EQ(dl2.size(), dl.size());
		EXPECT_TRUE(std::equal(dl2.begin(), dl2.end(), dl.begin(), dl.end()));
	}
	{
		fea::deque_list<int> dl2;
		dl2 = dl;
		EXPECT_EQ(std::distance(dl2.begin(), dl2.end()),
				std::distance(dl.begin(), dl.end()));
		EXPECT_EQ(dl2.empty(), dl.empty());
		EXPECT_EQ(dl2.size(), dl.size());
		EXPECT_TRUE(std::equal(dl2.begin(), dl2.end(), dl.begin(), dl.end()));
	}
	{
		fea::deque_list<int> dl3 = dl;
		fea::deque_list<int> dl2;
		dl2 = std::move(dl);
		EXPECT_EQ(std::distance(dl2.begin(), dl2.end()),
				std::distance(dl.begin(), dl.end()));
		EXPECT_EQ(dl2.empty(), dl.empty());
		EXPECT_EQ(dl2.size(), dl.size());
		EXPECT_TRUE(std::equal(dl2.begin(), dl2.end(), dl.begin(), dl.end()));
	}

	for (int i = 0; i < 128; ++i) {
		dl.push_back(i);
	}

	{
		fea::deque_list<int> dl2 = dl;
		EXPECT_EQ(dl2.front(), dl.front());
		EXPECT_EQ(dl2.back(), dl.back());
		EXPECT_EQ(std::distance(dl2.begin(), dl2.end()),
				std::distance(dl.begin(), dl.end()));
		EXPECT_EQ(dl2.empty(), dl.empty());
		EXPECT_EQ(dl2.size(), dl.size());
		EXPECT_TRUE(std::equal(dl2.begin(), dl2.end(), dl.begin(), dl.end()));
	}
	{
		fea::deque_list<int> dl3 = dl;
		fea::deque_list<int> dl2 = std::move(dl3);
		EXPECT_EQ(dl2.front(), dl.front());
		EXPECT_EQ(dl2.back(), dl.back());
		EXPECT_EQ(std::distance(dl2.begin(), dl2.end()),
				std::distance(dl.begin(), dl.end()));
		EXPECT_EQ(dl2.empty(), dl.empty());
		EXPECT_EQ(dl2.size(), dl.size());
		EXPECT_TRUE(std::equal(dl2.begin(), dl2.end(), dl.begin(), dl.end()));

		EXPECT_EQ(std::distance(dl3.begin(), dl3.end()), 0);
		EXPECT_TRUE(dl3.empty());
		EXPECT_EQ(dl3.size(), 0u);
	}
	{
		fea::deque_list<int> dl2;
		dl2 = dl;
		EXPECT_EQ(dl2.front(), dl.front());
		EXPECT_EQ(dl2.back(), dl.back());
		EXPECT_EQ(std::distance(dl2.begin(), dl2.end()),
				std::distance(dl.begin(), dl.end()));
		EXPECT_EQ(dl2.empty(), dl.empty());
		EXPECT_EQ(dl2.size(), dl.size());
		EXPECT_TRUE(std::equal(dl2.begin(), dl2.end(), dl.begin(), dl.end()));
	}
	{
		fea::deque_list<int> dl3 = dl;
		fea::deque_list<int> dl2;
		dl2 = std::move(dl3);
		EXPECT_EQ(dl2.front(), dl.front());
		EXPECT_EQ(dl2.back(), dl.back());
		EXPECT_EQ(std::distance(dl2.begin(), dl2.end()),
				std::distance(dl.begin(), dl.end()));
		EXPECT_EQ(dl2.empty(), dl.empty());
		EXPECT_EQ(dl2.size(), dl.size());
		EXPECT_TRUE(std::equal(dl2.begin(), dl2.end(), dl.begin(), dl.end()));

		EXPECT_EQ(std::distance(dl3.begin(), dl3.end()), 0);
		EXPECT_TRUE(dl3.empty());
		EXPECT_EQ(dl3.size(), 0u);
	}

	for (int i = 0; i < 32; ++i) {
		dl.pop_back();
	}

	{
		fea::deque_list<int> dl2 = dl;
		EXPECT_EQ(dl2.front(), dl.front());
		EXPECT_EQ(dl2.back(), dl.back());
		EXPECT_EQ(std::distance(dl2.begin(), dl2.end()),
				std::distance(dl.begin(), dl.end()));
		EXPECT_EQ(dl2.empty(), dl.empty());
		EXPECT_EQ(dl2.size(), dl.size());
		EXPECT_TRUE(std::equal(dl2.begin(), dl2.end(), dl.begin(), dl.end()));
	}
	{
		fea::deque_list<int> dl3 = dl;
		fea::deque_list<int> dl2 = std::move(dl3);
		EXPECT_EQ(dl2.front(), dl.front());
		EXPECT_EQ(dl2.back(), dl.back());
		EXPECT_EQ(std::distance(dl2.begin(), dl2.end()),
				std::distance(dl.begin(), dl.end()));
		EXPECT_EQ(dl2.empty(), dl.empty());
		EXPECT_EQ(dl2.size(), dl.size());
		EXPECT_TRUE(std::equal(dl2.begin(), dl2.end(), dl.begin(), dl.end()));

		EXPECT_EQ(std::distance(dl3.begin(), dl3.end()), 0);
		EXPECT_TRUE(dl3.empty());
		EXPECT_EQ(dl3.size(), 0u);
	}
	{
		fea::deque_list<int> dl2;
		dl2 = dl;
		EXPECT_EQ(dl2.front(), dl.front());
		EXPECT_EQ(dl2.back(), dl.back());
		EXPECT_EQ(std::distance(dl2.begin(), dl2.end()),
				std::distance(dl.begin(), dl.end()));
		EXPECT_EQ(dl2.empty(), dl.empty());
		EXPECT_EQ(dl2.size(), dl.size());
		EXPECT_TRUE(std::equal(dl2.begin(), dl2.end(), dl.begin(), dl.end()));
	}
	{
		fea::deque_list<int> dl3 = dl;
		fea::deque_list<int> dl2;
		dl2 = std::move(dl3);
		EXPECT_EQ(dl2.front(), dl.front());
		EXPECT_EQ(dl2.back(), dl.back());
		EXPECT_EQ(std::distance(dl2.begin(), dl2.end()),
				std::distance(dl.begin(), dl.end()));
		EXPECT_EQ(dl2.empty(), dl.empty());
		EXPECT_EQ(dl2.size(), dl.size());
		EXPECT_TRUE(std::equal(dl2.begin(), dl2.end(), dl.begin(), dl.end()));

		EXPECT_EQ(std::distance(dl3.begin(), dl3.end()), 0);
		EXPECT_TRUE(dl3.empty());
		EXPECT_EQ(dl3.size(), 0u);
	}

	for (int i = 0; i < 32; ++i) {
		dl.push_back(i);
	}

	{
		fea::deque_list<int> dl2 = dl;
		EXPECT_EQ(dl2.front(), dl.front());
		EXPECT_EQ(dl2.back(), dl.back());
		EXPECT_EQ(std::distance(dl2.begin(), dl2.end()),
				std::distance(dl.begin(), dl.end()));
		EXPECT_EQ(dl2.empty(), dl.empty());
		EXPECT_EQ(dl2.size(), dl.size());
		EXPECT_TRUE(std::equal(dl2.begin(), dl2.end(), dl.begin(), dl.end()));
	}
	{
		fea::deque_list<int> dl3 = dl;
		fea::deque_list<int> dl2 = std::move(dl3);
		EXPECT_EQ(dl2.front(), dl.front());
		EXPECT_EQ(dl2.back(), dl.back());
		EXPECT_EQ(std::distance(dl2.begin(), dl2.end()),
				std::distance(dl.begin(), dl.end()));
		EXPECT_EQ(dl2.empty(), dl.empty());
		EXPECT_EQ(dl2.size(), dl.size());
		EXPECT_TRUE(std::equal(dl2.begin(), dl2.end(), dl.begin(), dl.end()));

		EXPECT_EQ(std::distance(dl3.begin(), dl3.end()), 0);
		EXPECT_TRUE(dl3.empty());
		EXPECT_EQ(dl3.size(), 0u);
	}
	{
		fea::deque_list<int> dl2;
		dl2 = dl;
		EXPECT_EQ(dl2.front(), dl.front());
		EXPECT_EQ(dl2.back(), dl.back());
		EXPECT_EQ(std::distance(dl2.begin(), dl2.end()),
				std::distance(dl.begin(), dl.end()));
		EXPECT_EQ(dl2.empty(), dl.empty());
		EXPECT_EQ(dl2.size(), dl.size());
		EXPECT_TRUE(std::equal(dl2.begin(), dl2.end(), dl.begin(), dl.end()));
	}
	{
		fea::deque_list<int> dl3 = dl;
		fea::deque_list<int> dl2;
		dl2 = std::move(dl3);
		EXPECT_EQ(dl2.front(), dl.front());
		EXPECT_EQ(dl2.back(), dl.back());
		EXPECT_EQ(std::distance(dl2.begin(), dl2.end()),
				std::distance(dl.begin(), dl.end()));
		EXPECT_EQ(dl2.empty(), dl.empty());
		EXPECT_EQ(dl2.size(), dl.size());
		EXPECT_TRUE(std::equal(dl2.begin(), dl2.end(), dl.begin(), dl.end()));

		EXPECT_EQ(std::distance(dl3.begin(), dl3.end()), 0);
		EXPECT_TRUE(dl3.empty());
		EXPECT_EQ(dl3.size(), 0u);
	}
}
} // namespace
#include <fea/containers/unsigned_set.hpp>
#include <gtest/gtest.h>

namespace {
TEST(unsigned_set, basics) {
	// Empty test
	{
		fea::unsigned_set<unsigned> us;
		EXPECT_EQ(us.begin(), us.end());
		EXPECT_EQ(us.cbegin(), us.cend());
		EXPECT_TRUE(us.empty());
		EXPECT_EQ(us.size(), 0u);
		EXPECT_EQ(us.capacity(), 0u);
		EXPECT_EQ(us.count(0u), 0u);
		EXPECT_FALSE(us.contains(0u));
		EXPECT_EQ(us.find(0u), us.end());

		us.shrink_to_fit();
		EXPECT_EQ(us.begin(), us.end());
		EXPECT_EQ(us.cbegin(), us.cend());
		EXPECT_TRUE(us.empty());
		EXPECT_EQ(us.size(), 0u);
		EXPECT_EQ(us.capacity(), 0u);
		EXPECT_EQ(us.count(0u), 0u);
		EXPECT_FALSE(us.contains(0u));
		EXPECT_EQ(us.find(0u), us.end());

		us.clear();
		EXPECT_EQ(us.begin(), us.end());
		EXPECT_EQ(us.cbegin(), us.cend());
		EXPECT_TRUE(us.empty());
		EXPECT_EQ(us.size(), 0u);
		EXPECT_EQ(us.capacity(), 0u);
		EXPECT_EQ(us.count(0u), 0u);
		EXPECT_FALSE(us.contains(0u));
		EXPECT_EQ(us.find(0u), us.end());

		std::vector<unsigned> v;
		us = fea::unsigned_set<unsigned>(v.begin(), v.end());
		EXPECT_EQ(us.begin(), us.end());
		EXPECT_EQ(us.cbegin(), us.cend());
		EXPECT_TRUE(us.empty());
		EXPECT_EQ(us.size(), 0u);
		EXPECT_EQ(us.capacity(), 0u);
		EXPECT_EQ(us.count(0u), 0u);
		EXPECT_FALSE(us.contains(0u));
		EXPECT_EQ(us.find(0u), us.end());

		us.insert(v.begin(), v.end());
		EXPECT_EQ(us.begin(), us.end());
		EXPECT_EQ(us.cbegin(), us.cend());
		EXPECT_TRUE(us.empty());
		EXPECT_EQ(us.size(), 0u);
		EXPECT_EQ(us.capacity(), 0u);
		EXPECT_EQ(us.count(0u), 0u);
		EXPECT_FALSE(us.contains(0u));
		EXPECT_EQ(us.find(0u), us.end());
	}


	// Basics
	{
		std::vector<unsigned> v{ 0u, 2u, 4u, 6u, 8u };
		unsigned maxid = v.back();

		fea::unsigned_set<unsigned> us(v.begin(), v.end());
		EXPECT_NE(us.begin(), us.end());
		EXPECT_NE(us.cbegin(), us.cend());
		EXPECT_FALSE(us.empty());
		EXPECT_EQ(us.size(), 5u);
		EXPECT_GE(us.capacity(), maxid + 1u);
		EXPECT_EQ(us.count(0u), 1u);
		EXPECT_TRUE(us.contains(0u));
		EXPECT_EQ(us.find(0u), us.begin());
		EXPECT_EQ(us.find(8u), --us.end());

		us.shrink_to_fit();
		EXPECT_NE(us.begin(), us.end());
		EXPECT_NE(us.cbegin(), us.cend());
		EXPECT_FALSE(us.empty());
		EXPECT_EQ(us.size(), 5u);
		EXPECT_GE(us.capacity(), maxid + 1u);
		EXPECT_EQ(us.count(0u), 1u);
		EXPECT_TRUE(us.contains(0u));
		EXPECT_EQ(us.find(0u), us.begin());
		EXPECT_EQ(us.find(8u), --us.end());

		us.clear();
		EXPECT_EQ(us.begin(), us.end());
		EXPECT_EQ(us.cbegin(), us.cend());
		EXPECT_TRUE(us.empty());
		EXPECT_EQ(us.size(), 0u);
		EXPECT_GE(us.capacity(), maxid + 1u);
		EXPECT_EQ(us.count(0u), 0u);
		EXPECT_FALSE(us.contains(0u));
		EXPECT_EQ(us.find(0u), us.end());

		us.insert(v.begin(), v.end());
		EXPECT_NE(us.begin(), us.end());
		EXPECT_NE(us.cbegin(), us.cend());
		EXPECT_FALSE(us.empty());
		EXPECT_EQ(us.size(), 5u);
		EXPECT_GE(us.capacity(), maxid + 1u);
		EXPECT_EQ(us.count(0u), 1u);
		EXPECT_TRUE(us.contains(0u));
		EXPECT_EQ(us.find(0u), us.begin());
		EXPECT_EQ(us.find(8u), --us.end());

		fea::unsigned_set<unsigned> us2;
		us.swap(us2);
		EXPECT_NE(us2.begin(), us2.end());
		EXPECT_NE(us2.cbegin(), us2.cend());
		EXPECT_FALSE(us2.empty());
		EXPECT_EQ(us2.size(), 5u);
		EXPECT_GE(us2.capacity(), maxid + 1u);
		EXPECT_EQ(us2.count(0u), 1u);
		EXPECT_TRUE(us2.contains(0u));
		EXPECT_EQ(us2.find(0u), us2.begin());
		EXPECT_EQ(us2.find(8u), --us2.end());

		EXPECT_EQ(us.begin(), us.end());
		EXPECT_EQ(us.cbegin(), us.cend());
		EXPECT_TRUE(us.empty());
		EXPECT_EQ(us.size(), 0u);
		EXPECT_EQ(us.capacity(), 0u);
		EXPECT_EQ(us.count(0u), 0u);
		EXPECT_FALSE(us.contains(0u));
		EXPECT_EQ(us.find(0u), us.end());

		us.swap(us2);
		for (unsigned k : v) {
			EXPECT_EQ(us.count(k), 1u);
			EXPECT_TRUE(us.contains(k));
		}

		std::vector<unsigned> v2{ 1u, 3u, 5u, 7u, 9u };
		us2 = fea::unsigned_set<unsigned>(v2.begin(), v2.end());
		us.merge(us2);
		for (unsigned k : v) {
			EXPECT_EQ(us.count(k), 1u);
			EXPECT_TRUE(us.contains(k));
			EXPECT_NE(us.find(k), us.end());
		}
		for (unsigned k : v2) {
			EXPECT_EQ(us.count(k), 1u);
			EXPECT_TRUE(us.contains(k));
			EXPECT_NE(us.find(k), us.end());

			EXPECT_EQ(us2.count(k), 0u);
			EXPECT_FALSE(us2.contains(k));
			EXPECT_EQ(us2.find(k), us2.end());
		}
	}

	// Iterators
	{
		std::vector<unsigned> v{ 0u, 2u, 4u, 6u, 8u };
		// unsigned maxid = v.back().id;

		fea::unsigned_set<unsigned> us(v.begin(), v.end());
		EXPECT_FALSE(us.begin() == us.end());
		EXPECT_TRUE(us.begin() != us.end());
		EXPECT_TRUE(us.begin() < us.end());
		EXPECT_TRUE(us.begin() <= us.end());
		EXPECT_FALSE(us.begin() > us.end());
		EXPECT_FALSE(us.begin() >= us.end());
		EXPECT_FALSE(us.begin() < us.begin());
		EXPECT_FALSE(us.begin() > us.begin());
		EXPECT_TRUE(us.begin() <= us.begin());
		EXPECT_TRUE(us.begin() >= us.begin());

		auto it = us.begin();
		for (unsigned k : v) {
			EXPECT_EQ(*us.find(k), k);
			EXPECT_EQ(*it, k);
			++it;
		}

		std::vector<unsigned> visited;
		for (it = us.begin(); it != us.end(); ++it) {
			visited.push_back(*it);
		}
		EXPECT_EQ(visited, v);

		visited.clear();
		auto temp = us.end();
		--temp;
		for (it = temp; it != us.begin(); --it) {
			visited.push_back(*it);
		}
		visited.push_back(*it);
		std::reverse(visited.begin(), visited.end());
		EXPECT_EQ(visited, v);
	}

	// Insertion
	{
		std::vector<unsigned> v{ 0u, 2u, 4u, 6u, 8u };
		fea::unsigned_set<unsigned> us;
		size_t size = 0;
		for (unsigned k : v) {
			auto it = us.find(k);
			EXPECT_EQ(it, us.end());
			EXPECT_EQ(us.size(), size);

			auto pair = us.insert(k);
			it = pair.first;
			bool inserted = pair.second;

			EXPECT_EQ(it, --us.end());
			EXPECT_EQ(*it, k);
			EXPECT_TRUE(inserted);
			EXPECT_EQ(us.size(), ++size);

			it = us.erase(it);
			EXPECT_EQ(it, us.end());
			EXPECT_EQ(us.count(k), 0u);
			EXPECT_FALSE(us.contains(k));
			EXPECT_EQ(us.size(), --size);

			it = us.erase(it);
			EXPECT_EQ(it, us.end());
			EXPECT_EQ(us.count(k), 0u);
			EXPECT_FALSE(us.contains(k));
			EXPECT_EQ(us.size(), size);
		}

		size = 0;
		for (unsigned k : v) {
			auto it = us.find(k);
			EXPECT_EQ(it, us.end());
			EXPECT_EQ(us.size(), size);

			auto pair = us.insert(k);
			it = pair.first;
			bool inserted = pair.second;

			EXPECT_EQ(it, --us.end());
			EXPECT_EQ(*it, k);
			EXPECT_TRUE(inserted);
			EXPECT_EQ(us.size(), ++size);
		}

		for (auto it = us.begin(); it != us.end();) {
			auto tmp = it;
			unsigned tmpv = *tmp;
			EXPECT_TRUE(us.contains(*tmp));
			EXPECT_TRUE(us.contains(tmpv));
			EXPECT_EQ(us.size(), size);

			it = us.erase(it);
			EXPECT_NE(tmp, it);
			EXPECT_EQ(us.size(), --size);

			auto tmp2 = tmp;
			EXPECT_FALSE(us.contains(tmpv));
			EXPECT_EQ(us.erase(tmp), tmp2);
		}
		EXPECT_TRUE(us.empty());

		us.insert(v.begin(), v.end());
		size = us.size();
		EXPECT_EQ(us.erase(us.end(), us.end()), us.end());
		EXPECT_FALSE(us.empty());
		EXPECT_EQ(us.size(), size);
		EXPECT_EQ(us.erase(us.begin(), us.begin()), us.begin());
		EXPECT_FALSE(us.empty());
		EXPECT_EQ(us.size(), size);
		EXPECT_EQ(us.erase(us.begin(), us.end()), us.end());
		EXPECT_TRUE(us.empty());
		EXPECT_EQ(us.size(), 0u);

		us.insert(v.begin(), v.end());
		size = us.size();
		auto erase_it = ++us.begin();
		auto expected_it = erase_it;
		++expected_it;
		EXPECT_EQ(us.erase(us.begin(), erase_it), expected_it);
		EXPECT_FALSE(us.empty());
		EXPECT_EQ(us.size(), --size);
	}
}
} // namespace
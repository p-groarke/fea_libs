// TODO : id_set

// #include <fea/containers/unsigned_set.hpp>
// #include <gtest/gtest.h>
//
// namespace fea {
// using namespace experimental;
// }
//
// namespace {
// struct mkey {
//	mkey(unsigned v)
//			: id(v) {
//	}
//
//	constexpr bool operator==(const mkey& rhs) const noexcept {
//		return id == rhs.id;
//	}
//
//	unsigned id = 0;
// };
// } // namespace
//
// template <>
// struct fea::id_hash<mkey> {
//	constexpr unsigned operator()(const mkey& k) const noexcept {
//		return k.id;
//	}
// };
//
// namespace {
// TEST(unsigned_set, basics) {
//	// Empty test
//	{
//		fea::unsigned_set<mkey> us;
//		EXPECT_EQ(us.begin(), us.end());
//		EXPECT_EQ(us.cbegin(), us.cend());
//		EXPECT_TRUE(us.empty());
//		EXPECT_EQ(us.size(), 0u);
//		EXPECT_EQ(us.capacity(), 0u);
//		EXPECT_EQ(us.count(mkey{ 0u }), 0u);
//		EXPECT_FALSE(us.contains(mkey{ 0u }));
//		EXPECT_EQ(us.find(mkey{ 0u }), us.end());
//
//		us.shrink_to_fit();
//		EXPECT_EQ(us.begin(), us.end());
//		EXPECT_EQ(us.cbegin(), us.cend());
//		EXPECT_TRUE(us.empty());
//		EXPECT_EQ(us.size(), 0u);
//		EXPECT_EQ(us.capacity(), 0u);
//		EXPECT_EQ(us.count(mkey{ 0u }), 0u);
//		EXPECT_FALSE(us.contains(mkey{ 0u }));
//		EXPECT_EQ(us.find(mkey{ 0u }), us.end());
//
//		us.clear();
//		EXPECT_EQ(us.begin(), us.end());
//		EXPECT_EQ(us.cbegin(), us.cend());
//		EXPECT_TRUE(us.empty());
//		EXPECT_EQ(us.size(), 0u);
//		EXPECT_EQ(us.capacity(), 0u);
//		EXPECT_EQ(us.count(mkey{ 0u }), 0u);
//		EXPECT_FALSE(us.contains(mkey{ 0u }));
//		EXPECT_EQ(us.find(mkey{ 0u }), us.end());
//
//		std::vector<mkey> v;
//		us = fea::unsigned_set<mkey>(v.begin(), v.end());
//		EXPECT_EQ(us.begin(), us.end());
//		EXPECT_EQ(us.cbegin(), us.cend());
//		EXPECT_TRUE(us.empty());
//		EXPECT_EQ(us.size(), 0u);
//		EXPECT_EQ(us.capacity(), 0u);
//		EXPECT_EQ(us.count(mkey{ 0u }), 0u);
//		EXPECT_FALSE(us.contains(mkey{ 0u }));
//		EXPECT_EQ(us.find(mkey{ 0u }), us.end());
//
//		us.insert(v.begin(), v.end());
//		EXPECT_EQ(us.begin(), us.end());
//		EXPECT_EQ(us.cbegin(), us.cend());
//		EXPECT_TRUE(us.empty());
//		EXPECT_EQ(us.size(), 0u);
//		EXPECT_EQ(us.capacity(), 0u);
//		EXPECT_EQ(us.count(mkey{ 0u }), 0u);
//		EXPECT_FALSE(us.contains(mkey{ 0u }));
//		EXPECT_EQ(us.find(mkey{ 0u }), us.end());
//	}
//
//
//	// Basics
//	{
//		std::vector<mkey> v{ 0u, 2u, 4u, 6u, 8u };
//		unsigned maxid = v.back().id;
//
//		fea::unsigned_set<mkey> us(v.begin(), v.end());
//		EXPECT_NE(us.begin(), us.end());
//		EXPECT_NE(us.cbegin(), us.cend());
//		EXPECT_FALSE(us.empty());
//		EXPECT_EQ(us.size(), 5u);
//		EXPECT_GE(us.capacity(), maxid + 1u);
//		EXPECT_EQ(us.count(mkey{ 0u }), 1u);
//		EXPECT_TRUE(us.contains(mkey{ 0u }));
//		EXPECT_EQ(us.find(mkey{ 0u }), us.begin());
//		EXPECT_EQ(us.find(mkey{ 8u }), --us.end());
//
//		// us.resize(v.back());
//		// EXPECT_EQ(us.begin(), us.end());
//		// EXPECT_EQ(us.cbegin(), us.cend());
//		// EXPECT_FALSE(us.empty());
//		// EXPECT_EQ(us.size(), 5u);
//		// EXPECT_GE(us.capacity(), maxid + 1u);
//		// EXPECT_EQ(us.count(mkey{ 0u }), 1u);
//		// EXPECT_TRUE(us.contains(mkey{ 0u }));
//		// EXPECT_EQ(us.find(mkey{ 0u }), us.begin());
//		// EXPECT_EQ(us.find(mkey{ 8u }), --us.end());
//
//		us.shrink_to_fit();
//		EXPECT_NE(us.begin(), us.end());
//		EXPECT_NE(us.cbegin(), us.cend());
//		EXPECT_FALSE(us.empty());
//		EXPECT_EQ(us.size(), 5u);
//		EXPECT_GE(us.capacity(), maxid + 1u);
//		EXPECT_EQ(us.count(mkey{ 0u }), 1u);
//		EXPECT_TRUE(us.contains(mkey{ 0u }));
//		EXPECT_EQ(us.find(mkey{ 0u }), us.begin());
//		EXPECT_EQ(us.find(mkey{ 8u }), --us.end());
//
//		us.clear();
//		EXPECT_EQ(us.begin(), us.end());
//		EXPECT_EQ(us.cbegin(), us.cend());
//		EXPECT_TRUE(us.empty());
//		EXPECT_EQ(us.size(), 0u);
//		EXPECT_GE(us.capacity(), maxid + 1u);
//		EXPECT_EQ(us.count(mkey{ 0u }), 0u);
//		EXPECT_FALSE(us.contains(mkey{ 0u }));
//		EXPECT_EQ(us.find(mkey{ 0u }), us.end());
//
//		us.insert(v.begin(), v.end());
//		EXPECT_NE(us.begin(), us.end());
//		EXPECT_NE(us.cbegin(), us.cend());
//		EXPECT_FALSE(us.empty());
//		EXPECT_EQ(us.size(), 5u);
//		EXPECT_GE(us.capacity(), maxid + 1u);
//		EXPECT_EQ(us.count(mkey{ 0u }), 1u);
//		EXPECT_TRUE(us.contains(mkey{ 0u }));
//		EXPECT_EQ(us.find(mkey{ 0u }), us.begin());
//		EXPECT_EQ(us.find(mkey{ 8u }), --us.end());
//
//		fea::unsigned_set<mkey> us2;
//		us.swap(us2);
//		EXPECT_NE(us2.begin(), us2.end());
//		EXPECT_NE(us2.cbegin(), us2.cend());
//		EXPECT_FALSE(us2.empty());
//		EXPECT_EQ(us2.size(), 5u);
//		EXPECT_GE(us2.capacity(), maxid + 1u);
//		EXPECT_EQ(us2.count(mkey{ 0u }), 1u);
//		EXPECT_TRUE(us2.contains(mkey{ 0u }));
//		EXPECT_EQ(us2.find(mkey{ 0u }), us2.begin());
//		EXPECT_EQ(us2.find(mkey{ 8u }), --us2.end());
//
//		EXPECT_EQ(us.begin(), us.end());
//		EXPECT_EQ(us.cbegin(), us.cend());
//		EXPECT_TRUE(us.empty());
//		EXPECT_EQ(us.size(), 0u);
//		EXPECT_EQ(us.capacity(), 0u);
//		EXPECT_EQ(us.count(mkey{ 0u }), 0u);
//		EXPECT_FALSE(us.contains(mkey{ 0u }));
//		EXPECT_EQ(us.find(mkey{ 0u }), us.end());
//
//		us.swap(us2);
//		for (mkey k : v) {
//			EXPECT_EQ(us.count(k), 1u);
//			EXPECT_TRUE(us.contains(k));
//		}
//
//		std::vector<mkey> v2{ 1u, 3u, 5u, 7u, 9u };
//		us2 = fea::unsigned_set<mkey>(v2.begin(), v2.end());
//		us.merge(us2);
//		for (mkey k : v) {
//			EXPECT_EQ(us.count(k), 1u);
//			EXPECT_TRUE(us.contains(k));
//			EXPECT_NE(us.find(k), us.end());
//		}
//		for (mkey k : v2) {
//			EXPECT_EQ(us.count(k), 1u);
//			EXPECT_TRUE(us.contains(k));
//			EXPECT_NE(us.find(k), us.end());
//
//			EXPECT_EQ(us2.count(k), 0u);
//			EXPECT_FALSE(us2.contains(k));
//			EXPECT_EQ(us2.find(k), us2.end());
//		}
//	}
//
//	// Iterators
//	{
//		std::vector<mkey> v{ 0u, 2u, 4u, 6u, 8u };
//		// unsigned maxid = v.back().id;
//
//		fea::unsigned_set<mkey> us(v.begin(), v.end());
//		EXPECT_FALSE(us.begin() == us.end());
//		EXPECT_TRUE(us.begin() != us.end());
//		EXPECT_TRUE(us.begin() < us.end());
//		EXPECT_TRUE(us.begin() <= us.end());
//		EXPECT_FALSE(us.begin() > us.end());
//		EXPECT_FALSE(us.begin() >= us.end());
//		EXPECT_FALSE(us.begin() < us.begin());
//		EXPECT_FALSE(us.begin() > us.begin());
//		EXPECT_TRUE(us.begin() <= us.begin());
//		EXPECT_TRUE(us.begin() >= us.begin());
//
//		auto it = us.begin();
//		for (mkey k : v) {
//			EXPECT_EQ(*us.find(k), k);
//			EXPECT_EQ(*it, k);
//			++it;
//		}
//
//		std::vector<mkey> visited;
//		for (it = us.begin(); it != us.end(); ++it) {
//			visited.push_back(*it);
//		}
//		EXPECT_EQ(visited, v);
//
//		visited.clear();
//		auto temp = us.end();
//		--temp;
//		for (it = temp; it != us.begin(); --it) {
//			visited.push_back(*it);
//		}
//		visited.push_back(*it);
//		std::reverse(visited.begin(), visited.end());
//		EXPECT_EQ(visited, v);
//	}
// }
// } // namespace
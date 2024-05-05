#include <fea/containers/unsigned_compact_slotset.hpp>
#include <fea/numerics/random.hpp>
#include <gtest/gtest.h>
#include <numeric>

namespace {
TEST(unsigned_compact_slotset, basics) {
	// Empty test
	{
		fea::unsigned_compact_slotset<unsigned> us;
		EXPECT_EQ(us.begin(), us.end());
		EXPECT_EQ(us.cbegin(), us.cend());
		EXPECT_EQ(std::distance(us.begin(), us.end()), 0);
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
		us = fea::unsigned_compact_slotset<unsigned>(v.begin(), v.end());
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

		fea::unsigned_compact_slotset<unsigned> us(v.begin(), v.end());
		EXPECT_NE(us.begin(), us.end());
		EXPECT_NE(us.cbegin(), us.cend());
		EXPECT_EQ(std::distance(us.begin(), us.end()), 5);
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

		fea::unsigned_compact_slotset<unsigned> us2;
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
		us2 = fea::unsigned_compact_slotset<unsigned>(v2.begin(), v2.end());
		EXPECT_EQ(us.size(), 5u);
		EXPECT_EQ(us2.size(), 5u);

		us.merge(us2);
		EXPECT_EQ(us.size(), 10u);
		EXPECT_EQ(us2.size(), 0u);
		EXPECT_EQ(std::distance(us.begin(), us.end()), 10);

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

		fea::unsigned_compact_slotset<unsigned> us(v.begin(), v.end());
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
		fea::unsigned_compact_slotset<unsigned> us;
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

		EXPECT_EQ(us.erase(v.back()), 1u);
		EXPECT_EQ(us.size(), --size);
		EXPECT_EQ(us.erase(v.back()), 0u);
		EXPECT_EQ(us.size(), size);

		for (unsigned k : v) {
			if (!us.contains(k)) {
				EXPECT_EQ(us.erase(k), 0u);
				EXPECT_EQ(us.size(), size);
			} else {
				EXPECT_EQ(us.erase(k), 1u);
				EXPECT_EQ(us.size(), --size);
			}
		}
	}

	// Extras
	{
		std::vector<unsigned> v{ 2u, 3u, 7u, 8u };
		unsigned maxid = v.back();

		fea::unsigned_compact_slotset<unsigned> us(v.begin(), v.end());
		EXPECT_NE(us.begin(), us.end());
		EXPECT_NE(us.cbegin(), us.cend());
		EXPECT_FALSE(us.empty());
		EXPECT_EQ(us.size(), 4u);
		EXPECT_GE(us.capacity(), maxid + 1u);
		EXPECT_EQ(us.count(0u), 0u);
		EXPECT_EQ(us.count(2u), 1u);
		EXPECT_FALSE(us.contains(0u));
		EXPECT_TRUE(us.contains(2u));
		EXPECT_EQ(us.find(0u), us.end());
		EXPECT_EQ(us.find(2u), us.begin());
		EXPECT_EQ(us.find(8u), --us.end());
	}
}

TEST(unsigned_compact_slotset, fuzzing) {
	constexpr size_t fuzz_count = 1'000;

	enum class op : unsigned {
		shrink,
		insert_key,
		insert_batch,
		erase_key,
		erase_it,
		erase_batched,
		swap,
		merge,
		clear,
		count,
	};
	const std::array<size_t, size_t(op::count)> probabilities{
		10,
		150,
		10,
		50,
		50,
		10,
		10,
		10,
		1,
	};
	const size_t max_rand = std::accumulate(probabilities.begin(),
									probabilities.end(), size_t(0))
						  - 1u;

	auto get_op = [&](size_t rand_val) {
		for (size_t i = 0; i < probabilities.size(); ++i) {
			if (rand_val < probabilities[i]) {
				return op(i);
			}
			rand_val -= probabilities[i];
		}
		assert(false);
		return op::count;
	};

	std::vector<unsigned> keys(fuzz_count * 10u);
	fea::random_fill(keys.begin(), keys.end(), 0u, unsigned(fuzz_count));

	std::set<unsigned> expected;
	fea::unsigned_compact_slotset<unsigned> tested;
	for (size_t i = 0; i < fuzz_count; ++i) {
		size_t rand_op = fea::random_val(size_t(0), max_rand);
		op mop = get_op(rand_op);

		switch (mop) {
		case op::shrink: {
			tested.shrink_to_fit();
			EXPECT_TRUE(
					std::equal(tested.begin(), tested.end(), expected.begin()));
			EXPECT_EQ(expected.size(), tested.size());
			EXPECT_EQ(std::distance(expected.begin(), expected.end()),
					std::distance(tested.begin(), tested.end()));
			EXPECT_EQ(expected.empty(), tested.empty());
		} break;
		case op::insert_key: {
			unsigned k = keys.back();
			keys.pop_back();

			auto ep = expected.insert(k);
			auto tp = tested.insert(k);

			EXPECT_EQ(ep.second, tp.second);
			EXPECT_TRUE(
					std::equal(tested.begin(), tested.end(), expected.begin()));
			EXPECT_EQ(expected.size(), tested.size());
			EXPECT_EQ(std::distance(expected.begin(), expected.end()),
					std::distance(tested.begin(), tested.end()));
			EXPECT_EQ(expected.empty(), tested.empty());
			EXPECT_EQ(std::distance(expected.begin(), ep.first),
					std::distance(tested.begin(), tp.first));
			EXPECT_EQ(expected.count(k), tested.count(k));
			EXPECT_NE(tested.find(k), tested.end());
			EXPECT_EQ(std::distance(expected.begin(), expected.find(k)),
					std::distance(tested.begin(), tested.find(k)));
		} break;
		case op::insert_batch: {
			std::vector<unsigned> range(keys.end() - 10, keys.end());
			keys.erase(keys.end() - 10, keys.end());

			expected.insert(range.begin(), range.end());
			tested.insert(range.begin(), range.end());

			EXPECT_TRUE(
					std::equal(tested.begin(), tested.end(), expected.begin()));
			EXPECT_EQ(expected.size(), tested.size());
			EXPECT_EQ(std::distance(expected.begin(), expected.end()),
					std::distance(tested.begin(), tested.end()));
			EXPECT_EQ(expected.empty(), tested.empty());

			for (unsigned k : range) {
				EXPECT_EQ(expected.count(k), tested.count(k));
				EXPECT_NE(tested.find(k), tested.end());
				EXPECT_EQ(std::distance(expected.begin(), expected.find(k)),
						std::distance(tested.begin(), tested.find(k)));
			}
		} break;
		case op::erase_key: {
			unsigned k = keys.back();
			keys.pop_back();

			size_t er = expected.erase(k);
			size_t tr = tested.erase(k);

			EXPECT_EQ(er, tr);
			EXPECT_TRUE(
					std::equal(tested.begin(), tested.end(), expected.begin()));
			EXPECT_EQ(expected.size(), tested.size());
			EXPECT_EQ(std::distance(expected.begin(), expected.end()),
					std::distance(tested.begin(), tested.end()));
			EXPECT_EQ(expected.empty(), tested.empty());
			EXPECT_EQ(expected.count(k), tested.count(k));
			EXPECT_EQ(tested.find(k), tested.end());
			EXPECT_EQ(std::distance(expected.begin(), expected.find(k)),
					std::distance(tested.begin(), tested.find(k)));
		} break;
		case op::erase_it: {
			unsigned k = keys.back();
			keys.pop_back();

			auto feit = expected.find(k);
			auto ftit = tested.find(k);
			if (feit == expected.end()) {
				EXPECT_EQ(ftit, tested.end());
				continue;
			}

			auto eit = expected.erase(feit);
			auto tit = tested.erase(ftit);

			EXPECT_EQ(std::distance(expected.begin(), eit),
					std::distance(tested.begin(), tit));
			EXPECT_TRUE(
					std::equal(tested.begin(), tested.end(), expected.begin()));
			EXPECT_EQ(expected.size(), tested.size());
			EXPECT_EQ(std::distance(expected.begin(), expected.end()),
					std::distance(tested.begin(), tested.end()));
			EXPECT_EQ(expected.empty(), tested.empty());
			EXPECT_EQ(expected.count(k), tested.count(k));
			EXPECT_EQ(tested.find(k), tested.end());
			EXPECT_EQ(std::distance(expected.begin(), expected.find(k)),
					std::distance(tested.begin(), tested.find(k)));
		} break;
		case op::erase_batched: {
			if (expected.size() < 10) {
				EXPECT_EQ(expected.size(), tested.size());
				continue;
			}
			std::vector<unsigned> range(
					std::next(tested.end(), -10), tested.end());
			EXPECT_EQ(std::vector<unsigned>(
							  std::next(expected.end(), -10), expected.end()),
					range);

			auto eit = expected.erase(
					std::next(expected.end(), -10), expected.end());
			auto tit = tested.erase(std::next(tested.end(), -10), tested.end());
			EXPECT_EQ(std::distance(expected.begin(), eit),
					std::distance(tested.begin(), tit));

			EXPECT_TRUE(
					std::equal(tested.begin(), tested.end(), expected.begin()));
			EXPECT_EQ(expected.size(), tested.size());
			EXPECT_EQ(std::distance(expected.begin(), expected.end()),
					std::distance(tested.begin(), tested.end()));
			EXPECT_EQ(expected.empty(), tested.empty());

			for (unsigned k : range) {
				EXPECT_EQ(expected.count(k), tested.count(k));
				EXPECT_EQ(tested.find(k), tested.end());
				EXPECT_EQ(std::distance(expected.begin(), expected.find(k)),
						std::distance(tested.begin(), tested.find(k)));
			}
		} break;
		case op::swap: {
			std::set<unsigned> expected_s;
			fea::unsigned_compact_slotset<unsigned> tested_s;
			expected.swap(expected_s);
			tested.swap(tested_s);

			EXPECT_TRUE(
					std::equal(tested.begin(), tested.end(), expected.begin()));
			EXPECT_EQ(expected.size(), tested.size());
			EXPECT_EQ(std::distance(expected.begin(), expected.end()),
					std::distance(tested.begin(), tested.end()));
			EXPECT_EQ(expected.empty(), tested.empty());
			EXPECT_TRUE(std::equal(
					tested_s.begin(), tested_s.end(), expected_s.begin()));
			EXPECT_EQ(expected_s.size(), tested_s.size());
			EXPECT_EQ(std::distance(expected_s.begin(), expected_s.end()),
					std::distance(tested_s.begin(), tested_s.end()));
			EXPECT_EQ(expected_s.empty(), tested_s.empty());

			expected.swap(expected_s);
			tested.swap(tested_s);

			EXPECT_TRUE(
					std::equal(tested.begin(), tested.end(), expected.begin()));
			EXPECT_EQ(expected.size(), tested.size());
			EXPECT_EQ(std::distance(expected.begin(), expected.end()),
					std::distance(tested.begin(), tested.end()));
			EXPECT_EQ(expected.empty(), tested.empty());
			EXPECT_TRUE(std::equal(
					tested_s.begin(), tested_s.end(), expected_s.begin()));
			EXPECT_EQ(expected_s.size(), tested_s.size());
			EXPECT_EQ(std::distance(expected_s.begin(), expected_s.end()),
					std::distance(tested_s.begin(), tested_s.end()));
			EXPECT_EQ(expected_s.empty(), tested_s.empty());

		} break;
		case op::merge: {
			std::vector<unsigned> range(keys.end() - 20, keys.end());
			keys.erase(keys.end() - 20, keys.end());

			std::set<unsigned> expected_m(range.begin(), range.end());
			fea::unsigned_compact_slotset<unsigned> tested_m(
					range.begin(), range.end());

			EXPECT_TRUE(std::equal(
					tested_m.begin(), tested_m.end(), expected_m.begin()));
			EXPECT_EQ(expected_m.size(), tested_m.size());
			EXPECT_EQ(std::distance(expected_m.begin(), expected_m.end()),
					std::distance(tested_m.begin(), tested_m.end()));
			EXPECT_EQ(expected_m.empty(), tested_m.empty());

			expected.merge(expected_m);
			tested.merge(tested_m);

			EXPECT_TRUE(
					std::equal(tested.begin(), tested.end(), expected.begin()));
			EXPECT_EQ(expected.size(), tested.size());
			EXPECT_EQ(std::distance(expected.begin(), expected.end()),
					std::distance(tested.begin(), tested.end()));
			EXPECT_EQ(expected.empty(), tested.empty());
			EXPECT_TRUE(std::equal(
					tested_m.begin(), tested_m.end(), expected_m.begin()));
			EXPECT_EQ(expected_m.size(), tested_m.size());
			EXPECT_EQ(std::distance(expected_m.begin(), expected_m.end()),
					std::distance(tested_m.begin(), tested_m.end()));
			EXPECT_EQ(expected_m.empty(), tested_m.empty());

			for (unsigned k : range) {
				EXPECT_EQ(expected.count(k), tested.count(k));
				EXPECT_NE(tested.find(k), tested.end());
				EXPECT_EQ(std::distance(expected.begin(), expected.find(k)),
						std::distance(tested.begin(), tested.find(k)));
			}

		} break;
		case op::clear: {
			expected.clear();
			tested.clear();

			EXPECT_TRUE(
					std::equal(tested.begin(), tested.end(), expected.begin()));
			EXPECT_EQ(expected.size(), tested.size());
			EXPECT_EQ(std::distance(expected.begin(), expected.end()),
					std::distance(tested.begin(), tested.end()));
			EXPECT_EQ(expected.empty(), tested.empty());
		} break;
		default: {
			assert(false);
		}
		}
	}
}
} // namespace
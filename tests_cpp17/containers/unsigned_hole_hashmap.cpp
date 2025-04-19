#include <fea/containers/unsigned_hole_hashmap.hpp>
#include <fea/utility/platform.hpp>
#include <gtest/gtest.h>
#include <memory>
#include <random>
#include <unordered_map>
#include <unordered_set>

namespace {
struct test2 {
	test2() = default;
	~test2() = default;
	test2(const test2&) = default;
	test2(test2&&) = default;
	test2& operator=(const test2&) = default;
	test2& operator=(test2&&) = default;

	template <class T>
	test2(T v)
			: val(size_t(v)) {
	}

	size_t val = 42;
};
bool operator==(const test2& lhs, const test2& rhs) {
	return lhs.val == rhs.val;
}
bool operator!=(const test2& lhs, const test2& rhs) {
	return !operator==(lhs, rhs);
}

template <class KeyT>
void do_basic_test() {
	constexpr KeyT small_num = 10;

	fea::unsigned_hole_hashmap<KeyT, test2> map1{ size_t(small_num) };
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

	for (KeyT i = 0; i < small_num; ++i) {
		auto ret_pair = map1.insert(i, i);
		EXPECT_TRUE(ret_pair.second);
		EXPECT_EQ(*ret_pair.first, test2{ i });
	}

	EXPECT_EQ(
			size_t(std::distance(map1.begin(), map1.end())), size_t(small_num));
	EXPECT_EQ(size_t(std::distance(map1.key_begin(), map1.key_end())),
			size_t(small_num));

	for (KeyT i = 0; i < small_num; ++i) {
		EXPECT_EQ(*(map1.data() + size_t(i)), test2{ i });
		EXPECT_EQ(*(map1.key_data() + size_t(i)), i);
	}

	for (KeyT i = 0; i < small_num; ++i) {
		auto ret_pair = map1.insert(i, i);
		EXPECT_FALSE(ret_pair.second);
		EXPECT_EQ(*ret_pair.first, test2{ i });
	}
	for (KeyT i = 0; i < small_num; ++i) {
		test2 t{ i };
		auto ret_pair = map1.insert(i, t);
		EXPECT_FALSE(ret_pair.second);
		EXPECT_EQ(*ret_pair.first, t);
	}

	fea::unsigned_hole_hashmap<KeyT, test2> map2{ map1 };
	fea::unsigned_hole_hashmap<KeyT, test2> map_ded{ map1 };
	fea::unsigned_hole_hashmap<KeyT, test2> map3{ std::move(map_ded) };

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

	test2 ttt;
#if FEA_DEBUG || FEA_NOTHROW
	EXPECT_DEATH(ttt = map1.at(1), "");
#else
	EXPECT_THROW(ttt = map1.at(1), std::out_of_range);
#endif

	EXPECT_FALSE(map1.contains(1));
	EXPECT_EQ(map1.count(1), 0u);

	EXPECT_EQ(map1[1], test2{});

	map1.at(1) = test2{ 1 };
	EXPECT_NE(map1[1], test2{});


	map1 = map2;

	for (KeyT i = 0; i < small_num; ++i) {
		EXPECT_EQ(map1[i], test2{ i });
		EXPECT_EQ(map1.at(i), test2{ i });
		EXPECT_EQ(map1.at_unchecked(i), test2{ i });
		EXPECT_EQ(*map1.find(i), test2{ i });
		EXPECT_TRUE(map1.contains(i));
		EXPECT_EQ(map1.count(i), 1u);

		EXPECT_EQ(map2[i], test2{ i });
		EXPECT_EQ(map2.at(i), test2{ i });
		EXPECT_EQ(map2.at_unchecked(i), test2{ i });
		EXPECT_EQ(*map2.find(i), test2{ i });
		EXPECT_TRUE(map2.contains(i));
		EXPECT_EQ(map2.count(i), 1u);

		EXPECT_EQ(map3[i], test2{ i });
		EXPECT_EQ(map3.at(i), test2{ i });
		EXPECT_EQ(map3.at_unchecked(i), test2{ i });
		EXPECT_EQ(*map3.find(i), test2{ i });
		EXPECT_TRUE(map2.contains(i));
		EXPECT_EQ(map2.count(i), 1u);
	}

	map1.erase(1);
	EXPECT_EQ(map1.size(), small_num - 1u);
	EXPECT_NE(map1, map2);
	EXPECT_NE(map1, map3);
	EXPECT_FALSE(map1.contains(1));
	EXPECT_EQ(map1.count(1), 0u);

	map1.insert(1, 1);
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

#if FEA_DEBUG || FEA_NOTHROW
	EXPECT_DEATH(ttt = map1.at(1), "");
#else
	EXPECT_THROW(ttt = map1.at(1), std::out_of_range);
#endif

	EXPECT_FALSE(map1.contains(1));
	EXPECT_EQ(map1.count(1), 0u);

	map_ded = map2;
	map1 = std::move(map_ded);

	map1.erase(map1.begin());
	EXPECT_EQ(map1.size(), small_num - 1u);
	EXPECT_NE(map1, map2);
	EXPECT_NE(map1, map3);
	EXPECT_FALSE(map1.contains(0));
	EXPECT_EQ(map1.count(0), 0u);

#if FEA_DEBUG || FEA_NOTHROW
	EXPECT_DEATH(ttt = map1.at(0), "");
#else
	EXPECT_THROW(ttt = map1.at(0), std::out_of_range);
#endif

	map1 = map2;

	for (it = map1.begin(); it != map1.end();) {
		if (it->val % 2 == 1) {
			size_t idx = std::distance(map1.begin(), it);
			map1.erase(it);
			it = map1.begin() + idx;
		} else {
			++it;
		}
	}
	EXPECT_EQ(map1.size(), small_num / 2u);

	for (auto t : map1) {
		EXPECT_EQ(t.val % 2, 0u);
	}

	map1 = map2;

	for (it = map1.begin() + 1; it != map1.end();) {
		if (it->val % 2 == 0) {
			size_t idx = std::distance(map1.begin(), it);
			map1.erase(it, std::next(it, 2));
			it = map1.begin() + idx;
		} else {
			++it;
		}
	}
	EXPECT_EQ(map1.size(), 4u);
	EXPECT_TRUE(map1.contains(0));
	EXPECT_TRUE(map1.contains(1));
	EXPECT_TRUE(map1.contains(9));
	EXPECT_TRUE(map1.contains(7));

	map1 = map2;

	{
		auto ret_pair1 = map1.insert(19, 19);
		EXPECT_TRUE(ret_pair1.second);

		auto ret_pair2 = map1.insert(19, 42);
		EXPECT_FALSE(ret_pair2.second);
		EXPECT_EQ(ret_pair2.first, ret_pair1.first);
		EXPECT_EQ(map1.at(19), test2{ 19 });
		EXPECT_EQ(map1.at_unchecked(19), test2{ 19 });

		ret_pair2 = map1.insert_or_assign(19, test2{ 42 });
		EXPECT_FALSE(ret_pair2.second);
		EXPECT_EQ(ret_pair2.first, ret_pair1.first);
		EXPECT_EQ(map1.at(19), test2{ 42 });
		EXPECT_EQ(map1.at_unchecked(19), test2{ 42 });
		ret_pair2 = map1.insert_or_assign(19, test2{ 19 });
	}

	map2.insert(20, { 20 });
	map3.insert(20, { 20 });
	EXPECT_NE(map1, map2);
	EXPECT_NE(map1, map3);

	{
		map1.emplace(20, test2{ 20 });
		test2 t{ 21 };
		map1.emplace(21, t);
	}

	map1 = map2;
	map3 = map2;

	map1 = fea::unsigned_hole_hashmap<KeyT, test2>(
			{ { 0, { 0 } }, { 1, { 1 } }, { 2, { 2 } } });
	map2 = fea::unsigned_hole_hashmap<KeyT, test2>(
			{ { 3, { 3 } }, { 4, { 4 } }, { 5, { 5 } } });
	map3 = fea::unsigned_hole_hashmap<KeyT, test2>(
			{ { 6, { 6 } }, { 7, { 7 } }, { 8, { 8 } } });

	EXPECT_EQ(map1.size(), 3u);
	EXPECT_TRUE(map1.contains(0));
	EXPECT_TRUE(map1.contains(1));
	EXPECT_TRUE(map1.contains(2));
	EXPECT_EQ(map1.at(0), test2{ 0 });
	EXPECT_EQ(map1.at_unchecked(0), test2{ 0 });
	EXPECT_EQ(map1[1], test2{ 1 });
	EXPECT_EQ(*map1.find(2), test2{ 2 });

	EXPECT_EQ(map2.size(), 3u);
	EXPECT_TRUE(map2.contains(3));
	EXPECT_TRUE(map2.contains(4));
	EXPECT_TRUE(map2.contains(5));
	EXPECT_EQ(map2.at(3), test2{ 3 });
	EXPECT_EQ(map2.at_unchecked(3), test2{ 3 });
	EXPECT_EQ(map2[4], test2{ 4 });
	EXPECT_EQ(*map2.find(5), test2{ 5 });

	EXPECT_EQ(map3.size(), 3u);
	EXPECT_TRUE(map3.contains(6));
	EXPECT_TRUE(map3.contains(7));
	EXPECT_TRUE(map3.contains(8));
	EXPECT_EQ(map3[7], test2{ 7 });
	EXPECT_EQ(*map3.find(8), test2{ 8 });

	{
		fea::unsigned_hole_hashmap<KeyT, test2> map1_back = map1;
		fea::unsigned_hole_hashmap<KeyT, test2> map2_back{ map2 };
		fea::unsigned_hole_hashmap<KeyT, test2> map3_back{ map3 };

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

	EXPECT_EQ(map1.at(0), test2{ 0 });
	EXPECT_EQ(map1.at_unchecked(0), test2{ 0 });
	EXPECT_EQ(map1[1], test2{ 1 });
	EXPECT_EQ(*map1.find(2), test2{ 2 });
	EXPECT_EQ(map1.at(3), test2{ 3 });
	EXPECT_EQ(map1.at_unchecked(3), test2{ 3 });
	EXPECT_EQ(map1[4], test2{ 4 });
	EXPECT_EQ(*map1.find(5), test2{ 5 });

	// TODO :
	// map2 = fea::unsigned_hole_hashmap<size_t, test2>(map1.begin(),
	// map1.end()); EXPECT_EQ(map1.size(), map2.size()); EXPECT_EQ(map1, map2);

	// map3.clear();
	// map3.insert(map1.begin(), map1.end());
	// EXPECT_EQ(map1.size(), map3.size());
	// EXPECT_EQ(map1, map3);
	// EXPECT_EQ(map2.size(), map3.size());
	// EXPECT_EQ(map2, map3);

	// erase
	const KeyT init_hash_max = 7u;
	const double max_load = map1.max_load_factor();
	const KeyT num_keys = KeyT(init_hash_max * max_load) + 1u;

	{
		map1 = {};
		const KeyT key_init = init_hash_max;

		KeyT clashing_key = key_init;
		for (KeyT i = 0; i < num_keys; ++i) {
			map1.insert(clashing_key, { i });
			clashing_key *= 2;
		}

		map1.erase(key_init);
		EXPECT_FALSE(map1.contains(key_init));

		clashing_key = key_init * 2;
		for (KeyT i = 0; i < num_keys - 1; ++i) {
			EXPECT_TRUE(map1.contains(clashing_key));
			clashing_key *= 2;
		}
	}

	{
		map1 = {};
		constexpr KeyT key_init = 6;

		KeyT clashing_key = key_init;
		for (KeyT i = 0; i < num_keys; ++i) {
			map1.insert(clashing_key, { i });
			clashing_key *= 2;
		}

		map1.erase(key_init);
		EXPECT_FALSE(map1.contains(key_init));

		clashing_key = key_init * 2;
		for (KeyT i = 0; i < num_keys - 1; ++i) {
			EXPECT_TRUE(map1.contains(clashing_key));
			clashing_key *= 2;
		}
	}
}

TEST(flat_unsigned_hashmap, basics) {
	do_basic_test<uint8_t>();
	do_basic_test<uint16_t>();
	do_basic_test<uint32_t>();
	do_basic_test<uint64_t>();
}


TEST(flat_unsigned_hashmap, uniqueptr) {
	fea::unsigned_hole_hashmap<size_t, std::unique_ptr<unsigned>> map;

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
		map.insert(2, std::move(test));
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

template <class KeyT>
std::vector<KeyT> get_random_vec(size_t size) {
	std::vector<KeyT> ret(size);


	return ret;
}

template <class KeyT>
void do_fuzz_test() {
	constexpr size_t max_val = 254;

	fea::unsigned_hole_hashmap<KeyT, KeyT> map;

	auto test_it = [&](const std::vector<KeyT>& rand_numbers) {
		std::unordered_map<KeyT, size_t> visited;

		for (size_t i = 0; i < rand_numbers.size(); ++i) {
			KeyT k = rand_numbers[i];
			if (visited.count(k) == 0) {
				EXPECT_FALSE(map.contains(k));
				visited.insert({ k, 0 });
			}

			map.emplace(k, k);
			map.insert(k, k);
			map.insert_or_assign(k, k);

			++visited.at(k);

			EXPECT_TRUE(map.contains(k));
			EXPECT_EQ(map.at(k), k);
		}

		for (size_t i = 0; i < rand_numbers.size(); ++i) {
			KeyT k = KeyT(rand_numbers[i]);

			if (visited.at(k) != 0) {
				EXPECT_TRUE(map.contains(k));
				EXPECT_EQ(map.at(k), k);
				map.erase(k);
				visited.at(k) = 0;
			}

			if (visited.at(k) == 0) {
				EXPECT_FALSE(map.contains(k));
			}
		}

		EXPECT_EQ(map.size(), 0u);
	};

	// Contiguous vals random.
	std::vector<KeyT> rand_numbers(max_val);
	std::iota(rand_numbers.begin(), rand_numbers.end(), KeyT(0));
	auto rng = std::mt19937_64{};
	std::shuffle(rand_numbers.begin(), rand_numbers.end(), rng);
	test_it(rand_numbers);

	// Contiguous vals random.
	rand_numbers.clear();
	rand_numbers.resize(max_val / 2);
	std::iota(rand_numbers.begin(), rand_numbers.end(), KeyT(0));
	std::shuffle(rand_numbers.begin(), rand_numbers.end(), rng);
	test_it(rand_numbers);

	// Random vals with duplicates.
	rand_numbers.clear();
	std::uniform_int_distribution<size_t> uni_dist{ 0, max_val };
	for (size_t i = 0; i < max_val; ++i) {
		rand_numbers.push_back(KeyT(uni_dist(rng)));
	}
	test_it(rand_numbers);

	// Random vals with duplicates.
	rand_numbers.clear();
	uni_dist = std::uniform_int_distribution<size_t>{ 0, max_val / 2 };
	for (size_t i = 0; i < max_val; ++i) {
		rand_numbers.push_back(KeyT(uni_dist(rng)));
	}
	test_it(rand_numbers);

	// Random vals with duplicates.
	rand_numbers.clear();
	std::normal_distribution<> norm_dist{ 0.0, double(max_val) };
	for (size_t i = 0; i < max_val; ++i) {
		rand_numbers.push_back(KeyT(uni_dist(rng)));
	}
	test_it(rand_numbers);
}

TEST(flat_unsigned_hashmap, fuzzing) {
	do_fuzz_test<uint8_t>();
	do_fuzz_test<uint16_t>();
	do_fuzz_test<uint32_t>();
	do_fuzz_test<uint64_t>();
}

TEST(flat_unsigned_hashmap, trailing_holes) {
	fea::unsigned_hole_hashmap<size_t, size_t> map;

	// 10 will grow to hash 17, size 34.
	constexpr size_t num = 10;

	auto get_key = [](size_t i) {
		size_t key = 16 * i; // 17 - 1
		while (key % 17 != 16) {
			key += 1;
		}
		return key;
	};

	// Attack the trailing collision holes.
	for (size_t i = 1; i < num; ++i) {
		size_t key = get_key(i);
		map.insert(key, key);
		EXPECT_TRUE(map.contains(key));
		EXPECT_EQ(map.at(key), key);
	}

	for (size_t i = 1; i < num; ++i) {
		size_t key = get_key(i);
		EXPECT_TRUE(map.contains(key));
		EXPECT_EQ(map.at(key), key);
	}
}

TEST(flat_unsigned_hashmap, even_resize) {
	fea::unsigned_hole_hashmap<unsigned, unsigned> map;
	std::vector<unsigned> keys{ 1037, 2075, 70, 71, 1316, 518, 1318, 658, 659,
		2072, 1036, 322, 2073 };

	for (unsigned k : keys) {
		map.insert(k, k);
	}

	for (unsigned k : keys) {
		EXPECT_TRUE(map.contains(k));
	}
}

} // namespace

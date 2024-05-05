#include <fea/benchmark/benchmark.hpp>
#include <fea/containers/unsigned_compact_slotset.hpp>
#include <fea/containers/unsigned_slotset.hpp>
#include <fea/numerics/random.hpp>
#include <fea/utils/platform.hpp>
#include <format>
#include <gtest/gtest.h>
#include <iostream>
#include <set>
#include <string>
#include <string_view>
#include <unordered_set>

namespace {
using key_t = uint64_t;

#if FEA_RELEASE
constexpr size_t num_keys = 10'000'000u;
// constexpr key_t max_key = 10'000'000u;
constexpr key_t max_key = 100'000u;
#else
constexpr size_t num_keys = 10'000u;
constexpr key_t max_key = 10'000u;
#endif


// Create a random side effect to prevent compiler
// over-optimization.
key_t random_key = fea::random_val(key_t(0), max_key);
std::vector<key_t> to_print;
void print_random_key(const auto& set) {
	auto it = set.find(random_key);
	if (it != set.end()) {
		to_print.push_back(*it);
	}
}

TEST(unsigned_slotsets, insertion_deletion) {
	std::vector<key_t> keys(num_keys);
	fea::random_fill(keys.begin(), keys.end(), key_t(0), key_t(max_key));

	fea::bench::suite suite;
	suite.average(5u);
	// suite.sleep_between(std::chrono::milliseconds(500));

	{
		suite.title(std::format("{} Insertions", num_keys));
		auto benchmark = [&](auto& set) {
			for (key_t k : keys) {
				set.insert(k);
			}
			print_random_key(set);
		};
		suite.benchmark("fea::unsigned_slotset", [&]() {
			fea::unsigned_slotset<key_t> set;
			benchmark(set);
		});
		suite.benchmark("fea::unsigned_compact_slotset", [&]() {
			fea::unsigned_compact_slotset<key_t> set;
			benchmark(set);
		});
		suite.benchmark("std::unordered_set", [&]() {
			std::unordered_set<key_t> set;
			benchmark(set);
		});
		suite.benchmark("std::set", [&]() {
			std::set<key_t> set;
			benchmark(set);
		});
		suite.print();
	}

	{
		suite.title(std::format("{} Insertions (reserved)", num_keys));
		auto benchmark = [&](auto& set) {
			for (key_t k : keys) {
				set.insert(k);
			}
			print_random_key(set);
		};

		suite.benchmark("fea::unsigned_slotset", [&]() {
			fea::unsigned_slotset<key_t> set;
			set.reserve(max_key);
			benchmark(set);
		});
		suite.benchmark("fea::unsigned_compact_slotset", [&]() {
			fea::unsigned_compact_slotset<key_t> set;
			set.reserve(max_key);
			benchmark(set);
		});
		suite.benchmark("std::unordered_set", [&]() {
			std::unordered_set<key_t> set;
			set.reserve(max_key);
			benchmark(set);
		});
		// suite.benchmark("std::set", [&]() {
		//	std::set<key_t> set;
		//	insert_reserved_benchmark(set);
		// });
		suite.print();
	}

	{
		suite.title(std::format("{} Insertions (batched)", num_keys));
		auto benchmark
				= [&](auto& set) { set.insert(keys.begin(), keys.end()); };
		suite.benchmark("fea::unsigned_slotset", [&]() {
			fea::unsigned_slotset<key_t> set;
			benchmark(set);
		});
		suite.benchmark("fea::unsigned_compact_slotset", [&]() {
			fea::unsigned_compact_slotset<key_t> set;
			benchmark(set);
		});
		suite.benchmark("std::unordered_set", [&]() {
			std::unordered_set<key_t> set;
			benchmark(set);
		});
		suite.benchmark("std::set", [&]() {
			std::set<key_t> set;
			benchmark(set);
		});
		suite.print();
	}

	{
		suite.title(std::format(
				"{} Insertions (without set destruction)", num_keys));
		auto benchmark = [&](auto& set) {
			for (key_t k : keys) {
				set.insert(k);
			}
			print_random_key(set);
		};

		fea::unsigned_slotset<key_t> uss;
		fea::unsigned_compact_slotset<key_t> ucss;
		std::unordered_set<key_t> us;
		std::set<key_t> s;
		suite.benchmark("fea::unsigned_slotset", [&]() {
			uss.clear();
			benchmark(uss);
		});
		suite.benchmark("fea::unsigned_compact_slotset", [&]() {
			ucss.clear();
			benchmark(ucss);
		});
		suite.benchmark("std::unordered_set", [&]() {
			us.clear();
			benchmark(us);
		});
		suite.benchmark("std::set", [&]() {
			s.clear();
			benchmark(s);
		});
		suite.print();
	}

	{
		suite.title(std::format("{} Insertions and Deletions", num_keys));
		auto benchmark = [&](auto& set) {
			for (size_t i = 0; i < num_keys; ++i) {
				key_t k = keys[i];
				if (i % 2u == 0) {
					set.insert(k);
				} else {
					set.erase(k);
				}
			}
			print_random_key(set);
		};

		suite.benchmark("fea::unsigned_slotset", [&]() {
			fea::unsigned_slotset<key_t> set;
			benchmark(set);
		});
		suite.benchmark("fea::unsigned_compact_slotset", [&]() {
			fea::unsigned_compact_slotset<key_t> set;
			benchmark(set);
		});
		suite.benchmark("std::unordered_set", [&]() {
			std::unordered_set<key_t> set;
			benchmark(set);
		});
		suite.benchmark("std::set", [&]() {
			std::set<key_t> set;
			benchmark(set);
		});
		suite.print();
	}

	{
		suite.title(std::format("{} Iteration", num_keys));
		auto benchmark = [&](auto& set) {
			for (key_t k : set) {
				if (k == random_key) {
					to_print.push_back(k);
				}
			}
		};

		fea::unsigned_slotset<key_t> uss;
		fea::unsigned_compact_slotset<key_t> ucss;
		std::unordered_set<key_t> us;
		std::set<key_t> s;
		uss.insert(keys.begin(), keys.end());
		ucss.insert(keys.begin(), keys.end());
		us.insert(keys.begin(), keys.end());
		s.insert(keys.begin(), keys.end());

		suite.benchmark("fea::unsigned_slotset", [&]() { benchmark(uss); });
		suite.benchmark(
				"fea::unsigned_compact_slotset", [&]() { benchmark(ucss); });
		suite.benchmark("std::unordered_set", [&]() { benchmark(us); });
		suite.benchmark("std::set", [&]() { benchmark(s); });
		suite.print();
	}
}

TEST(unsigned_slotsets, ignore_sideeffects) {
	for (key_t k : to_print) {
		std::cout << k << " ";
	}
	std::cout << std::endl;
}
} // namespace
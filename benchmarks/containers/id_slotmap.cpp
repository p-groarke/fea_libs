#include <algorithm>
#include <array>
#include <cstdio>
#include <fea/benchmark/benchmark.hpp>
#include <fea/containers/id_slotmap.hpp>
#include <fea/utils/platform.hpp>
#include <gtest/gtest.h>
#include <map>
#include <random>
#include <string>
#include <unordered_map>

namespace {
#if FEA_RELEASE
constexpr size_t num_keys = 5'000'000;
#else
constexpr size_t num_keys = 100'000;
#endif

struct small_obj {
	float x{ 42 };
	float y{ 42 };
	float z{ 42 };
};

struct big_obj {
	big_obj() {
		data.fill(42);
	}
	~big_obj() {
	}
	big_obj(const big_obj& other) {
		data = other.data;
	}
	big_obj& operator=(const big_obj& other) {
		if (&other == this)
			return *this;

		data = other.data;
		return *this;
	}
	big_obj(big_obj&&) = delete;
	big_obj& operator=(big_obj&&) = delete;

	std::array<uint8_t, 1024> data{};
};

void benchmarks(const std::vector<size_t>& keys) {
	std::array<char, 128> title;
	title.fill('\0');

	fea::bench::suite suite;

	// Containers
	std::map<size_t, small_obj> map_small;
	std::unordered_map<size_t, small_obj> unordered_map_small;
	fea::id_slotmap<size_t, small_obj> unsigned_map_small;

	std::map<size_t, big_obj> map_big;
	std::unordered_map<size_t, big_obj> unordered_map_big;
	fea::id_slotmap<size_t, big_obj> unsigned_map_big;


	// Preheat
	for (size_t i = 0; i < keys.size(); ++i) {
		map_small.insert({ keys[i], { float(i), float(i), float(i) } });
	}
	for (size_t i = 0; i < keys.size(); ++i) {
		unordered_map_small.insert(
				{ keys[i], { float(i), float(i), float(i) } });
	}
	for (size_t i = 0; i < keys.size(); ++i) {
		unsigned_map_small.insert(
				{ keys[i], { float(i), float(i), float(i) } });
	}
	for (size_t i = 0; i < keys.size(); ++i) {
		map_big.insert({ keys[i], {} });
	}
	for (size_t i = 0; i < keys.size(); ++i) {
		unordered_map_big.insert({ keys[i], {} });
	}
	for (size_t i = 0; i < keys.size(); ++i) {
		unsigned_map_big.insert({ keys[i], {} });
	}
	printf("Num unique keys : %zu\n\n", map_small.size());
	// printf("%zu\n", unordered_map_small.size());
	// printf("%zu\n", unsigned_map_small.size());

	// printf("%f\n", map_small[rand() % num_keys].x);
	// printf("%f\n", unordered_map_small[rand() % num_keys].x);
	// printf("%f\n", unsigned_map_small[rand() % num_keys].x);
	// printf("%u\n", map_big[rand() % num_keys].);
	// printf("%u\n", unordered_map_big[rand() % num_keys].x);
	// printf("%u\n", unsigned_map_big[rand() % num_keys].x);


	// Bench : copy ctor
	title.fill('\0');
	std::snprintf(title.data(), title.size(), "Copy ctor %zu small objects",
			map_small.size());
	suite.title(title.data());

	suite.benchmark("std::map copy ctor",
			[&]() { std::map<size_t, small_obj> cpy(map_small); });
	suite.benchmark("std::unordered_map copy ctor", [&]() {
		std::unordered_map<size_t, small_obj> cpy(unordered_map_small);
	});
	suite.benchmark("fea::id_slotmap copy ctor", [&]() {
		fea::id_slotmap<size_t, small_obj> cpy(unsigned_map_small);
	});
	suite.print();


	// Bench : copy ctor big
	title.fill('\0');
	std::snprintf(title.data(), title.size(), "Copy ctor %zu big objects",
			map_big.size());
	suite.title(title.data());

	suite.benchmark("std::map copy ctor",
			[&]() { std::map<size_t, big_obj> cpy(map_big); });
	suite.benchmark("std::unordered_map copy ctor", [&]() {
		std::unordered_map<size_t, big_obj> cpy(unordered_map_big);
	});
	suite.benchmark("fea::id_slotmap copy ctor",
			[&]() { fea::id_slotmap<size_t, big_obj> cpy(unsigned_map_big); });
	suite.print();


	// Bench : clear small
	title.fill('\0');
	std::snprintf(title.data(), title.size(), "Clear %zu small objects",
			map_small.size());
	suite.title(title.data());

	suite.benchmark("std::map clear", [&]() { map_small.clear(); });
	suite.benchmark(
			"std::unordered_map clear", [&]() { unordered_map_small.clear(); });
	suite.benchmark(
			"fea::id_slotmap clear", [&]() { unsigned_map_small.clear(); });
	suite.print();

	// Bench : clear big
	title.fill('\0');
	std::snprintf(title.data(), title.size(), "Clear %zu big objects",
			map_big.size());
	suite.title(title.data());

	suite.benchmark("std::map clear", [&]() { map_big.clear(); });
	suite.benchmark(
			"std::unordered_map clear", [&]() { unordered_map_big.clear(); });
	suite.benchmark(
			"fea::id_slotmap clear", [&]() { unsigned_map_big.clear(); });
	suite.print();


	// Bench : insert small_obj
	title.fill('\0');
	std::snprintf(title.data(), title.size(), "Insert %zu small objects",
			keys.size());
	suite.title(title.data());

	suite.benchmark("std::map insert", [&]() {
		for (size_t i = 0; i < keys.size(); ++i) {
			map_small.insert({ keys[i], { float(i), float(i), float(i) } });
		}
	});
	suite.benchmark("std::unordered_map insert", [&]() {
		for (size_t i = 0; i < keys.size(); ++i) {
			unordered_map_small.insert(
					{ keys[i], { float(i), float(i), float(i) } });
		}
	});
	suite.benchmark("fea::id_slotmap insert", [&]() {
		for (size_t i = 0; i < keys.size(); ++i) {
			unsigned_map_small.insert(
					{ keys[i], { float(i), float(i), float(i) } });
		}
	});
	suite.print();
	map_small.clear();
	unordered_map_small.clear();
	unsigned_map_small.clear();


	// Bench : insert big_obj
	title.fill('\0');
	std::snprintf(
			title.data(), title.size(), "Insert %zu big objects", keys.size());
	suite.title(title.data());

	suite.benchmark("std::map insert", [&]() {
		for (size_t i = 0; i < keys.size(); ++i) {
			map_big.insert({ keys[i], {} });
		}
	});
	suite.benchmark("std::unordered_map insert", [&]() {
		for (size_t i = 0; i < keys.size(); ++i) {
			unordered_map_big.insert({ keys[i], {} });
		}
	});
	suite.benchmark("fea::id_slotmap insert", [&]() {
		for (size_t i = 0; i < keys.size(); ++i) {
			unsigned_map_big.insert({ keys[i], {} });
		}
	});
	suite.print();
	map_big.clear();
	unordered_map_big.clear();
	unsigned_map_big.clear();


	// Bench : erase small_obj
	for (size_t i = 0; i < keys.size(); ++i) {
		map_small.insert({ keys[i], { float(i), float(i), float(i) } });
	}
	for (size_t i = 0; i < keys.size(); ++i) {
		unordered_map_small.insert(
				{ keys[i], { float(i), float(i), float(i) } });
	}
	for (size_t i = 0; i < keys.size(); ++i) {
		unsigned_map_small.insert(
				{ keys[i], { float(i), float(i), float(i) } });
	}

	title.fill('\0');
	std::snprintf(title.data(), title.size(),
			"Erase %zu (all) small objects at random",
			unsigned_map_small.size());
	suite.title(title.data());

	std::vector<size_t> random_keys = keys;

	std::random_device rng;
	std::mt19937_64 urng(rng());
	std::shuffle(random_keys.begin(), random_keys.end(), urng);

	suite.benchmark("std::map erase", [&]() {
		for (size_t i = 0; i < random_keys.size(); ++i) {
			map_small.erase(random_keys[i]);
		}
	});
	suite.benchmark("std::unordered_map erase", [&]() {
		for (size_t i = 0; i < random_keys.size(); ++i) {
			unordered_map_small.erase(random_keys[i]);
		}
	});
	suite.benchmark("fea::id_slotmap erase", [&]() {
		for (size_t i = 0; i < random_keys.size(); ++i) {
			unsigned_map_small.erase(random_keys[i]);
		}
	});
	suite.print();
	map_small.clear();
	unordered_map_small.clear();
	unsigned_map_small.clear();


	// Bench : erase big_obj
	for (size_t i = 0; i < keys.size(); ++i) {
		map_big.insert({ keys[i], {} });
	}
	for (size_t i = 0; i < keys.size(); ++i) {
		unordered_map_big.insert({ keys[i], {} });
	}
	for (size_t i = 0; i < keys.size(); ++i) {
		unsigned_map_big.insert({ keys[i], {} });
	}

	title.fill('\0');
	std::snprintf(title.data(), title.size(),
			"Erase %zu (all) big objects at random", unsigned_map_big.size());
	suite.title(title.data());

	suite.benchmark("std::map erase", [&]() {
		for (size_t i = 0; i < random_keys.size(); ++i) {
			map_big.erase(random_keys[i]);
		}
	});
	suite.benchmark("std::unordered_map erase", [&]() {
		for (size_t i = 0; i < random_keys.size(); ++i) {
			unordered_map_big.erase(random_keys[i]);
		}
	});
	suite.benchmark("fea::id_slotmap erase", [&]() {
		for (size_t i = 0; i < random_keys.size(); ++i) {
			unsigned_map_big.erase(random_keys[i]);
		}
	});
	suite.print();
	map_big.clear();
	unordered_map_big.clear();
	unsigned_map_big.clear();


	// Bench : insert small_obj reserves
	title.fill('\0');
	std::snprintf(title.data(), title.size(),
			"Insert %zu small objects after reserve", keys.size());
	suite.title(title.data());

	unordered_map_small.reserve(keys.size());
	unsigned_map_small.reserve(keys.size());

	suite.benchmark("std::map insert", [&]() {
		for (size_t i = 0; i < keys.size(); ++i) {
			map_small.insert({ keys[i], {} });
		}
	});
	suite.benchmark("std::unordered_map insert", [&]() {
		for (size_t i = 0; i < keys.size(); ++i) {
			unordered_map_small.insert({ keys[i], {} });
		}
	});
	suite.benchmark("fea::id_slotmap insert", [&]() {
		for (size_t i = 0; i < keys.size(); ++i) {
			unsigned_map_small.insert({ keys[i], {} });
		}
	});
	suite.print();
	map_big.clear();
	unordered_map_small.clear();
	unsigned_map_small.clear();


	// Bench : insert big_obj reserves
	title.fill('\0');
	std::snprintf(title.data(), title.size(),
			"Insert %zu big objects after reserve", keys.size());
	suite.title(title.data());

	unordered_map_big.reserve(keys.size());
	unsigned_map_big.reserve(keys.size());

	suite.benchmark("std::map insert", [&]() {
		for (size_t i = 0; i < keys.size(); ++i) {
			map_big.insert({ keys[i], {} });
		}
	});
	suite.benchmark("std::unordered_map insert", [&]() {
		for (size_t i = 0; i < keys.size(); ++i) {
			unordered_map_big.insert({ keys[i], {} });
		}
	});
	suite.benchmark("fea::id_slotmap insert", [&]() {
		for (size_t i = 0; i < keys.size(); ++i) {
			unsigned_map_big.insert({ keys[i], {} });
		}
	});
	suite.print();
	map_big.clear();
	unordered_map_big.clear();
	unsigned_map_big.clear();


	// Bench : Iterate and assign value small_obj
	for (size_t i = 0; i < keys.size(); ++i) {
		map_small.insert({ keys[i], { float(i), float(i), float(i) } });
	}
	for (size_t i = 0; i < keys.size(); ++i) {
		unordered_map_small.insert(
				{ keys[i], { float(i), float(i), float(i) } });
	}
	for (size_t i = 0; i < keys.size(); ++i) {
		unsigned_map_small.insert(
				{ keys[i], { float(i), float(i), float(i) } });
	}

	title.fill('\0');
	std::snprintf(title.data(), title.size(),
			"Iterate %zu small objects and assign value",
			unsigned_map_small.size());
	suite.title(title.data());

	suite.benchmark("std::map iterate & assign", [&]() {
		for (auto& p : map_small) {
			p.second.y = float(rand() % 100);
		}
	});
	suite.benchmark("std::unordered_map iterate & assign", [&]() {
		for (auto& p : unordered_map_small) {
			p.second.y = float(rand() % 100);
		}
	});
	suite.benchmark("fea::id_slotmap iterate & assign", [&]() {
		for (auto& p : unsigned_map_small) {
			p.second.y = float(rand() % 100);
		}
	});
	suite.print();

	map_small.clear();
	unordered_map_small.clear();
	unsigned_map_small.clear();


	// Bench : Iterate and assign value big_obj
	for (size_t i = 0; i < keys.size(); ++i) {
		map_big.insert({ keys[i], {} });
	}
	for (size_t i = 0; i < keys.size(); ++i) {
		unordered_map_big.insert({ keys[i], {} });
	}
	for (size_t i = 0; i < keys.size(); ++i) {
		unsigned_map_big.insert({ keys[i], {} });
	}

	title.fill('\0');
	std::snprintf(title.data(), title.size(),
			"Iterate %zu big objects and assign value",
			unsigned_map_big.size());
	suite.title(title.data());

	suite.benchmark("std::map iterate & assign", [&]() {
		for (auto& p : map_big) {
			p.second.data.fill(rand() % 100);
		}
	});
	suite.benchmark("std::unordered_map iterate & assign", [&]() {
		for (auto& p : unordered_map_big) {
			p.second.data.fill(rand() % 100);
		}
	});
	suite.benchmark("fea::id_slotmap iterate & assign", [&]() {
		for (auto& p : unsigned_map_big) {
			p.second.data.fill(rand() % 100);
		}
	});
	suite.print();

	map_big.clear();
	unordered_map_big.clear();
	unsigned_map_big.clear();
}


TEST(id_slotmap, benchmarks) {
	srand(static_cast<unsigned int>(
			std::chrono::system_clock::now().time_since_epoch().count()));
	std::vector<size_t> keys;
	keys.reserve(num_keys);

	std::array<char, 128> title;
	title.fill('\0');

	// Linear keys, 0 to N
	{
		keys.clear();
		for (size_t i = 0; i < num_keys / 2; ++i) {
			keys.push_back(i);
		}

		title.fill('\0');
		std::snprintf(title.data(), title.size(),
				"Benchmark using linear keys, 0 to %zu, no duplicates",
				num_keys / 2);
		fea::bench::title(title.data());

		benchmarks(keys);
	}


	// Linear keys, N to 0
	{
		keys.clear();
		for (long long i = (long long)(num_keys / 2 - 1); i >= 0; --i) {
			keys.push_back(size_t(i));
		}

		printf("\n\n");
		title.fill('\0');
		std::snprintf(title.data(), title.size(),
				"Benchmark using linear keys, %zu to 0, no duplicates",
				num_keys / 2);
		fea::bench::title(title.data());

		benchmarks(keys);
	}


	// Random keys.
	{
		std::random_device rd{};
		std::mt19937_64 gen{ rd() };
		std::uniform_int_distribution<size_t> dis{ 0, num_keys / 4 };

		keys.clear();
		for (size_t i = 0; i < num_keys; ++i) {
			keys.push_back(dis(gen));
		}

		printf("\n\n");
		title.fill('\0');
		std::snprintf(title.data(), title.size(),
				"Benchmark using %zu random uniform distribution keys, with "
				"duplicates",
				num_keys);
		fea::bench::title(title.data());

		benchmarks(keys);
	}


	// Random keys.
	{
		keys.clear();
		for (size_t i = 0; i < num_keys; ++i) {
			keys.push_back(rand() % num_keys);
		}

		printf("\n\n");
		title.fill('\0');
		std::snprintf(title.data(), title.size(),
				"Benchmark using %zu rand() keys, many duplicates", num_keys);
		fea::bench::title(title.data());

		benchmarks(keys);
	}
}
} // namespace

#include <fea/meta/pack.hpp>
#include <fea/utils/file.hpp>
#include <fea/utils/serialize.hpp>
#include <filesystem>
#include <gtest/gtest.h>

extern const char* argv0;

namespace {
// Example for custom serialization of user types.
// Simply implement your own serialize function and return std::true_type.
// Remember to call 'using fea::serialize' so the appropriate overload is
// chosen. You could be serializing other user types internally.
struct potato {
	friend std::true_type serialize(const potato& p, std::ofstream& ofs) {
		using fea::serialize;
		serialize(p.val, ofs);
		serialize(p.vec, ofs);
		return {};
	}
	friend void deserialize(potato& p, std::ifstream& ifs) {
		using fea::deserialize;
		deserialize(p.val, ifs);
		deserialize(p.vec, ifs);
	}

	friend bool operator==(const potato& lhs, const potato& rhs) {
		return lhs.val == rhs.val && lhs.vec == rhs.vec;
	}
	friend bool operator!=(const potato& lhs, const potato& rhs) {
		return !(lhs == rhs);
	}
	friend bool operator<(const potato& lhs, const potato& rhs) {
		return lhs.val < rhs.val;
	}

	int val = 42;
	std::vector<int> vec{ 42, -42, 0, 1 };
};

std::filesystem::path filepath() {
	const std::filesystem::path dir = fea::executable_dir(argv0) / "tests_data";
	const std::filesystem::path filepath = dir / "pertatoes.bin";
	return filepath;
}

using fea::deserialize;
using fea::serialize;

TEST(serialize, basics) {
	// Internal stuff, you never need to bother with this unless you are working
	// with templates youself.
	static_assert(FEA_NEEDS_NESTING(std::vector<std::vector<int>>),
			"serialize.cpp : test failed");
	static_assert(
			FEA_NEEDS_NESTING(std::vector<int>), "serialize.cpp : test failed");
	static_assert(!FEA_NEEDS_NESTING(int), "serialize.cpp : test failed");

	static_assert(FEA_NEEDS_NESTING(std::vector<std::vector<potato>>),
			"serialize.cpp : test failed");

	static_assert(FEA_NEEDS_NESTING(potato), "serialize.cpp : test failed");


	// Test a simple vector.
	{
		std::vector<potato> potatoes(4);
		std::ofstream ofs{ filepath(), std::ios::binary };
		serialize(potatoes, ofs);
	}

	std::vector<potato> potatoes(4);
	for (potato& p : potatoes) {
		p.val = 5243;
		p.vec = {};
	}

	{
		std::ifstream ifs{ filepath(), std::ios::binary };
		deserialize(potatoes, ifs);
	}

	EXPECT_EQ(potatoes.size(), 4);
	for (const potato& p : potatoes) {
		EXPECT_EQ(p.val, 42);
		EXPECT_EQ(p.vec.size(), 4);
		EXPECT_EQ(p.vec, potato{}.vec);
	}

	// Test if it takes the correct overload.
	{
		potato a_potato;
		std::ofstream ofs{ filepath(), std::ios::binary };
		serialize(a_potato, ofs);
	}

	potato a_potato;
	a_potato.val = 0;
	a_potato.vec = {};
	// Deserialize.
	{
		std::ifstream ifs{ filepath(), std::ios::binary };
		deserialize(a_potato, ifs);
	}

	EXPECT_EQ(a_potato.val, potato{}.val);
	EXPECT_EQ(a_potato.vec, potato{}.vec);
}

TEST(serialize, array) {
	{
		std::array<int, 4> arr{ 1, 2, 3, 4 };
		std::ofstream ofs{ filepath(), std::ios::binary };
		serialize(arr, ofs);
	}

	{
		std::array<int, 4> arr{};
		std::ifstream ifs{ filepath(), std::ios::binary };
		deserialize(arr, ifs);

		std::array<int, 4> arr_comp{ 1, 2, 3, 4 };
		EXPECT_EQ(arr, arr_comp);
	}

	{
		std::array<potato, 4> arr{};
		std::ofstream ofs{ filepath(), std::ios::binary };
		serialize(arr, ofs);
	}

	{
		potato p;
		p.val = -42;
		p.vec = { 1111 };

		std::array<potato, 4> arr{ p, p, p, p };
		std::ifstream ifs{ filepath(), std::ios::binary };
		deserialize(arr, ifs);

		std::array<potato, 4> arr_comp{};
		EXPECT_EQ(arr, arr_comp);
	}

	{
		std::array<std::array<std::array<potato, 4>, 4>, 4> arr{};
		std::ofstream ofs{ filepath(), std::ios::binary };
		serialize(arr, ofs);
	}

	{
		potato p;
		p.val = -42;
		p.vec = { 1111 };

		std::array<std::array<std::array<potato, 4>, 4>, 4> arr{};
		for (auto& a1 : arr) {
			for (auto& a2 : a1) {
				for (auto& a3 : a2) {
					a3 = p;
				}
			}
		}

		std::ifstream ifs{ filepath(), std::ios::binary };
		deserialize(arr, ifs);

		std::array<std::array<std::array<potato, 4>, 4>, 4> arr_comp{};
		EXPECT_EQ(arr, arr_comp);
	}
}

TEST(serialize, vector) {
	{
		std::vector<int> c{ 1, 2, 3, 4 };
		std::ofstream ofs{ filepath(), std::ios::binary };
		serialize(c, ofs);
	}

	{
		std::vector<int> arr{};
		std::ifstream ifs{ filepath(), std::ios::binary };
		deserialize(arr, ifs);

		std::vector<int> c_comp{ 1, 2, 3, 4 };
		EXPECT_EQ(arr, c_comp);
	}

	{
		std::vector<potato> c(4);
		std::ofstream ofs{ filepath(), std::ios::binary };
		serialize(c, ofs);
	}

	{
		potato p;
		p.val = -42;
		p.vec = { 1111 };

		std::vector<potato> c{ p, p, p, p };
		std::ifstream ifs{ filepath(), std::ios::binary };
		deserialize(c, ifs);

		std::vector<potato> c_comp(4);
		EXPECT_EQ(c, c_comp);
	}

	{
		std::vector<std::vector<std::vector<potato>>> c(
				4, std::vector<std::vector<potato>>(4, std::vector<potato>(4)));
		std::ofstream ofs{ filepath(), std::ios::binary };
		serialize(c, ofs);
	}

	{
		potato p;
		p.val = -42;
		p.vec = { 1111 };

		std::vector<std::vector<std::vector<potato>>> c(
				4, std::vector<std::vector<potato>>(4, std::vector<potato>(4)));

		for (auto& a1 : c) {
			for (auto& a2 : a1) {
				for (auto& a3 : a2) {
					a3 = p;
				}
			}
		}

		std::ifstream ifs{ filepath(), std::ios::binary };
		deserialize(c, ifs);

		std::vector<std::vector<std::vector<potato>>> c_comp(
				4, std::vector<std::vector<potato>>(4, std::vector<potato>(4)));
		EXPECT_EQ(c, c_comp);
	}
}

TEST(serialize, map) {
	// plain
	{
		std::map<int, int> c{ { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 } };
		std::ofstream ofs{ filepath(), std::ios::binary };
		serialize(c, ofs);
	}

	{
		std::map<int, int> c{};
		std::ifstream ifs{ filepath(), std::ios::binary };
		deserialize(c, ifs);

		std::map<int, int> c_comp{ { 1, 1 }, { 2, 2 }, { 3, 3 }, { 4, 4 } };
		EXPECT_EQ(c, c_comp);
	}

	// map<nested,nested>
	{
		std::map<potato, potato> c{};

		for (int i = 0; i < 4; ++i) {
			potato k;
			k.val = i;

			potato v;
			v.val = i;
			std::fill(v.vec.begin(), v.vec.end(), i);

			c.insert({ k, v });
		}
		std::ofstream ofs{ filepath(), std::ios::binary };
		serialize(c, ofs);
	}

	{
		std::map<potato, potato> c;
		std::ifstream ifs{ filepath(), std::ios::binary };
		deserialize(c, ifs);

		std::map<potato, potato> c_comp{};
		for (int i = 0; i < 4; ++i) {
			potato k;
			k.val = i;

			potato v;
			v.val = i;
			std::fill(v.vec.begin(), v.vec.end(), i);

			c_comp.insert({ k, v });
		}
		EXPECT_EQ(c, c_comp);
	}

	// map<plain, nested>
	{
		std::map<int, potato> c{};

		for (int i = 0; i < 4; ++i) {
			potato v;
			v.val = i;
			std::fill(v.vec.begin(), v.vec.end(), i);

			c.insert({ i, v });
		}
		std::ofstream ofs{ filepath(), std::ios::binary };
		serialize(c, ofs);
	}

	{
		std::map<int, potato> c;
		std::ifstream ifs{ filepath(), std::ios::binary };
		deserialize(c, ifs);

		std::map<int, potato> c_comp{};
		for (int i = 0; i < 4; ++i) {
			potato v;
			v.val = i;
			std::fill(v.vec.begin(), v.vec.end(), i);

			c_comp.insert({ i, v });
		}
		EXPECT_EQ(c, c_comp);
	}

	// map<nested, plain>
	{
		std::map<potato, int> c{};

		for (int i = 0; i < 4; ++i) {
			potato k;
			k.val = i;
			std::fill(k.vec.begin(), k.vec.end(), i);

			c.insert({ k, i });
		}
		std::ofstream ofs{ filepath(), std::ios::binary };
		serialize(c, ofs);
	}

	{
		std::map<potato, int> c;
		std::ifstream ifs{ filepath(), std::ios::binary };
		deserialize(c, ifs);

		std::map<potato, int> c_comp{};
		for (int i = 0; i < 4; ++i) {
			potato k;
			k.val = i;
			std::fill(k.vec.begin(), k.vec.end(), i);

			c_comp.insert({ k, i });
		}
		EXPECT_EQ(c, c_comp);
	}


	auto make_big_map = []() {
		std::map<potato, std::map<int, std::map<potato, int>>> ret{};
		std::map<potato, int> a3;
		for (int i = 0; i < 4; ++i) {
			potato k;
			k.val = i;
			std::fill(k.vec.begin(), k.vec.end(), i);
			a3.insert({ k, i });
		}
		std::map<int, std::map<potato, int>> a2;
		for (int i = 0; i < 4; ++i) {
			a2.insert({ i, a3 });
		}
		for (int i = 0; i < 4; ++i) {
			potato k;
			k.val = i;
			std::fill(k.vec.begin(), k.vec.end(), i);
			ret.insert({ k, a2 });
		}
		return ret;
	};

	{
		std::map<potato, std::map<int, std::map<potato, int>>> c
				= make_big_map();
		std::ofstream ofs{ filepath(), std::ios::binary };
		serialize(c, ofs);
	}

	{
		std::map<potato, std::map<int, std::map<potato, int>>> c{};
		std::ifstream ifs{ filepath(), std::ios::binary };
		deserialize(c, ifs);

		std::map<potato, std::map<int, std::map<potato, int>>> c_comp
				= make_big_map();
		EXPECT_EQ(c, c_comp);
	}
}
} // namespace

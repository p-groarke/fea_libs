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

		// std::array<potato, 4> temp{ p, p, p, p };

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
} // namespace

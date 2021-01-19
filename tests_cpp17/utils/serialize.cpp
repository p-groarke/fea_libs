﻿#include <fea/meta/pack.hpp>
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
	potato() = default;
	potato(int i)
			: val(i)
			, vec({ i, i, i, i }) {
	}
	potato(char i)
			: potato(int(i)) {
	}

	friend std::true_type serialize(const potato& p, std::ofstream& ofs) {
		using fea::serialize;
		serialize(p.val, ofs);
		serialize(p.vec, ofs);
		return {};
	}
	friend bool deserialize(potato& p, std::ifstream& ifs) {
		using fea::deserialize;
		if (!deserialize(p.val, ifs)) {
			return false;
		}
		return deserialize(p.vec, ifs);
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
} // namespace

namespace std {
template <>
struct hash<potato> {
	size_t operator()(const potato& p) const noexcept {
		return std::hash<int>{}(p.val);
	}
};
} // namespace std

namespace {
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
		EXPECT_TRUE(deserialize(potatoes, ifs));
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
		EXPECT_TRUE(deserialize(a_potato, ifs));
	}

	EXPECT_EQ(a_potato.val, potato{}.val);
	EXPECT_EQ(a_potato.vec, potato{}.vec);


	// Should not compile
	//{
	//	int* ptr = nullptr;
	//	std::ofstream ofs{ filepath(), std::ios::binary };
	//	serialize(ptr, ofs);
	//}
}

TEST(serialize, array) {
	auto test_arr1 = [](auto a_type) {
		using arr_t = std::decay_t<decltype(a_type)>;
		arr_t c_comp{ { { 1 }, { 2 }, { 3 }, { 4 } } };

		{
			std::ofstream ofs{ filepath(), std::ios::binary };
			serialize(c_comp, ofs);
		}

		{
			arr_t c{};
			std::ifstream ifs{ filepath(), std::ios::binary };
			EXPECT_TRUE(deserialize(c, ifs));
			EXPECT_EQ(c, c_comp);
		}
	};
	test_arr1(std::array<int, 4>{});
	test_arr1(std::array<potato, 4>{});

	auto test_arr2 = [](auto m_type) {
		using arr_t = std::decay_t<decltype(m_type)>;
		arr_t c_comp{};
		// std::array<std::array<std::array<potato, 4>, 4>, 4> arr{};

		arr_t::value_type::value_type a3{ { { 't' }, { 'e' }, { 's' },
				{ 't' } } };
		arr_t::value_type a2{};
		for (int i = 0; i < 4; ++i) {
			a2[i] = a3;
		}
		for (int i = 0; i < 4; ++i) {
			c_comp[i] = a2;
		}

		{
			std::ofstream ofs{ filepath(), std::ios::binary };
			serialize(c_comp, ofs);
		}

		{
			arr_t c{};
			std::ifstream ifs{ filepath(), std::ios::binary };
			EXPECT_TRUE(deserialize(c, ifs));
			EXPECT_EQ(c, c_comp);
		}
	};
	test_arr2(std::array<std::array<std::array<int, 4>, 4>, 4>{});
	test_arr2(std::array<std::array<std::array<potato, 4>, 4>, 4>{});
}

TEST(serialize, vector_string) {
	auto test_buf1 = [](auto v_type) {
		using vec_t = std::decay_t<decltype(v_type)>;
		vec_t c_comp{ { 't' }, { 'e' }, { 's' }, { 't' } };

		{
			std::ofstream ofs{ filepath(), std::ios::binary };
			serialize(c_comp, ofs);
		}

		{
			vec_t c{};
			std::ifstream ifs{ filepath(), std::ios::binary };
			EXPECT_TRUE(deserialize(c, ifs));
			EXPECT_EQ(c, c_comp);
		}
	};
	test_buf1(std::vector<int>{});
	test_buf1(std::vector<potato>{});
	test_buf1(std::string{});
	test_buf1(std::wstring{});
	test_buf1(std::u16string{});
	test_buf1(std::u32string{});

	auto test_buf2 = [](auto m_type) {
		using buf_t = std::decay_t<decltype(m_type)>;
		buf_t c_comp{};

		buf_t::value_type::value_type a3{ { 't' }, { 'e' }, { 's' }, { 't' } };
		buf_t::value_type a2;
		for (int i = 0; i < 4; ++i) {
			a2.push_back(a3);
		}
		for (int i = 0; i < 4; ++i) {
			c_comp.push_back(a2);
		}

		{
			std::ofstream ofs{ filepath(), std::ios::binary };
			serialize(c_comp, ofs);
		}

		{
			buf_t c{};
			std::ifstream ifs{ filepath(), std::ios::binary };
			EXPECT_TRUE(deserialize(c, ifs));
			EXPECT_EQ(c, c_comp);
		}
	};

	test_buf2(std::vector<std::vector<std::vector<potato>>>{});
	test_buf2(std::vector<std::vector<std::vector<int>>>{});

	test_buf2(std::vector<std::vector<std::string>>{});
	test_buf2(std::vector<std::vector<std::wstring>>{});
	test_buf2(std::vector<std::vector<std::u16string>>{});
	test_buf2(std::vector<std::vector<std::u32string>>{});
}

TEST(serialize, map) {
	auto test_map1 = [](auto m_type) {
		using map_t = std::decay_t<decltype(m_type)>;
		map_t c_comp{};

		{
			for (int i = 0; i < 4; ++i) {
				c_comp.insert({ { i }, { i } });
			}
			std::ofstream ofs{ filepath(), std::ios::binary };
			serialize(c_comp, ofs);
		}

		{
			map_t c{};
			std::ifstream ifs{ filepath(), std::ios::binary };
			EXPECT_TRUE(deserialize(c, ifs));
			EXPECT_EQ(c, c_comp);
		}
	};

	test_map1(std::map<int, int>{});
	test_map1(std::map<potato, potato>{});
	test_map1(std::map<potato, int>{});
	test_map1(std::map<int, potato>{});

	test_map1(std::unordered_map<int, int>{});
	test_map1(std::unordered_map<potato, potato>{});
	test_map1(std::unordered_map<potato, int>{});
	test_map1(std::unordered_map<int, potato>{});

	auto test_map2 = [](auto m_type) {
		using map_t = std::decay_t<decltype(m_type)>;
		map_t c_comp{};

		// std::map<potato, std::map<int, std::map<potato, int>>> c_comp;

		map_t::mapped_type::mapped_type a3;
		for (int i = 0; i < 4; ++i) {
			a3.insert({ { i }, { i } });
		}
		map_t::mapped_type a2;
		for (int i = 0; i < 4; ++i) {
			a2.insert({ i, a3 });
		}
		for (int i = 0; i < 4; ++i) {
			c_comp.insert({ { i }, a2 });
		}

		{
			std::ofstream ofs{ filepath(), std::ios::binary };
			serialize(c_comp, ofs);
		}

		{
			map_t c{};
			std::ifstream ifs{ filepath(), std::ios::binary };
			EXPECT_TRUE(deserialize(c, ifs));
			EXPECT_EQ(c, c_comp);
		}
	};

	test_map2(std::map<potato, std::map<int, std::map<potato, int>>>{});
	test_map2(std::unordered_map<potato,
			std::unordered_map<int, std::unordered_map<potato, int>>>{});

	test_map2(
			std::map<potato, std::unordered_map<int, std::map<potato, int>>>{});
	test_map2(std::unordered_map<potato,
			std::map<int, std::unordered_map<potato, int>>>{});

	test_map2(std::map<int, std::map<potato, std::map<potato, int>>>{});
	test_map2(std::unordered_map<int,
			std::unordered_map<potato, std::unordered_map<potato, int>>>{});
}

TEST(serialize, set) {
	auto test_set1 = [](auto m_type) {
		using set_t = std::decay_t<decltype(m_type)>;
		set_t c_comp{};

		{
			for (int i = 0; i < 4; ++i) {
				c_comp.insert({ i });
			}
			std::ofstream ofs{ filepath(), std::ios::binary };
			serialize(c_comp, ofs);
		}

		{
			set_t c{};
			std::ifstream ifs{ filepath(), std::ios::binary };
			EXPECT_TRUE(deserialize(c, ifs));
			EXPECT_EQ(c, c_comp);
		}
	};

	test_set1(std::set<int>{});
	test_set1(std::set<potato>{});
	test_set1(std::unordered_set<int>{});
	test_set1(std::unordered_set<potato>{});

	auto test_set2 = [](auto m_type) {
		using set_t = std::decay_t<decltype(m_type)>;
		set_t c_comp{};

		// std::set<std::set<std::set<potato>>> c_comp;

		set_t::value_type::value_type a3;
		for (int i = 0; i < 4; ++i) {
			a3.insert({ i });
		}
		set_t::value_type a2;
		for (int i = 0; i < 4; ++i) {
			a2.insert(a3);
		}
		for (int i = 0; i < 4; ++i) {
			c_comp.insert(a2);
		}

		{
			std::ofstream ofs{ filepath(), std::ios::binary };
			serialize(c_comp, ofs);
		}

		{
			set_t c{};
			std::ifstream ifs{ filepath(), std::ios::binary };
			EXPECT_TRUE(deserialize(c, ifs));
			EXPECT_EQ(c, c_comp);
		}
	};

	test_set2(std::set<std::set<std::set<potato>>>{});
	test_set2(std::set<std::set<std::set<int>>>{});

	// no can do
	// test_set2(std::unordered_set<
	//		std::unordered_set<std::unordered_set<potato>>>{});
	// test_set2(
	//		std::unordered_set<std::unordered_set<std::unordered_set<int>>>{});
}

TEST(serialize, pair_tuple) {
	std::tuple<int, int, int, int> tup;

	auto test_tup1 = [](auto m_type) {
		using tup_t = std::decay_t<decltype(m_type)>;
		tup_t c_comp{};

		{
			std::get<0>(c_comp) = { 0 };
			std::get<1>(c_comp) = { 1 };
			if constexpr (std::tuple_size_v<tup_t> > 2) {
				std::get<2>(c_comp) = { 2 };
				std::get<3>(c_comp) = { 3 };
			}

			std::ofstream ofs{ filepath(), std::ios::binary };
			serialize(c_comp, ofs);
		}

		{
			tup_t c{};
			std::ifstream ifs{ filepath(), std::ios::binary };
			EXPECT_TRUE(deserialize(c, ifs));
			EXPECT_EQ(c, c_comp);
		}
	};
	test_tup1(std::pair<int, int>{});
	test_tup1(std::pair<potato, int>{});
	test_tup1(std::pair<int, potato>{});
	test_tup1(std::pair<potato, potato>{});

	test_tup1(std::tuple<int, int>{});
	test_tup1(std::tuple<potato, int>{});
	test_tup1(std::tuple<int, potato>{});
	test_tup1(std::tuple<potato, potato>{});

	test_tup1(std::tuple<int, int, int, int>{});
	test_tup1(std::tuple<potato, int, potato, int>{});
	test_tup1(std::tuple<int, potato, int, potato>{});
	test_tup1(std::tuple<potato, potato, potato, potato>{});

	auto test_tup2 = [](auto m_type) {
		using tup_t = std::decay_t<decltype(m_type)>;
		tup_t c_comp{};

		// std::set<std::set<std::set<potato>>> c_comp;

		std::tuple_element_t<1, std::tuple_element_t<1, tup_t>> a3{};
		std::get<0>(a3) = { 0 };
		std::get<1>(a3) = { 1 };

		std::tuple_element_t<1, tup_t> a2{};
		std::get<0>(a2) = { 10 };
		std::get<1>(a2) = a3;

		std::get<0>(c_comp) = { 20 };
		std::get<1>(c_comp) = a2;

		{
			std::ofstream ofs{ filepath(), std::ios::binary };
			serialize(c_comp, ofs);
		}

		{
			tup_t c{};
			std::ifstream ifs{ filepath(), std::ios::binary };
			EXPECT_TRUE(deserialize(c, ifs));
			EXPECT_EQ(c, c_comp);
		}
	};

	test_tup2(std::pair<potato, std::pair<int, std::pair<potato, potato>>>{});
	test_tup2(std::pair<int, std::pair<potato, std::pair<int, int>>>{});
	test_tup2(std::pair<potato, std::pair<potato, std::pair<potato, int>>>{});
	test_tup2(std::pair<potato, std::pair<int, std::pair<int, potato>>>{});

	test_tup2(std::tuple<potato, std::pair<int, std::pair<potato, potato>>>{});
	test_tup2(std::tuple<int, std::pair<potato, std::pair<int, int>>>{});
	test_tup2(std::tuple<potato, std::pair<potato, std::pair<potato, int>>>{});
	test_tup2(std::tuple<potato, std::pair<int, std::pair<int, potato>>>{});

	test_tup2(std::tuple<potato, std::tuple<int, std::pair<potato, potato>>>{});
	test_tup2(std::tuple<int, std::tuple<potato, std::pair<int, int>>>{});
	test_tup2(std::tuple<potato, std::tuple<potato, std::pair<potato, int>>>{});
	test_tup2(std::tuple<potato, std::tuple<int, std::pair<int, potato>>>{});

	test_tup2(
			std::tuple<potato, std::tuple<int, std::tuple<potato, potato>>>{});
	test_tup2(std::tuple<int, std::tuple<potato, std::tuple<int, int>>>{});
	test_tup2(
			std::tuple<potato, std::tuple<potato, std::tuple<potato, int>>>{});
	test_tup2(std::tuple<potato, std::tuple<int, std::tuple<int, potato>>>{});
}
} // namespace

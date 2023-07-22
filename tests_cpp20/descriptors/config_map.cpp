#include <cstdio>
#include <fea/descriptors/config_map.hpp>
#include <fea/functional/func_ptr.hpp>
#include <functional>
#include <gtest/gtest.h>
#include <type_traits>

namespace {
template <class EnumT>
struct my_descriptor {
	EnumT key = EnumT::count;
	bool p = false;
	int i = 0;
	std::array<int, 2> test{};
};

enum class potato {
	russet,
	yukon,
	count,
};

constexpr my_descriptor<potato> russet_desc{
	.key = potato::russet,
	.p = true,
	.i = 42,
	.test = { 0, 1 },
};
constexpr my_descriptor<potato> yukon_desc{
	.key = potato::yukon,
	.p = false,
	.i = -42,
	.test = { 42, -42 },
};


TEST(config_map, basics) {
	using my_map_t = fea::config_map<potato, my_descriptor<potato>, russet_desc,
			yukon_desc>;

	static_assert(my_map_t::size() == 2, "config_map.cpp : Test failed.");
	static_assert(
			my_map_t::descriptors.size() == 2, "config_map.cpp : Test failed.");

	static_assert(my_map_t::descriptor(potato::russet).key == potato::russet,
			"config_map.cpp : Test failed.");
	static_assert(my_map_t::descriptor(potato::yukon).key == potato::yukon,
			"config_map.cpp : Test failed.");

	constexpr std::tuple<bool, bool> tup
			= my_map_t::make_tuple([](auto d) { return d.p; });
	static_assert(tup == std::make_tuple(true, false),
			"config_map.cpp : Test failed.");

	constexpr std::array<int, 2> arr
			= my_map_t::make_array([](auto d) { return d.i; });
	static_assert(
			arr == std::array{ 42, -42 }, "config_map.cpp : Test failed.");

	constexpr fea::enum_array<int, potato> earr
			= my_map_t::make_enum_array([](auto d) { return d.i; });
	static_assert(earr == fea::enum_array<int, potato>{ 42, -42 },
			"config_map.cpp : Test failed.");

	constexpr auto v = my_map_t::for_each_descriptor([](auto d) {
		if (d.key == potato::russet) {
			return 1;
		} else {
			return 0;
		}
	});
	static_assert(v == std::array{ 1, 0 }, "config_map.cpp : Test failed.");
}
} // namespace
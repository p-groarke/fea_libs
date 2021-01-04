#include <array>
#include <fea/maps/type_map.hpp>
#include <gtest/gtest.h>

namespace {
TEST(tuple, basics) {
	{
		constexpr fea::type_pack<int, double> k;
		constexpr std::tuple<short, size_t> v{ short(0), size_t(42) };
		constexpr fea::type_map m = fea::make_type_map(k, v);

		static_assert(m.contains<int>(), "type_map.cpp : test failed");
		static_assert(!m.contains<short>(), "type_map.cpp : test failed");
		static_assert(m.find<double>() == 42, "type_map.cpp : test failed");

		using find_t = std::decay_t<decltype(m.find<double>())>;
		static_assert(std::is_same<find_t, size_t>::value,
				"type_map.cpp : test failed");
	}

	{
		enum class e {
			one,
			two,
			three,
			four,
			count,
		};

		constexpr fea::non_type_type_pack<e, e::one, e::two> k2;
		constexpr std::tuple<short, size_t> v2{ short(0), size_t(42) };
		constexpr fea::type_map m = fea::make_type_map(k2, v2);

		static_assert(m.contains<e::one>(), "type_map.cpp : test failed");
		static_assert(!m.contains<e::three>(), "type_map.cpp : test failed");
		static_assert(m.find<e::two>() == 42, "type_map.cpp : test failed");

		using find_t = std::decay_t<decltype(m.find<e::two>())>;
		static_assert(std::is_same<find_t, size_t>::value,
				"type_map.cpp : test failed");
	}
}
} // namespace

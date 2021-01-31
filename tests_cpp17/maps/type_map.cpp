#include <array>
#include <fea/maps/type_map.hpp>
#include <fea/utils/platform.hpp>
#include <gtest/gtest.h>

namespace {
TEST(type_map, basics) {
	{
		constexpr fea::pack<int, double> k;
		constexpr std::tuple<short, size_t> v{ short(0), size_t(42) };
		auto m = fea::make_type_map(k, v);

		static_assert(m.template contains<int>(), "type_map.cpp : test failed");
		static_assert(
				!m.template contains<short>(), "type_map.cpp : test failed");

		EXPECT_EQ(m.template find<double>(), 42u);

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

		constexpr fea::pack_nt<e, e::one, e::two> k2;
		constexpr std::tuple<short, size_t> v2{ short(0), size_t(42) };
		auto m = fea::make_type_map(k2, v2);

		static_assert(
				m.template contains<e::one>(), "type_map.cpp : test failed");
		static_assert(
				!m.template contains<e::three>(), "type_map.cpp : test failed");

		EXPECT_EQ(m.template find<e::two>(), 42u);

		using find_t = std::decay_t<decltype(m.find<e::two>())>;
		static_assert(std::is_same<find_t, size_t>::value,
				"type_map.cpp : test failed");
	}

	{
		auto tmap = fea::make_type_map(
				fea::kv_t{ int(), 42.f }, fea::kv_t{ short(), 42.0 });

		static_assert(
				std::is_same_v<decltype(tmap),
						fea::type_map<fea::pack<int, short>, float, double>>,
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

		auto tmap = fea::make_type_map(
				fea::make_kv_nt<e::one>(42.f), fea::make_kv_nt<e::two>(42.0));

		static_assert(std::is_same_v<decltype(tmap),
							  fea::type_map<fea::pack_nt<e, e::one, e::two>,
									  float, double>>,
				"type_map.cpp : test failed");
	}
}

} // namespace

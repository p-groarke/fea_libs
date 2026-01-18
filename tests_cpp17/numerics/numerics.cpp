#include <fea/meta/tuple.hpp>
#include <fea/numerics/numerics.hpp>
#include <gtest/gtest.h>

namespace {
#define FAIL_MSG "numerics.cpp : failed test"
TEST(numerics, basics) {
	{
		// constexpr test
		constexpr char c_min = (std::numeric_limits<int8_t>::min)();
		constexpr char c_max = (std::numeric_limits<int8_t>::max)();

		constexpr char t = fea::abs(c_min);
		static_assert(t == c_max, "numerics.cpp : test failed");
	}

	{
		// Checks that std::abs behaves for floating points.
		constexpr auto t_min = (std::numeric_limits<float>::lowest)();
		constexpr auto t_max = (std::numeric_limits<float>::max)();

		auto t = std::abs(t_min);
		EXPECT_EQ(t, t_max);
	}

	std::tuple<char, int8_t, int16_t, int32_t, int64_t, float, double,
			long double>
			test_types{};

	fea::tuple_for_each(
			[](auto test) {
				using T = decltype(test);
				constexpr T t_min = (std::numeric_limits<T>::lowest)();
				constexpr T t_max = (std::numeric_limits<T>::max)();

				T t = fea::abs(t_min);
				EXPECT_EQ(t, t_max);
			},
			test_types);

	{
		static_assert(std::is_same_v<fea::uint_t<8>, uint8_t>, FAIL_MSG);
		static_assert(std::is_same_v<fea::uint_t<16>, uint16_t>, FAIL_MSG);
		static_assert(std::is_same_v<fea::uint_t<32>, uint32_t>, FAIL_MSG);
		static_assert(std::is_same_v<fea::uint_t<64>, uint64_t>, FAIL_MSG);
		static_assert(std::is_same_v<fea::byte_uint_t<1>, uint8_t>, FAIL_MSG);
		static_assert(std::is_same_v<fea::byte_uint_t<2>, uint16_t>, FAIL_MSG);
		static_assert(std::is_same_v<fea::byte_uint_t<4>, uint32_t>, FAIL_MSG);
		static_assert(std::is_same_v<fea::byte_uint_t<8>, uint64_t>, FAIL_MSG);
	}
}
} // namespace

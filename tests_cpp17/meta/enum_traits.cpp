#include <algorithm>
#include <fea/meta/enum_traits.hpp>
#include <fea/meta/pack.hpp>
#include <fea/meta/safe_switch.hpp>
#include <fea/numerics/literals.hpp>
#include <fea/utility/unused.hpp>
#include <gtest/gtest.h>

namespace {
#define FAIL_MSG "enum_traits.cpp : Unit test failed."

enum class e1 : uint8_t {
	bla,
	blee,
};
enum class e2 : uint16_t {
	bla,
	blee,
};
enum class e3 : int {
	bla,
	blee,
};

TEST(enum_utility, basics) {
	using namespace fea::literals;

	using ret1_t = decltype(fea::to_underlying(e1{}));
	static_assert(std::is_same_v<ret1_t, uint8_t>, FAIL_MSG);

	using ret2_t = decltype(fea::to_underlying(e2{}));
	static_assert(std::is_same_v<ret2_t, uint16_t>, FAIL_MSG);

	using ret3_t = decltype(fea::to_underlying(e3{}));
	static_assert(std::is_same_v<ret3_t, int>, FAIL_MSG);

	EXPECT_EQ(fea::to_underlying(e1::bla), 0_u8);
	EXPECT_EQ(fea::to_underlying(e1::blee), 1_u8);
	EXPECT_EQ(fea::to_underlying(e2::bla), 0_u16);
	EXPECT_EQ(fea::to_underlying(e2::blee), 1_u16);
	EXPECT_EQ(fea::to_underlying(e3::bla), 0_i32);
	EXPECT_EQ(fea::to_underlying(e3::blee), 1_i32);
}

enum class e {
	one,
	two,
	three,
	four,
	count,
};

TEST(enum, traits) {
	fea::pack_nt<e::one, e::two, e::three, e::four> p
			= fea::explode_enum<e>([](auto... cs) {
				  using pack_t = fea::pack_nt<decltype(cs)::value...>;
				  constexpr size_t idx = fea::pack_idx_nt_v<e::three, pack_t>;
				  static_assert(idx == 2, "traits.cpp : test failed");
				  return fea::pack_nt<decltype(cs)::value...>{};
			  });
	fea::unused(p);

	static_assert(!fea::is_same_nt_v<e, e::one, e::two>, FAIL_MSG);
	static_assert(!fea::is_same_nt_v2<e::one, e::two>, FAIL_MSG);
	static_assert(fea::is_same_nt_v<e, e::one, e::one>, FAIL_MSG);
	static_assert(fea::is_same_nt_v2<e::one, e::one>, FAIL_MSG);

	{
		std::vector<e> visited;
		fea::enum_for_each<e::four, e::one>(
				[&](auto my_e) { visited.push_back(my_e); });
		EXPECT_EQ(visited.size(), 2u);
		EXPECT_EQ(visited[0], e::four);
		EXPECT_EQ(visited[1], e::one);

		visited.clear();
		fea::enum_for_each<e::count, e::four, e::three, e::two, e::one>(
				[&](auto my_e) { visited.push_back(my_e); });
		EXPECT_EQ(visited.size(), 5u);
		EXPECT_EQ(visited[0], e::count);
		EXPECT_EQ(visited[1], e::four);
		EXPECT_EQ(visited[2], e::three);
		EXPECT_EQ(visited[3], e::two);
		EXPECT_EQ(visited[4], e::one);
	}

	static_assert(fea::max_v<e::four, e::two, e::three> == e::four, FAIL_MSG);
	static_assert(fea::max_v<e::two, e::one, e::three> == e::three, FAIL_MSG);
	static_assert(fea::max_v<e::two, e::four, e::three> == e::four, FAIL_MSG);

	static_assert(fea::min_v<e::one, e::two, e::three> == e::one, FAIL_MSG);
	static_assert(fea::min_v<e::two, e::three, e::one> == e::one, FAIL_MSG);
	static_assert(
			fea::min_v<e::count, e::three, e::four> == e::three, FAIL_MSG);

	{
		constexpr std::array<size_t, 5u> lookup = fea::make_enum_lookup<e::four,
				e::two, e::one, e::three, e::count>();
		static_assert(lookup[size_t(e::four)] == 0u, FAIL_MSG);
		static_assert(lookup[size_t(e::two)] == 1u, FAIL_MSG);
		static_assert(lookup[size_t(e::one)] == 2u, FAIL_MSG);
		static_assert(lookup[size_t(e::three)] == 3u, FAIL_MSG);
		static_assert(lookup[size_t(e::count)] == 4u, FAIL_MSG);
	}
}

} // namespace

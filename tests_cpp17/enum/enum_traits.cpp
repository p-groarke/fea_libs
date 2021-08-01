#include <fea/enum/enum_traits.hpp>
#include <fea/enum/safe_switch.hpp>
#include <fea/meta/pack.hpp>
#include <fea/utils/unused.hpp>
#include <gtest/gtest.h>

namespace {
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

	static_assert(!fea::is_same_nt_v<e, e::one, e::two>,
			"enum_traits.cpp : test failed");
	static_assert(!fea::is_same_nt_v2<e::one, e::two>,
			"enum_traits.cpp : test failed");
	static_assert(fea::is_same_nt_v<e, e::one, e::one>,
			"enum_traits.cpp : test failed");
	static_assert(fea::is_same_nt_v2<e::one, e::one>,
			"enum_traits.cpp : test failed");

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

	static_assert(fea::max_v<e::four, e::two, e::three> == e::four,
			"enum_traits.cpp : test failed");
	static_assert(fea::max_v<e::two, e::one, e::three> == e::three,
			"enum_traits.cpp : test failed");
	static_assert(fea::max_v<e::two, e::four, e::three> == e::four,
			"enum_traits.cpp : test failed");

	static_assert(fea::min_v<e::one, e::two, e::three> == e::one,
			"enum_traits.cpp : test failed");
	static_assert(fea::min_v<e::two, e::three, e::one> == e::one,
			"enum_traits.cpp : test failed");
	static_assert(fea::min_v<e::count, e::three, e::four> == e::three,
			"enum_traits.cpp : test failed");

	{
		constexpr std::array<size_t, 5u> lookup = fea::make_enum_lookup<e::four,
				e::two, e::one, e::three, e::count>();
		static_assert(
				lookup[size_t(e::four)] == 0u, "enum_traits.cpp : test failed");
		static_assert(
				lookup[size_t(e::two)] == 1u, "enum_traits.cpp : test failed");
		static_assert(
				lookup[size_t(e::one)] == 2u, "enum_traits.cpp : test failed");
		static_assert(lookup[size_t(e::three)] == 3u,
				"enum_traits.cpp : test failed");
		static_assert(lookup[size_t(e::count)] == 4u,
				"enum_traits.cpp : test failed");
	}
}

} // namespace

#include <fea/maps/details/unsigned_lookup.hpp>
#include <fea/utils/unused.hpp>
#include <gtest/gtest.h>

namespace {
#define test_failed_msg "unsigned_lookup.cpp : Unit test failed."

struct my_id {
	my_id() = default;
	my_id(size_t id_)
			: id(uint8_t(id_)) {
	}

	friend bool operator==(const my_id& lhs, const my_id& rhs) noexcept {
		return lhs.id == rhs.id;
	}

	uint8_t id = 0;
	unsigned bla = 0;
};
} // namespace

namespace fea {
template <>
struct id_getter<my_id> {
	inline constexpr uint8_t operator()(const my_id& k) const noexcept {
		return k.id;
	}
};
} // namespace fea

namespace {
TEST(unsigned_lookup, basics) {
	{
		my_id id1(0u);
		my_id id2(0u);
		EXPECT_EQ(id1, id2);
	}

	{
		fea::detail::unsigned_lookup<size_t> ul;

		using ul_t = std::decay_t<decltype(ul)>;
		static_assert(std::is_same<typename ul_t::hasher,
							  fea::id_getter<size_t>>::value,
				test_failed_msg);
		static_assert(
				std::is_same<typename ul_t::underlying_key_type, size_t>::value,
				test_failed_msg);
		static_assert(std::is_same<typename ul_t::pos_type, size_t>::value,
				test_failed_msg);

		fea::detail::unsigned_lookup<size_t> ul2;
		ul.swap(ul2);

		size_t k = 0u;
		EXPECT_EQ(ul.data(), nullptr);
		EXPECT_EQ(ul.size(), 0u);
		EXPECT_EQ(ul.find(k, 1u), 1u);
		EXPECT_FALSE(ul.contains(k));

		ul.insert(k, k);
		EXPECT_EQ(ul.at_unchecked(k), k);
		EXPECT_EQ(ul.find(k, 1), 0u);
		EXPECT_NE(ul.data(), nullptr);
		EXPECT_EQ(ul.size(), 1u);
		EXPECT_TRUE(ul.contains(k));

		ul.clear();
		EXPECT_NE(ul.data(), nullptr);
		EXPECT_EQ(ul.size(), 0u);
		EXPECT_EQ(ul.find(k, 1u), 1u);
		EXPECT_FALSE(ul.contains(k));

		ul.insert(k, k);
		EXPECT_EQ(ul.at_unchecked(k), k);
		EXPECT_EQ(ul.find(k, 1u), 0u);
		EXPECT_NE(ul.data(), nullptr);
		EXPECT_EQ(ul.size(), 1u);
		EXPECT_TRUE(ul.contains(k));

		ul.invalidate(k);
#if !defined(NDEBUG)
		size_t v;
		fea::unused(v);
		EXPECT_DEATH(v = ul.at_unchecked(k), "");
		EXPECT_DEATH(ul.update(k, k), "");
#endif
		EXPECT_EQ(ul.find(k, 1u), 1u);
		EXPECT_FALSE(ul.contains(k));

		ul.insert(k, k);
		ul.update(k, 10u);
		EXPECT_EQ(ul.at_unchecked(k), 10u);
		EXPECT_EQ(ul.find(k, 20u), 10u);
		EXPECT_TRUE(ul.contains(k));

		ul.invalidate(k);
		ul.insert(k, k);
		EXPECT_EQ(ul.at_unchecked(k), k);
		EXPECT_EQ(ul.find(k, 1u), k);
		EXPECT_TRUE(ul.contains(k));

		ul.swap(ul2);
		EXPECT_EQ(ul.data(), nullptr);
		EXPECT_EQ(ul.size(), 0u);
		EXPECT_EQ(ul.find(k, 1u), 1u);
		EXPECT_FALSE(ul.contains(k));
	}

	{
		fea::detail::unsigned_lookup<my_id> ul;

		using ul_t = std::decay_t<decltype(ul)>;
		static_assert(std::is_same<typename ul_t::hasher,
							  fea::id_getter<my_id>>::value,
				test_failed_msg);
		static_assert(std::is_same<typename ul_t::underlying_key_type,
							  uint8_t>::value,
				test_failed_msg);
		static_assert(std::is_same<typename ul_t::pos_type, uint8_t>::value,
				test_failed_msg);
	}
}
} // namespace
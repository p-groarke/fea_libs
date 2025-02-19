#include <fea/containers/id_slot_lookup.hpp>
#include <fea/utils/unused.hpp>
#include <gtest/gtest.h>
#include <unordered_map>

namespace {
#define test_failed_msg "id_slot_lookup.cpp : Unit test failed."

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

template <>
struct fea::id_hash<my_id> {
	inline constexpr uint8_t operator()(const my_id& k) const noexcept {
		return k.id;
	}
};

template <>
struct std::hash<my_id> {
	inline constexpr uint8_t operator()(const my_id& k) const noexcept {
		return k.id;
	}
};

// Will this happily convert return value to size_t?
// std::unordered_map<my_id, int> testme;
// testme.insert({ my_id{}, 0 });

namespace {
TEST(id_lookup, basics) {
	{
		my_id id1(0u);
		my_id id2(0u);
		EXPECT_EQ(id1, id2);
	}

	{
		fea::id_slot_lookup<size_t> ul;

		using ul_t = std::decay_t<decltype(ul)>;
		static_assert(
				std::is_same_v<typename ul_t::hasher, fea::id_hash<size_t>>,
				test_failed_msg);
		static_assert(
				std::is_same_v<typename ul_t::underlying_key_type, size_t>,
				test_failed_msg);
		static_assert(std::is_same_v<typename ul_t::pos_type, size_t>,
				test_failed_msg);

		fea::id_slot_lookup<size_t> ul2;
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
		if constexpr (fea::debug_build) {
			size_t v;
			fea::unused(v);
			EXPECT_DEATH(v = ul.at_unchecked(k), "");
			EXPECT_DEATH(ul.update(k, k), "");
		}
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
		fea::id_slot_lookup<my_id> ul;

		using ul_t = std::decay_t<decltype(ul)>;
		static_assert(
				std::is_same_v<typename ul_t::hasher, fea::id_hash<my_id>>,
				test_failed_msg);
		static_assert(
				std::is_same_v<typename ul_t::underlying_key_type, uint8_t>,
				test_failed_msg);
		static_assert(std::is_same_v<typename ul_t::pos_type, uint8_t>,
				test_failed_msg);
	}
}
} // namespace
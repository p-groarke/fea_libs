#include <fea/descriptors/descriptor_map.hpp>
#include <gtest/gtest.h>
#include <string_view>

namespace {
struct desc_l {
	static constexpr size_t key = 0;
};
struct desc_u {
	static constexpr size_t Key = 0;
};
struct desc_nok {};
struct desc_badk {
	[[maybe_unused]] static constexpr std::string_view key = "bad";
};

enum class potato {
	russet,
	yukon_gold,
	bintje,
	king_edward,
	kennebec,
	count,
};

struct russet_desc {
	static constexpr potato key = potato::russet;
	static constexpr std::string_view name = "0";
};

struct yukon_desc {
	static constexpr potato key = potato::yukon_gold;
	static constexpr std::string_view name = "1";
};

struct bintje_desc {
	static constexpr potato key = potato::bintje;
	static constexpr std::string_view name = "2";
};

struct king_edward_desc {
	static constexpr potato key = potato::king_edward;
	static constexpr std::string_view name = "3";
};

struct kennebec_desc {
	static constexpr potato key = potato::kennebec;
	static constexpr std::string_view name = "4";
};


TEST(descriptors, basics) {
	static_assert(!fea::descriptor_map<size_t, desc_l>::upper_case_key,
			"descriptors.cpp : test failed");
	static_assert(fea::descriptor_map<size_t, desc_u>::upper_case_key,
			"descriptors.cpp : test failed");

	static_assert(
			std::is_same_v<fea::descriptor_map<potato, russet_desc>::key_t,
					potato>,
			"descriptors.cpp : test failed");

	fea::descriptor_map<potato, russet_desc, yukon_desc, bintje_desc,
			king_edward_desc, kennebec_desc>
			mmap;

	// VS v141 doesn't support constexpr array.
	std::array<std::string_view, size_t(potato::count)> test_arr
			= mmap.make_array([](auto desc) { return desc.name; });

	for (size_t i = 0; i < test_arr.size(); ++i) {
		EXPECT_EQ(test_arr[i], std::to_string(i));
	}

	// VS v141 doesn't support constexpr array.
	fea::enum_array<std::string_view, potato> test_arr2
			= mmap.make_enum_array([](auto desc) { return desc.name; });

	for (size_t i = 0; i < test_arr2.size(); ++i) {
		EXPECT_EQ(test_arr2[potato(i)], std::to_string(i));
	}

	size_t num_visited = 0;
	mmap.for_each_descriptor([&](auto desc) {
		constexpr size_t i = size_t(decltype(desc)::key);
		EXPECT_EQ(desc.name, std::to_string(i));
		++num_visited;
	});

	EXPECT_EQ(num_visited, mmap.size);

	// Shouldn't compile
	// fea::descriptor_map<potato, russet_desc, yukon_desc, bintje_desc,
	// kennebec_desc,
	//		king_edward_desc>
	//		m;
	// fea::descriptor_map<size_t, desc_nok> m2;
	// fea::descriptor_map<std::string_view, desc_badk> m3;
}
} // namespace

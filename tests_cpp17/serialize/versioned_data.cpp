#include <cstdint>
#include <fea/serialize/versioned_data.hpp>
#include <gtest/gtest.h>

namespace {
enum class version_e : uint32_t {
	v0,
	v1,
	v2,
	v3,
	v4,
	v5,
	count, // must be at end
};

template <version_e>
struct data;

template <>
struct data<version_e::v0> {
	int v = 0;
	std::vector<version_e> test;
};
template <>
struct data<version_e::v1> {
	int v = 1;
	std::vector<version_e> test;
};
template <>
struct data<version_e::v2> {
	int v = 2;
	std::vector<version_e> test;
};
template <>
struct data<version_e::v3> {
	int v = 3;
	std::vector<version_e> test;
};
template <>
struct data<version_e::v4> {
	int v = 4;
	std::vector<version_e> test;
};
template <>
struct data<version_e::v5> {
	int v = 5;
	std::vector<version_e> test;
};

// clang-format off
fea::versioned_data<version_e
	, data<version_e::v0>
	, data<version_e::v1>
	, data<version_e::v2>
	, data<version_e::v3>
	, data<version_e::v4>
	, data<version_e::v5>
> version_map{};
// clang-format on

void upgrade(const data<version_e::v0>& from, data<version_e::v1>& to) {
	EXPECT_EQ(from.v, 0);
	EXPECT_EQ(to.v, 1);
	to.test = from.test;
	to.test.push_back(version_e::v1);
}
void upgrade(const data<version_e::v1>& from, data<version_e::v2>& to) {
	EXPECT_EQ(from.v, 1);
	EXPECT_EQ(to.v, 2);
	to.test = from.test;
	to.test.push_back(version_e::v2);
}
void upgrade(const data<version_e::v2>& from, data<version_e::v3>& to) {
	EXPECT_EQ(from.v, 2);
	EXPECT_EQ(to.v, 3);
	to.test = from.test;
	to.test.push_back(version_e::v3);
}
void upgrade(const data<version_e::v3>& from, data<version_e::v4>& to) {
	EXPECT_EQ(from.v, 3);
	EXPECT_EQ(to.v, 4);
	to.test = from.test;
	to.test.push_back(version_e::v4);
}
void upgrade(const data<version_e::v4>& from, data<version_e::v5>& to) {
	EXPECT_EQ(from.v, 4);
	EXPECT_EQ(to.v, 5);
	to.test = from.test;
	to.test.push_back(version_e::v5);
}

TEST(versioned_data, basics) {
	// Test going through the versions one by one.
	{
		data<version_e::v0> datav0{};
		data<version_e::v5> datav5{};
		version_map.upgrade(datav0, datav5);

		const std::vector<version_e> expected{
			version_e::v1,
			version_e::v2,
			version_e::v3,
			version_e::v4,
			version_e::v5,
		};
		EXPECT_EQ(datav5.test, expected);

		//// Shouldn't compile.
		// version_map.upgrade(datav5, datav0);
	}

	using v0data_t = data<version_e::v0>;
	using v1data_t = data<version_e::v1>;
	// using v2data_t = data<version_e::v2>;
	// using v3data_t = data<version_e::v3>;
	// using v4data_t = data<version_e::v4>;
	// using v5data_t = data<version_e::v5>;

	// Test bypassing the system and calling a single update function.
	{
		v0data_t datav0{};
		v1data_t datav1{};
		version_map.upgrade(datav0, datav1);

		const std::vector<version_e> expected{
			version_e::v1,
		};
		EXPECT_EQ(datav1.test, expected);

		//// Shouldn't compile.
		// version_map.upgrade(datav1, datav0);
	}
}
} // namespace

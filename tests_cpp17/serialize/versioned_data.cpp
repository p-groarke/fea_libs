#include <cstdint>
#include <fea/serialize/versioned_data.hpp>
#include <fea/utils/platform.hpp>
#include <gtest/gtest.h>

#if FEA_VS_GT(2017)

namespace {
#define TEST_FAIL_MSG "versioned_data.cpp : Unit test failed."

struct test_cerealizer {
	std::vector<uint32_t> call_version;
	std::vector<uint32_t> downgrade_visited;
};

struct data_v0 {
	static constexpr uint32_t version = 0;

	// You put whatever here, it only enforces version.
	int v = 0;
	std::vector<uint32_t> test;
};

struct data_v1 {
	static constexpr uint32_t version = 1;
	int v = 1;
	std::vector<uint32_t> test;
};

namespace potato {
struct data_v2 {
	static constexpr uint32_t version = 2;
	int v = 2;
	std::vector<uint32_t> test;
};
} // namespace potato

struct data_v3 {
	static constexpr uint32_t version = 3;
	int v = 3;
	std::vector<uint32_t> test;
};

struct data_v4 {
	static constexpr uint32_t version = 4;
	int v = 4;
	std::vector<uint32_t> test;
};

struct data_v5 {
	static constexpr uint32_t version = 5;
	int v = 5;
	std::vector<uint32_t> test;
};

// clang-format off
constexpr fea::versioned_data<
	data_v0
	, data_v1
	, potato::data_v2
	, data_v3
	, data_v4
	, data_v5
> version_map{};
// clang-format on

void deserialize(test_cerealizer& d, data_v0& to) {
	d.call_version.push_back(to.version);
	to = {};
}
void serialize(const data_v0& from, test_cerealizer& s) {
	s.call_version.push_back(from.version);
	s.downgrade_visited.insert(
			s.downgrade_visited.end(), from.test.begin(), from.test.end());
}
void upgrade(const data_v0& from, data_v1& to) {
	EXPECT_EQ(from.v, 0);
	EXPECT_EQ(to.v, 1);
	to.test = from.test;
	to.test.push_back(from.version);
}
void downgrade(const data_v1& from, data_v0& to) {
	EXPECT_EQ(from.v, 1);
	EXPECT_EQ(to.v, 0);
	to.test = from.test;
	to.test.push_back(from.version);
}

void deserialize(test_cerealizer& d, data_v1& to) {
	d.call_version.push_back(to.version);
	to = {};
}
void serialize(const data_v1& from, test_cerealizer& s) {
	s.call_version.push_back(from.version);
	s.downgrade_visited.insert(
			s.downgrade_visited.end(), from.test.begin(), from.test.end());
}
void upgrade(const data_v1& from, potato::data_v2& to) {
	EXPECT_EQ(from.v, 1);
	EXPECT_EQ(to.v, 2);
	to.test = from.test;
	to.test.push_back(from.version);
}
void downgrade(const potato::data_v2& from, data_v1& to) {
	EXPECT_EQ(from.v, 2);
	EXPECT_EQ(to.v, 1);
	to.test = from.test;
	to.test.push_back(from.version);
}

namespace potato {
void deserialize(test_cerealizer& d, data_v2& to) {
	d.call_version.push_back(to.version);
	to = {};
}
void serialize(const data_v2& from, test_cerealizer& s) {
	s.call_version.push_back(from.version);
	s.downgrade_visited.insert(
			s.downgrade_visited.end(), from.test.begin(), from.test.end());
}
void upgrade(const data_v2& from, data_v3& to) {
	EXPECT_EQ(from.v, 2);
	EXPECT_EQ(to.v, 3);
	to.test = from.test;
	to.test.push_back(from.version);
}
void downgrade(const data_v3& from, data_v2& to) {
	EXPECT_EQ(from.v, 3);
	EXPECT_EQ(to.v, 2);
	to.test = from.test;
	to.test.push_back(from.version);
}
} // namespace potato

void deserialize(test_cerealizer& d, data_v3& to) {
	d.call_version.push_back(to.version);
	to = {};
}
void serialize(const data_v3& from, test_cerealizer& s) {
	s.call_version.push_back(from.version);
	s.downgrade_visited.insert(
			s.downgrade_visited.end(), from.test.begin(), from.test.end());
}
void upgrade(const data_v3& from, data_v4& to) {
	EXPECT_EQ(from.v, 3);
	EXPECT_EQ(to.v, 4);
	to.test = from.test;
	to.test.push_back(from.version);
}
void downgrade(const data_v4& from, data_v3& to) {
	EXPECT_EQ(from.v, 4);
	EXPECT_EQ(to.v, 3);
	to.test = from.test;
	to.test.push_back(from.version);
}

void deserialize(test_cerealizer& d, data_v4& to) {
	d.call_version.push_back(to.version);
	to = {};
}
void serialize(const data_v4& from, test_cerealizer& s) {
	s.call_version.push_back(from.version);
	s.downgrade_visited.insert(
			s.downgrade_visited.end(), from.test.begin(), from.test.end());
}
void upgrade(const data_v4& from, data_v5& to) {
	EXPECT_EQ(from.v, 4);
	EXPECT_EQ(to.v, 5);
	to.test = from.test;
	to.test.push_back(from.version);
}
void downgrade(const data_v5& from, data_v4& to) {
	EXPECT_EQ(from.v, 5);
	EXPECT_EQ(to.v, 4);
	to.test = from.test;
	to.test.push_back(from.version);
}

void deserialize(test_cerealizer& d, data_v5& to) {
	d.call_version.push_back(to.version);
	to = {};
}
void serialize(const data_v5& from, test_cerealizer& s) {
	s.call_version.push_back(from.version);
	s.downgrade_visited.insert(
			s.downgrade_visited.end(), from.test.begin(), from.test.end());
}

TEST(versioned_data, basics) {
	// Foundations
	{
		static_assert(
				fea::is_detected_v<fea::detail::mhas_upgrade, data_v0, data_v1>,
				TEST_FAIL_MSG);
		static_assert(fea::is_detected_v<fea::detail::mhas_downgrade, data_v1,
							  data_v0>,
				TEST_FAIL_MSG);
		static_assert(fea::is_detected_v<fea::detail::mhas_deserialize,
							  test_cerealizer, data_v0>,
				TEST_FAIL_MSG);
	}

	// Test going through the versions one by one.
	{
		data_v0 datav0{};
		data_v5 datav5{};
		version_map.upgrade(datav0, datav5);

		const std::vector<uint32_t> expected{ 0, 1, 2, 3, 4 };
		EXPECT_EQ(datav5.test, expected);

		//// Shouldn't compile.
		// version_map.upgrade(datav5, datav0);
	}

	// Test bypassing the system and calling a single update function.
	{
		data_v0 datav0{};
		data_v1 datav1{};
		version_map.upgrade(datav0, datav1);

		const std::vector<uint32_t> expected{ 0 };
		EXPECT_EQ(datav1.test, expected);

		// Shouldn't compile.
		// version_map.upgrade(datav1, datav0);
	}

	// Test going through the versions one by one.
	{
		data_v0 datav0{};
		data_v5 datav5{};
		version_map.downgrade(datav5, datav0);

		const std::vector<uint32_t> expected{ 5, 4, 3, 2, 1 };
		EXPECT_EQ(datav0.test, expected);

		//// Shouldn't compile.
		// version_map.downgrade(datav0, datav5);
	}

	// Test bypassing the system and calling a single update function.
	{
		data_v0 datav0{};
		data_v1 datav1{};
		version_map.downgrade(datav1, datav0);

		const std::vector<uint32_t> expected{ 1 };
		EXPECT_EQ(datav0.test, expected);

		//// Shouldn't compile.
		// version_map.downgrade(datav0, datav1);
	}

	// Deserialize
	{
		test_cerealizer d;
		data_v5 datav5{};
		version_map.deserialize(0, d, datav5);

		const std::vector<uint32_t> expected_d{ 0 };
		EXPECT_EQ(d.call_version, expected_d);

		const std::vector<uint32_t> expected{ 0, 1, 2, 3, 4 };
		EXPECT_EQ(datav5.test, expected);
	}

	{
		test_cerealizer d;
		data_v5 datav5{};
		version_map.deserialize(5, d, datav5);

		const std::vector<uint32_t> expected_d{ 5 };
		EXPECT_EQ(d.call_version, expected_d);

		const std::vector<uint32_t> expected{};
		EXPECT_EQ(datav5.test, expected);
	}

	// Serialize
	{
		test_cerealizer s;
		data_v5 datav5{};
		version_map.serialize(0, datav5, s);

		const std::vector<uint32_t> expected_d{ 0 };
		EXPECT_EQ(s.call_version, expected_d);

		const std::vector<uint32_t> expected{ 5, 4, 3, 2, 1 };
		EXPECT_EQ(s.downgrade_visited, expected);
	}

	{
		test_cerealizer s;
		data_v5 datav5{};
		version_map.serialize(5, datav5, s);

		const std::vector<uint32_t> expected_d{ 5 };
		EXPECT_EQ(s.call_version, expected_d);

		const std::vector<uint32_t> expected{};
		EXPECT_EQ(s.downgrade_visited, expected);
	}
}
} // namespace
#endif

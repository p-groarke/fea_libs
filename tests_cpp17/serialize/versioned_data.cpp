#include <cstdint>
#include <fea/serialize/versioned_data.hpp>
#include <fea/utils/platform.hpp>
#include <gtest/gtest.h>

#if !FEA_WINDOWS || (FEA_VSYEAR > 2017)

namespace {
#define ERROR_MSG "versioned_data.cpp : Unit test failed."

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

// void deserialize(int whatever, data_v0& to) {
// }
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

TEST(versioned_data, basics) {
	// Foundations
	{
		static_assert(
				fea::is_detected_v<fea::detail::mhas_upgrade, data_v0, data_v1>,
				ERROR_MSG);
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

	// struct ini {
	//	ini& operator[](std::string) {
	//		return *this;
	//	}
	//	ini& operator,(std::string) {
	//		return *this;
	//	}
	//	ini& operator=(int) {
	//		return *this;
	//	}
	// };

	// ini f;
	// f["New Section"]["new-key"] = 1234, "a comment";
}
} // namespace
#endif

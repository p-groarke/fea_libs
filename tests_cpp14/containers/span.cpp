#include <fea/containers/span.hpp>
#include <gtest/gtest.h>

namespace {
#define FAILMSG "span.cpp : Unit test failed."

TEST(span, basics) {
#if FEA_CPP20
	static_assert(std::is_same_v<std::span<int>, fea::span<int>>, FAILMSG);
#endif

	std::vector<size_t> vec{ 0u, 1u, 2u };
	{
		fea::imp::span<const size_t> s{ vec }; // test const from non-const vec
		EXPECT_EQ(s.size(), vec.size());
		EXPECT_EQ(s[0], vec[0]);
		EXPECT_EQ(s[1], vec[1]);
		EXPECT_EQ(s[2], vec[2]);
	}

	fea::imp::span<const size_t> s{ vec.data(), vec.size() };
	EXPECT_EQ(s.size(), vec.size());
	EXPECT_EQ(s.size_bytes(), vec.size() * sizeof(size_t));
	EXPECT_FALSE(s.empty());
	EXPECT_NE(s.data(), nullptr);

	EXPECT_EQ(size_t(std::distance(s.begin(), s.end())), 3u);
	EXPECT_EQ(size_t(std::distance(s.rbegin(), s.rend())), 3u);
	EXPECT_EQ(s.front(), 0u);
	EXPECT_EQ(s.back(), 2u);
	EXPECT_EQ(*s.data(), 0u);
	EXPECT_EQ(s.data()[1], 1u);

	size_t expected_val = 0u;
	for (size_t i : s) {
		EXPECT_EQ(i, expected_val);
		++expected_val;
	}

	for (size_t i = 0; i < s.size(); ++i) {
		EXPECT_EQ(s[i], i);
		EXPECT_EQ(*(s.begin() + i), i);
		EXPECT_EQ(s.data()[i], i);
	}

	int e_val = 2;
	for (auto it = s.rbegin(); it != s.rend(); ++it) {
		EXPECT_EQ(*it, size_t(e_val));
		--e_val;
	}
}

TEST(span, empty) {
	{
		fea::imp::span<const size_t> s;
		EXPECT_EQ(s.size(), 0u);
		EXPECT_EQ(s.size_bytes(), 0u);
		EXPECT_TRUE(s.empty());
		EXPECT_EQ(s.data(), nullptr);

		EXPECT_EQ(size_t(std::distance(s.begin(), s.end())), 0u);
		EXPECT_EQ(size_t(std::distance(s.rbegin(), s.rend())), 0u);
	}

	{
		std::vector<size_t> v;
		fea::imp::span<const size_t> s(v.begin(), v.end());
		EXPECT_TRUE(s.empty());
		EXPECT_EQ(s.size(), 0u);
		EXPECT_EQ(s.data(), nullptr);
	}
}
} // namespace

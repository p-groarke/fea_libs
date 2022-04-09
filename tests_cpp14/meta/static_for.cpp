#include <fea/meta/static_for.hpp>
#include <gtest/gtest.h>

namespace {

template <class... Args>
int foldit(Args... args) {
	int ret = 0;
	auto add = [&](auto i) { ret += i; };
	fea::fold(add, args...);
	return ret;
}

TEST(static_for, basics) {
	{
		int ans = foldit(0, 1, 2, 3);
		EXPECT_EQ(ans, 6);
	}

	{
		int ans = 0;
		fea::static_for<4>([&](auto v) { ans += int(v()); });
		EXPECT_EQ(ans, 6);
	}

	{
		int ans = 0;
		fea::apply_indexes<4>([&](auto... ic) {
			auto l = [&](auto v) { ans += int(v()); };

			fea::fold(l, ic...);
		});
		EXPECT_EQ(ans, 6);
	}
}

TEST(static_for, return_type) {

	{
		auto tup = std::make_tuple([]() {}, []() {}, []() {});
		using ret_t = decltype(fea::static_for<3>([&](auto const_i) {
			constexpr size_t i = const_i();
			return std::get<i>(tup)();
		}));
		using expected_t = void;

		static_assert(std::is_same<ret_t, expected_t>::value,
				"static_for.cpp : test failed");
	}

	{
		auto tup = std::make_tuple([]() { return int(42); },
				[]() { return double(42); }, []() { return char(42); });

		using ret_t = decltype(fea::static_for<3>([&](auto const_i) {
			constexpr size_t i = const_i();
			return std::get<i>(tup)();
		}));
		using expected_t = std::tuple<int, double, char>;

		static_assert(std::is_same<ret_t, expected_t>::value,
				"static_for.cpp : test failed");

		auto ans = fea::static_for<3>([&](auto const_i) {
			constexpr size_t i = const_i();
			return std::get<i>(tup)();
		});

		EXPECT_EQ(std::get<0>(ans), int(42));
		EXPECT_EQ(std::get<1>(ans), double(42));
		EXPECT_EQ(std::get<2>(ans), char(42));
	}

	{
		auto tup = std::make_tuple(
				[]() {}, []() { return double(42); }, []() {});

		using ret_t = decltype(fea::static_for<3>([&](auto const_i) {
			constexpr size_t i = const_i();
			return std::get<i>(tup)();
		}));
		using expected_t = std::tuple<std::nullptr_t, double, std::nullptr_t>;

		static_assert(std::is_same<ret_t, expected_t>::value,
				"static_for.cpp : test failed");

		auto ans = fea::static_for<3>([&](auto const_i) {
			constexpr size_t i = const_i();
			return std::get<i>(tup)();
		});

		EXPECT_EQ(std::get<0>(ans), std::nullptr_t{});
		EXPECT_EQ(std::get<1>(ans), double(42));
		EXPECT_EQ(std::get<2>(ans), std::nullptr_t{});
	}

	{
		auto tup = std::make_tuple([]() { return int(0); },
				[]() { return int(42); }, []() { return int(1); });

		using ret_t = decltype(fea::static_for<3>([&](auto const_i) {
			constexpr size_t i = const_i();
			return std::get<i>(tup)();
		}));
		using expected_t = std::array<int, 3>;

		static_assert(std::is_same<ret_t, expected_t>::value,
				"static_for.cpp : test failed");

		std::array<int, 3> ans = fea::static_for<3>([&](auto const_i) {
			constexpr size_t i = const_i();
			return std::get<i>(tup)();
		});

		EXPECT_EQ(std::get<0>(ans), int(0));
		EXPECT_EQ(std::get<1>(ans), int(42));
		EXPECT_EQ(std::get<2>(ans), int(1));
	}
}
} // namespace

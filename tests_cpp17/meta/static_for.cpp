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
		auto l = [&](auto const_i) {
			constexpr size_t i = const_i();
			return std::get<i>(tup)();
		};
		using ret_t = decltype(fea::static_for<3>(l));
		using expected_t = void;

		static_assert(std::is_same_v<ret_t, expected_t>,
				"static_for.cpp : test failed");
	}

	{
		auto tup = std::make_tuple([]() { return int(42); },
				[]() { return double(42); }, []() { return char(42); });

		auto l = [&](auto const_i) {
			constexpr size_t i = const_i();
			return std::get<i>(tup)();
		};
		using ret_t = decltype(fea::static_for<3>(l));
		using expected_t = std::tuple<int, double, char>;

		static_assert(std::is_same_v<ret_t, expected_t>,
				"static_for.cpp : test failed");

		std::tuple<int, double, char> ans = fea::static_for<3>(l);
		EXPECT_EQ(std::get<0>(ans), int(42));
		EXPECT_EQ(std::get<1>(ans), double(42));
		EXPECT_EQ(std::get<2>(ans), char(42));
	}

	{
		auto tup = std::make_tuple(
				[]() {}, []() { return double(42); }, []() {});

		auto l = [&](auto const_i) {
			constexpr size_t i = const_i();
			return std::get<i>(tup)();
		};
		using ret_t = decltype(fea::static_for<3>(l));
		using expected_t = std::tuple<std::nullptr_t, double, std::nullptr_t>;

		static_assert(std::is_same_v<ret_t, expected_t>,
				"static_for.cpp : test failed");

		auto ans = fea::static_for<3>(l);
		EXPECT_EQ(std::get<0>(ans), std::nullptr_t{});
		EXPECT_EQ(std::get<1>(ans), double(42));
		EXPECT_EQ(std::get<2>(ans), std::nullptr_t{});
	}

	{
		auto tup = std::make_tuple([]() { return int(0); },
				[]() { return int(42); }, []() { return int(1); });

		auto l = [&](auto const_i) {
			constexpr size_t i = const_i();
			return std::get<i>(tup)();
		};
		using ret_t = decltype(fea::static_for<3>(l));
		using expected_t = std::array<int, 3>;

		static_assert(std::is_same_v<ret_t, expected_t>,
				"static_for.cpp : test failed");

		std::array<int, 3> ans = fea::static_for<3>(l);
		EXPECT_EQ(std::get<0>(ans), int(0));
		EXPECT_EQ(std::get<1>(ans), int(42));
		EXPECT_EQ(std::get<2>(ans), int(1));
	}

	{
		int i(1);
		double d(2);
		char c(3);

		auto tup = std::make_tuple([&]() -> int& { return i; },
				[&]() -> double& { return d; }, [&]() -> char& { return c; });

		auto l = [&](auto const_i) -> auto& {
			constexpr size_t i = const_i();
			return std::get<i>(tup)();
		};

		using ret_t = decltype(fea::static_for<3>(l));
		using expected_t = std::tuple<int&, double&, char&>;

		static_assert(std::is_same_v<ret_t, expected_t>,
				"static_for.cpp : test failed");

		std::tuple<int&, double&, char&> ans = fea::static_for<3>(l);
		EXPECT_EQ(std::get<0>(ans), i);
		EXPECT_EQ(std::get<1>(ans), d);
		EXPECT_EQ(std::get<2>(ans), c);
	}

	{
		int i(1);
		char c(3);

		auto tup = std::make_tuple(
				[&]() -> int& { return i; }, [&]() {}, [&]() { return &c; });

		auto l = [&](auto const_i) -> decltype(auto) {
			constexpr size_t i = const_i();
			return std::get<i>(tup)();
		};

		using ret_t = decltype(fea::static_for<3>(l));
		using expected_t = std::tuple<int&, std::nullptr_t, char*>;

		static_assert(std::is_same_v<ret_t, expected_t>,
				"static_for.cpp : test failed");

		std::tuple<int&, std::nullptr_t, char*> ans = fea::static_for<3>(l);
		EXPECT_EQ(std::get<0>(ans), i);
		EXPECT_EQ(std::get<1>(ans), std::nullptr_t{});
		EXPECT_EQ(*std::get<2>(ans), c);
	}
}

TEST(static_for_reversed, basics) {
	std::array<int, 6> got_arr{};
	const std::array<int, 6> expected_arr{ 5, 4, 3, 2, 1, 0 };

	{
		size_t realidx = 0;
		fea::static_for_reversed<6>([&](auto const_i) {
			constexpr size_t i = const_i;
			got_arr[realidx++] = int(i);
		});
		EXPECT_EQ(got_arr, expected_arr);
	}
}

} // namespace

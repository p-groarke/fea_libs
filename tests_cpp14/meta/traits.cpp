#include <fea/meta/traits.hpp>
#include <fea/meta/tuple.hpp>
#include <functional>
#include <gtest/gtest.h>
#include <string>
#include <tuple>
#include <vector>

namespace {
TEST(traits, all_none_any) {
	std::tuple<int, float, unsigned, short, double> all_numeric{};
	std::tuple<std::string, std::function<void()>, std::vector<int>, int>
			any_numeric{};
	std::tuple<std::string, std::function<void()>, std::vector<int>>
			none_numeric{};

	fea::apply(
			[](auto... args) {
				constexpr bool all
						= fea::all_of_v<std::is_arithmetic<decltype(args)>...>;
				constexpr bool none
						= fea::none_of_v<std::is_arithmetic<decltype(args)>...>;
				constexpr bool any
						= fea::any_of_v<std::is_arithmetic<decltype(args)>...>;

				static_assert(all, "traits.cpp : failed test");
				static_assert(!none, "traits.cpp : failed test");
				static_assert(any, "traits.cpp : failed test");
			},
			all_numeric);

	fea::apply(
			[](auto... args) {
				constexpr bool all
						= fea::all_of_v<std::is_arithmetic<decltype(args)>...>;
				constexpr bool none
						= fea::none_of_v<std::is_arithmetic<decltype(args)>...>;
				constexpr bool any
						= fea::any_of_v<std::is_arithmetic<decltype(args)>...>;

				static_assert(!all, "traits.cpp : failed test");
				static_assert(!none, "traits.cpp : failed test");
				static_assert(any, "traits.cpp : failed test");
			},
			any_numeric);

	fea::apply(
			[](auto... args) {
				constexpr bool all
						= fea::all_of_v<std::is_arithmetic<decltype(args)>...>;
				constexpr bool none
						= fea::none_of_v<std::is_arithmetic<decltype(args)>...>;
				constexpr bool any
						= fea::any_of_v<std::is_arithmetic<decltype(args)>...>;

				static_assert(!all, "traits.cpp : failed test");
				static_assert(none, "traits.cpp : failed test");
				static_assert(!any, "traits.cpp : failed test");
			},
			none_numeric);
}

struct potato {
	void noargs_func() {
	}
	void args_func(int&) {
	}
};

struct tomato {};

template <class T>
using has_noargs_func = decltype(std::declval<T>().noargs_func());

template <class T>
using has_args_func
		= decltype(std::declval<T>().args_func(std::declval<int&>()));

TEST(traits, is_detected) {
	static_assert(fea::is_detected_v<has_noargs_func, potato>,
			"traits.cpp : failed test");

	static_assert(fea::is_detected_v<has_args_func, potato>,
			"traits.cpp : failed test");

	static_assert(!fea::is_detected_v<has_noargs_func, tomato>,
			"traits.cpp : failed test");

	static_assert(!fea::is_detected_v<has_args_func, tomato>,
			"traits.cpp : failed test");
}
} // namespace

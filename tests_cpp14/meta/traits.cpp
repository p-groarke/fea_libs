#include <fea/meta/traits.hpp>
#include <functional>
#include <gtest/gtest.h>
#include <set>
#include <string>
#include <tuple>
#include <utility>
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


// is_detected
// class with member functions
struct potato {
	void noargs_func() {
	}
	void args_func(int&) {
	}
};

// class without member functions
struct tomato {};

// you must implement these "detectors"
template <class T>
using has_noargs_func = decltype(std::declval<T>().noargs_func());

// you must implement these "detectors"
template <class T>
using has_args_func
		= decltype(std::declval<T>().args_func(std::declval<int&>()));

// detect if classes have the functions you want.
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

TEST(traits, misc) {
	static_assert(
			std::is_same<fea::remove_nested_const_t<std::pair<const int, int>>,
					std::pair<int, int>>::value,
			"traits.cpp : test failed");

	static_assert(fea::is_first_const_v<std::pair<const int, int>>,
			"traits.cpp : test failed");
	static_assert(!fea::is_first_const_v<std::pair<int, const int>>,
			"traits.cpp : test failed");

	static_assert(
			fea::is_container_v<std::vector<int>>, "traits.cpp : test failed");
	static_assert(
			!fea::is_container_v<std::tuple<int>>, "traits.cpp : test failed");

	static_assert(
			fea::is_tuple_like_v<std::tuple<int>>, "traits.cpp : test failed");
	static_assert(fea::is_tuple_like_v<std::pair<int, int>>,
			"traits.cpp : test failed");
	static_assert(!fea::is_tuple_like_v<std::vector<int>>,
			"traits.cpp : test failed");

	static_assert(
			fea::is_contiguous_v<std::vector<int>>, "traits.cpp : test failed");
	static_assert(
			!fea::is_contiguous_v<std::set<int>>, "traits.cpp : test failed");
}

TEST(traits, front_back_t) {
	static_assert(std::is_same<fea::front_t<int, double, float>, int>::value,
			"traits.cpp : test failed");
	static_assert(std::is_same<fea::back_t<int, double, float>, float>::value,
			"traits.cpp : test failed");
}
} // namespace

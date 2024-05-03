#include <fea/meta/traits.hpp>
#include <fea/meta/tuple.hpp>
#include <functional>
#include <gtest/gtest.h>
#include <set>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace {
#define fail_msg "traits.cpp : failed test"

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
				constexpr bool one
						= fea::one_of_v<std::is_arithmetic<decltype(args)>...>;
				constexpr bool one2
						= fea::one_of_v<std::is_same<decltype(args), int>...>;

				static_assert(all, fail_msg);
				static_assert(!none, fail_msg);
				static_assert(any, fail_msg);
				static_assert(!one, fail_msg);
				static_assert(one2, fail_msg);
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
				constexpr bool one
						= fea::one_of_v<std::is_arithmetic<decltype(args)>...>;

				static_assert(!all, fail_msg);
				static_assert(!none, fail_msg);
				static_assert(any, fail_msg);
				static_assert(one, fail_msg);
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
				constexpr bool one
						= fea::one_of_v<std::is_arithmetic<decltype(args)>...>;

				static_assert(!all, fail_msg);
				static_assert(none, fail_msg);
				static_assert(!any, fail_msg);
				static_assert(!one, fail_msg);
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
	static_assert(fea::is_detected_v<has_noargs_func, potato>, fail_msg);

	static_assert(fea::is_detected_v<has_args_func, potato>, fail_msg);

	static_assert(!fea::is_detected_v<has_noargs_func, tomato>, fail_msg);

	static_assert(!fea::is_detected_v<has_args_func, tomato>, fail_msg);
}

enum class an_enum { a, b, c };

TEST(traits, misc) {
	static_assert(
			std::is_same<fea::remove_nested_const_t<std::pair<const int, int>>,
					std::pair<int, int>>::value,
			fail_msg);

	static_assert(fea::is_first_const_v<std::pair<const int, int>>, fail_msg);
	static_assert(!fea::is_first_const_v<std::pair<int, const int>>, fail_msg);

	static_assert(fea::is_container_v<std::vector<int>>, fail_msg);
	static_assert(!fea::is_container_v<std::tuple<int>>, fail_msg);

	static_assert(fea::is_tuple_like_v<std::tuple<int>>, fail_msg);
	static_assert(fea::is_tuple_like_v<std::pair<int, int>>, fail_msg);
	static_assert(!fea::is_tuple_like_v<std::vector<int>>, fail_msg);

	static_assert(fea::is_contiguous_v<std::vector<int>>, fail_msg);
	static_assert(!fea::is_contiguous_v<std::set<int>>, fail_msg);

	static_assert(fea::is_same_template_v<std::vector, std::vector>, fail_msg);
	static_assert(!fea::is_same_template_v<std::vector, std::set>, fail_msg);

	static_assert(fea::is_template_template<std::vector<int>>::value, fail_msg);
	static_assert(!fea::is_template_template<potato>::value, fail_msg);

	static_assert(fea::is_template_template_v<std::vector<int>>, fail_msg);
	static_assert(!fea::is_template_template_v<potato>, fail_msg);

	static_assert(fea::is_static_castable_v<int, float>, fail_msg);
	static_assert(!fea::is_static_castable_v<std::string, float>, fail_msg);
	static_assert(fea::is_static_castable_v<an_enum, int>, fail_msg);
	static_assert(fea::is_static_castable_v<int, an_enum>, fail_msg);

#if FEA_CPP17
	auto throw_l = [](int&, const double&) { return 0; };
	auto nothrow_l = [](int&, const double&) noexcept { return 0; };

	static_assert(!fea::is_noexcept_v<decltype(throw_l), int&, const double&>,
			fail_msg);
	static_assert(fea::is_noexcept_v<decltype(nothrow_l), int&, const double&>,
			fail_msg);
#endif
}

TEST(traits, front_back_t) {
	static_assert(std::is_same<fea::front_t<int, double, float>, int>::value,
			fail_msg);
	static_assert(std::is_same<fea::back_t<int, double, float>, float>::value,
			fail_msg);
}

TEST(traits, reverse) {
	using expected_t = std::tuple<double, float, int, bool>;
	using got_t = fea::reverse_t<bool, int, float, double>;
	static_assert(std::is_same<got_t, expected_t>::value, fail_msg);
}

#if FEA_CPP17
TEST(traits, reversed_index_sequence) {
	{
		using got_t = fea::make_reverse_index_sequence<5>;
		using expected_t = std::index_sequence<4, 3, 2, 1, 0>;
		static_assert(std::is_same<got_t, expected_t>::value, fail_msg);
	}
}
#endif
} // namespace

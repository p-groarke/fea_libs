#include <fea/meta/traits.hpp>
#include <fea/meta/tuple.hpp>
#include <functional>
#include <gtest/gtest.h>
#include <iostream>
#include <set>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace {
#define FAIL_MSG "traits.cpp : failed test"

TEST(traits, all_none_any) {
	std::tuple<int, float, unsigned, short, double> all_numeric{};
	std::tuple<std::string, std::function<void()>, std::vector<int>, int>
			any_numeric{};
	std::tuple<std::string, std::function<void()>, std::vector<int>>
			none_numeric{};

	std::apply(
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

				static_assert(all, FAIL_MSG);
				static_assert(!none, FAIL_MSG);
				static_assert(any, FAIL_MSG);
				static_assert(!one, FAIL_MSG);
				static_assert(one2, FAIL_MSG);
			},
			all_numeric);

	std::apply(
			[](auto... args) {
				constexpr bool all
						= fea::all_of_v<std::is_arithmetic<decltype(args)>...>;
				constexpr bool none
						= fea::none_of_v<std::is_arithmetic<decltype(args)>...>;
				constexpr bool any
						= fea::any_of_v<std::is_arithmetic<decltype(args)>...>;
				constexpr bool one
						= fea::one_of_v<std::is_arithmetic<decltype(args)>...>;

				static_assert(!all, FAIL_MSG);
				static_assert(!none, FAIL_MSG);
				static_assert(any, FAIL_MSG);
				static_assert(one, FAIL_MSG);
			},
			any_numeric);

	std::apply(
			[](auto... args) {
				constexpr bool all
						= fea::all_of_v<std::is_arithmetic<decltype(args)>...>;
				constexpr bool none
						= fea::none_of_v<std::is_arithmetic<decltype(args)>...>;
				constexpr bool any
						= fea::any_of_v<std::is_arithmetic<decltype(args)>...>;
				constexpr bool one
						= fea::one_of_v<std::is_arithmetic<decltype(args)>...>;

				static_assert(!all, FAIL_MSG);
				static_assert(none, FAIL_MSG);
				static_assert(!any, FAIL_MSG);
				static_assert(!one, FAIL_MSG);
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
	static_assert(fea::is_detected_v<has_noargs_func, potato>, FAIL_MSG);

	static_assert(fea::is_detected_v<has_args_func, potato>, FAIL_MSG);

	static_assert(!fea::is_detected_v<has_noargs_func, tomato>, FAIL_MSG);

	static_assert(!fea::is_detected_v<has_args_func, tomato>, FAIL_MSG);
}

enum class an_enum { a, b, c };

TEST(traits, misc) {
	static_assert(std::is_same_v<
						  fea::remove_nested_const_t<std::pair<const int, int>>,
						  std::pair<int, int>>,
			FAIL_MSG);

	static_assert(fea::is_first_const_v<std::pair<const int, int>>, FAIL_MSG);
	static_assert(!fea::is_first_const_v<std::pair<int, const int>>, FAIL_MSG);

	static_assert(fea::is_container_v<std::vector<int>>, FAIL_MSG);
	static_assert(!fea::is_container_v<std::tuple<int>>, FAIL_MSG);

	static_assert(fea::is_tuple_like_v<std::tuple<int>>, FAIL_MSG);
	static_assert(fea::is_tuple_like_v<std::pair<int, int>>, FAIL_MSG);
	static_assert(!fea::is_tuple_like_v<std::vector<int>>, FAIL_MSG);

	static_assert(fea::is_contiguous_v<std::vector<int>>, FAIL_MSG);
	static_assert(!fea::is_contiguous_v<std::set<int>>, FAIL_MSG);

	static_assert(fea::is_same_template_v<std::vector, std::vector>, FAIL_MSG);
	static_assert(!fea::is_same_template_v<std::vector, std::set>, FAIL_MSG);

	static_assert(fea::is_template_template<std::vector<int>>::value, FAIL_MSG);
	static_assert(!fea::is_template_template<potato>::value, FAIL_MSG);

	static_assert(fea::is_template_template_v<std::vector<int>>, FAIL_MSG);
	static_assert(!fea::is_template_template_v<potato>, FAIL_MSG);

	static_assert(fea::is_static_castable_v<int, float>, FAIL_MSG);
	static_assert(!fea::is_static_castable_v<std::string, float>, FAIL_MSG);
	static_assert(fea::is_static_castable_v<an_enum, int>, FAIL_MSG);
	static_assert(fea::is_static_castable_v<int, an_enum>, FAIL_MSG);

	auto throw_l = [](int&, const double&) { return 0; };
	auto nothrow_l = [](int&, const double&) noexcept { return 0; };

	static_assert(!fea::is_noexcept_v<decltype(throw_l), int&, const double&>,
			FAIL_MSG);
	static_assert(fea::is_noexcept_v<decltype(nothrow_l), int&, const double&>,
			FAIL_MSG);

	struct potato {
		double p;
	};
	using potato_storage_t
			= fea::aligned_storage_t<sizeof(potato), alignof(potato)>;

	static_assert(sizeof(potato) == sizeof(potato_storage_t), FAIL_MSG);
	static_assert(alignof(potato) == alignof(potato_storage_t), FAIL_MSG);
}

TEST(traits, front_back_t) {
	static_assert(
			std::is_same_v<fea::front_t<int, double, float>, int>, FAIL_MSG);
	static_assert(
			std::is_same_v<fea::back_t<int, double, float>, float>, FAIL_MSG);
}

TEST(traits, reverse) {
	using expected_t = std::tuple<double, float, int, bool>;
	using got_t = fea::reverse_t<bool, int, float, double>;
	static_assert(std::is_same_v<got_t, expected_t>, FAIL_MSG);
}

TEST(traits, reversed_index_sequence) {
	{
		using got_t = fea::make_reverse_index_sequence<5>;
		using expected_t = std::index_sequence<4, 3, 2, 1, 0>;
		static_assert(std::is_same_v<got_t, expected_t>, FAIL_MSG);
	}
}

TEST(traits, rebind_alloc) {
	static_assert(
			std::is_same_v<fea::rebind_alloc_t<std::allocator<float>, int>,
					std::allocator<int>>,
			FAIL_MSG);
}

TEST(traits, iterator_valut) {
	using iter_t = std::vector<int>::iterator;
	static_assert(std::is_same_v<fea::iterator_value_t<iter_t>, int>, FAIL_MSG);
}

TEST(traits, aligned_storage) {
	using align_t = fea::aligned_storage_t<8, 4>;
	static_assert(alignof(align_t) == 4, FAIL_MSG);
	static_assert(sizeof(align_t) == 8, FAIL_MSG);
}

TEST(traits, output_iterator_traits) {
	std::vector<int> v;
	using bi_t = decltype(std::back_inserter(v));
	using i_t = decltype(std::inserter(v, v.begin() + 5));
	using fi_t = decltype(std::front_inserter(v));

	static_assert(std::is_same_v<fea::output_iterator_vt<bi_t>, int>, FAIL_MSG);
	static_assert(std::is_same_v<fea::output_iterator_vt<i_t>, int>, FAIL_MSG);
	static_assert(std::is_same_v<fea::output_iterator_vt<fi_t>, int>, FAIL_MSG);

	using os_t = decltype(std::ostream_iterator<float>(std::cout, ", "));
	using osb_t = decltype(std::ostreambuf_iterator<wchar_t>(std::wcout));
	static_assert(
			std::is_same_v<fea::output_iterator_vt<os_t>, float>, FAIL_MSG);
	static_assert(
			std::is_same_v<fea::output_iterator_vt<osb_t>, wchar_t>, FAIL_MSG);
}

} // namespace

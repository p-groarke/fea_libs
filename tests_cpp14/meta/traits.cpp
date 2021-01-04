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

TEST(traits, splice) {
	static_assert(std::is_same<fea::idx_splice_t<0, int, double, float, short>,
						  int>::value,
			"traits.cpp : test failed");
	static_assert(
			std::is_same<fea::idx_splice_before_t<0, int, double, float, short>,
					std::tuple<>>::value,
			"traits.cpp : test failed");
	static_assert(
			std::is_same<fea::idx_splice_after_t<0, int, double, float, short>,
					std::tuple<double, float, short>>::value,
			"traits.cpp : test failed");

	static_assert(std::is_same<fea::idx_splice_t<1, int, double, float, short>,
						  double>::value,
			"traits.cpp : test failed");
	static_assert(
			std::is_same<fea::idx_splice_before_t<1, int, double, float, short>,
					std::tuple<int>>::value,
			"traits.cpp : test failed");
	static_assert(
			std::is_same<fea::idx_splice_after_t<1, int, double, float, short>,
					std::tuple<float, short>>::value,
			"traits.cpp : test failed");

	static_assert(std::is_same<fea::idx_splice_t<2, int, double, float, short>,
						  float>::value,
			"traits.cpp : test failed");
	static_assert(
			std::is_same<fea::idx_splice_before_t<2, int, double, float, short>,
					std::tuple<int, double>>::value,
			"traits.cpp : test failed");
	static_assert(
			std::is_same<fea::idx_splice_after_t<2, int, double, float, short>,
					std::tuple<short>>::value,
			"traits.cpp : test failed");

	static_assert(std::is_same<fea::idx_splice_t<3, int, double, float, short>,
						  short>::value,
			"traits.cpp : test failed");
	static_assert(
			std::is_same<fea::idx_splice_before_t<3, int, double, float, short>,
					std::tuple<int, double, float>>::value,
			"traits.cpp : test failed");
	static_assert(
			std::is_same<fea::idx_splice_after_t<3, int, double, float, short>,
					std::tuple<>>::value,
			"traits.cpp : test failed");
}

struct obj {
	void func(int) {
	}
};

TEST(traits, member_func_ptr) {
	static_assert(std::is_class<obj>::value, "");

	using mem_fun = fea::member_func_ptr_t<void, obj*, int>;
	static_assert(std::is_same<mem_fun, decltype(&obj::func)>::value,
			"traits.cpp : test failed");

	using mem_fun2 = fea::member_func_ptr_t<void, int>;
	static_assert(
			std::is_same<mem_fun2, void*>::value, "traits.cpp : test failed");

	using mem_fun3 = fea::member_func_ptr_t<void, obj, int>;
	static_assert(
			std::is_same<mem_fun3, void*>::value, "traits.cpp : test failed");

	using mem_fun4 = fea::member_func_ptr_t<void, obj*, int, double, float>;
	static_assert(
			std::is_same<mem_fun4, void (obj::*)(int, double, float)>::value,
			"traits.cpp : test failed");

	using mem_fun5 = fea::member_func_ptr_t<void, int*>;
	static_assert(
			std::is_same<mem_fun5, void*>::value, "traits.cpp : test failed");
}

TEST(traits, enums) {
	enum class e {
		one,
		two,
		three,
		four,
		count,
	};

	fea::non_type_type_pack<e, e::one, e::two, e::three, e::four> p
			= fea::explode_enum<e>([](auto... cs) {
				  constexpr size_t idx = fea::non_type_pack_idx_v<e, e::three,
						  decltype(cs)::value...>;
				  static_assert(idx == 2, "traits.cpp : test failed");
				  return fea::non_type_type_pack<e, decltype(cs)::value...>{};
			  });

	static_assert(!fea::is_same_nt_v<e, e::one, e::two>,
			"type_map.cpp : test failed");
	static_assert(
			fea::is_same_nt_v<e, e::one, e::one>, "type_map.cpp : test failed");
}
} // namespace

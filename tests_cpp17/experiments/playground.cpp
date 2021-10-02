#include <gtest/gtest.h>

#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <string>
#include <tuple>

namespace {
#if 0
// Calls your func with tuple element.
template <class UserFunc, class Tuple, size_t N = 0>
void runtime_get(UserFunc&& func, Tuple& tup, size_t idx) {
	if (N == idx) {
		std::forward<UserFunc>(func)(std::get<N>(tup));
		return;
	}

	if constexpr (N + 1 < std::tuple_size_v<Tuple>) {
		return runtime_get<UserFunc, Tuple, N + 1>(
				std::forward<UserFunc>(func), tup, idx);
	}
}

void do_runtime_get() {
	std::tuple<int, double, std::string> my_tup = { 42, -42.0, "life_answer" };
	runtime_get([](auto& val) { std::cout << val << std::endl; }, my_tup, 2);
}

#elif 0

namespace detail {

// We need a static_for helper here.
// The static for accepts a compile time size, and calls your
// lambda with compile-time std::integral_constants so you can
// do fun things.
template <class Func, size_t... I>
constexpr void static_for(Func& func, std::index_sequence<I...>) {
	(func(std::integral_constant<size_t, I>{}), ...);
}

// Call a for loop at compile time.
// Your lambda is provided with an integral_constant.
template <size_t N, class Func>
constexpr void static_for(Func&& func) {
	static_for(func, std::make_index_sequence<N>{});
}

// Gets the tuple member offsetof at Idx.
template <size_t Idx, class... Args>
uintptr_t tuple_offset(std::tuple<Args...>& tup) {
	return uintptr_t((const char*)(&std::get<Idx>(tup)) - (const char*)&tup);
}

// Creates an array of offsets for each member in tuple.
// Array index matches std::get position in tuple.
template <class... Args>
auto make_offset_lookup(std::tuple<Args...>& tup) {
	constexpr size_t arr_size = sizeof...(Args);
	std::array<uintptr_t, arr_size> ret{};
	static_for<arr_size>([&](auto idx) { ret[idx] = tuple_offset<idx>(tup); });
	return ret;
}

} // namespace detail

// Get an item in a tuple at runtime.
template <class... Args>
void* runtime_get(size_t idx, std::tuple<Args...>& tup) {
	static const std::array<uintptr_t, sizeof...(Args)> lookup
			= detail::make_offset_lookup(tup);
	return (void*)((char*)&tup + lookup[idx]);
}

void do_runtime_get() {
	std::tuple<int, double, std::string> my_tup = { 42, -42.0, "life_answer" };
	std::string* val = reinterpret_cast<std::string*>(runtime_get(2, my_tup));
	std::cout << *val << std::endl;
}

#elif 0

struct my_class {
	// Calls your lambda with value at index.
	template <class UserFunc>
	void runtime_get(UserFunc&& func, size_t index) {
		switch (index) {
		case 0: {
			std::forward<UserFunc>(func)(std::get<0>(my_tup));
		} break;
		case 1: {
			std::forward<UserFunc>(func)(std::get<1>(my_tup));
		} break;
		case 2: {
			std::forward<UserFunc>(func)(std::get<2>(my_tup));
		} break;
		default: {
			assert(false);
		} break;
		}
	}

private:
	std::tuple<int, double, std::string> my_tup = { 42, -42.0, "life_answer" };
};

void do_runtime_get() {
	my_class potato;
	potato.runtime_get([](auto& val) { std::cout << val << std::endl; }, 2);
}

#elif 0

struct my_class {
	// Calls your lambda with value at index.
	template <class UserFunc>
	void runtime_get(UserFunc&& func, size_t index) {
		// MY_TUP_SIZE is used to add switch cases "dynamically", and verify we
		// don't have a mismatch when comparing to the actual tuple size.
#define MY_TUP_SIZE 3

		// Makes sure you don't forget to increase MY_TUP_SIZE.
		static_assert(MY_TUP_SIZE == std::tuple_size_v<decltype(my_tup)>,
				"This will fail compilation if the switch case is missing "
				"statements.");

		// Generates a macro to get tuple items up to MY_TUP_SIZE.
		// In practice, you'd want many more switch-cases.
		switch (index) {
#define FEA_CASE(n) \
	case (n): { \
		std::forward<UserFunc>(func)(std::get<n>(my_tup)); \
	} break;

#if MY_TUP_SIZE > 0
			FEA_CASE(0);
#endif
#if MY_TUP_SIZE > 1
			FEA_CASE(1);
#endif
#if MY_TUP_SIZE > 2
			FEA_CASE(2);
#endif
#if MY_TUP_SIZE > 3
			FEA_CASE(3);
#endif
#if MY_TUP_SIZE > 4
			FEA_CASE(4);
#endif
#if MY_TUP_SIZE > 5
			FEA_CASE(5);
#endif

		default: {
			assert(false);
		} break;
		}

#undef FEA_CASE
#undef MY_TUP_SIZE
	}

private:
	std::tuple<int, double, std::string> my_tup = { 42, -42.0, "life_answer" };
};

void do_runtime_get() {
	my_class potato;
	potato.runtime_get([](auto& val) { std::cout << val << std::endl; }, 2);
}

#elif 1

namespace detail {
// We need a static_for helper here.
// The static for accepts a compile time size, and calls your
// lambda with compile-time std::integral_constants so you can
// do fun things.
template <class Func, size_t... I>
constexpr void static_for(Func& func, std::index_sequence<I...>) {
	(func(std::integral_constant<size_t, I>{}), ...);
}

// Call a for loop at compile time.
// Your lambda is provided with an integral_constant.
template <size_t N, class Func>
constexpr void static_for(Func&& func) {
	static_for(func, std::make_index_sequence<N>{});
}

template <size_t Idx, class Func, class TupleRef>
void unerase(Func& func, TupleRef tup) {
	// Using the encoded index of our specialization, get the appropriate tuple
	// element and pass it on to the user provided lambda.
	func(std::get<Idx>(tup));
}

template <class Func, class TupleRef>
constexpr auto make_lookup() {
	// Our unerase function signature.
	// It accepts a lambda reference and a tuple reference.
	using unerase_t = void (*)(Func&, TupleRef);

	// The size of the user tuple.
	constexpr size_t tup_size = std::tuple_size_v<std::decay_t<TupleRef>>;

	// The lookup array used to call the appropriate unerase specialization.
	// The function pointers are stored at their associated tuple element index.
	std::array<unerase_t, tup_size> ret{};

	// Assign the unerase function pointers at their indexes.
	// The function is specialized according to the lookup index, the user
	// lambda type and the user tuple reference.
	static_for<tup_size>(
			[&](auto idx) { ret[idx] = &unerase<idx, Func, TupleRef>; });

	return ret;
}
} // namespace detail


// Get a tuple value at runtime, located at idx.
// Provide a generic lambda which accepts const auto& to recieve the value.
template <class Func, class... Args>
void runtime_get(Func&& func, const std::tuple<Args...>& tup, size_t idx) {
	// The tuple reference (const in this case, you'll want a non-const
	// version).
	using tup_ref_t = const std::tuple<Args...>&;

	// Creates a lookup array at compile time, so we don't pay that price.
	static constexpr auto lookup = detail::make_lookup<Func, tup_ref_t>();

	// At the given index, call the stored pointer with the user lambda and
	// tuple.
	return lookup[idx](func, tup);
}

void do_runtime_get() {
	std::tuple<int, double, std::string> my_tup = { 42, -42.0, "life_answer" };
	runtime_get([](auto& val) { std::cout << val << std::endl; }, my_tup, 2);
}
#endif

TEST(blog, runtime_loop) {
	do_runtime_get();
}
} // namespace

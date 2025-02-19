#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <fea/meta/static_for.hpp>
#include <fea/meta/tuple.hpp>
#include <fea/string/string.hpp>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <iostream>
#include <regex>
#include <span>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>

namespace {

#if 0
// Some classes to deserialize.
struct vec3 {
	vec3(const std::tuple<float, float, float>& input)
			: x(std::get<0>(input))
			, y(std::get<1>(input))
			, z(std::get<2>(input)) {
	}

	float x = 0.f;
	float y = 0.f;
	float z = 0.f;
};

struct ivec3 {
	ivec3(const std::tuple<int, int, int>& input)
			: x(std::get<0>(input))
			, y(std::get<1>(input))
			, z(std::get<2>(input)) {
	}

	int x = 0;
	int y = 0;
	int z = 0;
};

template <template <class...> class ReturnContainer, class TargetObj,
		class... Args>
struct deserializer {
	static_assert(std::is_trivially_copyable_v<TargetObj>,
			"deserializer : cannot deserialize non trivially-copyable object");

	/* Something magical, unicorns, etc. */

	ReturnContainer<std::tuple<Args...>> deserialize(
			std::span<const char>) const {
		// Things I shall not share with you, as to prevent Punning Triggered
		// Stress Disorder ;)
	}
};

// Our new templated class.
template <class Descriptor>
struct potato {
	/* A-lot of static_asserts for the descriptor. */
};

// In a header far-far-away.
struct VectorPotatoDescription {
	using container = std::vector<float>;
	[[maybe_unused]] static constexpr bool stack_optim = true;
	[[maybe_unused]] static constexpr size_t num_elements = 10;
	using some_var_args = std::tuple<int, int, float>;
	static inline const std::string name = "vector_potato";
};

// Later on...
[[maybe_unused]] potato<VectorPotatoDescription> p;
#endif


// We use this concept to choose our base class.
// One has a const conversion operator, the other doesn't.
template <class T>
concept has_const_paren = requires(const std::decay_t<T> t) { t.operator()(); };

template <class>
struct conversion_op;

template <class T>
	requires(has_const_paren<T>)
struct conversion_op<T> : T {
	using ret_t = decltype(std::declval<T>()());

	// Also handle noexcept lambdas, which allows us to customize
	// noexcept according to the return value.
	// More on this later.
	operator ret_t() const noexcept(noexcept(std::declval<T>()())) {
		return T::operator()();
	}
};

template <class T>
	requires(!has_const_paren<T>)
struct conversion_op<T> : T {
	using ret_t = decltype(std::declval<T>()());

	// Also handle noexcept.
	operator ret_t() noexcept(noexcept(std::declval<T>()())) {
		return T::operator()();
	}
};

// Inherits base classes which transform the lambdas' operator() into
// conversion operators instead.
template <class... Ts>
struct return_overload : conversion_op<Ts>... {

	// We expose the conversion operators of our base classes.
	// As long as they are not ambiguous, this is OK.
	using conversion_op<Ts>::operator typename conversion_op<Ts>::ret_t...;
};

// Deduction guides.
template <class... Ts>
return_overload(Ts...) -> return_overload<Ts...>;


auto my_func() {
	return return_overload{
		[]() -> int { return 42; },
		[]() -> float { return 0.f; },
		[]() -> std::string { return "string"; },
	};
}

// A "maybe throwing" function.
// The caller chooses which behavior they prefer by
// capturing a std::error_code or not.
auto my_func2() {
	return return_overload{
		[]() noexcept -> std::tuple<std::error_code, int> {
			// noexcept implementation...
			std::cout << "noexcept imp" << std::endl;
		},
		[]() -> int {
			// throwing implementation...
			std::cout << "throwing imp" << std::endl;
		},
	};
}

TEST(playground, cpp20) {
	int test_int = my_func();
	float test_float = my_func();
	std::string test_str = my_func();

	std::cout << test_int << std::endl;
	std::cout << test_float << std::endl;
	std::cout << test_str << std::endl;

	// auto [ec, val] = my_func();
	// auto val2 = my_func();
}


} // namespace

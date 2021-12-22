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
			std::span<const char> mem) const {
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
	static constexpr bool stack_optim = true;
	static constexpr size_t num_elements = 10;
	using some_var_args = std::tuple<int, int, float>;
	static inline const std::string name = "vector_potato";
};

// Later on...
potato<VectorPotatoDescription> p;

TEST(playground, cpp20) {
	// deserializer<std::vector, vec3, float, float, float> vec3_deserializer;
	// deserializer<std::vector, ivec3, int, int, int> ivec3_deserializer;
}


} // namespace

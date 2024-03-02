#include <fea/performance/constants.hpp>
#include <gtest/gtest.h>
#include <type_traits>

#if FEA_WITH_TBB
namespace fea {
template <>
struct default_grainsize_small<true> {
	static constexpr size_t value = 2u;
};

template <>
struct default_grainsize_medium<true> {
	static constexpr size_t value = 42u;
};

template <>
struct default_grainsize_large<true> {
	static constexpr size_t value = 420u;
};


template <>
struct default_partitioner<true> {
	using type = int;
};
} // namespace fea

namespace {
#define ERROR_MSG "constants2.cpp : Unit test failed."

TEST(constants, basics) {
	static_assert(fea::default_grainsize_small<true>::value == 2u, ERROR_MSG);
	static_assert(fea::default_grainsize_small_v<true> == 2u, ERROR_MSG);

	static_assert(fea::default_grainsize_medium<true>::value == 42u, ERROR_MSG);
	static_assert(fea::default_grainsize_medium_v<true> == 42u, ERROR_MSG);

	static_assert(fea::default_grainsize_large<true>::value == 420u, ERROR_MSG);
	static_assert(fea::default_grainsize_large_v<true> == 420u, ERROR_MSG);

	using default_partitioner_t = typename fea::default_partitioner<true>::type;
	static_assert(std::is_same<default_partitioner_t, int>::value, ERROR_MSG);
	static_assert(std::is_same<fea::default_partitioner_t<true>, int>::value,
			ERROR_MSG);
}
} // namespace
#endif

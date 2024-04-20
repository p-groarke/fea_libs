#include <fea/performance/constants.hpp>
#include <gtest/gtest.h>
#include <type_traits>

#if FEA_WITH_TBB
namespace {
#define TEST_FAIL_MSG "constants.cpp : Unit test failed."

TEST(constants, basics) {
	static_assert(
			fea::default_grainsize_small<true>::value == 1u, TEST_FAIL_MSG);
	static_assert(fea::default_grainsize_small_v<true> == 1u, TEST_FAIL_MSG);

	static_assert(
			fea::default_grainsize_medium<true>::value == 100u, TEST_FAIL_MSG);
	static_assert(fea::default_grainsize_medium_v<true> == 100u, TEST_FAIL_MSG);

	static_assert(
			fea::default_grainsize_large<true>::value == 1000u, TEST_FAIL_MSG);
	static_assert(fea::default_grainsize_large_v<true> == 1000u, TEST_FAIL_MSG);

	using default_partitioner_t = typename fea::default_partitioner<true>::type;
	static_assert(
			std::is_same<default_partitioner_t, tbb::static_partitioner>::value,
			TEST_FAIL_MSG);
	static_assert(std::is_same<fea::default_partitioner_t<true>,
						  tbb::static_partitioner>::value,
			TEST_FAIL_MSG);
}
} // namespace
#endif

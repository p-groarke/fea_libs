#include <fea/performance/simd_version.hpp>
#include <fea/utils/platform.hpp>
#include <gtest/gtest.h>

namespace {
TEST(simd_version, basics) {
	fea::simd_ver v = fea::simd_runtime_version();
	EXPECT_NE(v, fea::simd_ver::count);

#if FEA_64BIT
	// 64 bit x86 minimum is sse2
	EXPECT_GE(v, fea::simd_ver::sse2);
#endif
}
} // namespace
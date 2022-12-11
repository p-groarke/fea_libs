#include <cstdio>
#include <fea/performance/simd.hpp>
#include <gtest/gtest.h>

namespace {
template <fea::simd_ver Ver>
void test1(fea::simd_api<Ver>) {
	printf("%zu", size_t(Ver));

	using api_t = fea::simd_api<Ver>;
	using xmm_i32_t = typename fea::simd_api<Ver>::xmm_i32_t;
	using xmm_f32_t = typename fea::simd_api<Ver>::xmm_f32_t;

	xmm_i32_t xmm1, xmm2;
	fea::simd_api<Ver>::set1(41, xmm1);
	fea::simd_api<Ver>::set1(1, xmm2);
	xmm_i32_t ans = fea::simd_api<Ver>::add(xmm1, xmm2);
	alignas(16) std::array<int, 4> out{};
	fea::simd_api<Ver>::store(ans, out.data());

	{
		const std::array<int, 4> expected{ 42, 42, 42, 42 };
		EXPECT_EQ(out, expected);
	}
}

TEST(simd, simd_run) {
	// fea::simd_run<fea::simd_ver::sse2>([](auto api) { test1(api); }, []()
	// {});

	// fea::simd_api<fea::simd_ver::sse2> test;
}
} // namespace

#include <gtest/gtest.h>

// Undefine everything to make sure all is good.
#include <fea/utils/platform.hpp>
#undef FEA_MMX
#undef FEA_SSE
#undef FEA_SSE2
#undef FEA_SSE3
#undef FEA_SSSE3
#undef FEA_SSE41
#undef FEA_SSE42
#undef FEA_AVX
#undef FEA_AVX2
#undef FEA_AVX512F
#include <fea/simd/simd_register.hpp>

namespace {
#define fail_msg "simd_register : Unit test failed."

template <class RT, size_t RSize, class T, class B>
constexpr void do_asserts() {
	static_assert(RT::register_size_v == RSize, fail_msg);
	static_assert(RT::type_size_v == sizeof(T) * 8, fail_msg);
	static_assert(std::is_same<typename RT::value_type, T>::value, fail_msg);

	constexpr size_t byte_size = RSize / 8;
	using xmm_t = decltype(RT::xmm);
	static_assert(alignof(RT) == byte_size, fail_msg);
	static_assert(sizeof(xmm_t) == byte_size, fail_msg);
}

template <class RT, class T, class B = T>
constexpr void do_mmx_asserts() {
	do_asserts<RT, 64, T, B>();

	using xmm_t = decltype(RT::xmm);
	using arr_t = std::array<B, 8 / sizeof(B)>;

#if FEA_MMX
	static_assert(std::is_same<xmm_t, __m64>::value, fail_msg);
#else
	static_assert(std::is_same<xmm_t, arr_t>::value, fail_msg);
#endif
}

template <class RT, class T, class B = T>
constexpr void do_sse_asserts() {
	do_asserts<RT, 128, T, B>();

	constexpr size_t size = 16;
	using xmm_t = decltype(RT::xmm);
	using arr_t = std::array<B, size / sizeof(B)>;

#if FEA_SSE
	static_assert(std::is_same<xmm_t, __m128>::value, fail_msg);
#else
	static_assert(std::is_same<xmm_t, arr_t>::value, fail_msg);
#endif
}

template <class RT, class T, class B = T>
constexpr void do_sse2_asserts() {
	do_asserts<RT, 128, T, B>();

	constexpr size_t size = 16;
	using xmm_t = decltype(RT::xmm);
	using arr_t = std::array<B, size / sizeof(B)>;
	using expected_xmm_t = typename std::conditional<
			std::is_floating_point<T>::value,
			typename std::conditional<sizeof(T) <= 4, __m128, __m128d>::type,
			__m128i>::type;

#if FEA_SSE2
	static_assert(std::is_same<xmm_t, expected_xmm_t>::value, fail_msg);
#else
	static_assert(std::is_same<xmm_t, arr_t>::value, fail_msg);
#endif
}

template <class RT, class T, class B = T>
constexpr void do_avx_asserts() {
	do_asserts<RT, 256, T, B>();

	constexpr size_t size = 32;
	using xmm_t = decltype(RT::xmm);
	using arr_t = std::array<B, size / sizeof(B)>;
	using expected_xmm_t = typename std::conditional<
			std::is_floating_point<T>::value,
			typename std::conditional<sizeof(T) <= 4, __m256, __m256d>::type,
			__m256i>::type;

#if FEA_SSE2
	static_assert(std::is_same<xmm_t, expected_xmm_t>::value, fail_msg);
#else
	static_assert(std::is_same<xmm_t, arr_t>::value, fail_msg);
#endif
}

template <class RT, class T, class B = T>
constexpr void do_avx512f_asserts() {
	do_asserts<RT, 512, T, B>();

	constexpr size_t size = 64;
	using xmm_t = decltype(RT::xmm);
	using arr_t = std::array<B, size / sizeof(B)>;
	using expected_xmm_t = typename std::conditional<
			std::is_floating_point<T>::value,
			typename std::conditional<sizeof(T) <= 4, __m512, __m512d>::type,
			__m512i>::type;

#if FEA_SSE2
	static_assert(std::is_same<xmm_t, expected_xmm_t>::value, fail_msg);
#else
	static_assert(std::is_same<xmm_t, arr_t>::value, fail_msg);
#endif
}

TEST(simd_register, unavailable) {
	do_mmx_asserts<fea::m64_f32_t, float>();
	do_mmx_asserts<fea::m64_f64_t, double>();
	do_mmx_asserts<fea::m64_i8_t, char>();
	do_mmx_asserts<fea::m64_u8_t, unsigned char, char>();
	do_mmx_asserts<fea::m64_ii8_t, signed char, char>();
	do_mmx_asserts<fea::m64_i16_t, short>();
	do_mmx_asserts<fea::m64_u16_t, unsigned short, short>();
	do_mmx_asserts<fea::m64_i32_t, int>();
	do_mmx_asserts<fea::m64_u32_t, unsigned int, int>();
	do_mmx_asserts<fea::m64_i64_t, long long>();
	do_mmx_asserts<fea::m64_u64_t, unsigned long long, long long>();

	do_sse_asserts<fea::m128_f32_t, float>();
	do_sse2_asserts<fea::m128_f64_t, double>();
	do_sse2_asserts<fea::m128_i8_t, char>();
	do_sse2_asserts<fea::m128_u8_t, unsigned char, char>();
	do_sse2_asserts<fea::m128_ii8_t, signed char, char>();
	do_sse2_asserts<fea::m128_i16_t, short>();
	do_sse2_asserts<fea::m128_u16_t, unsigned short, short>();
	do_sse2_asserts<fea::m128_i32_t, int>();
	do_sse2_asserts<fea::m128_u32_t, unsigned int, int>();
	do_sse2_asserts<fea::m128_i64_t, long long>();
	do_sse2_asserts<fea::m128_u64_t, unsigned long long, long long>();

	do_avx_asserts<fea::m256_f32_t, float>();
	do_avx_asserts<fea::m256_f64_t, double>();
	do_avx_asserts<fea::m256_i8_t, char>();
	do_avx_asserts<fea::m256_u8_t, unsigned char, char>();
	do_avx_asserts<fea::m256_ii8_t, signed char, char>();
	do_avx_asserts<fea::m256_i16_t, short>();
	do_avx_asserts<fea::m256_u16_t, unsigned short, short>();
	do_avx_asserts<fea::m256_i32_t, int>();
	do_avx_asserts<fea::m256_u32_t, unsigned int, int>();
	do_avx_asserts<fea::m256_i64_t, long long>();
	do_avx_asserts<fea::m256_u64_t, unsigned long long, long long>();

	do_avx512f_asserts<fea::m512_f32_t, float>();
	do_avx512f_asserts<fea::m512_f64_t, double>();
	do_avx512f_asserts<fea::m512_i8_t, char>();
	do_avx512f_asserts<fea::m512_u8_t, unsigned char, char>();
	do_avx512f_asserts<fea::m512_ii8_t, signed char, char>();
	do_avx512f_asserts<fea::m512_i16_t, short>();
	do_avx512f_asserts<fea::m512_u16_t, unsigned short, short>();
	do_avx512f_asserts<fea::m512_i32_t, int>();
	do_avx512f_asserts<fea::m512_u32_t, unsigned int, int>();
	do_avx512f_asserts<fea::m512_i64_t, long long>();
	do_avx512f_asserts<fea::m512_u64_t, unsigned long long, long long>();
}
} // namespace
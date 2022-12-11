#include <fea/performance/simd_register_traits.hpp>
#include <gtest/gtest.h>
#include <type_traits>

namespace {
#define FAIL_MSG "simd_register_traits.cpp : Failed unit-test."

TEST(simd, register_traits) {
	// clang-format off

	// mmx
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::mmx, float>, fea::m64_f32_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::mmx, double>, fea::m64_f64_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::mmx, char>, fea::m64_i8_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::mmx, unsigned char>, fea::m64_u8_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::mmx, signed char>, fea::m64_ii8_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::mmx, short>, fea::m64_i16_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::mmx, unsigned short>, fea::m64_u16_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::mmx, int>, fea::m64_i32_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::mmx, unsigned int>, fea::m64_u32_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::mmx, long long>, fea::m64_i64_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::mmx, unsigned long long>, fea::m64_u64_t>::value, FAIL_MSG);

	// sse & sse2
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse, float>, fea::m128_f32_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse2, double>, fea::m128_f64_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse2, char>, fea::m128_i8_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse2, unsigned char>, fea::m128_u8_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse2, signed char>, fea::m128_ii8_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse2, short>, fea::m128_i16_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse2, unsigned short>, fea::m128_u16_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse2, int>, fea::m128_i32_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse2, unsigned int>, fea::m128_u32_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse2, long long>, fea::m128_i64_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse2, unsigned long long>, fea::m128_u64_t>::value, FAIL_MSG);

	// sse3
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse3, float>, fea::m128_f32_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse3, double>, fea::m128_f64_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse3, char>, fea::m128_i8_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse3, unsigned char>, fea::m128_u8_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse3, signed char>, fea::m128_ii8_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse3, short>, fea::m128_i16_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse3, unsigned short>, fea::m128_u16_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse3, int>, fea::m128_i32_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse3, unsigned int>, fea::m128_u32_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse3, long long>, fea::m128_i64_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse3, unsigned long long>, fea::m128_u64_t>::value, FAIL_MSG);

	// ssse3
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::ssse3, float>, fea::m128_f32_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::ssse3, double>, fea::m128_f64_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::ssse3, char>, fea::m128_i8_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::ssse3, unsigned char>, fea::m128_u8_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::ssse3, signed char>, fea::m128_ii8_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::ssse3, short>, fea::m128_i16_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::ssse3, unsigned short>, fea::m128_u16_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::ssse3, int>, fea::m128_i32_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::ssse3, unsigned int>, fea::m128_u32_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::ssse3, long long>, fea::m128_i64_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::ssse3, unsigned long long>, fea::m128_u64_t>::value, FAIL_MSG);

	// sse41
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse41, float>, fea::m128_f32_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse41, double>, fea::m128_f64_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse41, char>, fea::m128_i8_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse41, unsigned char>, fea::m128_u8_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse41, signed char>, fea::m128_ii8_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse41, short>, fea::m128_i16_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse41, unsigned short>, fea::m128_u16_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse41, int>, fea::m128_i32_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse41, unsigned int>, fea::m128_u32_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse41, long long>, fea::m128_i64_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse41, unsigned long long>, fea::m128_u64_t>::value, FAIL_MSG);

	// sse42
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse42, float>, fea::m128_f32_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse42, double>, fea::m128_f64_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse42, char>, fea::m128_i8_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse42, unsigned char>, fea::m128_u8_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse42, signed char>, fea::m128_ii8_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse42, short>, fea::m128_i16_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse42, unsigned short>, fea::m128_u16_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse42, int>, fea::m128_i32_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse42, unsigned int>, fea::m128_u32_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse42, long long>, fea::m128_i64_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::sse42, unsigned long long>, fea::m128_u64_t>::value, FAIL_MSG);

	// avx
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::avx, float>, fea::m256_f32_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::avx, double>, fea::m256_f64_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::avx, char>, fea::m256_i8_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::avx, unsigned char>, fea::m256_u8_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::avx, signed char>, fea::m256_ii8_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::avx, short>, fea::m256_i16_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::avx, unsigned short>, fea::m256_u16_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::avx, int>, fea::m256_i32_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::avx, unsigned int>, fea::m256_u32_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::avx, long long>, fea::m256_i64_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::avx, unsigned long long>, fea::m256_u64_t>::value, FAIL_MSG);

	// avx2
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::avx2, float>, fea::m256_f32_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::avx2, double>, fea::m256_f64_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::avx2, char>, fea::m256_i8_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::avx2, unsigned char>, fea::m256_u8_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::avx2, signed char>, fea::m256_ii8_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::avx2, short>, fea::m256_i16_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::avx2, unsigned short>, fea::m256_u16_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::avx2, int>, fea::m256_i32_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::avx2, unsigned int>, fea::m256_u32_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::avx2, long long>, fea::m256_i64_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::avx2, unsigned long long>, fea::m256_u64_t>::value, FAIL_MSG);

	// avx512f
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::avx512f, float>, fea::m512_f32_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::avx512f, double>, fea::m512_f64_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::avx512f, char>, fea::m512_i8_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::avx512f, unsigned char>, fea::m512_u8_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::avx512f, signed char>, fea::m512_ii8_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::avx512f, short>, fea::m512_i16_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::avx512f, unsigned short>, fea::m512_u16_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::avx512f, int>, fea::m512_i32_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::avx512f, unsigned int>, fea::m512_u32_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::avx512f, long long>, fea::m512_i64_t>::value, FAIL_MSG);
	static_assert(std::is_same<fea::simd_abi_register_t<fea::simd_ver::avx512f, unsigned long long>, fea::m512_u64_t>::value, FAIL_MSG);


	// clang-format on
}
} // namespace
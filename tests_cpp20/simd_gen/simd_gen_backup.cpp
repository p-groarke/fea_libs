// #include "../../build/bin/simd_api_mmx.hpp"
// #include "../../build/bin/simd_api_sse.hpp"
// #include "../../build/bin/simd_api_sse2.hpp"

#include <algorithm>
#include <array>
#include <cstdio>
#include <fea/containers/stack_vector.hpp>
#include <fea/enum/enum_array.hpp>
#include <fea/maps/unsigned_map.hpp>
#include <fea/simd/simd.hpp>
#include <fea/simd/simd_register.hpp>
#include <fea/simd/simd_register_traits.hpp>
#include <fea/simd/simd_version.hpp>
#include <fea/string/string.hpp>
#include <fea/utils/file.hpp>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <pugixml.hpp>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

extern const char* argv0;
namespace {

// Parses the intel intrinsics guide to generate the api header.
// Expects dataset (offline download) to be in bin folder, named :
// 'Intel Intrinsics Guide/'
#if 0
const std::string file_header = R"xx(/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2022, Philippe Groarke
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 **/
#pragma once
{prev_include}
#include "fea/performance/simd_register.hpp"
#include "fea/performance/simd_register_traits.hpp"
#include "fea/performance/simd_version.hpp"
#include "fea/utils/platform.hpp"

#include <immintrin.h>

/*
SIMD intrinsics, sorted and grouped by abi support.

All APIs and information come from :
https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html
*/

namespace fea {
{api_fwd}
)xx";

const std::string file_footer = R"xx(

using simd_{api_name}_api = simd_api<{simd_api_ver}>;

} // namespace fea
)xx";

const std::string api_base = R"xx(
template <fea::simd_ver>
struct simd_api {
	// These are here to help auto-complete.

	using xmm_f32_t = simd_abi_register_t<fea::simd_ver::count, float>;
	using xmm_f64_t = simd_abi_register_t<fea::simd_ver::count, double>;
	using xmm_i8_t = simd_abi_register_t<fea::simd_ver::count, char>;
	using xmm_u8_t = simd_abi_register_t<fea::simd_ver::count, unsigned char>;
	using xmm_ii8_t = simd_abi_register_t<fea::simd_ver::count, signed char>;
	using xmm_i16_t = simd_abi_register_t<fea::simd_ver::count, short>;
	using xmm_u16_t = simd_abi_register_t<fea::simd_ver::count, unsigned short>;
	using xmm_i32_t = simd_abi_register_t<fea::simd_ver::count, int>;
	using xmm_u32_t = simd_abi_register_t<fea::simd_ver::count, unsigned int>;
	using xmm_i64_t = simd_abi_register_t<fea::simd_ver::count, long long>;
	using xmm_u64_t = simd_abi_register_t<fea::simd_ver::count, unsigned long long>;
};
)xx";

const std::string api_fwd = R"xx(
template <fea::simd_ver>
struct simd_api;
)xx";


// disabled for debugging
// const std::string file_prev_include
//		= "#include \"fea/performance/{prev_filename}\"";

const std::string simd_prev_include = "#include \"{prev_filename}\"";

const std::string api_struct_inheritance = R"xx(
		: public simd_api<{simd_api_prev_ver}>
)xx";

const std::string api_struct_template_header = R"xx(
template <>
struct simd_api<{simd_api_ver}> {inheritance}{
// Default register types for this api.
// Previous registers and functionality are still available to you.
using xmm_f32_t = simd_abi_register_t<{simd_api_ver}, float>;
using xmm_f64_t = simd_abi_register_t<{simd_api_ver}, double>;
using xmm_i8_t = simd_abi_register_t<{simd_api_ver}, char>;
using xmm_u8_t = simd_abi_register_t<{simd_api_ver}, unsigned char>;
using xmm_ii8_t = simd_abi_register_t<{simd_api_ver}, signed char>;
using xmm_i16_t = simd_abi_register_t<{simd_api_ver}, short>;
using xmm_u16_t = simd_abi_register_t<{simd_api_ver}, unsigned short>;
using xmm_i32_t = simd_abi_register_t<{simd_api_ver}, int>;
using xmm_u32_t = simd_abi_register_t<{simd_api_ver}, unsigned int>;
using xmm_i64_t = simd_abi_register_t<{simd_api_ver}, long long>;
using xmm_u64_t = simd_abi_register_t<{simd_api_ver}, unsigned long long>;

)xx";

const std::string api_struct_template_footer = R"xx(
};
)xx";


//// disabled for debugging
// const std::string file_function_template = R"xx(
///*
//{description}
//{instruction}
//{operation}
//*/
// FEA_FORCEINLINE static {return_type} {feafuncname}({signature_params}) {
//	{maybe_return}{funcname}({passed_params});
//}
//)xx";

const std::string file_function_template = R"xx(
{template}FEA_FORCEINLINE static {return_type} {feafuncname}({signature_params}) {
	{maybe_return}{funcname}({passed_params});
}
)xx";

// Version to enum string
const fea::enum_array<std::string, fea::simd_ver> ver_to_fea_enum_str{
	"fea::simd_ver::mmx",
	"fea::simd_ver::sse",
	"fea::simd_ver::sse2",
	"fea::simd_ver::sse3",
	"fea::simd_ver::ssse3",
	"fea::simd_ver::sse41",
	"fea::simd_ver::sse42",
	"fea::simd_ver::avx",
	"fea::simd_ver::avx2",
	"fea::simd_ver::avx512f",
};

const fea::enum_array<std::string, fea::simd_ver> ver_to_filename{
	"simd_api_mmx.hpp",
	"simd_api_sse.hpp",
	"simd_api_sse2.hpp",
	"simd_api_sse3.hpp",
	"simd_api_ssse3.hpp",
	"simd_api_sse41.hpp",
	"simd_api_sse42.hpp",
	"simd_api_avx.hpp",
	"simd_api_avx2.hpp",
	"simd_api_avx512f.hpp",
};

const fea::enum_array<std::string, fea::simd_ver> ver_to_api_name{
	"mmx",
	"sse",
	"sse2",
	"sse3",
	"ssse3",
	"sse41",
	"sse42",
	"avx",
	"avx2",
	"avx512f",
};

const std::unordered_map<std::string, fea::simd_ver> supported_cpuids{
	{ "MMX", fea::simd_ver::mmx }, { "SSE", fea::simd_ver::sse },
	{ "SSE2", fea::simd_ver::sse2 },
	// debugging
	//{ "SSE3", fea::simd_ver::sse3 },
	//{ "SSSE3", fea::simd_ver::ssse3 },
	//{ "SSE4.1", fea::simd_ver::sse41 },
	//{ "SSE4.2", fea::simd_ver::sse42 },
	//{ "AVX", fea::simd_ver::avx },
	//{ "AVX2", fea::simd_ver::avx2 },
	//{ "AVX512F", fea::simd_ver::avx512f },
};

enum class etype : uint8_t {
	i8,
	i16,
	i32,
	i64,
	ui8,
	ui16,
	ui32,
	ui64,
	si8,
	si16,
	si32,
	si64,
	fp16,
	fp32,
	fp64,
	m64,
	m128,
	m256,
	m512,
	imm,
	mask,
	mvoid,
	mvoid_ptr,
	mvoid_const_ptr,
	count,
};

const std::unordered_map<std::string, etype> str_to_etype{
	{ "I8", etype::i8 },
	{ "I16", etype::i16 },
	{ "I32", etype::i32 },
	{ "I64", etype::i64 },
	{ "UI8", etype::ui8 },
	{ "UI16", etype::ui16 },
	{ "UI32", etype::ui32 },
	{ "UI64", etype::ui64 },
	{ "SI8", etype::si8 },
	{ "SI16", etype::si16 },
	{ "SI32", etype::si32 },
	{ "SI64", etype::si64 },
	{ "FP16", etype::fp16 },
	{ "FP32", etype::fp32 },
	{ "FP64", etype::fp64 },
	{ "M64", etype::m64 },
	{ "M128", etype::m128 },
	{ "M256", etype::m256 },
	{ "M512", etype::m512 },
	{ "IMM", etype::imm },
	{ "MASK", etype::mask },
	{ "void", etype::mvoid },
	{ "void*", etype::mvoid_ptr },
	{ "void *", etype::mvoid_ptr },
	{ "void const*", etype::mvoid_const_ptr },
	{ "void const *", etype::mvoid_const_ptr },
};

// Direct conversions.
// Types that don't need overload generation have a second string.
// All types are added here to make sure we don't miss anything.
const std::unordered_map<std::string, std::string> direct_type_conversions{
	{ "char", "char" },
	{ "short", "short" },
	{ "int", "int" },
	{ "long long", "long long" },
	{ "float", "float" },
	{ "double", "double" },
	{ "unsigned char", "unsigned char" },
	{ "unsigned short", "unsigned short" },
	{ "unsigned int", "unsigned int" },
	{ "unsigned long long", "unsigned long long" },

	{ "char*", "char*" },
	{ "short*", "short*" },
	{ "int*", "int*" },
	{ "long long*", "long long*" },
	{ "float*", "float*" },
	{ "double*", "double*" },
	{ "unsigned char*", "unsigned char*" },
	{ "unsigned short*", "unsigned short*" },
	{ "unsigned int*", "unsigned int*" },
	{ "unsigned long long*", "unsigned long long*" },

	{ "char *", "char*" },
	{ "short *", "short*" },
	{ "int *", "int*" },
	{ "long long *", "long long*" },
	{ "float *", "float*" },
	{ "double *", "double*" },
	{ "unsigned char *", "unsigned char*" },
	{ "unsigned short *", "unsigned short*" },
	{ "unsigned int *", "unsigned int*" },
	{ "unsigned long long *", "unsigned long long*" },

	{ "char const*", "char const*" },
	{ "short const*", "short const*" },
	{ "int const*", "int const*" },
	{ "long long const*", "long long const*" },
	{ "float const*", "float const*" },
	{ "double const*", "double const*" },
	{ "unsigned char const*", "unsigned char const*" },
	{ "unsigned short const*", "unsigned short const*" },
	{ "unsigned int const*", "unsigned int const*" },
	{ "unsigned long long const*", "unsigned long long const*" },

	{ "char const *", "char const*" },
	{ "short const *", "short const*" },
	{ "int const *", "int const*" },
	{ "long long const *", "long long const*" },
	{ "float const *", "float const*" },
	{ "double const *", "double const*" },
	{ "unsigned char const *", "unsigned char const*" },
	{ "unsigned short const *", "unsigned short const*" },
	{ "unsigned int const *", "unsigned int const*" },
	{ "unsigned long long const *", "unsigned long long const*" },

	{ "const char", "const char" },
	{ "const short", "const short" },
	{ "const int", "const int" },
	{ "const long long", "const long long" },
	{ "const float", "const float" },
	{ "const double", "const double" },
	{ "const unsigned char", "const unsigned char" },
	{ "const unsigned short", "const unsigned short" },
	{ "const unsigned int", "const unsigned int" },
	{ "const unsigned long long", "const unsigned long long" },

	{ "const char*", "const char*" },
	{ "const short*", "const short*" },
	{ "const int*", "const int*" },
	{ "const long long*", "const long long*" },
	{ "const float*", "const float*" },
	{ "const double*", "const double*" },
	{ "const unsigned char*", "const unsigned char*" },
	{ "const unsigned short*", "const unsigned short*" },
	{ "const unsigned int*", "const unsigned int*" },
	{ "const unsigned long long*", "const unsigned long long*" },

	{ "__int8", "__int8" },
	{ "__int16", "__int16" },
	{ "__int32", "__int32" },
	{ "__int64", "__int64" },
	{ "unsigned __int8", "unsigned __int8" },
	{ "unsigned __int16", "unsigned __int16" },
	{ "unsigned __int32", "unsigned __int32" },
	{ "unsigned __int64", "unsigned __int64" },

	{ "__int8*", "__int8*" },
	{ "__int16*", "__int16*" },
	{ "__int32*", "__int32*" },
	{ "__int64*", "__int64*" },
	{ "unsigned __int8*", "unsigned __int8*" },
	{ "unsigned __int16*", "unsigned __int16*" },
	{ "unsigned __int32*", "unsigned __int32*" },
	{ "unsigned __int64*", "unsigned __int64*" },

	{ "__int8 *", "__int8*" },
	{ "__int16 *", "__int16*" },
	{ "__int32 *", "__int32*" },
	{ "__int64 *", "__int64*" },
	{ "unsigned __int8 *", "unsigned __int8*" },
	{ "unsigned __int16 *", "unsigned __int16*" },
	{ "unsigned __int32 *", "unsigned __int32*" },
	{ "unsigned __int64 *", "unsigned __int64*" },

	{ "__int8 const*", "__int8 const*" },
	{ "__int16 const*", "__int16 const*" },
	{ "__int32 const*", "__int32 const*" },
	{ "__int64 const*", "__int64 const*" },
	{ "unsigned __int8 const*", "unsigned __int8 const*" },
	{ "unsigned __int16 const*", "unsigned __int16 const*" },
	{ "unsigned __int32 const*", "unsigned __int32 const*" },
	{ "unsigned __int64 const*", "unsigned __int64 const*" },

	{ "__int8 const *", "__int8 const*" },
	{ "__int16 const *", "__int16 const*" },
	{ "__int32 const *", "__int32 const*" },
	{ "__int64 const *", "__int64 const*" },
	{ "unsigned __int8 const *", "unsigned __int8 const*" },
	{ "unsigned __int16 const *", "unsigned __int16 const*" },
	{ "unsigned __int32 const *", "unsigned __int32 const*" },
	{ "unsigned __int64 const *", "unsigned __int64 const*" },

	{ "__mmask8", "__mmask8" },
	{ "__mmask16", "__mmask16" },
	{ "__mmask32", "__mmask32" },
	{ "__mmask64", "__mmask64" },

	{ "__mmask8*", "__mmask8*" },
	{ "__mmask16*", "__mmask16*" },
	{ "__mmask32*", "__mmask32*" },
	{ "__mmask64*", "__mmask64*" },

	{ "__mmask8 *", "__mmask8*" },
	{ "__mmask16 *", "__mmask16*" },
	{ "__mmask32 *", "__mmask32*" },
	{ "__mmask64 *", "__mmask64*" },

	{ "__mmask8 const*", "__mmask8 const*" },
	{ "__mmask16 const*", "__mmask16 const*" },
	{ "__mmask32 const*", "__mmask32 const*" },
	{ "__mmask64 const*", "__mmask64 const*" },

	{ "__mmask8 const *", "__mmask8 const*" },
	{ "__mmask16 const *", "__mmask16 const*" },
	{ "__mmask32 const *", "__mmask32 const*" },
	{ "__mmask64 const *", "__mmask64 const*" },

	{ "_MM_MANTISSA_NORM_ENUM", "_MM_MANTISSA_NORM_ENUM" },
	{ "_MM_MANTISSA_SIGN_ENUM", "_MM_MANTISSA_SIGN_ENUM" },
	{ "_MM_PERM_ENUM", "_MM_PERM_ENUM" },
	{ "_MM_CMPINT_ENUM", "_MM_CMPINT_ENUM" },
	{ "_MM_UPCONV_PS_ENUM", "_MM_UPCONV_PS_ENUM" },
	{ "_MM_UPCONV_PD_ENUM", "_MM_UPCONV_PD_ENUM" },
	{ "_MM_UPCONV_EPI32_ENUM", "_MM_UPCONV_EPI32_ENUM" },
	{ "_MM_UPCONV_EPI64_ENUM", "_MM_UPCONV_EPI64_ENUM" },
	{ "_MM_DOWNCONV_PS_ENUM", "_MM_DOWNCONV_PS_ENUM" },
	{ "_MM_DOWNCONV_PD_ENUM", "_MM_DOWNCONV_PD_ENUM" },
	{ "_MM_DOWNCONV_EPI32_ENUM", "_MM_DOWNCONV_EPI32_ENUM" },
	{ "_MM_DOWNCONV_EPI64_ENUM", "_MM_DOWNCONV_EPI64_ENUM" },

	{ "bool", "bool" },
	{ "void", "void" },
	{ "size_t", "size_t" },

	{ "__m128", "m128_f32_t" },
	{ "__m128d", "m128_f64_t" },
	{ "__m256", "m256_f32_t" },
	{ "__m256d", "m256_f64_t" },
	{ "__m512", "m512_f32_t" },
	{ "__m512d", "m512_f64_t" },

	{ "__m128*", "m128_f32_t*" },
	{ "__m128d*", "m128_f64_t*" },
	{ "__m256*", "m256_f32_t*" },
	{ "__m256d*", "m256_f64_t*" },
	{ "__m512*", "m512_f32_t*" },
	{ "__m512d*", "m512_f64_t*" },

	{ "__m128 *", "m128_f32_t*" },
	{ "__m128d *", "m128_f64_t*" },
	{ "__m256 *", "m256_f32_t*" },
	{ "__m256d *", "m256_f64_t*" },
	{ "__m512 *", "m512_f32_t*" },
	{ "__m512d *", "m512_f64_t*" },

	{ "__m128 const*", "m128_f32_t const*" },
	{ "__m128d const*", "m128_f64_t const*" },
	{ "__m256 const*", "m256_f32_t const*" },
	{ "__m256d const*", "m256_f64_t const*" },
	{ "__m512 const*", "m512_f32_t const*" },
	{ "__m512d const*", "m512_f64_t const*" },

	{ "__m128 const *", "m128_f32_t const*" },
	{ "__m128d const *", "m128_f64_t const*" },
	{ "__m256 const *", "m256_f32_t const*" },
	{ "__m256d const *", "m256_f64_t const*" },
	{ "__m512 const *", "m512_f32_t const*" },
	{ "__m512d const *", "m512_f64_t const*" },

	// Not direct conversions :
	{ "void*", "" },
	{ "void *", "" },
	{ "void const*", "" },
	{ "void const *", "" },

	{ "__m64", "" },
	{ "__m128i", "" },
	{ "__m256i", "" },
	{ "__m512i", "" },

	{ "__m64*", "" },
	{ "__m128i*", "" },
	{ "__m256i*", "" },
	{ "__m512i*", "" },

	{ "__m64 *", "" },
	{ "__m128i *", "" },
	{ "__m256i *", "" },
	{ "__m512i *", "" },

	{ "__m64 const*", "" },
	{ "__m128i const*", "" },
	{ "__m256i const*", "" },
	{ "__m512i const*", "" },

	{ "__m64 const *", "" },
	{ "__m128i const *", "" },
	{ "__m256i const *", "" },
	{ "__m512i const *", "" },
};

const std::vector<std::string> int_overload_fea_suffixes{
	"i8_t",
	"u8_t",
	"i16_t",
	"u16_t",
	"i32_t",
	"u32_t",
	"i64_t",
	"u64_t",
};

const std::unordered_map<std::string, std::string> suffix_to_cpp_type{
	{ "i8_t", "char" },
	{ "i16_t", "short" },
	{ "i32_t", "int" },
	{ "i64_t", "long long" },

	{ "u8_t", "unsigned char" },
	{ "u16_t", "unsigned short" },
	{ "u32_t", "unsigned int" },
	{ "u64_t", "unsigned long long" },

	{ "si8", "char" },
	{ "si16", "short" },
	{ "si32", "int" },
	{ "si64", "long long" },

	{ "su8", "unsigned char" },
	{ "su16", "unsigned short" },
	{ "su32", "unsigned int" },
	{ "su64", "unsigned long long" },

	{ "pi", "float" },
	{ "epi64", "long long" },
};

// The overloads to generate given a suffix.
const std::unordered_map<std::string, std::vector<std::string>>
		suffix_to_suffix_overloads{
			{ "si64", { "m64_i8_t", "m64_i16_t", "m64_i32_t", "m64_i64_t" } },
			{ "si128",
					{ "m128_i8_t", "m128_i16_t", "m128_i32_t", "m128_i64_t" } },
			{ "si256",
					{ "m256_i8_t", "m256_i16_t", "m256_i32_t", "m256_i64_t" } },
			{ "si512",
					{ "m512_i8_t", "m512_i16_t", "m512_i32_t", "m512_i64_t" } },
		};

// Skipped function overloads that don't bring anything new.
const std::unordered_set<std::string> ignored_intrins{
	// mmx
	"_mm_cvt_pi2ps",
	"_mm_cvt_ps2pi",
	"_mm_cvt_si2ss",
	"_mm_cvt_ss2si",
	"_mm_cvtt_ps2pi",
	"_mm_cvtt_ss2si",
	"_m_packssdw",
	"_m_packsswb",
	"_m_packuswb",
	"_m_punpckhbw",
	"_m_punpckhdq",
	"_m_punpckhwd",
	"_m_punpcklbw",
	"_m_punpckldq",
	"_m_punpcklwd",
	"_m_paddb",
	"_m_paddd",
	"_m_paddsb",
	"_m_paddsw",
	"_m_paddusb",
	"_m_paddusw",
	"_m_paddw",
	"_m_psubb",
	"_m_psubd",
	"_m_psubsb",
	"_m_psubsw",
	"_m_psubusb",
	"_m_psubusw",
	"_m_psubw",
	"_m_pmaddwd",
	"_m_pmulhuw",
	"_m_pmulhw",
	"_m_pmullw",
	"_m_pslld",
	"_m_pslldi",
	"_m_psllq",
	"_m_psllqi",
	"_m_psllw",
	"_m_psllwi",
	"_m_psrad",
	"_m_psradi",
	"_m_psraw",
	"_m_psrawi",
	"_m_psrld",
	"_m_psrldi",
	"_m_psrlq",
	"_m_psrlqi",
	"_m_psrlw",
	"_m_psrlwi",
	"_m_pand",
	"_m_pandn",
	"_m_por",
	"_m_pxor",
	"_m_pcmpeqb",
	"_m_pcmpeqd",
	"_m_pcmpeqw",
	"_m_pcmpgtb",
	"_m_pcmpgtd",
	"_m_pcmpgtw",
	"_m_empty",

	// sse
	"_m_maskmovq",
	"_m_pavgb",
	"_m_pavgw",
	"_m_pextrw",
	"_m_pinsrw",
	"_m_pmaxsw",
	"_m_pmaxub",
	"_m_pminsw",
	"_m_pminub",
	"_m_pmovmskb",
	"_m_pmulhuw",
	"_m_psadbw",
	"_m_pshufw",

	// sse2
	"_mm_cvtsi128_si64x", // same as "_si64"
	"_mm_cvtsd_si64x",
	"_mm_cvttsd_si64x",
	"_mm_cvtsi64x_sd",
	"_mm_cvtsi64x_si128",
	"_mm_slli_si128", // same as bslli_si128
	"_mm_srli_si128",
};

// All of mmx is wrapped in an #if FEA_32BIT.
// But some mmx intrinsics were added later and leak.
// Wrap those in bundle of 32bits.
const std::unordered_set<std::string> mmx_32bit_intrins{
	// sse
	"_mm_avg_pu8",
	"_mm_avg_pu16",
	"_mm_cvtpi8_ps",
	"_mm_cvtpu8_ps",
	"_mm_cvtpi16_ps",
	"_mm_cvtpi32x2_ps",
	"_mm_cvtpu16_ps",
	"_mm_cvtpi32_ps",
	"_mm_cvtps_pi8",
	"_mm_cvtps_pi16",
	"_mm_cvtps_pi32",
	"_mm_cvttps_pi32",
	"_mm_extract_pi16",
	"_mm_insert_pi16",
	"_mm_maskmove_si64",
	"_mm_max_pu8",
	"_mm_max_pi16",
	"_mm_min_pu8",
	"_mm_min_pi16",
	"_mm_movemask_pi8",
	"_mm_mulhi_pu16",
	"_mm_sad_pu8",
	"_mm_shuffle_pi16",
	"_mm_stream_pi",

	// sse2
	"_mm_add_si64",
	"_mm_cvtpd_pi32",
	"_mm_cvtpi32_pd",
	"_mm_cvttpd_pi32",
	"_mm_movpi64_epi64",
	"_mm_movepi64_pi64",
	"_mm_mul_su32",
	"_mm_set_epi64",
	"_mm_set1_epi64",
	"_mm_setr_epi64",
	"_mm_sub_si64",
};

// Not supported by MSVC. TBD clang and gcc.
const std::unordered_set<std::string> commented_intrins{
	// mmx
	"_m_from_int",
	"_m_from_int64",
	"_m_to_int",
	"_m_to_int64",
	"_mm_cvtm64_si64",
	"_mm_cvtsi64_m64",
};

// Used to assert and make sure we caught all suffixes.
const std::unordered_set<std::string> no_suffix_intrins{
	// mmx
	"_m_from_int64",
	"_m_to_int64",
	"_m_from_int",
	"_m_to_int",
	"_m_empty",
	"_mm_empty",

	// sse
	"_MM_GET_EXCEPTION_MASK",
	"_MM_GET_EXCEPTION_STATE",
	"_MM_GET_FLUSH_ZERO_MODE",
	"_MM_GET_ROUNDING_MODE",
	"_mm_getcsr",
	"_MM_SET_EXCEPTION_MASK",
	"_MM_SET_EXCEPTION_STATE",
	"_MM_SET_FLUSH_ZERO_MODE",
	"_MM_SET_ROUNDING_MODE",
	"_mm_setcsr",
	"_mm_prefetch",
	"_mm_sfence",
	"_mm_malloc",
	"_mm_free",

	// sse2
	"_mm_pause",
	"_mm_clflush",
	"_mm_lfence",
	"_mm_mfence",
};

// We append an underscore for reserved c++ names.
const std::unordered_set<std::string> reserved_func_names{
	"and",
	"or",
	"xor",
};

// These functions bypass argument deduction systems.
const std::unordered_set<std::string> as_is_funcs{
	"free",
	"malloc",
	"clflush",
};

// Double suffix functions handle 2 suffixes as 'from'_'to'.
// Moving the return value to an output parameter is handled by
// another set.
const std::unordered_set<std::string> double_suffix_funcs{
	"cvt",
	"cvtt",
	"cvtt",
	"mov",
	"cast",
};

// These functions translate 1:1 with their suffixes.
// Search with feafuncnames, not intrinsic names.
const std::unordered_set<std::string> suffix_funcs{
	// mmx
	"add",
	"adds",
	"adds",
	"cmpeq",
	"cmpgt",
	"madd",
	"mulhi",
	"mullo",
	"set",
	"set1",
	"setr",
	"sll",
	"slli",
	"sra",
	"srai",
	"srl",
	"srli",
	"sub",
	"subs",
	"unpackhi",
	"unpacklo",

	// sse
	"avg",
	"extract",
	"insert",
	"max",
	"min",
	"movemask",
	"shuffle",
	"div",
	"rem",
	"idiv",
	"idivrem",
	"irem",
	// following have bad suffixes
	"udiv",
	"udivrem",
	"urem",

	// sse2
	"mul",
	"cmplt",
	"move",
	"shufflehi",
	"shufflelo",
};

// Use the suffix to determine the register type,
// but convert pointers (refs) to c++ types.
// const std::unordered_set<std::string> suffix_plus_ptr_to_cpp{
//	"loadh",
//	"loadl",
//	"storeh",
//	"storel",
//};

// These functions have incorrect suffixes, and must be manually converted to
// unsigned.
const std::unordered_set<std::string> force_unsigned_funcs{
	"udiv",
	"udivrem",
	"urem",
};

// Typically, these functions return an int, change to bool.
const std::unordered_set<std::string> return_bool_funcs{
	"comieq",
	"comige",
	"comigt",
	"comile",
	"comilt",
	"comineq",
	"ucomieq",
	"ucomige",
	"ucomigt",
	"ucomile",
	"ucomilt",
	"ucomineq",
};

// Moves the return value to an output parameter ref.
const std::unordered_set<std::string> return_to_output_ref_funcs{
	"cvt", "cvtt", "setzero", "undefined", "mov", "cast", "set", "set1", "setr",
	//"load",
	//"loadu",
	//"load1",
};

// Change the pointer paramter to a reference, indicating a single value.
// Acts on strippedfuncnames.
const std::unordered_set<std::string> ptr_to_ref_funcs{
	"idivrem", "udivrem", "load1", "store1", "sincos",
	//"loadl",
	//"loadh",
	//"storel",
	//"storeh",
};

// We need to generate overloads for these functions.
// Uses strippedfuncname for search.
const std::unordered_set<std::string> overload_funcs{
	"and",
	"andnot",
	"or",
	"xor",
	"setzero",

	// sse2
	"undefined",
	"bslli", // si128 version
	"slli", // si128 version
	"bsrli", // si128 version
	"srli", // si128 version
	"",
};

// Converts the input/output pointers to c++ types.
const std::unordered_set<std::string> cpp_overload_funcs{
	"stream",
	"store",
	"load",

	"loadh",
	"loadl",
	"storeh",
	"storel",

	// sse2
	"loadu",
	"storeu",
};

// Map of function -> templated argument.
const std::unordered_map<std::string, std::string> template_args{
	// sse
	{ "prefetch", "i" },
	{ "shuffle", "imm8" },

	// sse2
	{ "bslli", "imm8" },
	{ "bsrli", "imm8" },
	{ "extract", "imm8" },
	{ "insert", "imm8" },
	{ "shufflehi", "imm8" },
	{ "shufflelo", "imm8" },
	{ "slli", "imm8" },
	{ "srli", "imm8" },
};

// Register types to their equivalent fea prefixes.
const std::unordered_map<std::string, std::string> register_to_fea_prefix{
	{ "__m64", "m64_" },
	{ "__m128", "m128_" },
	{ "__m128i", "m128_" },
	{ "__m128d", "m128_" },
	{ "__m256", "m256_" },
	{ "__m256i", "m256_" },
	{ "__m256d", "m256_" },
	{ "__m512", "m512_" },
	{ "__m512i", "m512_" },
	{ "__m512d", "m512_" },
};

// Suffixes to their equivalent fea suffixes.
const std::unordered_map<std::string, std::string> suffix_to_fea_suffix{
	{ "ps", "f32_t" },
	{ "ss", "f32_t" },
	{ "f32", "f32_t" },
	{ "f32x2", "f32_t" },
	{ "f32x4", "f32_t" },
	{ "f32x8", "f32_t" },
	{ "pi", "f32_t" }, // ???

	{ "pd", "f64_t" },
	{ "sd", "f64_t" },
	{ "f64", "f64_t" },
	{ "f64x2", "f64_t" },
	{ "f64x4", "f64_t" },

	{ "pi8", "i8_t" },
	{ "epi8", "i8_t" },
	{ "si8", "i8_t" },

	{ "pu8", "u8_t" },
	{ "epu8", "u8_t" },

	{ "pi16", "i16_t" },
	{ "epi16", "i16_t" },
	{ "si16", "i16_t" },

	{ "pu16", "u16_t" },
	{ "epu16", "u16_t" },

	{ "pi32", "i32_t" },
	{ "epi32", "i32_t" },
	{ "si32", "i32_t" },
	{ "pi32x2", "i32_t" },
	{ "i32x2", "i32_t" },
	{ "i32x4", "i32_t" },
	{ "i32x8", "i32_t" },

	{ "pu32", "u32_t" },
	{ "epu32", "u32_t" },
	{ "su32", "u32_t" },

	{ "pi64", "i64_t" },
	{ "epi64", "i64_t" },
	{ "epi64x", "i64_t" },
	{ "si64", "i64_t" },
	{ "m64", "i64_t" },
	{ "i64x2", "i64_t" },
	{ "i64x4", "i64_t" },

	{ "pu64", "u64_t" },
	{ "epu64", "u64_t" },

	// for generalized type maker
	{ "i8_t", "i8_t" },
	{ "u8_t", "u8_t" },
	{ "i16_t", "i16_t" },
	{ "u16_t", "u16_t" },
	{ "i32_t", "i32_t" },
	{ "u32_t", "u32_t" },
	{ "i64_t", "i64_t" },
	{ "u64_t", "u64_t" },
};
//{ "int", "i32_t" },
//{ "int64", "i64_t" },

// Unsigned equivalent for mislabeled suffixes.
const std::unordered_map<std::string, std::string> signed_to_unsigned_suffix{
	{ "epi8", "epu8" },
	{ "epi16", "epu16" },
	{ "epi32", "epu32" },
	{ "epi64", "epu64" },
};

const std::unordered_map<std::string, std::string> kept_suffixes_to_fea_pretty{
	//{ "ss", "one" },
	//{ "sd", "one" },
	//{ "ps1", "first" },
	//{ "pd1", "first" },
};

// Change the pointer paramter to a reference, indicating a single value.
// Acts on suffixes.
const std::unordered_set<std::string> ptr_to_ref_suffix{
	"ss",
	"ps1",
};


struct suffix_info {
	struct suf {
		std::string name;
		bool keep = false;
	};

	suffix_info() = default;
	suffix_info(std::string&& srch, std::string&& s1)
			: search(std::move(srch))
			, suffixes(make_subinfo(std::move(s1), false))
			, feafunc_suffix(make_feafunc_suffix(suffixes)) {
	}
	suffix_info(std::string&& srch, std::string&& s1, bool k1)
			: search(std::move(srch))
			, suffixes(make_subinfo(std::move(s1), k1))
			, feafunc_suffix(make_feafunc_suffix(suffixes)) {
	}
	suffix_info(std::string&& srch, std::string&& s1, std::string&& s2)
			: search(std::move(srch))
			, suffixes(make_subinfo(std::move(s1), std::move(s2), false, false))
			, feafunc_suffix(make_feafunc_suffix(suffixes)) {
	}
	suffix_info(std::string&& srch, std::string&& s1, std::string&& s2, bool k1,
			bool k2)
			: search(std::move(srch))
			, suffixes(make_subinfo(std::move(s1), std::move(s2), k1, k2))
			, feafunc_suffix(make_feafunc_suffix(suffixes)) {
	}

	static fea::stack_vector<suf, 2> make_subinfo(std::string&& s1, bool keep) {
		// In this case, the raw size == search.
		fea::stack_vector<suf, 2> ret;
		ret.push_back(suf{ std::move(s1), keep });
		return ret;
	}

	static fea::stack_vector<suf, 2> make_subinfo(
			std::string&& s1, std::string&& s2, bool keep1, bool keep2) {
		fea::stack_vector<suf, 2> ret;
		ret.push_back(suf{ std::move(s1), keep1 });
		ret.push_back(suf{ std::move(s2), keep2 });
		return ret;
	}

	static std::string make_feafunc_suffix(
			const fea::stack_vector<suf, 2>& sufs) {
		std::string ret = "_";
		for (const suf& s : sufs) {
			if (!s.keep) {
				continue;
			}

			if (kept_suffixes_to_fea_pretty.contains(s.name)) {
				ret += kept_suffixes_to_fea_pretty.at(s.name);
			} else {
				ret += s.name;
			}
		}

		if (ret.size() == 1) {
			return {};
		}
		return ret;
	}

	// The search term.
	std::string search;

	// Ordered by encountered.
	// In cvt, "from", "to".
	fea::stack_vector<suf, 2> suffixes;

	// The final function suffix.
	std::string feafunc_suffix;
};

// Our suffix info map.
const std::vector<suffix_info> suffix_infos{
	// These come before.

	// mmx
	{ "si32_si64", "si32", "si64", false, false },
	{ "si64_si32", "si64", "si32", false, false },
	{ "si64_m64", "si64", "m64", false, false },
	{ "m64_si64", "m64", "si64", false, false },

	// sse
	{ "ps_pi8", "ps", "pi8", false, false },
	{ "ps_pi16", "ps", "pi16", false, false },
	{ "ps_pi32", "ps", "pi32", false, false },
	{ "ss_f32", "ss", "f32", false, false },
	{ "ss_si32", "ss", "si32", false, false },
	{ "ss_si64", "ss", "si64", false, false },
	{ "si32_ss", "si32", "ss", false, true },
	{ "si64_ss", "si64", "ss", false, true },
	{ "pi8_ps", "pi8", "ps", false, false },
	{ "pi16_ps", "pi16", "ps", false, false },
	{ "pi32_ps", "pi32", "ps", false, false },
	{ "pi32x2_ps", "pi32x2", "ps", false, false },
	{ "pu8_ps", "pu8", "ps", false, false },
	{ "pu16_ps", "pu16", "ps", false, false },

	// sse2
	{ "si32_sd", "si32", "sd", false, false },
	{ "si32_si128", "si32", "si128", false, false },
	{ "epi32_pd", "epi32", "pd", false, false },
	{ "epi32_ps", "epi32", "ps", false, false },
	{ "pi32_pd", "pi32", "pd", false, false },
	{ "epi64_pi64", "epi64", "pi64", false, false },
	{ "pi64_epi64", "pi64", "epi64", false, false },
	{ "si64_sd", "si64", "sd", false, false },
	{ "si64_si128", "si64", "si128", false, false },
	{ "si64x_sd", "si64", "sd", false, false },
	{ "si64x_si128", "si64", "si128", false, false },
	{ "si128_pd", "si128", "pd", false, false },
	{ "si128_ps", "si128", "ps", false, false },
	{ "si128_si32", "si128", "si32", false, false },
	{ "si128_si64", "si128", "si64", false, false },
	{ "si128_si64x", "si128", "si64x", false, false },
	{ "pd_epi32", "pd", "epi32", false, false },
	{ "pd_pi32", "pd", "pi32", false, false },
	{ "pd_ps", "pd", "ps", false, false },
	{ "pd_si128", "pd", "si128", false, false },
	{ "sd_f64", "sd", "f64", false, false },
	{ "sd_si32", "sd", "si32", false, false },
	{ "sd_si64", "sd", "si64", false, false },
	{ "sd_si64x", "sd", "si64x", false, false },
	{ "sd_ss", "sd", "ss", false, true },
	{ "ps_epi32", "ps", "epi32", false, false },
	{ "ps_pd", "ps", "pd", false, false },
	{ "ps_si128", "ps", "si128", false, false },
	{ "ss_sd", "ss", "sd", false, false },


	{ "_ps", "ps" },
	{ "_PS", "ps" },
	{ "_ss", "ss", true },
	{ "_ps1", "ps1", true },
	{ "_f32x2", "f32x2", true },
	{ "_f32x4", "f32x4", true },
	{ "_f32x8", "f32x8", true },

	{ "_pd", "pd" },
	{ "_sd", "sd", true },
	{ "_pd1", "pd1", true },
	{ "_f64x2", "f64x2", true },
	{ "_f64x4", "f64x4", true },

	{ "_pi8", "pi8" },
	{ "_epi8", "epi8" },
	{ "_si8", "si8", true },

	{ "_pu8", "pu8" },
	{ "_epu8", "epu8" },

	{ "_pi16", "pi16" },
	{ "_epi16", "epi16" },
	{ "_si16", "si16", true },

	{ "_pu16", "pu16" },
	{ "_epu16", "epu16" },

	{ "_pi32", "pi32" },
	{ "_epi32", "epi32" },
	{ "_si32", "si32", true },
	{ "_i32x2", "i32x2", true },
	{ "_i32x4", "i32x4", true },
	{ "_i32x8", "i32x8", true },

	{ "_pu32", "pu32" },
	{ "_epu32", "epu32" },
	{ "_su32", "su32" },

	{ "_pi64", "pi64" },
	{ "_epi64", "epi64" },
	{ "_epi64x", "epi64x" },
	{ "_si64", "si64", true },
	{ "_i64x2", "i64x2", true },
	{ "_i64x4", "i64x4", true },

	{ "_pu64", "pu64" },
	{ "_epu64", "epu64" },

	// Will need overloads.
	{ "_pi", "pi" },
	{ "_si128", "si128" },
	{ "_si256", "si256" },
	{ "_si512", "si512" },
};

struct parameter_info {
	parameter_info() = default;
	parameter_info(const pugi::xml_node& param)
			: e_type(parse_etype(param))
			, type(param.attribute("type").value())
			, varname(param.attribute("varname").value())
			, output_ref(false)
			, direct_conversion(!direct_type_conversions.at(type).empty()) {
	}

	static etype parse_etype(const pugi::xml_node& param) {
		pugi::xml_attribute attr = param.attribute("etype");
		if (attr) {
			return str_to_etype.at(attr.value());
		} else {
			return str_to_etype.at(param.attribute("type").value());
		}
	}

	std::string to_signature() const {
		return type + " " + varname;
	}

	std::string to_passed() const {
		if (!fea::starts_with(type, 'm')) {
			return varname;
		}

		if (!fea::contains(type, "_t")
				|| std::count(type.begin(), type.end(), '_') != 2) {
			return varname;
		}

		// if (fea::contains(type, '*')) {
		//	return varname + "->xmm";
		// }
		return varname + ".xmm";
	}

	etype e_type = etype::count;
	std::string type;
	std::string varname;
	std::string cast_str;
	// bool output_ptr = false;
	bool output_ref = false; // changes to out = intrin
	bool direct_conversion = false;
	bool ref_to_ptr = false; // changes to &param
	bool first_passed = false; // order was changed, this is the first.
	bool template_arg = false; // move arg to template
};

struct intrinsic_info {
	static constexpr size_t max_params = 64;

	intrinsic_info() = default;
	intrinsic_info(const pugi::xml_node& intrin)
			: cpuid(intrin.child("CPUID").text().get())
			, simd_version(supported_cpuids.at(cpuid))
			, intrinname(intrin.attribute("name").value())
			, prefix(get_prefix(intrinname))
			, suffix(get_suffix_info(intrinname))
			, feafuncname(get_feafuncname(intrinname, suffix))
			, strippedfuncname(get_strippedfuncname(intrinname, suffix))
			, return_parameter(parameter_info{ intrin.child("return") })
			, description(clean_description(
					  intrin.child("description").text().get()))
			, operation(clean_operation(intrin.child("operation").text().get()))
			, instruction(get_instruction(intrin))
			, parameters(parse_params(intrin))
			, commented(commented_intrins.contains(intrinname))
			, reserved(reserved_func_names.contains(feafuncname)) {
	}

	static fea::stack_vector<parameter_info, max_params> parse_params(
			const pugi::xml_node& intrin) {

		fea::stack_vector<parameter_info, max_params> ret;
		for (pugi::xml_node p : intrin.children("parameter")) {
			assert(ret.size() < max_params);
			ret.push_back(parameter_info{ p });
		}
		return ret;
	}

	static std::string get_prefix(const std::string& fn) {
		size_t idx = fn.find('_', 1);
		if (idx != std::string::npos) {
			return fn.substr(1, idx - 1);
		}
		return {};
	}

	static suffix_info get_suffix_info(const std::string& fn) {
		suffix_info ret;
		size_t suf_idx = std::string::npos;
		for (const suffix_info& s : suffix_infos) {
			if (s.search.size() > ret.search.size()
					&& fea::ends_with(fn, s.search)) {
				ret = s;
				suf_idx = fn.size() - ret.search.size();
			}
		}

#if FEA_DEBUG
		// Make sure our list is unique.
		std::vector<std::string> searches;
		for (const suffix_info& s : suffix_infos) {
			searches.push_back(s.search);
		}
		std::sort(searches.begin(), searches.end());
		assert(std::unique(searches.begin(), searches.end()) == searches.end());

		// Not found, check if this a new unknown suffix.
		if (suf_idx == std::string::npos) {
			assert(no_suffix_intrins.contains(fn));
		}

		// Also, check that we didn't miss any double suffixes.
		std::string substr = fn.substr(0, suf_idx);
		bool found_missed = false;
		for (const suffix_info& s : suffix_infos) {
			if (fea::ends_with(substr, s.search)) {
				found_missed = true;
				break;
			}
		}
		assert(!found_missed);
#endif

		return ret;
	}

	static std::string get_feafuncname(
			const std::string& fn, const suffix_info& suf_info) {
		std::string ret = fea::to_lower_ascii(fn);

		// Delete _mmX_ prefix
		{
			size_t idx = ret.find('_', 1);
			if (idx != std::string::npos) {
				ret = ret.substr(idx + 1);
			}
		}

		if (suf_info.search.empty()) {
			return ret;
		}

		fea::replace_all_inplace(ret, suf_info.search, suf_info.feafunc_suffix);
		return ret;
	}

	static std::string get_strippedfuncname(
			const std::string& fn, const suffix_info& suf_info) {
		std::string ret = fea::to_lower_ascii(fn);

		// Delete _mmX_ prefix
		{
			size_t idx = ret.find('_', 1);
			if (idx != std::string::npos) {
				ret = ret.substr(idx + 1);
			}
		}

		if (suf_info.search.empty()) {
			return ret;
		}

		fea::replace_all_inplace(ret, suf_info.search, "");
		return ret;
	}

	static std::string clean_description(const std::string& desc) {
		if (desc.empty()) {
			return desc;
		}

		std::string ret = desc;
		if (fea::contains(desc, "\n\t")) {
			fea::replace_all_inplace(ret, "\n\t", " ");
		}

		constexpr size_t column_limit = 79;
		if (ret.size() <= column_limit) {
			if (!fea::ends_with(ret, '\n')) {
				ret += '\n';
			}
			return ret;
		}

		size_t prev_space_idx = 0;
		size_t running_idx = 0;
		for (size_t i = 0; i < ret.size(); ++i) {
			if (running_idx != 0 && running_idx % column_limit == 0) {
				ret[prev_space_idx] = '\n';
				running_idx += i - prev_space_idx;
			}

			if (ret[i] == ' ') {
				prev_space_idx = i;
			}

			++running_idx;
		}

		if (!fea::ends_with(ret, '\n')) {
			ret += '\n';
		}
		return ret;
	}

	static std::string clean_operation(const std::string& op) {
		if (op.empty()) {
			return op;
		}

		std::string ret = op;
		if (fea::starts_with(ret, '\n')) {
			ret = ret.substr(1);
		}

		if (fea::ends_with(ret, "\n\t")) {
			ret = ret.substr(0, op.size() - 2);
		}

		if (!fea::ends_with(ret, '\n')) {
			ret += '\n';
		}

		return ret;
	}

	static std::string get_instruction(const pugi::xml_node& intrin) {
		pugi::xml_attribute seq = intrin.attribute("sequence");
		if (seq && std::string_view{ seq.value() } == "TRUE") {
			return "SEQUENCE\n";
		}

		pugi::xml_node inst = intrin.child("instruction");
		if (inst) {
			std::string n = inst.attribute("name").value();
			std::string f = inst.attribute("form").value();
			fea::to_lower_ascii_inplace(n);
			fea::to_lower_ascii_inplace(f);
			return n + ' ' + f + '\n';
		}
		return {};
	}

	bool contains_type(const std::string& t) const {
		if (fea::contains(return_parameter.type, t)) {
			return true;
		}
		for (const parameter_info& p : parameters) {
			if (fea::contains(p.type, t)) {
				return true;
			}
		}
		return false;
	}

	std::string to_string() const {
		std::string ret = file_function_template;

		if (mmx_32bit_intrins.contains(intrinname) && contains_type("m64")) {
			ret.insert(1, "#if FEA_32BIT\n");
			ret += "#endif\n";
		}

		bool has_template = false;
		std::string template_str = "template <{arg}>\n";
		for (const parameter_info& p : parameters) {
			if (p.template_arg) {
				has_template = true;
				fea::replace_all_inplace(
						template_str, "{arg}", p.to_signature());
			}
		}

		if (has_template) {
			fea::replace_all_inplace(ret, "{template}", template_str);
		} else {
			fea::replace_all_inplace(ret, "{template}", "");
		}

		if (commented) {
			size_t idx = ret.find("FEA_FORCEINLINE");
			if (idx != std::string::npos) {
				ret.insert(idx, "/*");
				idx = ret.find_last_of("}") + 1;
				ret.insert(idx, "*/");
			}
		}

		fea::replace_all_inplace(ret, "{description}", description);
		fea::replace_all_inplace(ret, "{instruction}", instruction);
		fea::replace_all_inplace(ret, "{operation}", operation);
		fea::replace_all_inplace(ret, "{return_type}", return_parameter.type);
		if (reserved) {
			fea::replace_all_inplace(ret, "{feafuncname}", feafuncname + '_');
		} else {
			fea::replace_all_inplace(ret, "{feafuncname}", feafuncname);
		}

		std::string maybe_return_str;
		{
			std::string param_sig;
			for (const parameter_info& p : parameters) {
				if (p.e_type == etype::mvoid) {
					continue;
				}

				if (p.template_arg) {
					continue;
				}

				if (p.output_ref) {
					maybe_return_str = p.to_passed() + " = ";
				}
				param_sig += p.to_signature() + ", ";
			}

			if (!param_sig.empty()) {
				param_sig.pop_back();
				param_sig.pop_back();
			}
			fea::replace_all_inplace(ret, "{signature_params}", param_sig);
		}

		if (!maybe_return_str.empty()) {
			fea::replace_all_inplace(ret, "{maybe_return}", maybe_return_str);
		} else if (return_parameter.e_type != etype::mvoid) {
			fea::replace_all_inplace(ret, "{maybe_return}", "return ");
		} else {
			fea::replace_all_inplace(ret, "{maybe_return}", "");
		}

		fea::replace_all_inplace(ret, "{funcname}", intrinname);

		{
			std::string passed_params;
			for (const parameter_info& p : parameters) {
				if (p.output_ref) {
					continue;
				}

				std::string str;
				if (!p.cast_str.empty()) {
					str += p.cast_str;
				}

				if (p.ref_to_ptr) {
					str += '&';
				}

				str += p.to_passed() + ", ";

				if (p.first_passed) {
					passed_params.insert(0, str);
				} else {
					passed_params += str;
				}
			}

			if (!passed_params.empty()) {
				passed_params.pop_back();
				passed_params.pop_back();
			}
			fea::replace_all_inplace(ret, "{passed_params}", passed_params);
		}

		return ret;
	}

	std::string cpuid;
	fea::simd_ver simd_version = fea::simd_ver::count;
	std::string intrinname;
	std::string prefix;
	suffix_info suffix;
	std::string feafuncname;
	std::string strippedfuncname;
	parameter_info return_parameter;
	std::string description;
	std::string operation;
	std::string instruction;
	fea::stack_vector<parameter_info, max_params> parameters;
	bool commented = false;
	bool reserved = false;
};

std::string extract_ptr_suffix(std::string& type) {
	std::string ret;
	if (fea::contains(type, "const*")) {
		ret = " const*";
		fea::replace_all_inplace(type, "const*", "");
	} else if (fea::contains(type, "const *")) {
		ret = " const*";
		fea::replace_all_inplace(type, "const *", "");
	} else if (fea::contains(type, "*")) {
		assert(!fea::contains(type, "const"));
		ret = "*";
		fea::replace_all_inplace(type, "*", "");
	} else if (fea::contains(type, "const&")) {
		ret = " const&";
		fea::replace_all_inplace(type, "const&", "");
	} else if (fea::contains(type, "&")) {
		assert(!fea::contains(type, "const"));
		ret = "&";
		fea::replace_all_inplace(type, "&", "");
	}

	fea::replace_all_inplace(type, " ", "");
	return ret;
}

std::string make_fea_register_type(
		const std::string& type, const std::string& suffix) {
	std::string mtype = type;
	// Also cleans mtype:
	std::string ptr_suffix = extract_ptr_suffix(mtype);

	const std::string& fea_prefix = register_to_fea_prefix.at(mtype);
	const std::string& fea_suffix = suffix_to_fea_suffix.at(suffix);
	return fea_prefix + fea_suffix + ptr_suffix;
}

// The conversion functions get an output param for overload resolution.
void generate_double_suffix(
		intrinsic_info&& new_info, std::vector<intrinsic_info>& out) {
	assert(new_info.suffix.suffixes.size() == 2);
	const suffix_info::suf& from_suffix = new_info.suffix.suffixes.front();
	const suffix_info::suf& to_suffix = new_info.suffix.suffixes.back();

	// Setup our out param.
	{
		parameter_info& lastp = new_info.parameters.back();
		if (!lastp.direct_conversion) {
			std::string suffix_name = to_suffix.name;
			if (!suffix_to_fea_suffix.contains(to_suffix.name)) {
				// We have si >= 128 types, assume same type as other suffix.
				suffix_name = from_suffix.name;
			}


			// Convert using 'to' suffix.
			lastp.type = make_fea_register_type(lastp.type, suffix_name);
			lastp.direct_conversion = true;
		}
	}

	assert(new_info.parameters.size() >= 1);
	for (size_t i = 0; i < new_info.parameters.size() - 1; ++i) {
		parameter_info& p = new_info.parameters[i];
		if (!p.direct_conversion) {
			std::string suffix_name = from_suffix.name;
			if (!suffix_to_fea_suffix.contains(from_suffix.name)) {
				// We have si >= 128 types, assume same type as other suffix.
				suffix_name = to_suffix.name;
			}

			// Convert using 'from' suffix.
			p.type = make_fea_register_type(p.type, suffix_name);
			p.direct_conversion = true;
		}
	}

	out.push_back(std::move(new_info));
}

// For pack functions, the return type is the previously sized integer.
// Aka, input shorts, pack to chars.
void generate_pack(
		intrinsic_info&& new_info, std::vector<intrinsic_info>& out) {
	const fea::stack_vector<suffix_info::suf, 2>& suffixes
			= new_info.suffix.suffixes;

	// Expect single suffix.
	assert(suffixes.size() == 1);
	const suffix_info::suf& suffix = suffixes.front();

	// Get the input parameters' types.
	for (parameter_info& p : new_info.parameters) {
		if (p.direct_conversion) {
			continue;
		}
		p.type = make_fea_register_type(p.type, suffix.name);
		p.direct_conversion = true;
	}

	if (new_info.return_parameter.direct_conversion) {
		out.push_back(new_info);
		return;
	}

	// Return value, prefix is the same, suffix is 1 lower.
	parameter_info& retp = new_info.return_parameter;
	const std::string& fea_prefix = register_to_fea_prefix.at(retp.type);
	std::string fea_lower_suffix = suffix_to_fea_suffix.at(suffix.name);

	// Bump down suffix.
	if (fea::contains(fea_lower_suffix, "64")) {
		fea::replace_all_inplace(fea_lower_suffix, "64", "32");
	} else if (fea::contains(fea_lower_suffix, "32")) {
		fea::replace_all_inplace(fea_lower_suffix, "32", "16");
	} else if (fea::contains(fea_lower_suffix, "16")) {
		fea::replace_all_inplace(fea_lower_suffix, "16", "8");
	} else {
		assert(false);
	}

	retp.type = fea_prefix + fea_lower_suffix;
	retp.direct_conversion = true;

	out.push_back(std::move(new_info));
}

// Kindof the inverse of pack (type-wise). Takes lower sized register and
// returns next higher.
// sadge
void generate_sad(intrinsic_info&& new_info, std::vector<intrinsic_info>& out) {
	// Expect single suffix.
	assert(new_info.suffix.suffixes.size() == 1);
	const suffix_info::suf& suffix = new_info.suffix.suffixes.front();

	// Get the input parameters' types.
	// These are converted to fea as usual.
	for (parameter_info& p : new_info.parameters) {
		if (p.direct_conversion) {
			continue;
		}
		p.type = make_fea_register_type(p.type, suffix.name);
		p.direct_conversion = true;
	}
	assert(!new_info.return_parameter.direct_conversion);

	// Return value, prefix is the same, suffix is 1 higher.
	parameter_info& retp = new_info.return_parameter;
	const std::string& fea_prefix = register_to_fea_prefix.at(retp.type);
	std::string fea_higher_suffix = suffix_to_fea_suffix.at(suffix.name);

	// Bump down suffix.
	if (fea::contains(fea_higher_suffix, "8")) {
		fea::replace_all_inplace(fea_higher_suffix, "8", "16");
	} else if (fea::contains(fea_higher_suffix, "16")) {
		fea::replace_all_inplace(fea_higher_suffix, "16", "32");
	} else if (fea::contains(fea_higher_suffix, "32")) {
		fea::replace_all_inplace(fea_higher_suffix, "32", "64");
	} else {
		assert(false);
	}

	retp.type = fea_prefix + fea_higher_suffix;
	retp.direct_conversion = true;

	out.push_back(std::move(new_info));
}

// Here, we simply check the intrinsic suffix, and convert the __mXXX to the fea
// equivalent.
void generate_suffix_typed(
		intrinsic_info&& new_info, std::vector<intrinsic_info>& out) {

	assert(new_info.suffix.suffixes.size() == 1);
	suffix_info::suf suffix = new_info.suffix.suffixes.front();

	if (force_unsigned_funcs.contains(new_info.strippedfuncname)) {
		suffix.name = signed_to_unsigned_suffix.at(suffix.name);
	}

	// Return type
	if (!new_info.return_parameter.direct_conversion) {
		parameter_info& retp = new_info.return_parameter;
		retp.type = make_fea_register_type(retp.type, suffix.name);
		retp.direct_conversion = true;
	}

	// Params
	for (parameter_info& p : new_info.parameters) {
		if (!p.direct_conversion) {
			p.type = make_fea_register_type(p.type, suffix.name);
			p.direct_conversion = true;
		}
	}

	out.push_back(std::move(new_info));
}

// These functions have simple overloads for all int types.
// We generate 8 overloads (i8, i16, i32, i64, u8 ...).
void generate_overloads(
		intrinsic_info&& info, std::vector<intrinsic_info>& out) {
	assert(info.suffix.suffixes.size() == 1);

	for (const std::string& new_suffix : int_overload_fea_suffixes) {
		intrinsic_info new_info = info;

		// Return type
		parameter_info& retp = new_info.return_parameter;
		if (!retp.direct_conversion) {
			retp.type = make_fea_register_type(retp.type, new_suffix);
			retp.direct_conversion = true;
		}

		// Params
		for (parameter_info& p : new_info.parameters) {
			if (!p.direct_conversion) {
				p.type = make_fea_register_type(p.type, new_suffix);
				p.direct_conversion = true;
			}
		}
		out.push_back(std::move(new_info));
	}
}

void generate_cpp_overloads(
		intrinsic_info&& info, std::vector<intrinsic_info>& out) {
	// Float versions should be caught by all direct check.
	assert(info.suffix.suffixes.size() == 1);
	const suffix_info::suf& suffix = info.suffix.suffixes.front();

	auto replace_type = [](parameter_info& p, const std::string& cpp_type) {
		assert(fea::contains(p.type, "void") || fea::contains(p.type, "__m64")
				|| fea::contains(p.type, "__m128i")
				|| fea::contains(p.type, "__m256i")
				|| fea::contains(p.type, "__m512i"));

		p.cast_str = '(' + p.type + ')';
		fea::replace_all_inplace(p.type, "void", cpp_type);
		fea::replace_all_inplace(p.type, "__m64", cpp_type);
		fea::replace_all_inplace(p.type, "__m128i", cpp_type);
		fea::replace_all_inplace(p.type, "__m256i", cpp_type);
		fea::replace_all_inplace(p.type, "__m512i", cpp_type);
		p.direct_conversion = true;
	};

	if (suffix_to_cpp_type.contains(suffix.name)) {
		// We convert as is, to c++ type. No overloads.
		// Generate direct will take care of return parameter.
		for (parameter_info& p : info.parameters) {
			if (p.direct_conversion) {
				continue;
			}
			if (!fea::contains(p.type, '*') && !fea::contains(p.type, '&')) {
				continue;
			}

			const std::string& cpp_type = suffix_to_cpp_type.at(suffix.name);
			replace_type(p, cpp_type);
		}

		return generate_suffix_typed(std::move(info), out);
	}

	// Deal with the params that need to become cpp type overloads.
	// Could be void ptrs or __mXXX ptrs.
	for (const std::string& fea_suffix : int_overload_fea_suffixes) {
		intrinsic_info new_info = info;
		const std::string& cpp_type = suffix_to_cpp_type.at(fea_suffix);

		// Return type.
		{
			parameter_info& retp = new_info.return_parameter;
			if (!retp.direct_conversion) {
				retp.type = make_fea_register_type(retp.type, fea_suffix);
				retp.direct_conversion = true;
			}
		}

		for (parameter_info& p : new_info.parameters) {
			if (p.direct_conversion) {
				continue;
			}

			if (!fea::contains(p.type, '*') && !fea::contains(p.type, '&')) {
				// typical overload creation.
				p.type = make_fea_register_type(p.type, fea_suffix);
			} else {
				replace_type(p, cpp_type);
			}
			p.direct_conversion = true;
		}

		out.push_back(std::move(new_info));
	}
}

void generate_maskmove(
		intrinsic_info&& info, std::vector<intrinsic_info>& out) {
	// For mask move, we interpret the output char* as c++ overload output.
	// The mask is is 8bits however.

	// For the generate call, interpret mem_addr as void*.
	{
		auto it = std::find_if(info.parameters.begin(), info.parameters.end(),
				[](const parameter_info& p) { return p.type == "char*"; });
		assert(it != info.parameters.end());
		it->type = "void*";
		it->direct_conversion = false;
	}

	// Now do traditional cpp overload generation.
	assert(out.empty());
	generate_cpp_overloads(std::move(info), out);
	assert(!out.empty());

	// Finally, change all the masks to u8 and set the right cast.
	for (intrinsic_info& new_info : out) {
		auto it = std::find_if(new_info.parameters.begin(),
				new_info.parameters.end(),
				[](const parameter_info& p) { return p.varname == "mask"; });
		assert(it != new_info.parameters.end());

		std::string& masktype = it->type;
		size_t idx = masktype.find('_');
		masktype = masktype.substr(0, idx + 1);
		masktype += "u8_t";

		parameter_info& outp = new_info.parameters.back();
		assert(fea::contains(outp.type, '*'));
		outp.cast_str = "(char*)";
	}
}

void generate_cast(intrinsic_info&& info, std::vector<intrinsic_info>& out) {
	assert(info.suffix.suffixes.size() == 2);
	// const suffix_info::suf& from_suffix = info.suffix.suffixes.front();
	// const suffix_info::suf& to_suffix = info.suffix.suffixes.back();

	for (const std::string& new_suffix : int_overload_fea_suffixes) {
		intrinsic_info new_info = info;

		// Return type
		parameter_info& retp = new_info.parameters.back();
		if (!retp.direct_conversion) {
			retp.type = make_fea_register_type(retp.type, new_suffix);
			retp.direct_conversion = true;
		}

		// Params
		assert(new_info.parameters.size() >= 1);
		for (size_t i = 0; i < new_info.parameters.size() - 1; ++i) {
			parameter_info& p = new_info.parameters[i];
			if (!p.direct_conversion) {
				p.type = make_fea_register_type(p.type, new_suffix);
				p.direct_conversion = true;
			}
		}
		out.push_back(std::move(new_info));
	}
}

// void generate_suffix_plus_ptr_to_cpp(
//		intrinsic_info&& new_info, std::vector<intrinsic_info>& out) {
//	assert(new_info.suffix.suffixes.size() == 1);
//	suffix_info::suf suffix = new_info.suffix.suffixes.front();
//
//	if (force_unsigned_funcs.count(new_info.strippedfuncname) != 0) {
//		suffix.name = signed_to_unsigned_suffix.at(suffix.name);
//	}
//
//	// Return type
//	if (!new_info.return_parameter.direct_conversion) {
//		parameter_info& retp = new_info.return_parameter;
//		retp.type = make_fea_register_type(retp.type, suffix.name);
//		retp.direct_conversion = true;
//	}
//
//	// Params
//	for (parameter_info& p : new_info.parameters) {
//		if (!p.direct_conversion) {
//			if (fea::contains(p.type, '*') || fea::contains(p.type, '&')) {
//				p.type = suffix_to_cpp_type
//			} else {
//				p.type = make_fea_register_type(p.type, suffix.name);
//				p.direct_conversion = true;
//			}
//		}
//	}
//
//	out.push_back(std::move(new_info));
// }

bool is_suffix_func(const intrinsic_info& info) {
	if (!suffix_funcs.contains(info.strippedfuncname)) {
		return false;
	}

	if (info.suffix.suffixes.empty()) {
		return true;
	}

	return info.suffix.suffixes.front().name != "si128";
}

void generate_functions(
		const intrinsic_info& info, std::vector<intrinsic_info>& out) {
	// Check if we have manually entered overloads.
	// if (manual_overloads.contains(info.intrinname)) {
	//	const std::vector<std::vector<std::string>>& overloads
	//			= manual_overloads.at(info.intrinname);

	//	for (const std::vector<std::string>& overload : overloads) {
	//		intrinsic_info new_info = info;
	//		new_info.return_parameter.type = overload.front();

	//		for (size_t i = 0; i < new_info.parameters.size(); ++i) {
	//			parameter_info& param = new_info.parameters[i];
	//			param.type = overload[i + 1];
	//		}

	//		out.push_back(std::move(new_info));
	//	}
	//	return;
	//}

	intrinsic_info new_info = info;

	// If we have ouput parameters, place them at the end.
	{
		auto it = std::find_if(new_info.parameters.begin(),
				new_info.parameters.end(), [](const parameter_info& p) {
					return fea::contains(p.type, '*')
							&& !fea::contains(p.type, "const");
				});
		if (it != new_info.parameters.end()) {
			size_t idx = std::distance(new_info.parameters.begin(), it);
			if (idx != new_info.parameters.size() - 1) {
				it->first_passed = true;
				std::iter_swap(it,
						new_info.parameters.begin() + new_info.parameters.size()
								- 1);
			}
		}
	}

	// If we have to bool functions, change the return types.
	if (return_bool_funcs.contains(new_info.strippedfuncname)) {
		assert(new_info.return_parameter.direct_conversion);
		new_info.return_parameter.type = "bool";
	}

	// Return type
	if (new_info.return_parameter.direct_conversion) {
		parameter_info& ret_param = new_info.return_parameter;
		ret_param.type = direct_type_conversions.at(ret_param.type);
	}

	// Parameters.
	for (parameter_info& new_param : new_info.parameters) {
		if (new_param.direct_conversion) {
			new_param.type = direct_type_conversions.at(new_param.type);
		}
	}

	// Check if we need to convert some pointer parameters to references.
	for (const suffix_info::suf& s : new_info.suffix.suffixes) {
		if (!ptr_to_ref_suffix.contains(s.name)
				&& !ptr_to_ref_funcs.contains(new_info.strippedfuncname)) {
			continue;
		}

		for (parameter_info& p : new_info.parameters) {
			if (!fea::contains(p.type, '*')) {
				continue;
			}
			fea::replace_all_inplace(p.type, '*', '&');
			p.ref_to_ptr = true;
		}
	}

	// Move the return parameter to the last parameter of the signature.
	if (return_to_output_ref_funcs.contains(new_info.strippedfuncname)) {
		// Move the return parameter as an output parameter.
		new_info.parameters.push_back(new_info.return_parameter);
		new_info.return_parameter.e_type = etype::mvoid;
		new_info.return_parameter.type = "void";
		new_info.return_parameter.direct_conversion = true;

		// Setup our out param.
		parameter_info& lastp = new_info.parameters.back();
		lastp.type += "&";
		lastp.varname = "dst";
		lastp.output_ref = true;
	}

	// Set template params.
	if (template_args.contains(new_info.strippedfuncname)) {
		const std::string& arg_name
				= template_args.at(new_info.strippedfuncname);
		for (parameter_info& p : new_info.parameters) {
			if (p.varname == arg_name) {
				p.template_arg = true;
			}
		}
	}

	if (new_info.commented) {
		out.push_back(std::move(new_info));
		return;
	}

	if (as_is_funcs.contains(new_info.strippedfuncname)) {
		new_info.return_parameter.direct_conversion = true;
		for (parameter_info& p : new_info.parameters) {
			p.direct_conversion = true;
		}

		out.push_back(std::move(new_info));
		return;
	}

	// If everything was or is now direct, we are done.
	{
		bool all_direct = std::all_of(new_info.parameters.begin(),
				new_info.parameters.end(),
				[](const parameter_info& p) { return p.direct_conversion; });
		all_direct &= new_info.return_parameter.direct_conversion;

		if (all_direct) {
			out.push_back(std::move(new_info));
			return;
		}
	}

	if (fea::contains(new_info.intrinname, "pack")
			&& !fea::contains(new_info.intrinname, "unpack")) {
		return generate_pack(std::move(new_info), out);
	}
	if (fea::contains(new_info.intrinname, "sad")) {
		return generate_sad(std::move(new_info), out);
	}
	// if (fea::contains(new_info.intrinname, "divrem")) {
	//	return generate_divrem(std::move(new_info), out);
	// }
	if (fea::contains(new_info.intrinname, "maskmove")) {
		return generate_maskmove(std::move(new_info), out);
	}
	if (fea::contains(new_info.strippedfuncname, "cast")) {
		return generate_cast(std::move(new_info), out);
	}
	if (double_suffix_funcs.contains(new_info.strippedfuncname)) {
		return generate_double_suffix(std::move(new_info), out);
	}
	if (is_suffix_func(new_info)) {
		return generate_suffix_typed(std::move(new_info), out);
	}
	if (overload_funcs.contains(new_info.strippedfuncname)) {
		return generate_overloads(std::move(new_info), out);
	}
	if (cpp_overload_funcs.contains(new_info.strippedfuncname)) {
		return generate_cpp_overloads(std::move(new_info), out);
	}

	// out.push_back(std::move(new_info));
	assert(false);
}


std::string load_xml_str(const std::filesystem::path& bin_dir) {
	std::string ret;
	const std::filesystem::path data_filepath
			= bin_dir / "tests_data/intelintrinsicsguide.js";

	std::string data_str;
	fea::open_text_file_raw(data_filepath, data_str);

	if (data_str.empty()) {
		fprintf(stderr, "Problem reading file : '%s'\n",
				data_filepath.string().c_str());
		return ret;
	}

	size_t start_idx = data_str.find_first_of('"') + 1;
	size_t end_idx = data_str.find_last_of('"');
	ret = std::string(data_str.begin() + start_idx, data_str.begin() + end_idx);

	fea::replace_all_inplace(ret, "\\n\\", "");
	fea::replace_all_inplace(ret, "\\\"", "\"");
	return ret;
}

// Returns true on success.
bool load_doc(const std::filesystem::path& bin_dir, pugi::xml_document& doc) {
	const std::filesystem::path xml_cache_filepath
			= bin_dir / "intelinstrinsicsguide_cache.xml";

	// Check if we have an xml cache of the cleaned data.
	if (!std::filesystem::exists(xml_cache_filepath)) {
		std::string xml_str = load_xml_str(bin_dir);
		pugi::xml_parse_result res = doc.load_string(xml_str.c_str());
		if (!res) {
			fprintf(stderr, "pugixml : Couldn't parse xml string.\n");
			return false;
		}

		if (!doc.save_file(xml_cache_filepath.string().c_str())) {
			fprintf(stderr, "pugixml : Couldn't save xml document.\n");
			return false;
		}
	} else {
		doc.load_file(xml_cache_filepath.string().c_str());
	}
	return true;
}

using map_key_t = std::underlying_type_t<fea::simd_ver>;
fea::unsigned_map<map_key_t, std::vector<intrinsic_info>> deserialize_doc(
		const pugi::xml_document& doc) {
	printf("XML loaded, finding intrinsics...\n");

	fea::unsigned_map<map_key_t, std::vector<intrinsic_info>> ret;
	pugi::xml_node root = doc.first_child();
	assert(std::string{ root.name() } == "intrinsics_list");

	for (pugi::xml_node intrin : root.children("intrinsic")) {
		// Don't port non-SIMD intrinsics.
		// Q : Maybe we should, with simd_api<simd_ver::count> ?
		if (!intrin.child("CPUID")) {
			continue;
		}

		// Some _ph leaks in avx512f, without types however.
		// Since we would require _ph types, these are not included.
		std::string intrin_name = intrin.attribute("name").value();
		if (fea::ends_with(intrin_name, "_ph")) {
			continue;
		}

		// Some intrinsics are redundant (synonyms that call the same
		// instruction). Skip those we don't want.
		// Ex, _mm_cvt_ps2pi and _mm_cvtps_pi32
		if (ignored_intrins.contains(intrin_name)) {
			continue;
		}

		// Some instructions have multiple cpuids. Make sure we support
		// all of them.
		bool unsupported = false;
		for (pugi::xml_node cpuid_node : intrin.children("CPUID")) {
			std::string cpuid_str = cpuid_node.text().get();
			if (!supported_cpuids.contains(cpuid_str)) {
				unsupported = true;
				break;
			}
		}
		if (unsupported) {
			continue;
		}

		std::string cpuid_str = intrin.child("CPUID").text().get();
		map_key_t ver = map_key_t(supported_cpuids.at(cpuid_str));
		if (!ret.contains(ver)) {
			ret.insert({ ver, {} });
		}

		std::vector<intrinsic_info>& vec = ret.at_unchecked(ver);
		vec.push_back(intrinsic_info{ intrin });

		printf("\tFound : %s ", intrin.first_attribute().value());
		printf("[%s]\n", intrin.child("CPUID").text().get());
	}

	return ret;
}

bool param_type_compare(const std::string& lhs, const std::string& rhs) {
	std::string mlhs = lhs;
	std::string mrhs = rhs;

	fea::replace_all_inplace(mlhs, "m64_", "");
	fea::replace_all_inplace(mlhs, "m128_", "");
	fea::replace_all_inplace(mlhs, "m256_", "");
	fea::replace_all_inplace(mlhs, "m512_", "");
	fea::replace_all_inplace(mrhs, "m64_", "");
	fea::replace_all_inplace(mrhs, "m128_", "");
	fea::replace_all_inplace(mrhs, "m256_", "");
	fea::replace_all_inplace(mrhs, "m512_", "");

	auto get_info = [](std::string& str, char& prefix, int& val) {
		if (fea::contains(str, "_t")) {
			prefix = str.front();
			assert(prefix == 'f' || prefix == 'i' || prefix == 'u');
			fea::replace_all_inplace(str, prefix, "");

			size_t idx = str.find("_t");
			str = str.substr(0, idx);
			val = std::stoi(str);
		}
	};

	char lhs_prefix = '\0';
	int lhs_size = 0;
	char rhs_prefix = '\0';
	int rhs_size = 0;
	get_info(mlhs, lhs_prefix, lhs_size);
	get_info(mrhs, rhs_prefix, rhs_size);

	if (lhs_prefix == '\0' || rhs_prefix == '\0') {
		return mlhs < mrhs;
	}

	if (lhs_prefix == rhs_prefix) {
		return lhs_size < rhs_size;
	}
	return lhs_prefix < rhs_prefix;

	// if (lhs.size() == rhs.size()) {
	//	return lhs < rhs;
	// }
	// return lhs.size() < rhs.size();
}

bool param_compare(const intrinsic_info& lhs, const intrinsic_info& rhs) {
	const parameter_info& lhs_ret = lhs.return_parameter;
	const parameter_info& rhs_ret = rhs.return_parameter;

	if (lhs_ret.type == rhs_ret.type) {
		size_t count = std::min(lhs.parameters.size(), rhs.parameters.size());
		for (size_t i = 0; i < count; ++i) {
			const parameter_info& lhs_p = lhs.parameters[i];
			const parameter_info& rhs_p = rhs.parameters[i];
			if (lhs_p.type == rhs_p.type) {
				continue;
			}

			return param_type_compare(lhs_p.type, rhs_p.type);
		}
	}

	return param_type_compare(lhs_ret.type, rhs_ret.type);
}

fea::unsigned_map<map_key_t, std::vector<intrinsic_info>> create_overloads(
		const fea::unsigned_map<map_key_t, std::vector<intrinsic_info>>& in) {

	fea::unsigned_map<map_key_t, std::vector<intrinsic_info>> ret;
	for (const std::pair<map_key_t, std::vector<intrinsic_info>>& p : in) {
		ret.insert({ p.first, {} });
		ret.at(p.first).reserve(p.second.size());
	}

	std::vector<intrinsic_info> new_intrins;
	for (const std::pair<map_key_t, std::vector<intrinsic_info>>& p : in) {
		std::vector<intrinsic_info>& ret_vec = ret.at(p.first);

		for (const intrinsic_info& info : p.second) {
			new_intrins.clear();
			generate_functions(info, new_intrins);

#if FEA_DEBUG
			for (const intrinsic_info& new_info : new_intrins) {
				if (new_info.commented) {
					continue;
				}
				assert(!register_to_fea_prefix.contains(
						new_info.return_parameter.type));
				for (const parameter_info& param : new_info.parameters) {
					assert(!register_to_fea_prefix.contains(param.type));
					assert(param.direct_conversion == true);
				}
			}
#endif

			ret_vec.insert(
					ret_vec.end(), new_intrins.begin(), new_intrins.end());
		}

		std::sort(ret_vec.begin(), ret_vec.end(),
				[](const intrinsic_info& lhs, const intrinsic_info& rhs) {
					if (lhs.feafuncname == rhs.feafuncname) {
						return param_compare(lhs, rhs);
					}
					return lhs.feafuncname < rhs.feafuncname;
				});
	}

	return ret;
}

void save_headers(const std::filesystem::path& bin_dir,
		const fea::unsigned_map<map_key_t, std::vector<intrinsic_info>>& db) {
	std::string out;

	for (map_key_t i = 0; i < map_key_t(fea::simd_ver::count); ++i) {
		if (!db.contains(i)) {
			continue;
		}

		out = file_header;

		fea::simd_ver ver = fea::simd_ver(i);
		const std::string& filename = ver_to_filename[ver];

		if (ver == fea::simd_ver::mmx) {
			fea::replace_all_inplace(out, "{api_fwd}", api_base);
		} else {
			fea::replace_all_inplace(out, "{api_fwd}", api_fwd);
		}


		if (ver == fea::simd_ver::mmx) {
			fea::replace_all_inplace(out, "{prev_include}", "");
		} else {
			std::string prev_include = simd_prev_include;
			const std::string& prev_filename = ver_to_filename[ver - 1];
			fea::replace_all_inplace(
					prev_include, "{prev_filename}", prev_filename);
			fea::replace_all_inplace(out, "{prev_include}", prev_include);
		}

		const std::string& ver_str = ver_to_fea_enum_str[ver];
		const std::vector<intrinsic_info>& data = db.at(i);

		std::string struct_str = api_struct_template_header;
		fea::replace_all_inplace(struct_str, "{simd_api_ver}", ver_str);

		if (ver == fea::simd_ver::mmx) {
			fea::replace_all_inplace(struct_str, "{inheritance}", "");
			struct_str += "#if FEA_32BIT\n";
		} else {
			std::string inherit_str = api_struct_inheritance;
			const std::string& prev_ver_str = ver_to_fea_enum_str[ver - 1];
			fea::replace_all_inplace(
					inherit_str, "{simd_api_prev_ver}", prev_ver_str);
			fea::replace_all_inplace(struct_str, "{inheritance}", inherit_str);
		}

		for (const intrinsic_info& intrin : data) {
			struct_str += intrin.to_string();
		}

		if (ver == fea::simd_ver::mmx) {
			struct_str += "#endif\n";
		}

		struct_str += api_struct_template_footer;
		out += struct_str;

		std::string footer = file_footer;
		const std::string& api_name = ver_to_api_name[ver];
		fea::replace_all_inplace(footer, "{api_name}", api_name);
		fea::replace_all_inplace(footer, "{simd_api_ver}", ver_str);
		out += footer;

		const std::filesystem::path header_filepath = bin_dir / filename;
		std::ofstream ofs{ header_filepath };
		if (!ofs.is_open()) {
			fprintf(stderr, "Couldn't open file for writing : '%s'\n",
					header_filepath.string().c_str());
		}
		ofs << out;
	}
}

TEST(simd, generate_header) {
	const std::filesystem::path bin_dir = fea::executable_dir(argv0);

	pugi::xml_document doc;
	if (!load_doc(bin_dir, doc)) {
		return;
	}

	// All our intrsics data.
	fea::unsigned_map<map_key_t, std::vector<intrinsic_info>> db
			= deserialize_doc(doc);

	// To fix signature problems like multi-return types and collisions.
	db = create_overloads(db);

	// The header text.
	save_headers(bin_dir, db);
}

// tbd if this is better.
// enum class ropt : uint8_t {
//	untouched,
//	overloads,
//	template_,
//	ptr,
//	ref,
//	dot_xmm,
//	f32,
//	f64,
//	i8,
//	u8,
//	i16,
//	u16,
//	i32,
//	u32,
//	i64,
//	u64,
//	to_param,
//	count,
//};
//
// struct intrin_desc {
//	std::string intrin_name;
//	std::string func_name;
//	std::vector<ropt> return_opts;
//	std::vector<std::vector<ropt>> arg_opts;
//	bool m32bits = false;
//	bool commented = false;
//
//	// runtime appended
//	std::string comment;
//};
//
// const std::vector<intrin_desc> descriptions{
//	intrin_desc{
//			.intrin_name = "_mm_add_ps",
//			.func_name = "add",
//			.return_opts{ ropt::f32, ropt::ref, ropt::dot_xmm, ropt::to_param },
//			.arg_opts{
//					{ ropt::f32 },
//					{ ropt::f32 },
//			},
//	},
//	intrin_desc{
//			.intrin_name = "_mm_cvt_pi32_ps",
//			.func_name = "cvt",
//			.return_opts{ ropt::f32, ropt::ref, ropt::dot_xmm, ropt::to_param },
//			.arg_opts{
//					{ ropt::i32 },
//					{ ropt::i32 },
//			},
//	},
//};

#endif
} // namespace

#pragma once
#include <string>

// disabled for debugging
// const std::string file_prev_include
//		= "#include \"fea/performance/{prev_filename}\"";
const std::string simd_prev_include = "#include \"{prev_filename}\"";

const std::string simd_api_root = R"xx(template <fea::simd_ver>
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
};)xx";


const std::string prev_api_inherit = R"xx(
		: public simd_api<{simd_api_prev_ver}>
)xx";


// disabled for debugging
#if 1
const std::string simd_function_template = R"xx(
/*
{description}
{instruction}
{operation}
*/
{template}FEA_FORCEINLINE static {return_type} {func_name}({signature_params}) {
	{maybe_return}{intrin_name}({passed_params});
}
)xx";
#else
const std::string simd_function_template = R"xx(
{template}FEA_FORCEINLINE static {return_type} {func_name}({signature_params}) {
	{maybe_return}{intrin_name}({passed_params});
}
)xx";
#endif

const std::string simd_api_header = R"xx(/**
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
{simd_api_root}

template <>
struct simd_api<{simd_api_ver}> {prev_api_inherit}{
// Default register types for this api.
// Previous registers and functionality are still available.
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

{if_32bit}
{simd_api_contents}
{endif_32bit}

};

using simd_{api_name}_api = simd_api<{simd_api_ver}>;

} // namespace fea
)xx";

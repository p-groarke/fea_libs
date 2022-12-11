/*
BSD 3-Clause License

Copyright (c) 2022, Philippe Groarke
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once
#include "fea/performance/cpu_info.hpp"

#include <cstdint>

namespace fea {
namespace detail {
using simd_ver_enum_t = uint8_t;
}

// Supported simd versions.
enum class simd_ver : detail::simd_ver_enum_t {
	mmx,
	sse,
	sse2,
	sse3,
	ssse3,
	sse41,
	sse42,
	avx,
	avx2,
	avx512f,
	count,
};

inline constexpr bool operator<(simd_ver lhs, simd_ver rhs) noexcept {
	return detail::simd_ver_enum_t(lhs) < detail::simd_ver_enum_t(rhs);
}
inline constexpr bool operator>(simd_ver lhs, simd_ver rhs) noexcept {
	return rhs < lhs;
}
inline constexpr bool operator<=(simd_ver lhs, simd_ver rhs) noexcept {
	return !(rhs < lhs);
}
inline constexpr bool operator>=(simd_ver lhs, simd_ver rhs) noexcept {
	return !(lhs < rhs);
}
inline constexpr simd_ver operator-(simd_ver lhs, int rhs) noexcept {
	return simd_ver(detail::simd_ver_enum_t(lhs) - rhs);
}

// Tags for tag dispatch functionality.
//// Currently unused.
// struct simd_mmx_tag {};
// struct simd_sse_tag : public simd_mmx_tag {};
// struct simd_sse2_tag : public simd_sse_tag {};
// struct simd_sse3_tag : public simd_sse2_tag {};
// struct simd_ssse3_tag : public simd_sse3_tag {};
// struct simd_sse41_tag : public simd_ssse3_tag {};
// struct simd_sse42_tag : public simd_sse41_tag {};
// struct simd_avx_tag : public simd_sse42_tag {};
// struct simd_avx2_tag : public simd_avx_tag {};
// struct simd_avx512f_tag : public simd_avx2_tag {};


// Returns currently supported (runtime) simd version.
inline fea::simd_ver simd_runtime_version() {
	static const fea::cpu_info_t mcpu_info;

	auto get_ver_once = [](const fea::cpu_info_t& cpu_) {
		if (cpu_.avx512_f()) {
			return fea::simd_ver::avx512f;
		}
		if (cpu_.avx2()) {
			return fea::simd_ver::avx2;
		}
		if (cpu_.avx()) {
			return fea::simd_ver::avx;
		}
		if (cpu_.sse42()) {
			return fea::simd_ver::sse42;
		}
		if (cpu_.sse41()) {
			return fea::simd_ver::sse41;
		}
		if (cpu_.ssse3()) {
			return fea::simd_ver::ssse3;
		}
		if (cpu_.sse3()) {
			return fea::simd_ver::sse3;
		}
		if (cpu_.sse2()) {
			return fea::simd_ver::sse2;
		}
		if (cpu_.sse()) {
			return fea::simd_ver::sse;
		}
		if (cpu_.mmx()) {
			return fea::simd_ver::mmx;
		}
		return fea::simd_ver::count;
	};

	static const fea::simd_ver ret = get_ver_once(mcpu_info);
	return ret;
}
} // namespace fea

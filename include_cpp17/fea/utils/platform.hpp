﻿/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2024, Philippe Groarke
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
#include "fea/utils/bitmask.hpp"

#include <cstdint>

// Required for BSD check.
#if defined(__unix__) && !defined(__linux__)
#include <sys/param.h>
#endif

// Required for iphone check.
#if defined(__APPLE__) && defined(__MACH__)
#include <TargetConditionals.h>
#endif

// Required for posix check.
#if !defined(_WIN32) \
		&& (defined(__unix__) || defined(__unix) \
				|| (defined(__APPLE__) && defined(__MACH__)) \
				|| defined(__CYGWIN__))
#include <unistd.h>
#endif

namespace fea {
// Build customization (with / without libraries).
#undef FEA_WITH_TBB
#undef FEA_WITH_DATE
#define FEA_WITH_TBB 0
#define FEA_WITH_DATE 0

#if defined(FEA_WITH_TBB_DEF)
#undef FEA_WITH_TBB
#define FEA_WITH_TBB 1
#endif

#if defined(FEA_WITH_DATE_DEF)
#undef FEA_WITH_DATE
#define FEA_WITH_DATE 1
#endif

// Reset everything and force #if FEA_BLA to prevent missing include.
#undef FEA_CPP23
#undef FEA_CPP20
#undef FEA_CPP17
#undef FEA_CPP14
#undef FEA_CPP11
#undef FEA_CPP98
#define FEA_CPP23 0
#define FEA_CPP20 0
#define FEA_CPP17 0
#define FEA_CPP14 0
#define FEA_CPP11 0
#define FEA_CPP98 0

// Nicer way to check for C++ versions.
#if __cplusplus >= 202302L
#undef FEA_CPP23
#define FEA_CPP23 1
#endif

#if __cplusplus >= 202002L
#undef FEA_CPP20
#define FEA_CPP20 1
#endif

#if __cplusplus >= 201703L
#undef FEA_CPP17
#define FEA_CPP17 1
#endif

#if __cplusplus >= 201402L
#undef FEA_CPP14
#define FEA_CPP14 1
#endif

#if __cplusplus >= 201103L
#undef FEA_CPP11
#define FEA_CPP11 1
#endif

#if __cplusplus >= 199711L
#undef FEA_CPP98
#define FEA_CPP98 1
#endif


/**
 * Helper Defines
 */
// Are we building in 32 bits or 64 bits?
#undef FEA_ARCH
#undef FEA_8BIT
#undef FEA_16BIT
#undef FEA_32BIT
#undef FEA_64BIT
#undef FEA_128BIT
#undef FEA_256BIT
#define FEA_ARCH 0
#define FEA_8BIT 0
#define FEA_16BIT 0
#define FEA_32BIT 0
#define FEA_64BIT 0
#define FEA_128BIT 0
#define FEA_256BIT 0

#if INTPTR_MAX == INT8_MAX
#undef FEA_ARCH
#undef FEA_8BIT
#define FEA_ARCH 8
#define FEA_8BIT 1
inline constexpr size_t arch = 8u;
#elif INTPTR_MAX == INT16_MAX
#undef FEA_ARCH
#undef FEA_16BIT
#define FEA_ARCH 16
#define FEA_16BIT 1
inline constexpr size_t arch = 16u;
#elif INTPTR_MAX == INT32_MAX
#undef FEA_ARCH
#undef FEA_32BIT
#define FEA_ARCH 32
#define FEA_32BIT 1
inline constexpr size_t arch = 32u;
#elif INTPTR_MAX == INT64_MAX
#undef FEA_ARCH
#undef FEA_64BIT
#define FEA_ARCH 64
#define FEA_64BIT 1
inline constexpr size_t arch = 64u;
// #elif INTPTR_MAX == INT128_MAX
// #undef FEA_ARCH
// #undef FEA_128BIT
// #define FEA_ARCH 128
// #define FEA_128BIT 1
// inline constexpr size_t arch = 128u;
// #elif INTPTR_MAX == INT256_MAX
// #undef FEA_ARCH
// #undef FEA_256BIT
// #define FEA_ARCH 256
// #define FEA_256BIT 1
// inline constexpr size_t arch = 256u;
#else
static_assert(false, "fea::arch : Missing architecture.");
#endif

// Disables exceptions in classes that support it.
// TODO : Review all classes that throw, make sure exceptions can be disabled,
// or trigger build warning if they can't and this is set.
#undef FEA_NOTHROW
#define FEA_NOTHROW 0

#if defined(FEA_NOTHROW_DEF)
#undef FEA_NOTHROW
#define FEA_NOTHROW 1
inline constexpr bool nothrow_build = true;
#else
inline constexpr bool nothrow_build = false;
#endif

// Allows using MSVC regions without causing errors on other OSes.
#define FEA_REGION(...)

// NDEBUG is often double negative, offer alternatives.
#undef FEA_RELEASE
#undef FEA_DEBUG
#define FEA_RELEASE 0
#define FEA_DEBUG 0

#if defined(NDEBUG)
#undef FEA_RELEASE
#define FEA_RELEASE 1
inline constexpr bool release_build = true;
inline constexpr bool debug_build = false;
#else
#undef FEA_DEBUG
#define FEA_DEBUG 1
inline constexpr bool release_build = false;
inline constexpr bool debug_build = true;
#endif

// Used to set index type in serialization. Driven by build.
#undef FEA_SERIALIZE_SIZE_T
#define FEA_SERIALIZE_SIZE_T size_t

#if defined(FEA_SERIALIZE_SIZE_T_DEF)
#undef FEA_SERIALIZE_SIZE_T
#define FEA_SERIALIZE_SIZE_T FEA_SERIALIZE_SIZE_T_DEF
#endif

/**
 * OS Detection
 */

// Detect the OS precisely and set both defines and a global constexpr enum.

enum class platform_t : unsigned {
	aix,
	bsd,
	hpux,
	ios,
	linuxx,
	macos,
	solaris,
	windows,
	emscripten,
	count,
};

enum class platform_group_t : unsigned {
	posix = 0b0001,
	unixx = 0b0010,
	count = 0b0000,
};
FEA_REGISTER_BITMASK(platform_group_t);
} // namespace fea

namespace fea {
#undef FEA_AIX
#undef FEA_BSD
#undef FEA_HPUX
#undef FEA_LINUX
#undef FEA_IOS
#undef FEA_MACOS
#undef FEA_SOLARIS
#undef FEA_WINDOWS
#undef FEA_EMSCRIPTEN
#undef FEA_POSIX
#undef FEA_UNIX
#define FEA_AIX 0
#define FEA_BSD 0
#define FEA_HPUX 0
#define FEA_LINUX 0
#define FEA_IOS 0
#define FEA_MACOS 0
#define FEA_SOLARIS 0
#define FEA_WINDOWS 0
#define FEA_EMSCRIPTEN 0
#define FEA_POSIX 0
#define FEA_UNIX 0

#if defined(_AIX)
#undef FEA_AIX
#define FEA_AIX 1
inline constexpr platform_t platform = platform_t::aix;

#elif defined(BSD) || defined(__FreeBSD__)
#undef FEA_BSD
#define FEA_BSD 1
inline constexpr platform_t platform = platform_t::bsd;

#elif defined(__hpux)
#undef FEA_HPUX
#define FEA_HPUX 1
inline constexpr platform_t platform = platform_t::hpux;

#elif defined(__linux__)
#undef FEA_LINUX
#define FEA_LINUX 1
inline constexpr platform_t platform = platform_t::linuxx;

#elif defined(__APPLE__) && defined(__MACH__) && TARGET_OS_IPHONE == 1
#undef FEA_IOS
#define FEA_IOS 1
inline constexpr platform_t platform = platform_t::ios;

#elif defined(__APPLE__) && defined(__MACH__) && TARGET_OS_MAC == 1
#undef FEA_MACOS
#define FEA_MACOS 1
inline constexpr platform_t platform = platform_t::macos;

#elif defined(__sun) && defined(__SVR4)
#undef FEA_SOLARIS
#define FEA_SOLARIS 1
inline constexpr platform_t platform = platform_t::solaris;

#elif defined(_WIN32)
#undef FEA_WINDOWS
#define FEA_WINDOWS 1
inline constexpr platform_t platform = platform_t::windows;

#elif defined(__EMSCRIPTEN__)
#undef FEA_EMSCRIPTEN
#define FEA_EMSCRIPTEN 1
inline constexpr platform_t platform = platform_t::emscripten;

#else
// We are on an unknown platform :scream:
inline constexpr platform_t platform = platform_t::count;
#endif


// Check for posix.
#if defined(_POSIX_VERSION)
#undef FEA_POSIX
#define FEA_POSIX 1
#undef FEA_UNIX
#define FEA_UNIX 1
inline constexpr platform_group_t platform_group
		= platform_group_t::posix | platform_group_t::unixx;

#elif !defined(_WIN32) \
		&& (defined(__unix__) || defined(__unix) \
				|| (defined(__APPLE__) && defined(__MACH__)) \
				|| defined(__CYGWIN__))
#undef FEA_UNIX
#define FEA_UNIX 1
inline constexpr platform_group_t platform_group = platform_group_t::unixx;

#else
inline constexpr platform_group_t platform_group = platform_group_t::count;
#endif

// Architecture.
#undef FEA_ARM
#undef FEA_X86
#define FEA_ARM 0
#define FEA_X86 0
#if defined(__arm__) || defined(__arm) || defined(__arm64__) \
		|| defined(__arm64) \
		|| (defined(TARGET_CPU_ARM64) && TARGET_CPU_ARM64 == 1)
#undef FEA_ARM
#define FEA_ARM 1
#else
#undef FEA_X86
#define FEA_X86 1
#endif

// Compiler identification.
#undef FEA_MSVC
#undef FEA_CLANG
#undef FEA_GCC
#define FEA_MSVC 0
#define FEA_CLANG 0
#define FEA_GCC 0

#if defined(_MSC_VER)
#undef FEA_MSVC
#define FEA_MSVC 1
#elif defined(__clang__) || defined(__llvm__)
// Call before gnuc check.
#undef FEA_CLANG
#define FEA_CLANG 1
#elif defined(__GNUC__)
#undef FEA_GCC
#define FEA_GCC 1
#endif

// VS versions (years).
// Returns true on other compilers.
#undef FEA_VS_GT
#undef FEA_VS_GE
#undef FEA_VS_EQ
#undef FEA_VS_LE
#undef FEA_VS_LT
#define FEA_VS_GT(year) 1
#define FEA_VS_GE(year) 1
#define FEA_VS_EQ(year) 1
#define FEA_VS_LE(year) 1
#define FEA_VS_LT(year) 1

#if FEA_MSVC
#if _MSC_VER >= 1930 && _MSC_VER < 1940
#undef FEA_DETAIL_VS_YEAR
#define FEA_DETAIL_VS_YEAR 2022
#elif _MSC_VER >= 1920 && _MSC_VER < 1930
#undef FEA_DETAIL_VS_YEAR
#define FEA_DETAIL_VS_YEAR 2019
#elif _MSC_VER >= 1910 && _MSC_VER < 1920
#undef FEA_DETAIL_VS_YEAR
#define FEA_DETAIL_VS_YEAR 2017
#elif _MSC_VER >= 1900 && _MSC_VER < 1910
#undef FEA_DETAIL_VS_YEAR
#define FEA_DETAIL_VS_YEAR 2015
#endif

#undef FEA_VS_GT
#undef FEA_VS_GE
#undef FEA_VS_EQ
#undef FEA_VS_LE
#undef FEA_VS_LT
#define FEA_VS_GT(year) FEA_DETAIL_VS_YEAR > year
#define FEA_VS_GE(year) FEA_DETAIL_VS_YEAR >= year
#define FEA_VS_EQ(year) FEA_DETAIL_VS_YEAR == year
#define FEA_VS_LE(year) FEA_DETAIL_VS_YEAR <= year
#define FEA_VS_LT(year) FEA_DETAIL_VS_YEAR < year
#endif

// GCC versions.
// Returns true on other compilers.
#undef FEA_GCC_GT
#undef FEA_GCC_GE
#undef FEA_GCC_EQ
#undef FEA_GCC_LE
#undef FEA_GCC_LT
#define FEA_GCC_GT(major_ver) 1
#define FEA_GCC_GE(major_ver) 1
#define FEA_GCC_EQ(major_ver) 1
#define FEA_GCC_LE(major_ver) 1
#define FEA_GCC_LT(major_ver) 1

#if FEA_GCC
#undef FEA_GCC_GT
#undef FEA_GCC_GE
#undef FEA_GCC_EQ
#undef FEA_GCC_LE
#undef FEA_GCC_LT
#define FEA_GCC_GT(major_ver) __GNUC__ > major_ver
#define FEA_GCC_GE(major_ver) __GNUC__ >= major_ver
#define FEA_GCC_EQ(major_ver) __GNUC__ == major_ver
#define FEA_GCC_LE(major_ver) __GNUC__ <= major_ver
#define FEA_GCC_LT(major_ver) __GNUC__ < major_ver
#endif

//// Cross-platform compiler warning.
//// Note MSVC isn't a true warning, just a message.
// #if FEA_WINDOWS
// #define FEA_WARNING(x) __pragma(message("warning : "##x))
// #else
// #define FEA_WARNING(x) #warning x
// #endif

// Strict data packing for cross-platform/cross-compiler support.
// Use like so :
// FEA_PACKED(struct my_struct {});
#if FEA_WINDOWS
#undef FEA_PACKED
#define FEA_PACKED(...) __pragma(pack(push, 1)) __VA_ARGS__ __pragma(pack(pop))
#else
#undef FEA_PACKED
#define FEA_PACKED(...) __VA_ARGS__ __attribute__((__packed__))
#endif

} // namespace fea

// Fail in consteval context, assert at runtime.
#if FEA_CPP20
#include <cassert>
#include <type_traits>
namespace fea {
#define FEA_CONSTEXPR_ASSERT(expression) \
	do { \
		if (std::is_constant_evaluated()) { \
			if (!(expression)) \
				throw 1; \
		} else { \
			assert(!!(expression)); \
		} \
	} while (0)
} // namespace fea
#endif

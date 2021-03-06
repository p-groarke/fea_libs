﻿/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2020, Philippe Groarke
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

namespace fea {
// Nicer way to check for C++ versions.
#if __cplusplus >= 202002L
#undef FEA_CPP20
#undef FEA_CPP17
#undef FEA_CPP14
#undef FEA_CPP11
#undef FEA_CPP98
#define FEA_CPP20 1
#define FEA_CPP17 1
#define FEA_CPP14 1
#define FEA_CPP11 1
#define FEA_CPP98 1
#elif __cplusplus >= 201703L
#undef FEA_CPP17
#undef FEA_CPP14
#undef FEA_CPP11
#undef FEA_CPP98
#define FEA_CPP17 1
#define FEA_CPP14 1
#define FEA_CPP11 1
#define FEA_CPP98 1
#elif __cplusplus >= 201402L
#undef FEA_CPP14
#undef FEA_CPP11
#undef FEA_CPP98
#define FEA_CPP14 1
#define FEA_CPP11 1
#define FEA_CPP98 1
#elif __cplusplus >= 201103L
#undef FEA_CPP11
#undef FEA_CPP98
#define FEA_CPP11 1
#define FEA_CPP98 1
#else
#undef FEA_CPP98
#define FEA_CPP98 1
#endif


/**
 * Helper Defines
 */
// C++17 offers inline variables, but they can be replaced by static vars in
// older versions.
#if FEA_CPP17
#undef FEA_INLINE_VAR
#define FEA_INLINE_VAR inline
#else
#undef FEA_INLINE_VAR
#define FEA_INLINE_VAR static
#endif

// Disables exceptions in classes that support it.
// TODO : Review all classes that throw, make sure exceptions can be disabled,
// or trigger build warning if they can't and this is set.
#if defined(FEA_NOTHROW)
#undef FEA_NOTHROW
#define FEA_NOTHROW 1
FEA_INLINE_VAR constexpr bool nothrow_build = true;
#else
FEA_INLINE_VAR constexpr bool nothrow_build = false;
#endif

// Allows using MSVC regions without causing errors on other OSes.
#define FEA_REGION(...)

// NDEBUG is often double negative, offer alternatives.
#if defined(NDEBUG)
#undef FEA_RELEASE
#define FEA_RELEASE 1

FEA_INLINE_VAR constexpr bool release_build = true;
FEA_INLINE_VAR constexpr bool debug_build = false;

#else
#undef FEA_DEBUG
#define FEA_DEBUG 1

FEA_INLINE_VAR constexpr bool release_build = false;
FEA_INLINE_VAR constexpr bool debug_build = true;
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
	count,
};

enum class platform_group_t : unsigned {
	posix = 0b0001,
	unixx = 0b0010,
	count = 0b0000,
};
FEA_ENABLE_BITMASK_OPERATORS(platform_group_t)
} // namespace fea
FEA_ENABLE_IS_BITMASK(platform_group_t)

namespace fea {
#if defined(_AIX)
#undef FEA_AIX
#define FEA_AIX 1

FEA_INLINE_VAR constexpr platform_t platform = platform_t::aix;

#elif defined(__unix__)
} // namespace fea
#include <sys/param.h>
namespace fea {

#if defined(BSD)
#undef FEA_BSD
#define FEA_BSD 1

FEA_INLINE_VAR constexpr platform_t platform = platform_t::bsd;
#endif

#elif defined(__hpux)
#undef FEA_HPUX
#define FEA_HPUX 1

FEA_INLINE_VAR constexpr platform_t platform = platform_t::hpux;

#elif defined(__linux__)
#undef FEA_LINUX
#define FEA_LINUX 1

FEA_INLINE_VAR constexpr platform_t platform = platform_t::linuxx;

#elif defined(__APPLE__) && defined(__MACH__)
} // namespace fea
#include <TargetConditionals.h>
namespace fea {

#if TARGET_OS_IPHONE == 1
#undef FEA_IOS
#define FEA_IOS 1

FEA_INLINE_VAR constexpr platform_t platform = platform_t::ios;

#elif TARGET_OS_MAC == 1
#undef FEA_MACOS
#define FEA_MACOS 1

FEA_INLINE_VAR constexpr platform_t platform = platform_t::macos;
#endif

#elif defined(__sun) && defined(__SVR4)
#undef FEA_SOLARIS
#define FEA_SOLARIS 1

FEA_INLINE_VAR constexpr platform_t platform = platform_t::solaris;

#elif defined(_WIN32)
#undef FEA_WINDOWS
#define FEA_WINDOWS 1

FEA_INLINE_VAR constexpr platform_t platform = platform_t::windows;

#else
FEA_INLINE_VAR constexpr platform_t platform = platform_t::count;
#endif

#if !defined(_WIN32) \
		&& (defined(__unix__) || defined(__unix) \
				|| (defined(__APPLE__) && defined(__MACH__)) \
				|| defined(__CYGWIN__))
} // namespace fea
#include <unistd.h>
namespace fea {

#if defined(_POSIX_VERSION)
#undef FEA_POSIX
#define FEA_POSIX 1
#undef FEA_UNIX
#define FEA_UNIX 1

FEA_INLINE_VAR constexpr platform_group_t platform_group
		= platform_group_t::posix | platform_group_t::unixx;

#else
#undef FEA_UNIX
#define FEA_UNIX 1

FEA_INLINE_VAR constexpr platform_group_t platform_group
		= platform_group_t::unixx;
#endif

#else
FEA_INLINE_VAR constexpr platform_group_t platform_group
		= platform_group_t::count;
#endif


//// Cross-platform compiler warning.
//// Note MSVC isn't a true warning, just a message.
//#if FEA_WINDOWS
//#define FEA_WARNING(x) __pragma(message("warning : "##x))
//#else
//#define FEA_WARNING(x) #warning x
//#endif

} // namespace fea

/**
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


#if defined(NDEBUG)
#undef FEA_RELEASE_BUILD
#define FEA_RELEASE_BUILD 1

#if FEA_CPP17
inline constexpr bool debug_build = false;
#endif

#else
#undef FEA_DEBUG_BUILD
#define FEA_DEBUG_BUILD 1

#if FEA_CPP17
inline constexpr bool debug_build = true;
#endif
#endif


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

#if FEA_CPP17
inline constexpr platform_t platform = platform_t::aix;
#endif

#elif defined(__unix__)
}
#include <sys/param.h>
namespace fea {

#if defined(BSD)
#undef FEA_BSD
#define FEA_BSD 1

#if FEA_CPP17
inline constexpr platform_t platform = platform_t::bsd;
#endif
#endif

#elif defined(__hpux)
#undef FEA_HPUX
#define FEA_HPUX 1

#if FEA_CPP17
inline constexpr platform_t platform = platform_t::hpux;
#endif

#elif defined(__linux__)
#undef FEA_LINUX
#define FEA_LINUX 1

#if FEA_CPP17
inline constexpr platform_t platform = platform_t::linuxx;
#endif

#elif defined(__APPLE__) && defined(__MACH__)
}
#include <TargetConditionals.h>
namespace fea {

#if TARGET_OS_IPHONE == 1
#undef FEA_IOS
#define FEA_IOS 1

#if FEA_CPP17
inline constexpr platform_t platform = platform_t::ios;
#endif

#elif TARGET_OS_MAC == 1
#undef FEA_MACOS
#define FEA_MACOS 1

#if FEA_CPP17
inline constexpr platform_t platform = platform_t::macos;
#endif
#endif

#elif defined(__sun) && defined(__SVR4)
#undef FEA_SOLARIS
#define FEA_SOLARIS 1

#if FEA_CPP17
inline constexpr platform_t platform = platform_t::solaris;
#endif

#elif defined(_WIN32)
#undef FEA_WINDOWS
#define FEA_WINDOWS 1

#if FEA_CPP17
inline constexpr platform_t platform = platform_t::windows;
#endif

#else
#if FEA_CPP17
inline constexpr platform_t platform = platform_t::count;
#endif
#endif

#if !defined(_WIN32) \
		&& (defined(__unix__) || defined(__unix) \
				|| (defined(__APPLE__) && defined(__MACH__)) \
				|| defined(__CYGWIN__))
}
#include <unistd.h>
namespace fea {

#if defined(_POSIX_VERSION)
#undef FEA_POSIX
#define FEA_POSIX 1
#undef FEA_UNIX
#define FEA_UNIX 1

#if FEA_CPP17
inline constexpr platform_group_t platform_group
		= platform_group_t::posix | platform_group_t::unixx;
#endif

#else
#undef FEA_UNIX
#define FEA_UNIX 1

#if FEA_CPP17
inline constexpr platform_group_t platform_group = platform_group_t::unixx;
#endif
#endif

#else
#if FEA_CPP17
inline constexpr platform_group_t platform_group = platform_group_t::count;
#endif
#endif

} // namespace fea

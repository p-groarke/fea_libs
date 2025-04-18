/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2025, Philippe Groarke
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
#include "fea/utils/platform.hpp"

#include <cstdint>

#if FEA_WITH_TBB
#include <tbb/partitioner.h>
#endif

/*
Specialize the traits in the fea namespace to override the defaults.
Do this before usage.

Example :
namespace fea {
template <>
struct default_grainsize<true> {
	static constexpr size_t value = 200;
};
}
*/

namespace fea {
#if FEA_WITH_TBB
template <bool Overridden>
struct default_grainsize_small {
	static constexpr size_t value = 1u;
};
template <>
struct default_grainsize_small<false>;

template <bool B>
inline constexpr size_t default_grainsize_small_v
		= default_grainsize_small<B>::value;


template <bool Overridden>
struct default_grainsize_medium {
	static constexpr size_t value = 100u;
};
template <>
struct default_grainsize_medium<false>;

template <bool B>
inline constexpr size_t default_grainsize_medium_v
		= default_grainsize_medium<B>::value;


template <bool Overridden>
struct default_grainsize_large {
	static constexpr size_t value = 1000u;
};
template <>
struct default_grainsize_large<false>;

template <bool B>
inline constexpr size_t default_grainsize_large_v
		= default_grainsize_large<B>::value;


template <bool Overridden>
struct default_partitioner {
	using type = tbb::static_partitioner;
};
template <>
struct default_partitioner<false>;

template <bool B>
using default_partitioner_t = typename default_partitioner<B>::type;
#endif

} // namespace fea

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
#include "fea/simd/simd_api_mmx.hpp"
#include "fea/simd/simd_api_sse.hpp"
#include "fea/simd/simd_api_sse2.hpp"
#include "fea/simd/simd_register.hpp"
#include "fea/simd/simd_version.hpp"
#include "fea/utils/platform.hpp"

#include <array>
#include <cassert>
#include <limits>
#include <tuple>
#include <type_traits>

namespace fea {
template <fea::simd_ver>
struct simd_api;

namespace detail {
template <simd_ver... Versions>
size_t best_version_idx(fea::simd_ver runtime_ver) {
	constexpr size_t sentinel = (std::numeric_limits<size_t>::max)();
	std::array<simd_ver, sizeof...(Versions)> requested_versions{ Versions... };

	size_t ret = sentinel;
	size_t dist = sentinel;
	for (size_t i = 0; i < requested_versions.size(); ++i) {
		simd_ver requested_version = requested_versions[i];
		if (requested_version > runtime_ver) {
			continue;
		}

		size_t delta = size_t(runtime_ver) - size_t(requested_version);
		if (delta == 0) {
			// Perfect match.
			return i;
		}

		if (delta < dist) {
			dist = delta;
			ret = i;
		}
	}

	// Confirm our sanity has not yet fully escaped.
	assert(ret != sentinel || dist == sentinel);
	return ret;
}

// C++14, tag dispatch.
// Exists.
template <size_t Idx, class ApiTupT, class Func>
decltype(auto) maybe_call_simd_func(Func&& func, std::true_type) {
	using api_t = typename std::tuple_element<Idx, ApiTupT>::type;
	return std::forward<Func>(func)(api_t{});
}

// Doesn't exist.
template <size_t, class, class Func>
decltype(auto) maybe_call_simd_func(Func&&, std::false_type) {
	assert(false);
}
} // namespace detail

// Compiles your SIMD callback for all target versions,
// but at runtime, only executes the callback overload for the highest supported
// target version.
// If no SIMD version is supported, calls the plain callback.
template <simd_ver... TargetVersions, class SIMDCallback, class PlainCallback>
decltype(auto) simd_run(SIMDCallback&& simd_func, PlainCallback&& plain_func) {
	static_assert(sizeof...(TargetVersions) > 0,
			"fea::simd_run : Number of target apis must be greater than 0.");

	// Can be size_t max if unmatched.
	static const size_t tup_idx = detail::best_version_idx<TargetVersions...>(
			fea::simd_runtime_version());

	using api_tup_t = std::tuple<simd_api<TargetVersions>...>;
	constexpr size_t tup_size = std::tuple_size<api_tup_t>::value;

	switch (tup_idx) {
	case 0: {
		constexpr size_t idx = 0;
		constexpr bool callit = idx < tup_size;
		return detail::maybe_call_simd_func<idx, api_tup_t>(
				std::forward<SIMDCallback>(simd_func),
				std::integral_constant<bool, callit>{});
	} break;
	case 1: {
		constexpr size_t idx = 1;
		constexpr bool callit = idx < tup_size;
		return detail::maybe_call_simd_func<idx, api_tup_t>(
				std::forward<SIMDCallback>(simd_func),
				std::integral_constant<bool, callit>{});
	} break;
	case 2: {
		constexpr size_t idx = 2;
		constexpr bool callit = idx < tup_size;
		return detail::maybe_call_simd_func<idx, api_tup_t>(
				std::forward<SIMDCallback>(simd_func),
				std::integral_constant<bool, callit>{});
	} break;
	case 3: {
		constexpr size_t idx = 3;
		constexpr bool callit = idx < tup_size;
		return detail::maybe_call_simd_func<idx, api_tup_t>(
				std::forward<SIMDCallback>(simd_func),
				std::integral_constant<bool, callit>{});
	} break;
	case 4: {
		constexpr size_t idx = 4;
		constexpr bool callit = idx < tup_size;
		return detail::maybe_call_simd_func<idx, api_tup_t>(
				std::forward<SIMDCallback>(simd_func),
				std::integral_constant<bool, callit>{});
	} break;
	case 5: {
		constexpr size_t idx = 5;
		constexpr bool callit = idx < tup_size;
		return detail::maybe_call_simd_func<idx, api_tup_t>(
				std::forward<SIMDCallback>(simd_func),
				std::integral_constant<bool, callit>{});
	} break;
	case 6: {
		constexpr size_t idx = 6;
		constexpr bool callit = idx < tup_size;
		return detail::maybe_call_simd_func<idx, api_tup_t>(
				std::forward<SIMDCallback>(simd_func),
				std::integral_constant<bool, callit>{});
	} break;
	case 7: {
		constexpr size_t idx = 7;
		constexpr bool callit = idx < tup_size;
		return detail::maybe_call_simd_func<idx, api_tup_t>(
				std::forward<SIMDCallback>(simd_func),
				std::integral_constant<bool, callit>{});
	} break;
	case 8: {
		constexpr size_t idx = 8;
		constexpr bool callit = idx < tup_size;
		return detail::maybe_call_simd_func<idx, api_tup_t>(
				std::forward<SIMDCallback>(simd_func),
				std::integral_constant<bool, callit>{});
	} break;
	case 9: {
		constexpr size_t idx = 9;
		constexpr bool callit = idx < tup_size;
		return detail::maybe_call_simd_func<idx, api_tup_t>(
				std::forward<SIMDCallback>(simd_func),
				std::integral_constant<bool, callit>{});
	} break;
	default: {
		static_assert(size_t(fea::simd_ver::count) == 10,
				"fea::simd_run : When adding simd versions, this "
				"switch-case must be updated.");
		return std::forward<PlainCallback>(plain_func)();
	} break;
	}
}
} // namespace fea

/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2023, Philippe Groarke
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
#include "fea/enum/enum_array.hpp"
#include "fea/meta/pack.hpp"

#include <array>
#include <tuple>
#include <type_traits>

namespace fea {
//// This is a compile-time var filter.
// template <class VEnum, bool... Args>
// struct cexpr_var_filter {
// private:
//	template <bool>
//	static constexpr bool make_true() {
//		return true;
//	}
//
//	template <bool>
//	static constexpr bool make_false() {
//		return true;
//	}
//
//	template <bool Val, size_t CurrentIdx, size_t... TargetIdxes>
//	static constexpr bool flip_true_imp(fea::pack_nt<TargetIdxes...>) {
//		constexpr bool is_a_target = ((CurrentIdx == TargetIdxes) || ...);
//		if constexpr (is_a_target) {
//			return true;
//		} else {
//			return Val;
//		}
//	}
//
//	template <size_t... TargetIdxes, bool... Vals, size_t... Idxes>
//	static constexpr auto flip_true(fea::pack_nt<TargetIdxes...> pack,
//			fea::pack_nt<Vals...>, std::index_sequence<Idxes...>) {
//		// auto pack = fea::pack_nt<TargetIdxes...>{};
//		return cexpr_var_filter<VEnum, flip_true_imp<Vals, Idxes>(pack)...>{};
//	}
//
//	template <bool Val, size_t CurrentIdx, size_t... TargetIdxes>
//	static constexpr bool flip_false_imp(fea::pack_nt<TargetIdxes...>) {
//		constexpr bool is_a_target = ((CurrentIdx == TargetIdxes) || ...);
//		if constexpr (is_a_target) {
//			return false;
//		} else {
//			return Val;
//		}
//	}
//
//	template <size_t... TargetIdxes, bool... Vals, size_t... Idxes>
//	static constexpr auto flip_false(fea::pack_nt<TargetIdxes...> pack,
//			fea::pack_nt<Vals...>, std::index_sequence<Idxes...>) {
//		// auto pack = fea::pack_nt<TargetIdxes...>{};
//		return cexpr_var_filter<VEnum, flip_false_imp<Vals, Idxes>(pack)...>{};
//	}
//
// public:
//	static constexpr auto enable_all() {
//		return cexpr_var_filter<VEnum, make_true<Args>()...>{};
//	}
//
//	static constexpr auto disable_all() {
//		return cexpr_var_filter<VEnum, make_false<Args>()...>{};
//	}
//
//	template <VEnum... Es>
//	static constexpr auto enable() {
//		return flip_true(fea::pack_nt<size_t(Es)...>{}, fea::pack_nt<Args...>{},
//				std::make_index_sequence<sizeof...(Args)>{});
//	}
//
//	template <VEnum... Es>
//	static constexpr auto disable() {
//		return flip_false(fea::pack_nt<size_t(Es)...>{},
//				fea::pack_nt<Args...>{},
//				std::make_index_sequence<sizeof...(Args)>{});
//	}
//
//	template <VEnum E>
//	static constexpr bool at() {
//		return fea::pack_element_nt_v<size_t(E), fea::pack_nt<Args...>>;
//	}
//};
//
// namespace detail {
// template <bool Val, size_t>
// constexpr bool make_val() {
//	return Val;
//}
//
// template <class VEnum, bool Val, size_t... Idxes>
// constexpr auto make_var_filter(std::index_sequence<Idxes...>) {
//	return cexpr_var_filter<VEnum, make_val<Val, Idxes>()...>{};
//}
//} // namespace detail
//
//// Creates a var_filter with Val filled in, of Size.
// template <class VEnum, bool Val, size_t Size>
// constexpr auto make_var_filter() {
//	return detail::make_var_filter<VEnum, Val>(
//			std::make_index_sequence<Size>{});
//}

// A runtime variable filter.
template <class VEnum>
struct var_filter {
	constexpr var_filter<VEnum>& enable_all() {
		std::fill(_data.begin(), _data.end(), true);
		return *this;
	}
	constexpr var_filter<VEnum>& disable_all() {
		std::fill(_data.begin(), _data.end(), false);
		return *this;
	}

	template <VEnum... Es>
	constexpr var_filter<VEnum>& enable() {
		((fea::get<Es>(_data) = true), ...);
		return *this;
	}
	template <VEnum... Es>
	constexpr var_filter<VEnum>& disable() {
		((fea::get<Es>(_data) = false), ...);
		return *this;
	}

	template <class... Args>
	constexpr var_filter<VEnum>& enable(Args... e) {
		static_assert((std::is_same_v<Args, VEnum> && ...),
				"var_filter : incorrect enable argument type");

		((_data[e] = true), ...);
		return *this;
	}
	template <class... Args>
	constexpr var_filter<VEnum>& disable(Args... e) {
		static_assert((std::is_same_v<Args, VEnum> && ...),
				"var_filter : incorrect disable argument type");

		((_data[e] = false), ...);
		return *this;
	}

	template <VEnum E>
	constexpr bool at() const {
		return fea::get<E>(_data);
	}

	constexpr bool at(VEnum e) const {
		return _data[e];
	}

private:
	fea::enum_array<bool, VEnum> _data{};
};
} // namespace fea

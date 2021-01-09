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
#include "fea/meta/traits.hpp"
#include "fea/meta/tuple.hpp"

namespace fea {
// Index of element T in paramater pack.
template <class, class...>
struct pack_idx;
template <class T, class... Ts>
struct pack_idx<T, T, Ts...> : std::integral_constant<size_t, 0> {};
template <class T, class U, class... Ts>
struct pack_idx<T, U, Ts...>
		: std::integral_constant<size_t, 1 + pack_idx<T, Ts...>::value> {};

// Index of element T in paramater pack.
template <class T, class... Ts>
FEA_INLINE_VAR constexpr size_t pack_idx_v = pack_idx<T, Ts...>::value;

// Index of element T in non-type paramater pack.
template <class NT, NT, NT...>
struct non_type_pack_idx;
template <class NT, NT T, NT... Ts>
struct non_type_pack_idx<NT, T, T, Ts...> : std::integral_constant<size_t, 0> {
};
template <class NT, NT T, NT U, NT... Ts>
struct non_type_pack_idx<NT, T, U, Ts...>
		: std::integral_constant<size_t,
				  1 + non_type_pack_idx<NT, T, Ts...>::value> {};

// Index of element T in non-type paramater pack.
template <class NT, NT T, NT... Ts>
FEA_INLINE_VAR constexpr size_t non_type_pack_idx_v
		= non_type_pack_idx<NT, T, Ts...>::value;


namespace detail {
template <size_t, size_t, class, class...>
struct idx_splice_impl;

// Found idx.
template <size_t TargetIdx, class BeforeTup, class T, class... Rest>
struct idx_splice_impl<TargetIdx, TargetIdx, BeforeTup, T, Rest...> {
	using before_tuple = BeforeTup;
	using type = T;
	using after_tuple = std::tuple<Rest...>;
};

template <size_t TargetIdx, size_t CurrentIdx, class BeforeTup, class T,
		class... Rest>
struct idx_splice_impl<TargetIdx, CurrentIdx, BeforeTup, T, Rest...>
		: idx_splice_impl<TargetIdx, CurrentIdx + 1,
				  fea::tuple_type_cat_t<BeforeTup, std::tuple<T>>, Rest...> {};

} // namespace detail

// Splice a parameter pack at index Idx.
// Finds the type at Idx, and stores the remaining parameters (parameters after
// splice point) in a tuple type.
template <size_t Idx, class... Args>
struct idx_splice : detail::idx_splice_impl<Idx, 0, std::tuple<>, Args...> {
	static_assert(
			Idx < sizeof...(Args), "fea::idx_splice : index out-of-range");
};

// Get the element type at index Idx in parameter pack.
template <size_t Idx, class... Args>
using idx_splice_t = typename idx_splice<Idx, Args...>::type;

// Get the elements before Idx in parameter pack, stored as a tuple type.
template <size_t Idx, class... Args>
using idx_splice_before_t = typename idx_splice<Idx, Args...>::before_tuple;

// Get the elements after Idx in parameter pack, stored as a tuple type.
template <size_t Idx, class... Args>
using idx_splice_after_t = typename idx_splice<Idx, Args...>::after_tuple;


// Get the first type in a parameter pack.
template <class... Args>
using first_t = idx_splice_t<0, Args...>;

// Get the last type in a parameter pack.
template <class... Args>
using last_t = idx_splice_t<sizeof...(Args) - 1, Args...>;


// Holder for types.
template <class... Args>
struct type_pack {
	// Size of paramter pack.
	static constexpr size_t size() {
		return sizeof...(Args);
	}

	// Get the index of the first encountered type T.
	template <class T>
	static constexpr size_t find_idx() {
		return pack_idx_v<T, Args...>;
	}
};

template <class T, T... Ts>
struct non_type_type_pack {
	// Size of paramter pack.
	static constexpr size_t size() {
		return sizeof...(Ts);
	}

	// Get the index of the first encountered type T.
	template <T S>
	static constexpr size_t find_idx() {
		return non_type_pack_idx_v<T, S, Ts...>;
	}
};

template <class T, T... Ts>
using nt_type_pack = non_type_type_pack<T, Ts...>;

} // namespace fea

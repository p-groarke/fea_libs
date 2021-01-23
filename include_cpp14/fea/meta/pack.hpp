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
#include "fea/meta/traits.hpp"
#include "fea/meta/tuple.hpp"
#include "fea/utils/platform.hpp"

/*
fea::pack and fea::pack_nt are like std::tuple without storage. You can use the
provided traits and helpers to query things about the pack.
*/

namespace fea {
// Holder for types.
template <class... Args>
struct pack {};

template <class T, T... Ts>
struct pack_nt {
	using non_type = T;
};

/**
 * Non-member functions
 */

template <class... Args>
constexpr fea::pack<Args...> make_pack(Args&&...) {
	return fea::pack<Args...>{};
}

// make_pack_nt doesn't make sense, since you can't deduce non-type parameters.


template <class...>
struct pack_cat_type;

template <class... Args, template <class...> class Pack>
struct pack_cat_type<Pack<Args...>> {
	using type = Pack<Args...>;
};

template <class NT, NT... Args, template <class, NT...> class Pack>
struct pack_cat_type<Pack<NT, Args...>> {
	using type = Pack<NT, Args...>;
};

template <class... Args1, class... Args2, template <class...> class Pack1,
		template <class...> class Pack2, class... Rest>
struct pack_cat_type<Pack1<Args1...>, Pack2<Args2...>, Rest...>
		: pack_cat_type<fea::pack<Args1..., Args2...>, Rest...> {};

template <class NT, NT... Args1, NT... Args2,
		template <class, NT...> class Pack1,
		template <class, NT...> class Pack2, class... Rest>
struct pack_cat_type<Pack1<NT, Args1...>, Pack2<NT, Args2...>, Rest...>
		: pack_cat_type<fea::pack_nt<NT, Args1..., Args2...>, Rest...> {};

// The type of a pack_cat.
template <class... Packs>
using pack_cat_t = typename pack_cat_type<Packs...>::type;


template <class... Packs>
constexpr auto pack_cat(Packs...) {
	return pack_cat_t<Packs...>{};
}


/**
 * Helper classes
 */

// Size of parameter pack.
template <class>
struct pack_size;

template <class... Args>
struct pack_size<fea::pack<Args...>>
		: std::integral_constant<size_t, sizeof...(Args)> {};

template <class NT, NT... Args>
struct pack_size<fea::pack_nt<NT, Args...>>
		: std::integral_constant<size_t, sizeof...(Args)> {};

template <class Pack>
struct pack_size<const Pack>
		: std::integral_constant<size_t, pack_size<Pack>::value> {};

template <class Pack>
FEA_INLINE_VAR constexpr size_t pack_size_v = pack_size<Pack>::value;


template <size_t, class>
struct pack_element;

template <size_t I, class Head, class... Tail>
struct pack_element<I, fea::pack<Head, Tail...>>
		: fea::pack_element<I - 1, fea::pack<Tail...>> {};

template <class Head, class... Tail>
struct pack_element<0, fea::pack<Head, Tail...>> {
	using type = Head;
};

template <size_t I, class Pack>
struct pack_element<I, const Pack> {
	using type = typename std::add_const<
			typename fea::pack_element<I, Pack>::type>::type;
};

// Element type at index I.
template <size_t I, class Pack>
using pack_element_t = typename pack_element<I, Pack>::type;


template <size_t, class>
struct pack_element_nt;

template <size_t I, class NT, NT Head, NT... Tail>
struct pack_element_nt<I, fea::pack_nt<NT, Head, Tail...>>
		: fea::pack_element_nt<I - 1, fea::pack_nt<NT, Tail...>> {};

template <class NT, NT Head, NT... Tail>
struct pack_element_nt<0, fea::pack_nt<NT, Head, Tail...>> {
	static constexpr NT value = Head;
};

template <size_t I, class Pack>
struct pack_element_nt<I, const Pack> {
	static constexpr const auto value = fea::pack_element_nt<I, Pack>::value;
};

// Non-type element at index I.
template <size_t I, class Pack>
FEA_INLINE_VAR constexpr auto pack_element_nt_v
		= pack_element_nt<I, Pack>::value;


// Index of element T in pack.
template <class, class>
struct pack_idx;

template <class T, class... Ts>
struct pack_idx<T, fea::pack<T, Ts...>> : std::integral_constant<size_t, 0> {};

template <class T, class U, class... Ts>
struct pack_idx<T, fea::pack<U, Ts...>>
		: std::integral_constant<size_t,
				  1 + pack_idx<T, fea::pack<Ts...>>::value> {};

template <class T, class Pack>
struct pack_idx<T, const Pack>
		: std::integral_constant<size_t, pack_idx<T, Pack>::value> {};

// Index of element T in paramater pack.
template <class T, class Pack>
FEA_INLINE_VAR constexpr size_t pack_idx_v = pack_idx<T, Pack>::value;


// Index of element T in non-type paramater pack.
template <class NT, NT, class>
struct pack_idx_nt;

template <class NT, NT T, NT... Ts>
struct pack_idx_nt<NT, T, fea::pack_nt<NT, T, Ts...>>
		: std::integral_constant<size_t, 0> {};

template <class NT, NT T, NT U, NT... Ts>
struct pack_idx_nt<NT, T, fea::pack_nt<NT, U, Ts...>>
		: std::integral_constant<size_t,
				  1 + pack_idx_nt<NT, T, fea::pack_nt<NT, Ts...>>::value> {};

template <class NT, NT T, class Pack>
struct pack_idx_nt<NT, T, const Pack>
		: std::integral_constant<size_t, pack_idx_nt<NT, T, Pack>::value> {};

// Index of element T in non-type paramater pack.
template <class NT, NT T, class Pack>
FEA_INLINE_VAR constexpr size_t pack_idx_nt_v = pack_idx_nt<NT, T, Pack>::value;

#if FEA_CPP17
// Index of element T in non-type paramater pack.
template <auto T, class Pack>
inline constexpr size_t pack_idx_nt_v2
		= pack_idx_nt<decltype(T), T, Pack>::value;
#endif

// Indexes of all elements T in pack.
// Returns a fea::pack_nt<size_t, found_indexes...>;
template <class, class Pack, size_t Count = fea::pack_size_v<Pack>>
struct pack_idxes;

template <class T, size_t Count>
struct pack_idxes<T, fea::pack<>, Count> {
	using idxes = fea::pack_nt<size_t>;
};

template <class T, class... Ts, size_t Count>
struct pack_idxes<T, fea::pack<T, Ts...>, Count>
		: pack_idxes<T, fea::pack<Ts...>, Count> {
	using idxes
			= fea::pack_cat_t<fea::pack_nt<size_t, Count - (sizeof...(Ts) + 1)>,
					typename pack_idxes<T, fea::pack<Ts...>, Count>::idxes>;
};

template <class T, class U, class... Ts, size_t Count>
struct pack_idxes<T, fea::pack<U, Ts...>, Count>
		: pack_idxes<T, fea::pack<Ts...>, Count> {
	using idxes = typename pack_idxes<T, fea::pack<Ts...>, Count>::idxes;
};

template <class T, class Pack, size_t Count>
struct pack_idxes<T, const Pack, Count> : pack_idxes<T, Pack, Count> {};

// Indexes of all elements T in pack.
// Returns a fea::pack_nt<size_t, found_indexes...>;
template <class T, class Pack>
using pack_idxes_t = typename pack_idxes<T, Pack>::idxes;


// Indexes of all non-type T in pack.
// Returns a fea::pack_nt<size_t, found_indexes...>;
template <class NT, NT, class Pack, size_t Count = fea::pack_size_v<Pack>>
struct pack_idxes_nt;

template <class NT, NT T, size_t Count>
struct pack_idxes_nt<NT, T, fea::pack_nt<NT>, Count> {
	using idxes = fea::pack_nt<size_t>;
};

template <class NT, NT T, NT... Ts, size_t Count>
struct pack_idxes_nt<NT, T, fea::pack_nt<NT, T, Ts...>, Count>
		: pack_idxes_nt<NT, T, fea::pack_nt<NT, Ts...>, Count> {
	using idxes
			= fea::pack_cat_t<fea::pack_nt<size_t, Count - (sizeof...(Ts) + 1)>,
					typename pack_idxes_nt<NT, T, fea::pack_nt<NT, Ts...>,
							Count>::idxes>;
};

template <class NT, NT T, NT U, NT... Ts, size_t Count>
struct pack_idxes_nt<NT, T, fea::pack_nt<NT, U, Ts...>, Count>
		: pack_idxes_nt<NT, T, fea::pack_nt<NT, Ts...>, Count> {
	using idxes = typename pack_idxes_nt<NT, T, fea::pack_nt<NT, Ts...>,
			Count>::idxes;
};

template <class NT, NT T, class Pack, size_t Count>
struct pack_idxes_nt<NT, T, const Pack, Count>
		: pack_idxes_nt<NT, T, Pack, Count> {};

// Indexes of all elements T in pack.
// Returns a fea::pack_nt<size_t, found_indexes...>;
template <class NT, NT T, class Pack>
using pack_idxes_nt_t = typename pack_idxes_nt<NT, T, Pack>::idxes;

#if FEA_CPP17
// Indexes of all elements T in pack.
// Returns a fea::pack_nt<size_t, found_indexes...>;
template <auto T, class Pack>
using pack_idxes_nt_t2 = typename pack_idxes_nt<decltype(T), T, Pack>::idxes;
#endif


// Does pack contain type?
template <class, class>
struct pack_contains;

template <class T>
struct pack_contains<T, fea::pack<>> : std::false_type {};

template <class T, class U, class... Ts>
struct pack_contains<T, fea::pack<U, Ts...>>
		: pack_contains<T, fea::pack<Ts...>> {};

template <class T, class... Ts>
struct pack_contains<T, fea::pack<T, Ts...>> : std::true_type {};

template <class T, class Pack>
struct pack_contains<T, const Pack> : pack_contains<T, Pack> {};

// Does pack contain type T?
template <class T, class Pack>
FEA_INLINE_VAR constexpr bool pack_contains_v = pack_contains<T, Pack>::value;


template <class NT, NT, class>
struct pack_contains_nt;

template <class NT, NT T>
struct pack_contains_nt<NT, T, fea::pack_nt<NT>> : std::false_type {};

template <class NT, NT T, NT U, NT... Ts>
struct pack_contains_nt<NT, T, fea::pack_nt<NT, U, Ts...>>
		: pack_contains_nt<NT, T, fea::pack_nt<NT, Ts...>> {};

template <class NT, NT T, NT... Ts>
struct pack_contains_nt<NT, T, fea::pack_nt<NT, T, Ts...>> : std::true_type {};

template <class NT, NT T, class Pack>
struct pack_contains_nt<NT, T, const Pack> : pack_contains_nt<NT, T, Pack> {};

// Does pack contain type T?
template <class NT, NT T, class Pack>
FEA_INLINE_VAR constexpr bool pack_contains_nt_v
		= pack_contains_nt<NT, T, Pack>::value;

#if FEA_CPP17
// Does pack contain type T?
template <auto T, class Pack>
FEA_INLINE_VAR constexpr bool pack_contains_nt_v2
		= pack_contains_nt<decltype(T), T, Pack>::value;
#endif


namespace detail {
template <size_t, size_t, class, class...>
struct idx_splice_impl;

// Found idx.
template <size_t TargetIdx, class BeforePack, class T, class... Rest>
struct idx_splice_impl<TargetIdx, TargetIdx, BeforePack, T, Rest...> {
	using before_pack = BeforePack;
	using type = T;
	using after_pack = fea::pack<Rest...>;
};

template <size_t TargetIdx, size_t CurrentIdx, class BeforePack, class T,
		class... Rest>
struct idx_splice_impl<TargetIdx, CurrentIdx, BeforePack, T, Rest...>
		: idx_splice_impl<TargetIdx, CurrentIdx + 1,
				  fea::pack_cat_t<BeforePack, fea::pack<T>>, Rest...> {};

} // namespace detail

// Splice a parameter pack at index Idx.
// Finds the type at Idx, and stores the remaining parameters (parameters after
// splice point) in a fea::pack.
template <size_t Idx, class... Args>
struct idx_splice : detail::idx_splice_impl<Idx, 0, fea::pack<>, Args...> {
	static_assert(
			Idx < sizeof...(Args), "fea::idx_splice : index out-of-range");
};

// Get the element type at index Idx in parameter pack.
template <size_t Idx, class... Args>
using idx_splice_t = typename idx_splice<Idx, Args...>::type;

// Get the elements before Idx in parameter pack, stored as a tuple type.
template <size_t Idx, class... Args>
using idx_splice_before_t = typename idx_splice<Idx, Args...>::before_pack;

// Get the elements after Idx in parameter pack, stored as a tuple type.
template <size_t Idx, class... Args>
using idx_splice_after_t = typename idx_splice<Idx, Args...>::after_pack;
} // namespace fea

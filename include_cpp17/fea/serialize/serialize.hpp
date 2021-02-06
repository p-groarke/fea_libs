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
#include "fea/serialize/serializer.hpp"

#include <iterator>
#include <type_traits>

// Special snow-flakes.
#include <queue>
#include <tuple>
#include <utility> // pair

/*
Helpers for simple serialization and deserialization. The serialization will
recurse deeper into containers of containers. However, it is smart about
serializing contiguous containers.

For ex, a std::vector<int> will be written in one go (flat). But a
std::vector<std::vector<int>> will be recursed once. The nested vectors are
written as flat data chunks.

Container serialization has corrupted data checks. It returns false if
deserialization fails. It clears or default initializes the passed in
deserialization object/container.

You may define a different size_t size with FEA_SERIALIZE_SIZE_T.
By default, uses the current platform size_t size.
Use this if you expect different platforms to load your data. Though it is
up to you to make sure your serialized data-structures are cross-platform
compatible.

On pointers.
fea::serialize is a low level building blocks library. As such, it doesn't
serialize pointers.
You may choose to serialize pointed-to objects in place, but you must do it
manually. Or better yet, have an id system so you can patch pointers on
deserialization.

On references.
fea::serialize cannot differentiate between your passed in refs and refs
themselved. As such, it will serialize the references in place. When
deserializing, the original reference will be filled with data.
Not ideal, but good enough.
*/

namespace fea {

template <class T>
void serialize(const T&, fea::serializer&);

template <class T, class... Args>
void serialize(const std::queue<T, Args...>&, fea::serializer&);

namespace detail {
template <class T>
using has_begin = decltype(std::begin(std::declval<T>()));
template <class T>
using has_end = decltype(std::end(std::declval<T>()));
template <class T>
using has_data = decltype(std::data(std::declval<T>()));
template <class T>
using has_size = decltype(std::size(std::declval<T>()));
template <class T>
using has_get = decltype(std::get<0>(std::declval<T>()));

template <class T>
inline constexpr bool is_container_v
		= fea::is_detected_v<has_begin, T>&& fea::is_detected_v<has_end, T>;

template <class T>
inline constexpr bool is_contiguous_v
		= fea::is_detected_v<has_data, T>&& fea::is_detected_v<has_size, T>;

template <class T>
inline constexpr bool is_tuple_v = fea::is_detected_v<has_get, T>;

// c++20 has contiguous_iterator_tag
#if !FEA_CPP20
struct contiguous_iterator_tag : public std::random_access_iterator_tag {};
#else
using contiguous_iterator_tag = std::contiguous_iterator_tag;
#endif

template <class Iter>
void serialize(
		Iter begin, Iter end, fea::serializer& os, std::input_iterator_tag) {

	using traits_t = std::iterator_traits<Iter>;
	using val_t = typename traits_t::value_type;

	if constexpr (std::is_trivially_copyable_v<val_t>) {
		// Limits vector resizing...
		// os.reserve<val_t>(std::distance(begin, end));
		for (Iter it = begin; it != end; ++it) {
			os.write(*it);
		}
	} else {
		using fea::serialize;
		for (Iter it = begin; it != end; ++it) {
			serialize(*it, os);
		}
	}
}

template <class Iter>
void serialize(
		Iter begin, Iter end, fea::serializer& os, contiguous_iterator_tag) {

	using traits_t = std::iterator_traits<Iter>;
	using val_t = typename traits_t::value_type;

	if constexpr (std::is_trivially_copyable_v<val_t>) {
		os.write(std::addressof(*begin), std::distance(begin, end));
	} else {
		using fea::serialize;
		for (Iter it = begin; it != end; ++it) {
			serialize(*it, os);
		}
	}
}

template <class... Args, template <class...> class Tup>
void serialize_tup(const Tup<Args...>& t, fea::serializer& os) {
	using fea::serialize;
	fea::tuple_for_each([&](const auto& v) { serialize(v, os); }, t);
}
} // namespace detail

// Can't expose this because of contiguous_iterator_tag detection.
// template <class Iter>
// void serialize(Iter begin, Iter end, fea::serializer& os);

// Not implemented, fea::serialize doesn't support serializing pointers.
template <class T>
void serialize(const T*, fea::serializer&) {
	static_assert(
			false, "fea::serialize : doesn't support pointer serialization");
}

// Serialize object t.
// Recurses appropriately.
// Implement 'friend void serialize(const YourClass&, fea::serializer&)'
// for custom non-trivially-copyable types.
template <class T>
void serialize(const T& t, fea::serializer& os) {
	if constexpr (detail::is_container_v<T>) {
		// Is container, call iterator overload.
		using it_t = decltype(std::begin(t));
		using traits_t = std::iterator_traits<it_t>;
		using category_t = typename traits_t::iterator_category;

#if FEA_CPP20
		serialize(std::begin(t), std::end(t), os, category_t{});
#else
		if constexpr (detail::is_contiguous_v<T>) {
			detail::serialize(std::begin(t), std::end(t), os,
					detail::contiguous_iterator_tag{});
		} else {
			detail::serialize(std::begin(t), std::end(t), os, category_t{});
		}
#endif
	} else if constexpr (detail::is_tuple_v<T>) {
		// Is a tuple-like thing (has std::get).
		detail::serialize_tup(t, os);
	} else {
		// Is not container or snow-flake, try to write as-is.
		// static_asserts is_trivially_copyable.
		os.write(t);
	}
}

template <class T>
[[nodiscard]] bool deserialize(T&, fea::deserializer&) {
	return false;
}

// Helpers for snow-flake library types.
template <class T, class... Args>
void serialize(const std::queue<T, Args...>& t, fea::serializer& os) {
	using fea::serialize;
	auto cpy = t;
	while (!cpy.empty()) {
		serialize(cpy.front(), os);
		cpy.pop();
	}
}

} // namespace fea

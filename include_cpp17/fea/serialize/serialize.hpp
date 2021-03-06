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
#include "fea/utils/memory.hpp"

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

template <class T>
[[nodiscard]] bool deserialize(T&, fea::deserializer&);

template <class T, class... Args>
[[nodiscard]] bool deserialize(std::queue<T, Args...>&, fea::deserializer&);

namespace detail {

// c++20 has contiguous_iterator_tag
#if !FEA_CPP20
struct contiguous_iterator_tag : public std::random_access_iterator_tag {};
#else
using contiguous_iterator_tag = std::contiguous_iterator_tag;
#endif

// Can't expose this because of contiguous_iterator_tag detection.
// template <class Iter>
// void serialize(Iter begin, Iter end, fea::serializer& os);

template <class Iter>
void serialize(
		Iter begin, Iter end, fea::serializer& os, std::input_iterator_tag) {

	using msize_t = FEA_SERIALIZE_SIZE_T;
	msize_t size = msize_t(std::distance(begin, end));

	os.write_unvalidated(size);
	using fea::serialize;
	for (Iter it = begin; it != end; ++it) {
		serialize(*it, os);
	}
	os.write_unvalidated(size);
}

template <class T>
auto get_pair_type() {
	if constexpr (is_pair_v<T>) {
		if constexpr (fea::is_first_const_v<T>) {
			return fea::remove_nested_const_t<T>{};
		} else {
			return T{};
		}
	} else {
		return T{};
	}
}

template <class T>
[[nodiscard]] bool deserialize(
		T& t, fea::deserializer& is, std::input_iterator_tag) {

	using Iter = typename T::iterator;
	using traits_t = std::iterator_traits<Iter>;
	using val_t = typename traits_t::value_type;
	using msize_t = FEA_SERIALIZE_SIZE_T;

	msize_t size = 0;
	if (!is.read_unvalidated(size)) {
		return false;
	}

	if constexpr (fea::is_detected_v<has_reserve, T>) {
		t.reserve(size_t(size));
	}

	static_assert(std::is_default_constructible_v<val_t>,
			"fea::deserialize : type must be default constructible");

	for (size_t i = 0; i < size_t(size); ++i) {
		decltype(get_pair_type<val_t>()) v;
		using fea::deserialize;
		if (!deserialize(v, is)) {
			return false;
		}
		std::inserter(t, std::end(t)) = fea::maybe_move(v);
	}

	msize_t size2 = 0;
	if (!is.read_unvalidated(size2)) {
		return false;
	}

	if (size != size2) {
		assert(false);
		return false;
	}
	return true;
}


template <class Iter>
void serialize(
		Iter begin, Iter end, fea::serializer& os, contiguous_iterator_tag) {

	using traits_t = std::iterator_traits<Iter>;
	using val_t = typename traits_t::value_type;

	using msize_t = FEA_SERIALIZE_SIZE_T;
	msize_t size = msize_t(std::distance(begin, end));

	// Shouldn't be serializing empty containers.
	assert(size != 0);

	os.write_unvalidated(size);
	if constexpr (std::is_trivially_copyable_v<val_t>) {
		os.write(std::addressof(*begin), std::distance(begin, end));
	} else {
		using fea::serialize;
		for (auto it = begin; it != end; ++it) {
			serialize(*it, os);
		}
	}
	os.write_unvalidated(size);
}

template <class T>
[[nodiscard]] bool deserialize(
		T& t, fea::deserializer& is, contiguous_iterator_tag) {

	using Iter = typename T::iterator;
	using traits_t = std::iterator_traits<Iter>;
	using val_t = typename traits_t::value_type;
	using msize_t = FEA_SERIALIZE_SIZE_T;

	msize_t size = 0;
	if (!is.read_unvalidated(size)) {
		return false;
	}

	if constexpr (fea::is_detected_v<has_resize, T>) {
		t.resize(size_t(size));
	} else {
		if (size_t(std::distance(std::begin(t), std::end(t))) != size_t(size)) {
			assert(false);
			return false;
		}
	}

	if constexpr (std::is_trivially_copyable_v<val_t>) {
		if (!is.read(std::addressof(*std::begin(t)), size_t(size))) {
			return false;
		}
	} else {
		using fea::deserialize;
		for (size_t i = 0; i < size_t(size); ++i) {
			if (!deserialize(t[i], is)) {
				return false;
			}
		}
	}

	msize_t size2 = 0;
	if (!is.read_unvalidated(size2)) {
		return false;
	}

	if (size != size2) {
		assert(false);
		return false;
	}
	return true;
}

template <class... Args, template <class...> class Tup>
void serialize_tup(const Tup<Args...>& t, fea::serializer& os) {
	fea::tuple_for_each(
			[&](const auto& v) {
				using fea::serialize;
				serialize(v, os);
			},
			t);
}

template <class... Args, template <class...> class Tup>
[[nodiscard]] bool deserialize_tup(Tup<Args...>& t, fea::deserializer& is) {
	bool ok = true;
	fea::tuple_for_each(
			[&](auto& v) {
				using fea::deserialize;
				if (!ok) {
					return;
				}
				ok &= deserialize(v, is);
			},
			t);
	return ok;
}
} // namespace detail

// Not implemented, fea::serialize doesn't support serializing pointers.
template <class T>
void serialize(const T*, fea::serializer&);

// Not implemented, fea::deserialize doesn't support serializing pointers.
template <class T>
[[nodiscard]] bool deserialize(T*, fea::deserializer&);

// Serialize object t.
// Recurses appropriately.
// Implement 'friend void serialize(const YourClass&, fea::serializer&)'
// for custom non-trivially-copyable types.
template <class T>
void serialize(const T& t, fea::serializer& os) {
	if constexpr (fea::is_container_v<T>) {
		// Is container, call iterator overload.
		using it_t = decltype(std::begin(t));
		using traits_t = std::iterator_traits<it_t>;
		using category_t = typename traits_t::iterator_category;

		if constexpr (fea::is_contiguous_v<T>) {
			detail::serialize(std::begin(t), std::end(t), os,
					detail::contiguous_iterator_tag{});
		} else {
			detail::serialize(std::begin(t), std::end(t), os, category_t{});
		}

	} else if constexpr (fea::is_tuple_like_v<T>) {
		// Is a tuple-like thing (has std::get).
		detail::serialize_tup(t, os);
	} else {
		// Is not container or snow-flake, try to write as-is.
		// static_asserts is_trivially_copyable.
		os.write(t);
	}
}

template <class T>
[[nodiscard]] bool deserialize(T& t, fea::deserializer& is) {
	if constexpr (fea::is_container_v<T>) {
		// Is container, call iterator overload.
		using it_t = decltype(std::begin(t));
		using traits_t = std::iterator_traits<it_t>;
		using category_t = typename traits_t::iterator_category;

		if constexpr (fea::is_contiguous_v<T>) {
			return detail::deserialize(
					t, is, detail::contiguous_iterator_tag{});
		} else {
			return detail::deserialize(t, is, category_t{});
		}

	} else if constexpr (fea::is_tuple_like_v<T>) {
		// Is a tuple-like thing (has std::get).
		return detail::deserialize_tup(t, is);
	} else {
		// Is not container or snow-flake, try to write as-is.
		// static_asserts is_trivially_copyable.
		return is.read(t);
	}
}

// Helpers for snow-flake library types.
template <class T, class... Args>
void serialize(const std::queue<T, Args...>& t, fea::serializer& os) {
	using msize_t = FEA_SERIALIZE_SIZE_T;
	using fea::serialize;

	auto cpy = t;
	msize_t size = msize_t(cpy.size());
	os.write_unvalidated(size);
	while (!cpy.empty()) {
		serialize(cpy.front(), os);
		cpy.pop();
	}
	os.write_unvalidated(size);
}

template <class T, class... Args>
[[nodiscard]] bool deserialize(
		std::queue<T, Args...>& t, fea::deserializer& is) {
	using msize_t = FEA_SERIALIZE_SIZE_T;
	using fea::deserialize;

	msize_t size = 0;
	if (!is.read_unvalidated(size)) {
		return false;
	}
	for (size_t i = 0; i < size_t(size); ++i) {
		T v{};
		if (!deserialize(v, is)) {
			return false;
		}
		t.push(fea::maybe_move(v));
	}

	msize_t size2 = 0;
	if (!is.read_unvalidated(size2)) {
		return false;
	}

	if (size != size2) {
		assert(false);
		return false;
	}
	return true;
}

} // namespace fea

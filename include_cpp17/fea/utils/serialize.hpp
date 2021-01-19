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
#include "fea/utils/memory.hpp"
#include "fea/utils/platform.hpp"

// Q : Split into a few headers so we don't include the full stl?
#include <array>
#include <cassert>
#include <fstream>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

/*
Helpers for simple serialization and deserialization. The serialization will
recurse deeper into containers of containers. However, it is smart about
serializing "flat" containers.

For ex, a std::vector<int> will be written in one go (flat). But a
std::vector<std::vector<int>> will be recursed once. The nested vectors are
written as flat data chunks.

To achieve this, the serialize functions return std::false_type or
std::true_type. When implementing your own serialize overloads, return
std::true_type.

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
deserializing, the original reference will be filled with the data.
Not ideal, but good enough.

TODO : pair, tuple, queue, deque, fea containers
*/

#if !defined(FEA_SERIALIZE_SIZE_T)
#define FEA_SERIALIZE_SIZE_T size_t
#endif

// Delay evaluation to consider user overloads.
// Probably a better way to do this.
#define FEA_NEEDS_NESTING(T) \
	decltype(serialize( \
			std::declval<const T&>(), std::declval<std::ofstream&>()))::value

namespace fea {
using cerial_size_t = FEA_SERIALIZE_SIZE_T;

namespace detail {
template <class T>
using has_reserve = decltype(std::declval<T>().reserve(std::declval<size_t>()));

template <class T>
using has_clear = decltype(std::declval<T>().clear());

template <class T>
using has_size = decltype(std::declval<T>().size());

// Deserializes a size and checks if it is valid.
// If not, calls clear() on t.
template <class T>
[[nodiscard]] bool sanity_check(
		T& t, std::ifstream& ifs, cerial_size_t expected_size);

[[nodiscard]] inline bool read(std::ifstream& ifs, char* ptr, size_t size) {
	assert(ifs.is_open());
	ifs.read(ptr, size);
	assert(ifs.good());
	return ifs.good();
}
inline void write(std::ofstream& ofs, const char* ptr, size_t size) {
	assert(ofs.is_open());
	ofs.write(ptr, size);
	assert(ofs.good());
}

// Helper to generalize plain (non-nested) serialization.
template <class T>
void serialize_plain(const T& data, std::ofstream& ofs);

// Helper to generalize plain (non-nested) deserialization.
template <class T>
[[nodiscard]] bool deserialize_plain(T& data, std::ifstream& ifs);

// Helper to generalize contiguous buffer serialization.
template <class T, class... Args, template <class, class...> class Buf>
void serialize_buffer(const Buf<T, Args...>& buf, std::ofstream& ofs);

// Helper to generalize contiguous buffer deserialization.
template <class T, class... Args, template <class, class...> class Buf>
[[nodiscard]] bool deserialize_buffer(Buf<T, Args...>& buf, std::ifstream& ifs);

// Helper to generalize map serialization.
template <class K, class V, class... Args,
		template <class, class, class...> class Map>
void serialize_map(const Map<K, V, Args...>& map, std::ofstream& ofs);

// Helper to generalize map deserialization.
template <class K, class V, class... Args,
		template <class, class, class...> class Map>
[[nodiscard]] bool deserialize_map(Map<K, V, Args...>& map, std::ifstream& ifs);

// Helper to generalize set serialization.
template <class T, class... Args, template <class, class...> class Set>
void serialize_set(const Set<T, Args...>& set, std::ofstream& ofs);

// Helper to generalize set deserialization.
template <class T, class... Args, template <class, class...> class Set>
[[nodiscard]] bool deserialize_set(Set<T, Args...>& set, std::ifstream& ifs);

// Helper to generalize tuple-like serialization.
template <class... Args, template <class...> class Tup>
void serialize_tuple(const Tup<Args...>& tup, std::ofstream& ofs);

// Helper to generalize tuple-like deserialization.
template <class... Args, template <class...> class Tup>
[[nodiscard]] bool deserialize_tuple(Tup<Args...>& tup, std::ifstream& ifs);

} // namespace detail

// Serialize anything.
template <class T>
std::false_type serialize(const T& t, std::ofstream& ofs) {
	static_assert(!std::is_pointer_v<T>,
			"fea::serialize : doesn't support serializing pointers");
	detail::serialize_plain(t, ofs);
	return {};
}

// Serialize std::pair.
template <class K, class V>
std::true_type serialize(const std::pair<K, V>& pair, std::ofstream& ofs) {
	detail::serialize_tuple(pair, ofs);
	return {};
}

// Serialize std::tuple.
template <class... Args>
std::true_type serialize(const std::tuple<Args...>& tup, std::ofstream& ofs) {
	detail::serialize_tuple(tup, ofs);
	return {};
}

// Serialize std::array.
template <class T, size_t N>
std::true_type serialize(const std::array<T, N>& arr, std::ofstream& ofs) {
	if constexpr (FEA_NEEDS_NESTING(T)) {
		cerial_size_t size(arr.size());
		serialize(size, ofs);
		for (const T& t : arr) {
			serialize(t, ofs);
		}
		serialize(size, ofs);
	} else {
		detail::serialize_plain(arr, ofs);
	}

	return {};
}

// Serialize std::vector.
template <class T, class... Args>
std::true_type serialize(
		const std::vector<T, Args...>& vec, std::ofstream& ofs) {
	detail::serialize_buffer(vec, ofs);
	return {};
}

// Serialize std::unordered_map.
template <class K, class V, class... Args>
std::true_type serialize(
		const std::unordered_map<K, V, Args...>& umap, std::ofstream& ofs) {
	detail::serialize_map(umap, ofs);
	return {};
}

// Serialize std::map.
template <class K, class V, class... Args>
std::true_type serialize(
		const std::map<K, V, Args...>& map, std::ofstream& ofs) {
	detail::serialize_map(map, ofs);
	return {};
}

// Serialize std::unordered_set.
template <class T, class... Args>
std::true_type serialize(
		const std::unordered_set<T, Args...>& uset, std::ofstream& ofs) {
	detail::serialize_set(uset, ofs);
	return {};
}

// Serialize std::set.
template <class T, class... Args>
std::true_type serialize(const std::set<T, Args...>& set, std::ofstream& ofs) {
	detail::serialize_set(set, ofs);
	return {};
}

// Serialize std::string.
template <class T, class... Args>
std::true_type serialize(
		const std::basic_string<T, Args...>& string, std::ofstream& ofs) {
	detail::serialize_buffer(string, ofs);
	return {};
}


// Deserialize anything.
template <class T>
[[nodiscard]] bool deserialize(T& t, std::ifstream& ifs) {
	return detail::deserialize_plain(t, ifs);
}

// Deserialize std::pair.
// If the sanity check fails, pair is default initialized and returns false.
template <class K, class V>
[[nodiscard]] bool deserialize(std::pair<K, V>& pair, std::ifstream& ifs) {
	return detail::deserialize_tuple(pair, ifs);
}

// Deserialize std::tuple.
// If the sanity check fails, tuple is default initialized and returns false.
template <class... Args>
[[nodiscard]] bool deserialize(std::tuple<Args...>& tup, std::ifstream& ifs) {
	return detail::deserialize_tuple(tup, ifs);
}

// Deserialize std::array.
// If sanity check fails, array will be defaulted and returns false.
template <class T, size_t N>
[[nodiscard]] bool deserialize(std::array<T, N>& arr, std::ifstream& ifs) {

	if constexpr (FEA_NEEDS_NESTING(T)) {
		cerial_size_t size = 0;
		if (!deserialize(size, ifs)) {
			return false;
		}

		if (arr.size() != size) {
			assert(false);
			return false;
		}

		for (cerial_size_t i = 0; i < size; ++i) {
			if (!deserialize(arr[i], ifs)) {
				return false;
			}
		}

		return detail::sanity_check(arr, ifs, size);
	} else {
		return detail::deserialize_plain(arr, ifs);
	}
}

// Deserialize std::vector.
// If sanity check fails, container will be empty.
template <class T, class... Args>
[[nodiscard]] bool deserialize(
		std::vector<T, Args...>& vec, std::ifstream& ifs) {
	return detail::deserialize_buffer(vec, ifs);
}

// Deserialize std::unordered_map.
// If sanity check fails, container will be empty.
template <class K, class V, class... Args>
[[nodiscard]] bool deserialize(
		std::unordered_map<K, V, Args...>& umap, std::ifstream& ifs) {
	return detail::deserialize_map(umap, ifs);
}

// Deserialize std::map.
// If sanity check fails, container will be empty.
template <class K, class V, class... Args>
[[nodiscard]] bool deserialize(
		std::map<K, V, Args...>& map, std::ifstream& ifs) {
	return detail::deserialize_map(map, ifs);
}

// Deserialize std::unordered_set.
// If sanity check fails, container will be empty.
template <class T, class... Args>
[[nodiscard]] bool deserialize(
		std::unordered_set<T, Args...>& uset, std::ifstream& ifs) {
	return detail::deserialize_set(uset, ifs);
}

// Deserialize std::set.
// If sanity check fails, container will be empty.
template <class T, class... Args>
[[nodiscard]] bool deserialize(std::set<T, Args...>& set, std::ifstream& ifs) {
	return detail::deserialize_set(set, ifs);
}

// Deserialize std::string.
// If sanity check fails, container will be empty.
template <class T, class... Args>
[[nodiscard]] bool deserialize(
		std::basic_string<T, Args...>& string, std::ifstream& ifs) {
	return detail::deserialize_buffer(string, ifs);
}


// Helpers
namespace detail {
template <class T>
bool sanity_check(T& t, std::ifstream& ifs, cerial_size_t expected_size) {
	// Also check the container contains the right amount of things.
	if constexpr (fea::is_detected_v<has_size, T>) {
		assert(t.size() == expected_size);
		if (t.size() != expected_size) {
			return false;
		}
	}

	// Deserialize a container size, and check it is equal to expected.
	cerial_size_t sanity = 0;
	if (!deserialize(sanity, ifs)) {
		return false;
	}

	if (expected_size != sanity) {
		assert(false);
		if constexpr (fea::is_detected_v<has_clear, T>) {
			t.clear();
		} else {
			t = {};
		}
		return false;
	}
	return true;
}

// Helper to generalize plain (non-nested) serialization.
template <class T>
void serialize_plain(const T& data, std::ofstream& ofs) {
	constexpr cerial_size_t size(sizeof(T));
	write(ofs, reinterpret_cast<const char*>(&size), sizeof(size));
	write(ofs, reinterpret_cast<const char*>(&data), sizeof(T));
}

// Helper to generalize plain (non-nested) deserialization.
template <class T>
bool deserialize_plain(T& data, std::ifstream& ifs) {
	cerial_size_t size = 0;
	if (!read(ifs, reinterpret_cast<char*>(&size), sizeof(size))) {
		return false;
	}

	if (size != sizeof(T)) {
		assert(false);
		return false;
	}

	return read(ifs, reinterpret_cast<char*>(&data), sizeof(T));
}

// Helper to generalize contiguous buffer serialization.
template <class T, class... Args, template <class, class...> class Buf>
void serialize_buffer(const Buf<T, Args...>& buf, std::ofstream& ofs) {
	cerial_size_t buf_size(buf.size());
	serialize(buf_size, ofs);

	if constexpr (FEA_NEEDS_NESTING(T)) {
		for (const T& t : buf) {
			serialize(t, ofs);
		}
	} else {
		cerial_size_t t_size(sizeof(T));
		serialize(t_size, ofs);

		write(ofs, reinterpret_cast<const char*>(buf.data()),
				sizeof(T) * buf_size);
	}

	serialize(buf_size, ofs);
}

// Helper to generalize contiguous buffer deserialization.
template <class T, class... Args, template <class, class...> class Buf>
bool deserialize_buffer(Buf<T, Args...>& buf, std::ifstream& ifs) {
	buf.clear();

	cerial_size_t size = 0;
	if (!deserialize(size, ifs)) {
		return false;
	}

	buf.resize(size);

	if constexpr (FEA_NEEDS_NESTING(T)) {
		for (cerial_size_t i = 0; i < size; ++i) {
			if (!deserialize(buf[i], ifs)) {
				return false;
			}
		}
	} else {
		cerial_size_t t_size = 0;
		if (!deserialize(t_size, ifs)) {
			return false;
		}

		if (t_size != sizeof(T)) {
			assert(false);
			return false;
		}

		if (!read(ifs, reinterpret_cast<char*>(buf.data()), sizeof(T) * size)) {
			return false;
		}
	}

	return detail::sanity_check(buf, ifs, size);
}

template <class K, class V, class... Args,
		template <class, class, class...> class Map>
void serialize_map(const Map<K, V, Args...>& map, std::ofstream& ofs) {
	using kv_pair = std::pair<const K, V>;

	cerial_size_t size(map.size());

	if constexpr (!FEA_NEEDS_NESTING(K) && !FEA_NEEDS_NESTING(V)) {
		// Check if nothing needs nesting.
		// If that is the case, just write everything.

		std::vector<kv_pair> kvs(map.begin(), map.end());
		assert(kvs.size() == size);
		serialize(kvs, ofs);

	} else if constexpr (FEA_NEEDS_NESTING(K) && FEA_NEEDS_NESTING(V)) {
		// Both keys and values need nested serialization.

		serialize(size, ofs);
		for (const kv_pair& p : map) {
			serialize(p.first, ofs);
			serialize(p.second, ofs);
		}
		serialize(size, ofs);

	} else if constexpr (FEA_NEEDS_NESTING(K)) {
		// Keys need nesting, values do not.
		// We output all keys first, then a flat buffer of the values.
		std::vector<V> values;
		values.reserve(size);

		serialize(size, ofs);
		for (const kv_pair& p : map) {
			serialize(p.first, ofs);
			values.push_back(p.second);
		}
		serialize(size, ofs);

		assert(values.size() == size);
		serialize(values, ofs);

	} else if constexpr (FEA_NEEDS_NESTING(V)) {
		// Same idea as keys but in this case, values come first and keys after.
		// We do this to limit the number of loops on umap.

		std::vector<K> keys;
		keys.reserve(size);

		serialize(size, ofs);
		for (const kv_pair& p : map) {
			keys.push_back(p.first);
			serialize(p.second, ofs);
		}
		serialize(size, ofs);

		assert(keys.size() == size);
		serialize(keys, ofs);
	}
}

template <class K, class V, class... Args,
		template <class, class, class...> class Map>
bool deserialize_map(Map<K, V, Args...>& map, std::ifstream& ifs) {
	using kv_pair = std::pair<const K, V>;
	map.clear();

	if constexpr (!FEA_NEEDS_NESTING(K) && !FEA_NEEDS_NESTING(V)) {
		std::vector<kv_pair> kvs;
		if (!deserialize(kvs, ifs)) {
			return false;
		}
		map.insert(kvs.begin(), kvs.end());
		assert(map.size() == kvs.size());
		return true;

	} else if constexpr (FEA_NEEDS_NESTING(K) && FEA_NEEDS_NESTING(V)) {
		cerial_size_t size = 0;
		if (!deserialize(size, ifs)) {
			return false;
		}

		if constexpr (fea::is_detected_v<has_reserve, Map<K, V, Args...>>) {
			map.reserve(size);
		}

		for (cerial_size_t i = 0; i < size; ++i) {
			K k{};
			V v{};
			if (!deserialize(k, ifs)) {
				return false;
			}
			if (!deserialize(v, ifs)) {
				return false;
			}
			map.insert({ fea::maybe_move(k), fea::maybe_move(v) });
		}

		return detail::sanity_check(map, ifs, size);

	} else if constexpr (FEA_NEEDS_NESTING(K)) {
		cerial_size_t size = 0;
		if (!deserialize(size, ifs)) {
			return false;
		}

		std::vector<K> keys(size);
		assert(keys.size() == size);

		for (cerial_size_t i = 0; i < size; ++i) {
			if (!deserialize(keys[i], ifs)) {
				return false;
			}
		}

		if (!detail::sanity_check(keys, ifs, size)) {
			return false;
		}

		std::vector<V> values;
		if (!deserialize(values, ifs)) {
			return false;
		}

		if constexpr (fea::is_detected_v<has_reserve, Map<K, V, Args...>>) {
			map.reserve(size);
		}

		for (cerial_size_t i = 0; i < size; ++i) {
			map.insert({ fea::maybe_move(keys[i]), values[i] });
		}

		assert(map.size() == size);
		return true;

	} else if constexpr (FEA_NEEDS_NESTING(V)) {
		cerial_size_t size = 0;
		if (!deserialize(size, ifs)) {
			return false;
		}

		std::vector<V> values(size);
		assert(values.size() == size);
		for (cerial_size_t i = 0; i < size; ++i) {
			if (!deserialize(values[i], ifs)) {
				return false;
			}
		}

		if (!detail::sanity_check(values, ifs, size)) {
			return false;
		}

		std::vector<K> keys;
		if (!deserialize(keys, ifs)) {
			return false;
		}

		if constexpr (fea::is_detected_v<has_reserve, Map<K, V, Args...>>) {
			map.reserve(size);
		}

		for (cerial_size_t i = 0; i < size; ++i) {
			map.insert({ keys[i], fea::maybe_move(values[i]) });
		}
		assert(map.size() == size);
		return true;
	}
}

template <class T, class... Args, template <class, class...> class Set>
void serialize_set(const Set<T, Args...>& set, std::ofstream& ofs) {
	if constexpr (FEA_NEEDS_NESTING(T)) {
		cerial_size_t size(set.size());
		serialize(size, ofs);
		for (const T& t : set) {
			serialize(t, ofs);
		}
		serialize(size, ofs);
	} else {
		std::vector data(set.begin(), set.end());
		serialize(data, ofs);
	}
}

template <class T, class... Args, template <class, class...> class Set>
bool deserialize_set(Set<T, Args...>& set, std::ifstream& ifs) {
	set.clear();

	if constexpr (FEA_NEEDS_NESTING(T)) {
		cerial_size_t size = 0;
		if (!deserialize(size, ifs)) {
			return false;
		}

		if constexpr (fea::is_detected_v<has_reserve, Set<T, Args...>>) {
			set.reserve(size);
		}

		for (cerial_size_t i = 0; i < size; ++i) {
			T t{};
			if (!deserialize(t, ifs)) {
				return false;
			}
			set.insert(fea::maybe_move(t));
		}
		return detail::sanity_check(set, ifs, size);
	} else {
		std::vector<T> data;
		if (!deserialize(data, ifs)) {
			return false;
		}

		set.insert(data.begin(), data.end());
		assert(set.size() == data.size());
		return true;
	}
}

// Helper to generalize tuple-like serialization.
template <class... Args, template <class...> class Tup>
void serialize_tuple(const Tup<Args...>& tup, std::ofstream& ofs) {
	constexpr bool do_write = (!FEA_NEEDS_NESTING(Args) && ...);
	if constexpr (do_write) {
		serialize_plain(tup, ofs);
	} else {
		cerial_size_t size(std::tuple_size_v<Tup<Args...>>);
		serialize(size, ofs);
		fea::tuple_for_each([&](const auto& val) { serialize(val, ofs); }, tup);
		serialize(size, ofs);
	}
}

// Helper to generalize tuple-like deserialization.
template <class... Args, template <class...> class Tup>
bool deserialize_tuple(Tup<Args...>& tup, std::ifstream& ifs) {
	constexpr bool do_read = (!FEA_NEEDS_NESTING(Args) && ...);
	if constexpr (do_read) {
		return deserialize_plain(tup, ifs);
	} else {
		cerial_size_t size = 0;
		if (!deserialize(size, ifs)) {
			return false;
		}

		bool ok = true;
		fea::tuple_for_each(
				[&](auto& val) {
					if (!ok) {
						return;
					}
					ok = deserialize(val, ifs);
					assert(ok);
				},
				tup);

		if (!ok) {
			return false;
		}
		return detail::sanity_check(tup, ifs, size);
	}
}

} // namespace detail
} // namespace fea

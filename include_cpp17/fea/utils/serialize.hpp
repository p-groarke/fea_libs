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
#include "fea/utils/memory.hpp"
#include "fea/utils/platform.hpp"

// Q : Split into a few headers so we don't include the full stl?
#include <array>
#include <cassert>
#include <fstream>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

/*
Helpers for simple serialization and deserialization. The serialization will
recurse deeper into containers of containers. However, it is smart about
serializing "flat" containers.

For ex, a std::vector<int> will be written in one go (flat). But a
std::vector<std::vector<int>> will be recursed once. The nested vectors are
written as flat data chunks.

To achieve this, the serialize functions return std::false_type or
std::true_type. When implementing your own serialize overloads, remember to
return the appropriate value.

Container serialization has corrupted data checks.

You may define a different size_t size with FEA_SERIALIZE_SIZE_T.
By default, uses the current platform size_t size.
Use this if you expect different platforms to load your data. Though it is
up to you to make sure your serialized data-structures are cross-platform
compatible.

TODO : string, pair, tuple, fea containers, queue, deque
Q : pointers and refs? should expand or write as-is or write nullptr?
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
// Deserializes a size and checks if it is valid.
// If not, calls clear() on t.
template <class T>
void sanity_check(T& t, std::ifstream& ifs, cerial_size_t expected_size);

// Helper to generalize contiguous buffer serialization.
template <class T, class... Args, template <class, class...> class Buf>
void serialize_buffer(const Buf<T, Args...>& buf, std::ofstream& ofs);

// Helper to generalize contiguous buffer deserialization.
template <class T, class... Args, template <class, class...> class Buf>
void deserialize_buffer(Buf<T, Args...>& buf, std::ifstream& ifs);

// Helper to generalize map serialization.
template <class K, class V, class... Args,
		template <class, class, class...> class Map>
void serialize_map(const Map<K, V, Args...>& map, std::ofstream& ofs);

// Helper to generalize map deserialization.
template <class K, class V, class... Args,
		template <class, class, class...> class Map>
void deserialize_map(Map<K, V, Args...>& map, std::ifstream& ifs);

// Helper to generalize set serialization.
template <class T, class... Args, template <class, class...> class Set>
void serialize_set(const Set<T, Args...>& set, std::ofstream& ofs);

// Helper to generalize set deserialization.
template <class T, class... Args, template <class, class...> class Set>
void deserialize_set(Set<T, Args...>& set, std::ifstream& ifs);

} // namespace detail

// Serialize anything.
template <class T>
std::false_type serialize(const T& t, std::ofstream& ofs) {
	assert(ofs.is_open());
	ofs.write(reinterpret_cast<const char*>(&t), sizeof(T));
	return {};
}

// Serialize std::array.
template <class T, size_t N>
std::true_type serialize(const std::array<T, N>& arr, std::ofstream& ofs) {
	assert(ofs.is_open());

	cerial_size_t size = arr.size();
	serialize(size, ofs);

	if constexpr (FEA_NEEDS_NESTING(T)) {
		for (const T& t : arr) {
			serialize(t, ofs);
		}
	} else {
		ofs.write(reinterpret_cast<const char*>(arr.data()), sizeof(T) * size);
	}

	serialize(size, ofs);
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


// Deserialize anything.
template <class T>
void deserialize(T& t, std::ifstream& ifs) {
	assert(ifs.is_open());
	ifs.read(reinterpret_cast<char*>(&t), sizeof(T));
}

// Deserialize std::array.
// If sanity check fails, array will be defaulted and returns false.
template <class T, size_t N>
bool deserialize(std::array<T, N>& arr, std::ifstream& ifs) {
	assert(ifs.is_open());

	cerial_size_t size = 0;
	deserialize(size, ifs);

	assert(arr.size() == size);

	if constexpr (FEA_NEEDS_NESTING(T)) {
		for (cerial_size_t i = 0; i < size; ++i) {
			deserialize(arr[i], ifs);
		}
	} else {
		ifs.read(reinterpret_cast<char*>(arr.data()), sizeof(T) * size);
	}

	cerial_size_t sanity = 0;
	deserialize(sanity, ifs);

	assert(size == sanity);
	if (size != sanity) {
		arr = {};
		return false;
	}
	return true;
}

// Deserialize std::vector.
// If sanity check fails, container will be empty.
template <class T, class... Args>
void deserialize(std::vector<T, Args...>& vec, std::ifstream& ifs) {
	detail::deserialize_buffer(vec, ifs);
}

// Deserialize std::unordered_map.
// If sanity check fails, container will be empty.
template <class K, class V, class... Args>
void deserialize(std::unordered_map<K, V, Args...>& umap, std::ifstream& ifs) {
	detail::deserialize_map(umap, ifs);
}

// Deserialize std::map.
// If sanity check fails, container will be empty.
template <class K, class V, class... Args>
void deserialize(std::map<K, V, Args...>& map, std::ifstream& ifs) {
	detail::deserialize_map(map, ifs);
}

// Deserialize std::set.
// If sanity check fails, container will be empty.
template <class T, class... Args>
void deserialize(std::unordered_set<T, Args...>& uset, std::ifstream& ifs) {
	detail::deserialize_set(uset, ifs);
}

// Deserialize std::set.
// If sanity check fails, container will be empty.
template <class T, class... Args>
void deserialize(std::set<T, Args...>& set, std::ifstream& ifs) {
	detail::deserialize_set(set, ifs);
}


// Helpers
namespace detail {
template <class T>
void sanity_check(T& t, std::ifstream& ifs, cerial_size_t expected_size) {
	cerial_size_t sanity = 0;
	deserialize(sanity, ifs);

	assert(expected_size == sanity);
	if (expected_size != sanity) {
		t.clear();
	}
}

// Helper to generalize contiguous buffer serialization.
template <class T, class... Args, template <class, class...> class Buf>
void serialize_buffer(const Buf<T, Args...>& buf, std::ofstream& ofs) {
	assert(ofs.is_open());

	cerial_size_t size = buf.size();
	serialize(size, ofs);

	if constexpr (FEA_NEEDS_NESTING(T)) {
		for (const T& t : buf) {
			serialize(t, ofs);
		}
	} else {
		ofs.write(reinterpret_cast<const char*>(buf.data()), sizeof(T) * size);
	}

	serialize(size, ofs);
}

// Helper to generalize contiguous buffer deserialization.
template <class T, class... Args, template <class, class...> class Buf>
void deserialize_buffer(Buf<T, Args...>& buf, std::ifstream& ifs) {
	assert(ifs.is_open());
	buf.clear();

	cerial_size_t size = 0;
	deserialize(size, ifs);

	buf.resize(size);
	assert(buf.size() == size);

	if constexpr (FEA_NEEDS_NESTING(T)) {
		for (cerial_size_t i = 0; i < size; ++i) {
			deserialize(buf[i], ifs);
		}
	} else {
		ifs.read(reinterpret_cast<char*>(buf.data()), sizeof(T) * size);
	}

	detail::sanity_check(buf, ifs, size);
}

template <class K, class V, class... Args,
		template <class, class, class...> class Map>
void serialize_map(const Map<K, V, Args...>& map, std::ofstream& ofs) {
	assert(ofs.is_open());
	using kv_pair = std::pair<const K, V>;

	cerial_size_t size = map.size();
	serialize(size, ofs);

	if constexpr (!FEA_NEEDS_NESTING(K) && !FEA_NEEDS_NESTING(V)) {
		// Check if nothing needs nesting.
		// If that is the case, just write everything.

		std::vector<kv_pair> kvs(map.begin(), map.end());
		assert(kvs.size() == size);

		ofs.write(reinterpret_cast<const char*>(kvs.data()),
				sizeof(kv_pair) * size);

	} else if constexpr (FEA_NEEDS_NESTING(K) && FEA_NEEDS_NESTING(V)) {
		// Both keys and values need nested serialization.
		for (const kv_pair& p : map) {
			serialize(p.first, ofs);
			serialize(p.second, ofs);
		}

	} else if constexpr (FEA_NEEDS_NESTING(K)) {
		// Keys need nesting, values do not.
		// We output all keys first, then a flat buffer of the values.
		std::vector<V> values;
		values.reserve(size);

		for (const kv_pair& p : map) {
			serialize(p.first, ofs);
			values.push_back(p.second);
		}
		assert(values.size() == size);

		ofs.write(
				reinterpret_cast<const char*>(values.data()), sizeof(V) * size);

	} else if constexpr (FEA_NEEDS_NESTING(V)) {
		// Same idea as keys but in this case, values come first and keys after.
		// We do this to limit the number of loops on umap.

		std::vector<K> keys;
		keys.reserve(size);

		for (const kv_pair& p : map) {
			keys.push_back(p.first);
			serialize(p.second, ofs);
		}
		assert(keys.size() == size);

		ofs.write(reinterpret_cast<const char*>(keys.data()), sizeof(K) * size);
	}

	serialize(size, ofs);
}

template <class K, class V, class... Args,
		template <class, class, class...> class Map>
void deserialize_map(Map<K, V, Args...>& map, std::ifstream& ifs) {
	assert(ifs.is_open());
	using kv_pair = std::pair<const K, V>;
	map.clear();

	cerial_size_t size = 0;
	deserialize(size, ifs);

	if constexpr (!FEA_NEEDS_NESTING(K) && !FEA_NEEDS_NESTING(V)) {
		std::vector<kv_pair> kvs(size);
		assert(kvs.size() == size);

		ifs.read(reinterpret_cast<char*>(kvs.data()), sizeof(kv_pair) * size);
		map.insert(kvs.begin(), kvs.end());

	} else if constexpr (FEA_NEEDS_NESTING(K) && FEA_NEEDS_NESTING(V)) {
		for (cerial_size_t i = 0; i < size; ++i) {
			K k;
			V v;
			deserialize(k, ifs);
			deserialize(v, ifs);
			map.insert({ fea::maybe_move(k), fea::maybe_move(v) });
		}

	} else if constexpr (FEA_NEEDS_NESTING(K)) {
		std::vector<K> keys(size);
		std::vector<V> values(size);

		assert(keys.size() == size);
		assert(values.size() == size);

		for (cerial_size_t i = 0; i < size; ++i) {
			deserialize(keys[i], ifs);
		}
		ifs.read(reinterpret_cast<char*>(values.data()), sizeof(V) * size);

		for (cerial_size_t i = 0; i < size; ++i) {
			map.insert({ keys[i], values[i] });
		}

	} else if constexpr (FEA_NEEDS_NESTING(V)) {
		std::vector<K> keys(size);
		std::vector<V> values(size);

		assert(keys.size() == size);
		assert(values.size() == size);

		for (cerial_size_t i = 0; i < size; ++i) {
			deserialize(values[i], ifs);
		}
		ifs.read(reinterpret_cast<char*>(keys.data()), sizeof(K) * size);

		for (cerial_size_t i = 0; i < size; ++i) {
			map.insert({ keys[i], values[i] });
		}
	}
	assert(map.size() == size);

	detail::sanity_check(map, ifs, size);
}

template <class T, class... Args, template <class, class...> class Set>
void serialize_set(const Set<T, Args...>& set, std::ofstream& ofs) {
	assert(ofs.is_open());

	cerial_size_t size = set.size();
	serialize(size, ofs);

	if constexpr (FEA_NEEDS_NESTING(T)) {
		for (const T& t : set) {
			serialize(t, ofs);
		}
	} else {
		std::vector data(set.begin(), set.end());
		assert(data.size() == size);
		ofs.write(reinterpret_cast<const char*>(data.data()), sizeof(T) * size);
	}

	serialize(size, ofs);
}

template <class T, class... Args, template <class, class...> class Set>
void deserialize_set(Set<T, Args...>& set, std::ifstream& ifs) {
	assert(ifs.is_open());
	set.clear();

	cerial_size_t size = 0;
	deserialize(size, ifs);

	if constexpr (FEA_NEEDS_NESTING(T)) {
		set.reserve(size);

		for (cerial_size_t i = 0; i < size; ++i) {
			T t;
			deserialize(t, ifs);
			set.insert(fea::maybe_move(t));
		}
	} else {
		std::vector<T> data(size);
		ifs.read(reinterpret_cast<char*>(data.data()), sizeof(T) * size);
		set.insert(data.begin(), data.end());
	}
	assert(set.size() == size);
	detail::sanity_check(set, ifs, size);
}

} // namespace detail
} // namespace fea

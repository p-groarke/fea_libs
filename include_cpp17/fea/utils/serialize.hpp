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
#include <deque>
#include <fstream>
#include <map>
#include <queue>
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
std::true_type unless your type may be directly serialized.

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

fea::serialize_scope and fea::deserialize_enter/deserialize_exit helpers are
available to you. You may use them to simplify and harden your custom class
serialization.

TODO : queue, deque, fea containers
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
// Detects if a type has reserve()
template <class T>
using has_reserve = decltype(std::declval<T>().reserve(std::declval<size_t>()));

// Detects if a type has clear()
template <class T>
using has_clear = decltype(std::declval<T>().clear());

// Detects if a type has size()
template <class T>
using has_size = decltype(std::declval<T>().size());

// Helper to generalize plain (non-nested) serialization.
template <class T>
void serialize_plain(const T& data, std::ofstream& ofs);

// Helper to generalize plain (non-nested) deserialization.
template <class T>
[[nodiscard]] bool deserialize_plain(T& data, std::ifstream& ifs);

// Helper to generalize contiguous array serialization.
template <class T, size_t N, template <class, size_t> class Arr>
[[nodiscard]] auto serialize_array(const Arr<T, N>& arr, std::ofstream& ofs);

// Helper to generalize contiguous array deserialization.
template <class T, size_t N, template <class, size_t> class Arr>
[[nodiscard]] bool deserialize_array(Arr<T, N>& arr, std::ifstream& ifs);

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
[[nodiscard]] auto serialize_tuple(const Tup<Args...>& tup, std::ofstream& ofs);

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
auto serialize(const std::pair<K, V>& pair, std::ofstream& ofs) {
	return detail::serialize_tuple(pair, ofs);
}

// Serialize std::tuple.
template <class... Args>
auto serialize(const std::tuple<Args...>& tup, std::ofstream& ofs) {
	return detail::serialize_tuple(tup, ofs);
}

// Serialize std::array.
template <class T, size_t N>
auto serialize(const std::array<T, N>& arr, std::ofstream& ofs) {
	return detail::serialize_array(arr, ofs);
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

// Serialize std::deque.
template <class T, class... Args>
std::true_type serialize(
		const std::deque<T, Args...>& deq, std::ofstream& ofs) {
	std::vector<T, Args...> v(deq.begin(), deq.end());
	detail::serialize_buffer(v, ofs);
	return {};
}

// Serialize std::queue.
template <class T, class... Args>
std::true_type serialize(const std::queue<T, Args...>& q, std::ofstream& ofs) {
	std::queue<T, Args...> cpy = q;
	std::vector<T> v;
	v.reserve(cpy.size());
	while (!cpy.empty()) {
		v.push_back(fea::maybe_move(cpy.front()));
		cpy.pop();
	}
	detail::serialize_buffer(v, ofs);
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
	return detail::deserialize_array(arr, ifs);
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

// Deserialize std::deque.
// If sanity check fails, container will be empty.
template <class T, class... Args>
[[nodiscard]] bool deserialize(
		std::deque<T, Args...>& deq, std::ifstream& ifs) {
	std::vector<T, Args...> v;
	if (!detail::deserialize_buffer(v, ifs)) {
		return false;
	}
	deq.clear();
	deq.insert(deq.begin(), fea::maybe_make_move_iterator(v.begin()),
			fea::maybe_make_move_iterator(v.end()));
	return deq.size() == v.size();
}

// Deserialize std::queue.
// If sanity check fails, container will be empty.
template <class T, class... Args>
[[nodiscard]] bool deserialize(std::queue<T, Args...>& q, std::ifstream& ifs) {
	assert(q.empty());

	std::vector<T> v;
	if (!detail::deserialize_buffer(v, ifs)) {
		return false;
	}

	for (auto& val : v) {
		q.push(fea::maybe_move(val));
	}
	return q.size() == v.size();
}


// User helpers.
// Serializes the container size and T size.
// On destruction, serializes the container size again (sentinel).
template <class T>
struct serialize_scope {
	serialize_scope(size_t size, std::ofstream& ofs)
			: _size(size)
			, _ofs(ofs) {
		serialize(_size, _ofs);
		cerial_size_t t_size(sizeof(T));
		serialize(t_size, _ofs);
	}
	~serialize_scope() {
		serialize(_size, _ofs);
	}
	cerial_size_t size() const {
		return _size;
	}

private:
	cerial_size_t _size = 0;
	std::ofstream& _ofs;
};

// Deserializes container size, then type size.
// Checks to make sure sizeof(T) is ok.
template <class T>
bool deserialize_enter(cerial_size_t& size, std::ifstream& ifs) {
	size = 0;
	if (!deserialize(size, ifs)) {
		return false;
	}

	cerial_size_t t_size = 0;
	if (!deserialize(t_size, ifs)) {
		return false;
	}

	if (t_size != sizeof(T)) {
		assert(false);
		return false;
	}
	return true;
}

// Checks that size sentinel is equal to expected_size.
// Also checks your container is of correct size.
// Resets passed in object on failure.
template <class T>
bool deserialize_exit(T& t, std::ifstream& ifs, cerial_size_t expected_size) {
	// Also check the container contains the right amount of things.
	if constexpr (fea::is_detected_v<detail::has_size, T>) {
		if (t.size() != expected_size) {
			assert(false);
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
		if constexpr (fea::is_detected_v<detail::has_clear, T>) {
			t.clear();
		} else {
			t = {};
		}
		return false;
	}
	return true;
}


// Helpers
namespace detail {
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
void serialize_plain(const T& data, std::ofstream& ofs) {
	write(ofs, reinterpret_cast<const char*>(&data), sizeof(T));
}

// Helper to generalize plain (non-nested) deserialization.
template <class T>
bool deserialize_plain(T& data, std::ifstream& ifs) {
	return read(ifs, reinterpret_cast<char*>(&data), sizeof(T));
}

// Helper to generalize contiguous array serialization.
template <class T, size_t N, template <class, size_t> class Arr>
[[nodiscard]] auto serialize_array(const Arr<T, N>& arr, std::ofstream& ofs) {
	serialize_scope<Arr<T, N>> s(arr.size(), ofs);

	if constexpr (FEA_NEEDS_NESTING(T)) {
		for (const T& t : arr) {
			serialize(t, ofs);
		}
		return std::true_type{};
	} else {
		serialize_plain(arr, ofs);
		return std::false_type{};
	}
}

// Helper to generalize contiguous array deserialization.
template <class T, size_t N, template <class, size_t> class Arr>
[[nodiscard]] bool deserialize_array(Arr<T, N>& arr, std::ifstream& ifs) {
	cerial_size_t size = 0;
	deserialize_enter<Arr<T, N>>(size, ifs);

	if constexpr (FEA_NEEDS_NESTING(T)) {
		for (cerial_size_t i = 0; i < size; ++i) {
			if (!deserialize(arr[i], ifs)) {
				return false;
			}
		}
	} else {
		if (!deserialize_plain(arr, ifs)) {
			return false;
		}
	}

	return deserialize_exit(arr, ifs, size);
}

// Helper to generalize contiguous buffer serialization.
template <class T, class... Args, template <class, class...> class Buf>
void serialize_buffer(const Buf<T, Args...>& buf, std::ofstream& ofs) {
	serialize_scope<T> s(buf.size(), ofs);

	if constexpr (FEA_NEEDS_NESTING(T)) {
		for (const T& t : buf) {
			serialize(t, ofs);
		}
	} else {
		write(ofs, reinterpret_cast<const char*>(buf.data()),
				sizeof(T) * s.size());
	}
}

// Helper to generalize contiguous buffer deserialization.
template <class T, class... Args, template <class, class...> class Buf>
bool deserialize_buffer(Buf<T, Args...>& buf, std::ifstream& ifs) {
	cerial_size_t size = 0;
	if (!deserialize_enter<T>(size, ifs)) {
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
		if (!read(ifs, reinterpret_cast<char*>(buf.data()), sizeof(T) * size)) {
			return false;
		}
	}

	return deserialize_exit(buf, ifs, size);
}

template <class K, class V, class... Args,
		template <class, class, class...> class Map>
void serialize_map(const Map<K, V, Args...>& map, std::ofstream& ofs) {
	using kv_pair = std::pair<const K, V>;
	serialize_scope<kv_pair> s(map.size(), ofs);

	if constexpr (!FEA_NEEDS_NESTING(K) && !FEA_NEEDS_NESTING(V)) {
		// Check if nothing needs nesting.
		// If that is the case, just write everything.

		std::vector<kv_pair> kvs(map.begin(), map.end());
		assert(kvs.size() == map.size());
		serialize(kvs, ofs);

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
		values.reserve(s.size());

		for (const kv_pair& p : map) {
			serialize(p.first, ofs);
			values.push_back(p.second);
		}

		assert(values.size() == map.size());
		serialize(values, ofs);

	} else if constexpr (FEA_NEEDS_NESTING(V)) {
		// Same idea as keys but in this case, values come first and keys
		// after. We do this to limit the number of loops on umap.

		std::vector<K> keys;
		keys.reserve(s.size());

		for (const kv_pair& p : map) {
			keys.push_back(p.first);
			serialize(p.second, ofs);
		}

		assert(keys.size() == map.size());
		serialize(keys, ofs);
	}
}

template <class K, class V, class... Args,
		template <class, class, class...> class Map>
bool deserialize_map(Map<K, V, Args...>& map, std::ifstream& ifs) {
	using kv_pair = std::pair<const K, V>;

	cerial_size_t size = 0;
	if (!deserialize_enter<kv_pair>(size, ifs)) {
		return false;
	}

	map.clear();
	if constexpr (!FEA_NEEDS_NESTING(K) && !FEA_NEEDS_NESTING(V)) {
		std::vector<kv_pair> kvs;
		if (!deserialize(kvs, ifs)) {
			return false;
		}
		// Don't need to move, non-nested types are self-contained.
		map.insert(kvs.begin(), kvs.end());

	} else if constexpr (FEA_NEEDS_NESTING(K) && FEA_NEEDS_NESTING(V)) {
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


	} else if constexpr (FEA_NEEDS_NESTING(K)) {
		std::vector<K> keys(size);
		assert(keys.size() == size);

		for (cerial_size_t i = 0; i < size; ++i) {
			if (!deserialize(keys[i], ifs)) {
				return false;
			}
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

	} else if constexpr (FEA_NEEDS_NESTING(V)) {
		std::vector<V> values(size);
		assert(values.size() == size);

		for (cerial_size_t i = 0; i < size; ++i) {
			if (!deserialize(values[i], ifs)) {
				return false;
			}
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
	}

	return deserialize_exit(map, ifs, size);
}

template <class T, class... Args, template <class, class...> class Set>
void serialize_set(const Set<T, Args...>& set, std::ofstream& ofs) {
	serialize_scope<T> s(set.size(), ofs);

	if constexpr (FEA_NEEDS_NESTING(T)) {
		for (const T& t : set) {
			serialize(t, ofs);
		}
	} else {
		std::vector data(set.begin(), set.end());
		serialize(data, ofs);
	}
}

template <class T, class... Args, template <class, class...> class Set>
bool deserialize_set(Set<T, Args...>& set, std::ifstream& ifs) {
	cerial_size_t size = 0;
	deserialize_enter<T>(size, ifs);

	set.clear();
	if constexpr (FEA_NEEDS_NESTING(T)) {
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

	} else {
		std::vector<T> data;
		if (!deserialize(data, ifs)) {
			return false;
		}

		// Don't need to move, non-nested data.
		set.insert(data.begin(), data.end());
	}

	return deserialize_exit(set, ifs, size);
}

// Helper to generalize tuple-like serialization.
template <class... Args, template <class...> class Tup>
auto serialize_tuple(const Tup<Args...>& tup, std::ofstream& ofs) {
	serialize_scope<Tup<Args...>> s(std::tuple_size_v<Tup<Args...>>, ofs);

	constexpr bool do_write = (!FEA_NEEDS_NESTING(Args) && ...);
	if constexpr (do_write) {
		serialize_plain(tup, ofs);
		return std::false_type{};
	} else {
		fea::tuple_for_each([&](const auto& val) { serialize(val, ofs); }, tup);
		return std::true_type{};
	}
}

// Helper to generalize tuple-like deserialization.
template <class... Args, template <class...> class Tup>
bool deserialize_tuple(Tup<Args...>& tup, std::ifstream& ifs) {
	cerial_size_t size = 0;
	deserialize_enter<Tup<Args...>>(size, ifs);

	constexpr bool do_read = (!FEA_NEEDS_NESTING(Args) && ...);
	if constexpr (do_read) {
		if (!deserialize_plain(tup, ifs)) {
			return false;
		}
	} else {
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
	}
	return deserialize_exit(tup, ifs, size);
}

} // namespace detail
} // namespace fea

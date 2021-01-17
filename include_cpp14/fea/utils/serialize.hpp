#pragma once
#include "fea/utils/memory.hpp"
#include "fea/utils/platform.hpp"

#include <cassert>
#include <fstream>
#include <set>
#include <unordered_map>
#include <vector>

/*
Helpers for simple serialization and deserialization.

Container serialization has sanity checks.
*/

namespace fea {
template <class T>
struct serializer {
	static constexpr size_t depth = 0;
};

// Single serialized arg container.
template <class T, template <class> class Container>
struct serializer<Container<T>> {
	static constexpr size_t depth = 1 + serializer<T>::depth;
};

template <class T>
struct serialize_depth : std::integral_constant<size_t, 0> {};

template <class T, class... Args>
struct serialize_depth<std::vector<T, Args...>> {
	static constexpr size_t value = 1 + serialize_depth<T>::value;
};

// template <class... Args, class Test = serialize_more<std::vector<Args...>>>
// struct serialize_more<std::vector<Args...>> : std::false_type {};

template <class T>
FEA_INLINE_VAR constexpr bool serialize_more_v = serialize_depth<T>::value > 1;


// Serialize anything.
template <class T>
void serialize(const T& t, std::ofstream& ofs) {
	assert(ofs.is_open());
	ofs.write(reinterpret_cast<const char*>(&t), sizeof(T));
}

// Serialize vector.
// First serialize size, then data, then size again for sanity check.
template <class T, class... Args>
void serialize(const std::vector<T, Args...>& vec, std::ofstream& ofs) {
	assert(ofs.is_open());

	size_t s = vec.size();
	serialize(s, ofs);
	ofs.write(reinterpret_cast<const char*>(vec.data()), sizeof(T) * s);
	serialize(s, ofs);
}

// Serialize unordered_map.
template <class K, class V, class... Args>
void serialize(
		const std::unordered_map<K, V, Args...>& umap, std::ofstream& ofs) {
	assert(ofs.is_open());

	size_t s = umap.size();
	std::vector<K> keys;
	std::vector<V> values;
	keys.reserve(s);
	values.reserve(s);

	for (const std::pair<K, V>& p : umap) {
		keys.push_back(p.first);
		values.push_back(p.second);
	}

	assert(keys.size() == s);
	assert(values.size() == s);

	serialize(s, ofs);
	ofs.write(reinterpret_cast<const char*>(keys.data()), sizeof(K) * s);
	ofs.write(reinterpret_cast<const char*>(values.data()), sizeof(V) * s);
	serialize(s, ofs);
}

// Serialize set.
template <class T, class... Args>
void serialize(const std::set<T, Args...>& set, std::ofstream& ofs) {
	assert(ofs.is_open());

	size_t s = set.size();
	std::vector data(set.begin(), set.end());
	assert(data.size() == s);

	serialize(s, ofs);
	ofs.write(reinterpret_cast<const char*>(data.data()), sizeof(T) * s);
	serialize(s, ofs);
}


// Deserialize anything.
template <class T>
void deserialize(T& t, std::ifstream& ifs) {
	assert(ifs.is_open());
	ifs.read(reinterpret_cast<char*>(&t), sizeof(T));
}

// Deserialize vector.
// First deserialize size, then data, then size again and sanity check.
// If sanity check fails, vector is cleared as data cannot be trusted.
template <class T, class... Args>
void deserialize(std::vector<T, Args...>& vec, std::ifstream& ifs) {
	assert(ifs.is_open());
	size_t s = 0;
	deserialize(s, ifs);

	vec.resize(s);
	ifs.read(reinterpret_cast<char*>(vec.data()), sizeof(T) * s);

	size_t sanity = 0;
	deserialize(sanity, ifs);

	assert(s == sanity);
	if (s != sanity) {
		vec.clear();
	}
}

// Deserialize unordered_map.
template <class K, class V, class... Args>
void deserialize(std::unordered_map<K, V, Args...>& umap, std::ifstream& ifs) {
	assert(ifs.is_open());

	size_t s = 0;
	deserialize(s, ifs);

	std::vector<K> keys(s);
	std::vector<V> values(s);
	umap.clear();
	umap.reserve(s);

	ifs.read(reinterpret_cast<char*>(keys.data()), sizeof(K) * s);
	ifs.read(reinterpret_cast<char*>(values.data()), sizeof(V) * s);

	size_t sanity = 0;
	deserialize(sanity, ifs);

	assert(s == sanity);
	if (s != sanity) {
		return;
	}

	for (size_t i = 0; i < s; ++i) {
		umap.insert({ fea::maybe_move(keys[i]), fea::maybe_move(values[i]) });
	}

	assert(umap.size() == s);
}

// Deserialize set.
template <class T, class... Args>
void deserialize(std::set<T, Args...>& set, std::ifstream& ifs) {
	assert(ifs.is_open());

	size_t s = 0;
	deserialize(s, ifs);

	std::vector<T> data(s);
	ifs.read(reinterpret_cast<char*>(data.data()), sizeof(T) * s);

	size_t sanity = 0;
	deserialize(sanity, ifs);

	assert(s == sanity);
	if (s != sanity) {
		return;
	}

	set = std::set(data.begin(), data.end());
}


} // namespace fea

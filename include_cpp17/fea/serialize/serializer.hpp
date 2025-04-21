/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2025, Philippe Groarke
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
#include "fea/utility/file.hpp"
#include "fea/utility/platform.hpp"
#include "fea/utility/scope.hpp"
#include "fea/utility/throw.hpp"

#include <algorithm>
#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <vector>

/*
fea::serializer and fea::deserializer builds a binary representation of your
data. It adds size sentinels for buffer types to detect data corruption, and it
checks to make sure your deserialization structures match the serialized
structures.

When constructing the serializer structs with file paths, the serializer will
write to file on destruction or if the user calls flush. Whichever comes first.

When constructing the serializer with the default constructor, it will not do
anything with the data. You must call 'extract()' to retrieve the serialized
data. The destructor will check you emptied the serializer.
*/

// #define FEA_THROW_MSG(x) std::string{ __FUNCTION__ } + " : " + x

static_assert(std::is_unsigned_v<FEA_SERIALIZE_SIZE_T>,
		"FEA_SERIALIZE_SIZE_T must be unsigned");

namespace fea {
namespace detail {
struct size_token {
	// The size of a given objects.
	FEA_SERIALIZE_SIZE_T size = 0;
	// How many calls wrote this size.
	FEA_SERIALIZE_SIZE_T count = 0;
};
static_assert(std::is_trivially_copyable_v<detail::size_token>,
		"fea::serializer : something has gone horribly wrong");
} // namespace detail

// Builds data in memory and serializes to file or makes it available to user.
struct serializer {
	using msize_t = FEA_SERIALIZE_SIZE_T;

	// Serialize data only.
	serializer() = default;

	// Serialize to file.
	serializer(const std::filesystem::path& filepath)
			: _filepath(filepath) {
	}

	~serializer() {
		if (_filepath.empty()) {
			// Make sure user extracted data, or else pointless.
			assert(_size_table.empty());
			assert(_data.empty());
			return;
		}

		std::FILE* ofs = fea::fopen(_filepath, "wb");
		if (ofs == nullptr) {
			fea::print_error_message(__FUNCTION__, __LINE__,
					"Couldn't open file '" + _filepath.string() + "'.");
			return;
		}

		fea::on_exit e{ [&ofs]() { std::fclose(ofs); } };

		std::vector<std::byte> data = extract();
		size_t written = std::fwrite(
				data.data(), sizeof(data.front()), data.size(), ofs);

		if (written != data.size()) {
			fea::maybe_throw(__FUNCTION__, __LINE__,
					"Couldn't write to file '" + _filepath.string() + "'.");
		}
	}

	// Doesn't write type size. Used for sentinels.
	template <class T>
	void write_unvalidated(const T& t) {
		static_assert(std::is_trivially_copyable_v<T>,
				"fea::serializer : Cannot serialize non-trivially copyable "
				"type. "
				"You must implement 'friend void serialize(const T&, "
				"fea::serializer&);'");

		// No count sentinel for single objects.
		push_back(&t, 1, &_data);
	}

	// Overload for single objects.
	template <class T>
	void write(const T& t) {
		// Keep track of binary compat.
		push_size_token(sizeof(T));
		write_unvalidated(t);
	}

	// Overload for buffers.
	template <class T>
	void write(const T* ts, size_t count) {
		static_assert(std::is_trivially_copyable_v<T>,
				"fea::serializer : Cannot serialize non-trivially copyable "
				"type. "
				"You must implement 'friend void serialize(const T&, "
				"fea::serializer&);'");

		push_back(ts, count, &_data);
		push_size_token(sizeof(T));
	}

	// Moves out the serialized data. Serializer is left empty.
	[[nodiscard]] std::vector<std::byte> extract() {
		if (_data.empty()) {
			assert(_size_table.empty());
			return {};
		}

		// Cleans vectors on exit.
		fea::on_exit e{ [this]() {
			_size_table.clear();
			_data.clear();
		} };

		// size_table data + 2 count sentinels.
		size_t table_byte_size = sizeof(detail::size_token) * _size_table.size()
				+ sizeof(msize_t) * 2;

		std::vector<std::byte> ret;
		ret.reserve(table_byte_size + _data.size());

		// Write size_table to beginning of data stream.

		// Sentinel for deserializing.
		msize_t count_sentinel = msize_t(_size_table.size());
		push_back(&count_sentinel, 1, &ret);
		push_back(_size_table.data(), _size_table.size(), &ret);
		push_back(&count_sentinel, 1, &ret);

		push_back(_data.data(), _data.size(), &ret);

		return ret;
	}

	// Reserve more memory, in bytes.
	// Don't need to call this when serializing containers.
	void reserve(size_t new_cap_bytes) {
		_data.reserve(_data.capacity() + new_cap_bytes);
	}

	// Overload that computes reserve bytes for T and count.
	template <class T>
	void reserve(size_t count) {
		_data.reserve(_data.capacity() + sizeof(T) * count);
	}

private:
	template <class T>
	static void push_back(
			const T* ts, size_t count, std::vector<std::byte>* vec) {
		size_t begin = vec->size();
		vec->resize(vec->size() + sizeof(T) * count);
		std::memcpy(vec->data() + begin, ts, sizeof(T) * count);
	}

	void push_size_token(size_t obj_size) {
		assert(obj_size != 0);

		if (!_size_table.empty() && _size_table.back().size == obj_size) {
			// Previous objects were same size, increment count.
			++_size_table.back().count;
			return;
		}

		// If not, add new size token.
		_size_table.push_back({ msize_t(obj_size), msize_t(1) });
	}

	// The file to write.
	std::filesystem::path _filepath;

	// This is a rolling vector of type sizes, in order of written data.
	// A new size_info is added when a new type size is encountered.
	// It contains a size and the number of elements of a given size.
	// It is used for binary compatibility checking open deserialization.
	std::vector<detail::size_token> _size_table;

	// The data to serialize. First stored in vector and written to file in one
	// go.
	std::vector<std::byte> _data;
};

struct deserializer {
	using msize_t = FEA_SERIALIZE_SIZE_T;

	// Deserialize from memory.
	deserializer(const std::vector<std::byte>& data)
			: _data(data) {
		if (!deserialize_size_table()) {
			_is_gucci = false;
			clear();
		}
	}

	// Deserialize from memory.
	deserializer(std::vector<std::byte>&& data)
			: _data(std::move(data)) {
		if (!deserialize_size_table()) {
			_is_gucci = false;
			clear();
		}
	}

	// Deserialize from file.
	deserializer(const std::filesystem::path& filepath)
			: _filepath(filepath) {

		std::FILE* ifs = fea::fopen(_filepath, "rb");
		if (ifs == nullptr) {
			fea::print_error_message(__FUNCTION__, __LINE__,
					"Couldn't open file '" + _filepath.string() + "'.");
			_is_gucci = false;
			return;
		}

		fea::on_exit e{ [&ifs]() { std::fclose(ifs); } };

		size_t total_size = fea::file_size(ifs);
		_data.resize(total_size);

		// Read everything.
		size_t read_count = std::fread(
				_data.data(), sizeof(_data.front()), total_size, ifs);

		if (read_count != total_size) {
			fea::maybe_throw(__FUNCTION__, __LINE__,
					"Problem reading file '" + _filepath.string() + "'.");
		}

		if (!deserialize_size_table()) {
			_is_gucci = false;
			clear();
		}
	}

	// Check if constructors executed correctly.
	[[nodiscard]] bool is_gucci() const noexcept {
		return _is_gucci;
	}

	// Reset everything except filepath.
	void clear() noexcept {
		_size_table.clear();
		_deserialized_counts.clear();
		_data.clear();
		_size_table_idx = 0;
		_data_idx = 0;
	}

	// Doesn't register type sizes. Used for sentinels.
	template <class T>
	[[nodiscard]] bool read_unvalidated(T& t) {
		static_assert(std::is_trivially_copyable_v<T>,
				"fea::deserializer : Cannot deserialize non-trivially copyable "
				"type. "
				"You must implement 'friend bool deserialize(T&, "
				"fea::deserializer&);'");
		return pop_front(&t, 1);
	}

	// Overload for single object.
	template <class T>
	[[nodiscard]] bool read(T& t) {
		// First, check if the provided size matches the expect size.
		// AKA, did you break your binary compatibility?
		if (!validate_size<T>()) {
			return false;
		}
		// Size matches, deserialize.
		return read_unvalidated(t);
	}

	// Overload for object buffers.
	template <class T>
	[[nodiscard]] bool read(T* t, size_t count) {
		static_assert(std::is_trivially_copyable_v<T>,
				"fea::deserializer : Cannot deserialize non-trivially copyable "
				"type. "
				"You must implement 'friend bool deserialize(T&, "
				"fea::deserializer&);'");

		// First, check if the provided size matches the expect size.
		// AKA, did you break your binary compatibility?
		if (!validate_size<T>()) {
			return false;
		}

		// Size matches, deserialize.
		return pop_front(t, count);
	}


private:
	// Deserializes data into memory.
	// Prepares structure.
	[[nodiscard]] bool deserialize_size_table() {
		msize_t size_table_count = 0;
		if (!pop_front(&size_table_count, 1)) {
			return false;
		}

		_size_table.resize(size_table_count);
		if (!pop_front(_size_table.data(), _size_table.size())) {
			return false;
		}

		msize_t size_table_count2 = 0;
		if (!pop_front(&size_table_count2, 1)) {
			return false;
		}

		if (size_table_count != size_table_count2) {
			// Corrupted data.
			assert(false);
			return false;
		}

		// Special check for first one.
		if (_size_table[_size_table_idx].size == 0) {
			assert(false);
			return false;
		}

		if (_size_table[_size_table_idx].count == 0) {
			assert(false);
			return false;
		}

		_deserialized_counts.resize(_size_table.size(), 0);
		return true;
	}

	template <class T>
	[[nodiscard]] bool pop_front(T* t, size_t count) {
		if (_data_idx >= _data.size()) {
			assert(false);
			return false;
		}

		std::memcpy(static_cast<void*>(t), _data.data() + _data_idx,
				sizeof(T) * count);
		_data_idx += sizeof(T) * count;
		return true;
	}

	template <class T>
	[[nodiscard]] bool validate_size() {
		// Check if we need to bump index.
		if (_size_table[_size_table_idx].count
				== _deserialized_counts[_size_table_idx]) {
			++_size_table_idx;

			// _size_table_idx 0 checks are done in ctor.
			if (_size_table[_size_table_idx].size == 0) {
				assert(false);
				return false;
			}

			if (_size_table[_size_table_idx].count == 0) {
				assert(false);
				return false;
			}
		}

		if (_size_table[_size_table_idx].size != sizeof(T)) {
			assert(false);
			return false;
		}
		++_deserialized_counts[_size_table_idx];
		return true;
	}

	// The file to read.
	std::filesystem::path _filepath;

	// This is a rolling vector of type sizes, in order of written data.
	// A new size_info is added when a new type size is encountered.
	// When reading, the deserializer will return failure if their is a mismatch
	// between your provided type sizes and read in data.
	std::vector<detail::size_token> _size_table;
	std::vector<msize_t> _deserialized_counts;

	// The deserialized data. Read in one go from file.
	std::vector<std::byte> _data;

	// Indexes of read data.
	size_t _size_table_idx = 0;
	size_t _data_idx = 0;

	bool _is_gucci = true;
}; // namespace fea


} // namespace fea

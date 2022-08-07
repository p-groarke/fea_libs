/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2022, Philippe Groarke
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
#include "fea/containers/span.hpp"
#include "fea/utils/platform.hpp"
#include "fea/utils/throw.hpp"

#include <cstddef>
#include <cstdint>
#include <filesystem>

#if FEA_WINDOWS
#include <windows.h>
#else
#endif

/*
A basic, cross-platform, memory-mapped file view.
fmap maps whole files to virtual memory, either in read or write mode.
*/

namespace fea {
enum class fmap_mode : uint8_t {
	// read_write,
	read,
	write,
	count,
};

namespace detail {
struct fmap_os_data {
	fmap_os_data() = default;
	fmap_os_data(const fmap_os_data&) = default;
	fmap_os_data& operator=(const fmap_os_data&) = default;

	fmap_os_data(fmap_os_data&& other) noexcept
#if FEA_WINDOWS
			: file_handle(other.file_handle)
			, map_handle(other.map_handle)
#else
#endif
			, ptr(other.ptr)
			, byte_size(other.byte_size) {

#if FEA_WINDOWS
		other.file_handle = nullptr;
		other.map_handle = nullptr;
#else
#endif
		other.ptr = nullptr;
		other.byte_size = 0;
	}

	fmap_os_data& operator=(fmap_os_data&& other) noexcept {
		if (this != &other) {
#if FEA_WINDOWS
			file_handle = other.file_handle;
			map_handle = other.map_handle;
			other.file_handle = nullptr;
			other.map_handle = nullptr;
#else
#endif
			ptr = other.ptr;
			byte_size = other.byte_size;
			other.ptr = nullptr;
			other.byte_size = 0;
		}
		return *this;
	}

#if FEA_WINDOWS
	HANDLE file_handle = nullptr;
	HANDLE map_handle = nullptr;
#else
#endif

	void* ptr = nullptr;
	size_t byte_size = 0;
};

template <class T>
fmap_os_data os_map(const std::filesystem::path& filepath, fmap_mode mode) {
	if (!std::filesystem::exists(filepath)
			|| std::filesystem::is_directory(filepath)) {
		return {};
	}

	size_t file_size = std::filesystem::file_size(filepath);
	if (file_size == 0) {
		return {};
	}

	if (file_size % sizeof(T) != 0) {
		fea::maybe_throw(
				__FUNCTION__, __LINE__, "Cannot map file to T*, size invalid.");
		return {};
	}

	assert(mode != fmap_mode::count);
	assert(!filepath.empty());

	fmap_os_data ret;

#if FEA_WINDOWS
	DWORD win_mode = mode == fmap_mode::write ? GENERIC_READ | GENERIC_WRITE
											  : GENERIC_READ;

	ret.file_handle = CreateFileW(filepath.wstring().c_str(), win_mode,
			FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, nullptr);

	if (ret.file_handle == INVALID_HANDLE_VALUE) {
		// todo : GetLastError?
		return {};
	}

	ret.map_handle = CreateFileMapping(
			ret.file_handle, nullptr, PAGE_READONLY, 0, 0, nullptr);
	if (ret.map_handle == nullptr) {
		// todo : GetLastError
		return {};
	}

	ret.ptr = MapViewOfFile(ret.map_handle, FILE_MAP_READ, 0, 0, 0);
	if (ret.ptr == nullptr) {
		// todo : GetLastError
		return {};
	}

#else
#endif

	ret.byte_size = file_size;
	return ret;
}

void os_unmap(const fmap_os_data& os_data) {
	assert(os_data.ptr != nullptr);
	assert(os_data.byte_size != 0);

#if FEA_WINDOWS
	if (!FlushViewOfFile(os_data.ptr, 0)) {
		// todo : getlasterror
		// return;
	}

	if (!FlushFileBuffers(os_data.file_handle)) {
		// todo : getlasterror
		// return;
	}

	if (!UnmapViewOfFile(os_data.ptr)) {
		// todo : getlasterror
		// return;
	}

	if (!CloseHandle(os_data.map_handle)) {
		// todo : getlasterror
		// return;
	}

	if (!CloseHandle(os_data.file_handle)) {
	}

	// if (!bFlag) {
	//	_tprintf(TEXT("\nError %ld occurred closing the file!"),
	//			GetLastError());
	//}
#else
#endif
}

} // namespace detail

// template <fmap_mode mode, class>
// struct basic_fmap;

template <class T>
struct basic_fmap_read {
	using value_type = T;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	// using reference = const value_type&;
	using const_reference = const value_type&;
	// using pointer = const value_type*;
	using const_pointer = const value_type*;

	using const_iterator = typename fea::span<const T>::iterator;
	using const_reverse_iterator =
			typename fea::span<const T>::reverse_iterator;

	// Ctors
	basic_fmap_read() = default;

	explicit basic_fmap_read(const std::filesystem::path& filepath)
			: _os_data(detail::os_map<T>(filepath, fmap_mode::read))
			, _data(static_cast<const T*>(_os_data.ptr),
					  _os_data.byte_size / sizeof(T)) {
	}

	explicit basic_fmap_read(std::filesystem::path&& p)
			: basic_fmap_read(p) {
	}

	~basic_fmap_read() {
		if (!is_open()) {
			return;
		}

		detail::os_unmap(_os_data);
	}

	basic_fmap_read(basic_fmap_read&& other) noexcept
			: _os_data(std::move(other._os_data))
			, _data(std::move(other._data)) {
		other._data = {};
	}

	basic_fmap_read& operator=(basic_fmap_read&& other) noexcept {
		if (this != &other) {
			_os_data = std::move(other._os_data);
			_data = std::move(other._data);
			other._data = {};
		}
		return *this;
	}

	// Non-copyable
	basic_fmap_read(const basic_fmap_read&) = delete;
	basic_fmap_read& operator=(const basic_fmap_read&) = delete;

	/**
	 * Iterators
	 */

	// Returns an iterator pointing to the beginning of mapped data.
	const_iterator begin() const noexcept {
		return _data.begin();
	}

	// Returns an iterator pointing to the end of mapped data.
	const_iterator end() const noexcept {
		return _data.end();
	}

	// Returns an iterator pointing to the beginning of the reversed data.
	const_reverse_iterator rbegin() const noexcept {
		return _data.rbegin();
	}

	// Returns an iterator pointing to the end of the reversed data.
	const_reverse_iterator rend() const noexcept {
		return _data.rend();
	}

	/**
	 * Element access
	 */

	// Returns the mapped data.
	const_pointer data() const noexcept {
		return _data.data();
	}

	// Returns the nth element.
	const_reference operator[](size_t idx) const {
		return _data[idx];
	}

	/**
	 * Observers
	 */

	// Returns true if the file was mapped without errors.
	bool is_open() const noexcept {
		return !empty();
	}

	// Returns the size of the available data.
	// If using the default fmaps, this is the byte size.
	size_type size() const noexcept {
		return _data.size();
	}

	// Returns whether the map contains any data.
	bool empty() const noexcept {
		return _data.empty();
	}

	///**
	// * Non-Standard Helpers
	// */
	// template <class U>
	// fea::span<const U> as_span() const {
	//	return fea::span<const U>{ reinterpret_cast<const char*>(_data.data()),
	//		_data.size() };
	//}

private:
	detail::fmap_os_data _os_data;
	fea::span<const T> _data;
};

//// A read-write file map.
// using fmap = detail::fmap<detail::fmap_mode::read_write>;
//
//// A write-only file map.
// using ofmap = detail::fmap<detail::fmap_mode::write>;

// A read-only file map.
// using ifmap = basic_fmap<fmap_mode::read, std::byte>;
using ifmap = basic_fmap_read<std::byte>;

} // namespace fea

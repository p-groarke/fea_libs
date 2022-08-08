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
#include "fea/utils/error.hpp"
#include "fea/utils/platform.hpp"
#include "fea/utils/throw.hpp"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <iterator>
#include <string_view>

#if FEA_WINDOWS
#include <windows.h>

// Unreachable code on v142, 32bits (only)
#pragma warning(push)
#pragma warning(disable : 4702)
#else
#include <fcntl.h>
#include <sys/mman.h>

//#include <sys/stat.h>
//#include <unistd.h>
#endif

/*
fmap : A light-weight, cross-platform, memory-mapped file view.

fmap maps whole files to virtual memory, either in read or write mode.
*/

namespace fea {
namespace detail {
enum class fmap_mode : uint8_t {
	read,
	write,
	count,
};

struct fmap_os_data {
	fmap_os_data() = default;
	fmap_os_data(const fmap_os_data&) = default;
	fmap_os_data& operator=(const fmap_os_data&) = default;

	fmap_os_data(fmap_os_data&& other) noexcept
#if FEA_WINDOWS
			: file_handle(other.file_handle)
			, map_handle(other.map_handle)
			,
#else
			:
#endif
			ptr(other.ptr)
			, byte_size(other.byte_size)
#if FEA_WINDOWS
			, mode(other.mode)
#endif
	{

#if FEA_WINDOWS
		other.file_handle = nullptr;
		other.map_handle = nullptr;
		other.mode = fmap_mode::count;
#endif
		other.ptr = nullptr;
		other.byte_size = 0;
	}

	fmap_os_data& operator=(fmap_os_data&& other) noexcept {
		if (this != &other) {
#if FEA_WINDOWS
			file_handle = other.file_handle;
			map_handle = other.map_handle;
			mode = other.mode;
			other.file_handle = nullptr;
			other.map_handle = nullptr;
			other.mode = fmap_mode::count;
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
#endif

	std::byte* ptr = nullptr;
	size_t byte_size = 0;

#if FEA_WINDOWS
	// Keep it tightly packed.
	fmap_mode mode = fmap_mode::count;
#endif
};

inline fmap_os_data os_map(
		const std::filesystem::path& filepath, fmap_mode mode) {
	if (!std::filesystem::exists(filepath)
			|| std::filesystem::is_directory(filepath)) {
		return {};
	}

	size_t file_size = size_t(std::filesystem::file_size(filepath));
	if (file_size == 0) {
		return {};
	}

	assert(mode != fmap_mode::count);
	assert(!filepath.empty());

	fmap_os_data ret;

#if FEA_WINDOWS
	DWORD file_mode = mode == fmap_mode::write ? GENERIC_READ | GENERIC_WRITE
											   : GENERIC_READ;
	ret.file_handle = CreateFileW(filepath.wstring().c_str(), file_mode,
			FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, nullptr);

	if (ret.file_handle == INVALID_HANDLE_VALUE) {
		fea::maybe_throw(__FUNCTION__, __LINE__, fea::last_os_error());
		return {};
	}

	DWORD map_mode = mode == fmap_mode::write ? PAGE_READWRITE : PAGE_READONLY;
	ret.map_handle = CreateFileMappingW(
			ret.file_handle, nullptr, map_mode, 0, 0, nullptr);
	if (ret.map_handle == nullptr) {
		fea::maybe_throw(__FUNCTION__, __LINE__, fea::last_os_error());
		return {};
	}

	DWORD view_mode = mode == fmap_mode::write ? FILE_MAP_WRITE : FILE_MAP_READ;
	ret.ptr = reinterpret_cast<std::byte*>(
			MapViewOfFile(ret.map_handle, view_mode, 0, 0, 0));
	if (ret.ptr == nullptr) {
		fea::maybe_throw(__FUNCTION__, __LINE__, fea::last_os_error());
		return {};
	}

	ret.mode = mode;
#else
	int file_mode = mode == fmap_mode::write ? O_RDWR : O_RDONLY;
	int fd = open(filepath.c_str(), file_mode);
	if (fd == -1) {
		fea::maybe_throw(__FUNCTION__, __LINE__, fea::last_os_error());
		return {};
	}

	int view_mode
			= mode == fmap_mode::write ? PROT_READ | PROT_WRITE : PROT_READ;
	void* map_ptr = mmap(nullptr, file_size, view_mode, MAP_SHARED, fd, 0);
	if (map_ptr == MAP_FAILED) {
		fea::maybe_throw(__FUNCTION__, __LINE__, fea::last_os_error());
		return {};
	}
	ret.ptr = reinterpret_cast<std::byte*>(map_ptr);

	if (close(fd) == -1) {
		fea::maybe_throw(__FUNCTION__, __LINE__, fea::last_os_error());
		return {};
	}

#endif

	ret.byte_size = file_size;
	return ret;
}

inline void os_unmap(const fmap_os_data& os_data) {
	assert(os_data.ptr != nullptr);
	assert(os_data.byte_size != 0);

#if FEA_WINDOWS
	assert(os_data.mode != fmap_mode::count);

	if (os_data.mode == fmap_mode::write) {
		if (!FlushViewOfFile(os_data.ptr, 0)) {
			fea::maybe_throw(__FUNCTION__, __LINE__, fea::last_os_error());
		}

		if (!FlushFileBuffers(os_data.file_handle)) {
			fea::maybe_throw(__FUNCTION__, __LINE__, fea::last_os_error());
		}
	}

	if (!UnmapViewOfFile(os_data.ptr)) {
		fea::maybe_throw(__FUNCTION__, __LINE__, fea::last_os_error());
	}

	if (!CloseHandle(os_data.map_handle)) {
		fea::maybe_throw(__FUNCTION__, __LINE__, fea::last_os_error());
	}

	if (!CloseHandle(os_data.file_handle)) {
		fea::maybe_throw(__FUNCTION__, __LINE__, fea::last_os_error());
	}
#else
	if (munmap(os_data.ptr, os_data.byte_size) == -1) {
		fea::maybe_throw(__FUNCTION__, __LINE__, fea::last_os_error());
	}
#endif
}
} // namespace detail


struct basic_fmap_read {
	using value_type = std::byte;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using const_reference = const value_type&;
	using const_pointer = const value_type*;
	using const_iterator = const_pointer;
	using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	// Ctors
	basic_fmap_read() = default;
	basic_fmap_read(const std::filesystem::path& filepath)
			: basic_fmap_read(filepath, detail::fmap_mode::read) {
	}

	~basic_fmap_read() {
		if (!is_open()) {
			return;
		}

		detail::os_unmap(_data);
	}

	basic_fmap_read(basic_fmap_read&& other) noexcept = default;
	basic_fmap_read& operator=(basic_fmap_read&& other) noexcept = default;
	basic_fmap_read(const basic_fmap_read&) = delete;
	basic_fmap_read& operator=(const basic_fmap_read&) = delete;

	/**
	 * Iterators
	 */

	// Returns an iterator pointing to the beginning of mapped data.
	const_iterator begin() const noexcept {
		return _data.ptr;
	}

	// Returns an iterator pointing to the end of mapped data.
	const_iterator end() const noexcept {
		return _data.ptr + _data.byte_size;
	}

	// Returns an iterator pointing to the beginning of the reversed data.
	const_reverse_iterator rbegin() const noexcept {
		return std::reverse_iterator{ end() };
	}

	// Returns an iterator pointing to the end of the reversed data.
	const_reverse_iterator rend() const noexcept {
		return std::reverse_iterator{ begin() };
	}

	/**
	 * Element access
	 */

	// Returns the mapped data.
	const_pointer data() const noexcept {
		return _data.ptr;
	}

	// Returns the nth byte.
	const_reference operator[](size_t idx) const {
		assert(idx < _data.byte_size);
		return _data.ptr[idx];
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
		return _data.byte_size;
	}

	// Returns whether the map contains any data.
	bool empty() const noexcept {
		return _data.byte_size == 0;
	}

	/**
	 * File operations
	 */
	void open(const std::filesystem::path& filepath) {
		close();
		_data = detail::os_map(filepath, detail::fmap_mode::read);
	}

	void close() {
		this->~basic_fmap_read();
		_data = {};
	}

protected:
	basic_fmap_read(
			const std::filesystem::path& filepath, detail::fmap_mode mode)
			: _data(detail::os_map(filepath, mode)) {
	}

	detail::fmap_os_data _data;
};


struct basic_fmap_write : public basic_fmap_read {
	using typename basic_fmap_read::difference_type;
	using typename basic_fmap_read::size_type;
	using typename basic_fmap_read::value_type;
	using reference = value_type&;
	using pointer = value_type*;
	using iterator = pointer;
	using reverse_iterator = std::reverse_iterator<iterator>;

	// Ctors
	basic_fmap_write() = default;
	basic_fmap_write(const std::filesystem::path& filepath)
			: basic_fmap_read(filepath, detail::fmap_mode::write) {
	}

	~basic_fmap_write() = default;
	basic_fmap_write(basic_fmap_write&& other) noexcept = default;
	basic_fmap_write& operator=(basic_fmap_write&& other) noexcept = default;
	basic_fmap_write(const basic_fmap_write&) = delete;
	basic_fmap_write& operator=(const basic_fmap_write&) = delete;

	/**
	 * Iterators
	 */

	// Returns an iterator pointing to the beginning of mapped data.
	iterator begin() noexcept {
		return const_cast<iterator>(basic_fmap_read::begin());
	}

	// Returns an iterator pointing to the end of mapped data.
	iterator end() noexcept {
		return const_cast<iterator>(basic_fmap_read::end());
	}

	// Returns an iterator pointing to the beginning of the reversed data.
	reverse_iterator rbegin() noexcept {
		return std::reverse_iterator{ end() };
	}

	// Returns an iterator pointing to the end of the reversed data.
	reverse_iterator rend() noexcept {
		return std::reverse_iterator{ begin() };
	}

	/**
	 * Element access
	 */

	// Returns the mapped data.
	pointer data() noexcept {
		return const_cast<pointer>(basic_fmap_read::data());
	}

	// Returns the nth byte.
	reference operator[](size_t idx) {
		return const_cast<reference>(basic_fmap_read::operator[](idx));
	}

	/**
	 * File operations
	 */
	void open(const std::filesystem::path& filepath) {
		close();
		_data = detail::os_map(filepath, detail::fmap_mode::write);
	}
};


/**
 * Helpers
 */
template <class U>
fea::span<const U> to_span(const basic_fmap_read& ifm) {
	if (ifm.size() % sizeof(U) != 0) {
		fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
				"Cannot convert to span<U>, total size not multiple of "
				"U size.");
		return {};
	}

	return fea::span<const U>{ reinterpret_cast<const U*>(ifm.data()),
		ifm.size() / sizeof(U) };
}

template <class U>
void to_span(const fea::basic_fmap_read& ifm, fea::span<const U>& out) {
	out = to_span<U>(ifm);
}

template <class U>
fea::span<U> to_span(basic_fmap_write& ofm) {
	fea::span<const U> const_ret
			= to_span<U>(static_cast<const basic_fmap_read&>(ofm));
	return fea::span<U>{ const_cast<U*>(const_ret.data()), const_ret.size() };
}

template <class U>
void to_span(fea::basic_fmap_write& ofm, fea::span<U>& out) {
	out = to_span<U>(ofm);
}

inline std::string_view to_sv(const basic_fmap_read& ifm) {
	return std::string_view{ reinterpret_cast<const char*>(ifm.data()),
		ifm.size() };
}

inline std::wstring_view to_wsv(const basic_fmap_read& ifm) {
	if (ifm.size() % sizeof(wchar_t) != 0) {
		fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
				"Cannot convert to std::wstring_view, total size not multiple "
				"of wchar_t size.");
		return {};
	}
	return std::wstring_view{ reinterpret_cast<const wchar_t*>(ifm.data()),
		ifm.size() / sizeof(wchar_t) };
}


/**
 * Aliases
 */

// A read-write file map.
using fmap = basic_fmap_write;
using ofmap = basic_fmap_write;

// A read-only file map.
using ifmap = basic_fmap_read;

} // namespace fea

#if FEA_WINDOWS
#pragma warning(pop)
#endif

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

#include <cstddef>
#include <cstdint>
#include <filesystem>

/*
A basic, cross-platform, memory-mapped file view.
*/

namespace fea {
enum class fmap_mode : uint8_t {
	read_write,
	read,
	write,
	count,
};

template <fmap_mode mode, class>
struct basic_fmap;

template <class T>
struct basic_fmap<fmap_mode::read, T> {
	using value_type = T;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using reference = const value_type&;
	using const_reference = const value_type&;
	using pointer = const value_type*;
	using const_pointer = const value_type*;

	using iterator = typename fea::span<const T>::iterator;
	using reverse_iterator = typename fea::span<const T>::reverse_iterator;

	// Ctors
	explicit basic_fmap(const std::filesystem::path&) {
	}

	/**
	 * Iterators
	 */

	// Returns an iterator pointing to the beginning of mapped data.
	iterator begin() const noexcept {
		return _data.begin();
	}

	// Returns an iterator pointing to the end of mapped data.
	iterator end() const noexcept {
		return _data.end();
	}

	// Returns an iterator pointing to the beginning of the reversed data.
	reverse_iterator rbegin() const noexcept {
		return _data.rbegin();
	}

	// Returns an iterator pointing to the end of the reversed data.
	reverse_iterator rend() const noexcept {
		return _data.rend();
	}

	/**
	 * Element access
	 */

	// Returns the mapped data.
	pointer data() const noexcept {
		return _data.data();
	}

	// Returns the nth element.
	reference operator[](size_t idx) const {
		return _data[idx];
	}

	/**
	 * Observers
	 */

	// Returns true if the file was mapped without errors.
	bool is_open() const noexcept;

	// Returns the size of the available data.
	// If using the default fmaps, this is the byte size.
	size_type size() const noexcept {
		return _data.size();
	}

	// Returns whether the map contains any data.
	bool empty() const noexcept {
		return _data.empty();
	}

private:
	fea::span<const T> _data;
};

//// A read-write file map.
// using fmap = detail::fmap<detail::fmap_mode::read_write>;
//
//// A write-only file map.
// using ofmap = detail::fmap<detail::fmap_mode::write>;

// A read-only file map.
using ifmap = basic_fmap<fmap_mode::read, std::byte>;

} // namespace fea

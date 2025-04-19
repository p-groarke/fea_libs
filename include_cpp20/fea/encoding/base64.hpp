/*
BSD 3-Clause License

Copyright (c) 2025, Philippe Groarke
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once
#include "fea/memory/memory.hpp"
#include "fea/meta/traits.hpp"
#include "fea/utility/platform.hpp"

#include <array>
#include <bit>
#include <cassert>
// #include <cstring>
#include <iterator>
#include <type_traits>

namespace fea {
// Encodes the data pointed at by the input iterators into a base64 string.
// Outputs chars to the output iterator.
template <std::forward_iterator FwdIt, std::output_iterator<char> OutIt>
void to_base64(FwdIt first, FwdIt last, OutIt out) noexcept;

// Decodes base64 and deserializes to whatever type the iterator points to.
// Supports an output iterator, a single pointer, an iterator to types T, etc.
template <std::forward_iterator FwdIt, class OutIt>
void from_base64(FwdIt first, FwdIt last, OutIt out);

} // namespace fea


/**
 * Implementation
 */
namespace fea {
namespace detail {
// 6 bit part to char.
constexpr inline std::array<char, 64> base64_lut{ 'A', 'B', 'C', 'D', 'E', 'F',
	'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U',
	'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
	'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y',
	'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/' };

// char to 6 bit part.
constexpr inline std::array<uint8_t, 256> base64_rlut{ 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 62, 0, 0, 0, 63, 52, 53, 54, 55, 56, 57, 58,
	59, 60, 61, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
	13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 0, 0, 0, 0, 0, 0, 26,
	27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45,
	46, 47, 48, 49, 50, 51, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
} // namespace detail

template <std::forward_iterator FwdIt, std::output_iterator<char> OutIt>
void to_base64(FwdIt first, FwdIt last, OutIt out) noexcept {
	using value_t = typename std::iterator_traits<FwdIt>::value_type;
	using storage_t = fea::aligned_storage_t<sizeof(value_t), alignof(value_t)>;
	static_assert(std::is_trivially_copyable_v<value_t>,
			"fea::encode_base64 : Input type must be trivially copyable "
			"(it is reinterpreted as bytes).");

	if (first == last) {
		return;
	}

	// Stage the encoding in 4 bytes.
	// When the staging is full, flush to output and clear.
	constexpr int insert_idx_init = 2;
	int insert_idx = insert_idx_init;
	uint32_t staging = 0;

	// Expects the output char at LSB in shifted_staging.
	auto write_char = [](uint32_t shifted_staging, auto& out) {
		uint32_t idx = shifted_staging & 0b0011'1111;
		char c = detail::base64_lut[idx];
		*out = c;
		++out;
	};

	for (; first != last; ++first) {
		// Convert pointed-to value to bytes.
		storage_t bytes = std::bit_cast<storage_t>(*first);

		// For each byte, add to staging. If the staging is full,
		// output encoding (3 bytes into 4 bytes).
		for (size_t i = 0; i < sizeof(value_t); ++i) {
			assert(0 <= insert_idx && insert_idx <= insert_idx_init);
			staging |= uint32_t(bytes.data[i]) << (insert_idx-- * 8);

			if (insert_idx != -1) {
				continue;
			}

			// We've filled our staging, flush to output iterator.
			for (int j = 3; j >= 0; --j) {
				write_char(staging >> (j * 6), out);
			}

			// Reset staging & insert index.
			staging = 0;
			insert_idx = insert_idx_init;
		}
	}
	assert(0 <= insert_idx && insert_idx <= insert_idx_init);

	if (insert_idx == insert_idx_init) {
		// We are done, divisible by 3.
		assert(staging == 0);
		return;
	}

	if (insert_idx == 1) {
		// We have 1 value stored.
		write_char(staging >> 18, out);
		write_char(staging >> 12, out);
		*out = '=';
		++out;
		*out = '=';
		++out;
	}

	if (insert_idx == 0) {
		// We have 2 values stored.
		write_char(staging >> 18, out);
		write_char(staging >> 12, out);
		write_char(staging >> 6, out);
		*out = '=';
		++out;
	}
}

namespace detail {
template <class T, std::forward_iterator FwdIt, std::output_iterator<T> OutIt>
void decode_base64(FwdIt first, FwdIt last, OutIt out) noexcept {
	using value_t = T;
	using storage_t = fea::aligned_storage_t<sizeof(value_t), alignof(value_t)>;
	static_assert(std::is_trivially_copyable_v<value_t>,
			"fea::encode_base64 : Output type must be trivially copyable "
			"(it is reinterpreted as bytes).");

	if (first == last) {
		return;
	}

	if constexpr (fea::debug_build) {
		// A bit heavy, only check in debug.
		// Check out count is multiple of 4 (atm we require output padding).
		size_t count = std::distance(first, last);
		assert(count % 4 == 0);

		// Check that we can fit a perfect amount of Ts inside the base64
		// encoding.
		size_t byte_size = (count / 4) * 3;
		FwdIt tmp_last = last - 1;
		if (char(*tmp_last) == '=') {
			--byte_size;

			if (tmp_last != first && char(*--tmp_last) == '=') {
				--byte_size;
			}
		}

		assert(byte_size % sizeof(value_t) == 0);
	}

	// We store 4 "byte parts" (6bit portions) in a staging area.
	// Once the staging is full, we flush the staging to a temporary data
	// buffer.
	constexpr int insert_idx_init = 3;
	int insert_idx = insert_idx_init;
	uint32_t staging = 0;

	// It's not enough to have the input (decoding) staging. We can only cast
	// the object once we have all bytes. The second staging area is an array
	// getting filled with bytes, when it is full, we flush (bit_cast) to
	// object.
	// Unused if output is 1 byte.
	size_t value_staging_idx = 0;
	storage_t value_staging;

	// Expects a shifted staging value.
	auto write_obj = [](uint32_t shifted_staging,
							 [[maybe_unused]] storage_t& obj_staging,
							 [[maybe_unused]]
							 size_t& obj_byte_idx,
							 auto& out) {
		uint8_t byte = uint8_t(shifted_staging & 0b1111'1111);
		if constexpr (sizeof(value_t) == 1) {
			// Single byte output, output as-is.
			*out = value_t(byte);
			++out;
		} else {
			// Store in temporary storage.
			obj_staging.data[obj_byte_idx++] = byte;

			assert(obj_byte_idx <= sizeof(value_t));
			if (obj_byte_idx != sizeof(value_t)) {
				return;
			}

			// We have a "full" object stored, flush it.
			*out = std::bit_cast<value_t>(obj_staging);
			++out;
			obj_byte_idx = 0;
		}
	};


	// For each input char, reverse lookup, pack in staging and flush
	// to temp object.
	// When the object is filled, output to iterator.
	for (; first != last; ++first) {
		assert(0 <= insert_idx && insert_idx <= insert_idx_init);
		assert(value_staging_idx < sizeof(value_t));

		// Get input, convert to index and get reverse lut value.
		size_t ridx = size_t(*first);
		assert(ridx < detail::base64_rlut.size());

		// We hit a padding character. Early exit and check everything is gucci.
		if (ridx == size_t('=')) {
			assert(0 <= insert_idx && insert_idx < insert_idx_init);
			for (int j = 2; j > insert_idx; --j) {
				write_obj((staging >> (j * 8)), value_staging,
						value_staging_idx, out);
			}
			assert(value_staging_idx == 0);
			break;
		}

		// Store in staging.
		uint32_t part_bits = uint32_t(detail::base64_rlut[ridx]);
		staging |= part_bits << (insert_idx-- * 6);

		if (insert_idx != -1) {
			continue;
		}

		// Flush to storage.
		for (int j = 2; j >= 0; --j) {
			write_obj((staging >> (j * 8)), value_staging, value_staging_idx,
					out);
		}

		// Reset staging & index.
		staging = 0;
		insert_idx = insert_idx_init;
	}
}

// Overload for real output iterators. Deduces the output value_type.
template <std::forward_iterator FwdIt, template <class, class...> class OutIt,
		class First, class... Args>
void decode_base64(FwdIt first, FwdIt last, OutIt<First, Args...> out,
		std::output_iterator_tag) {
	using value_t = fea::output_iterator_vt<OutIt<First, Args...>>;
	return decode_base64<value_t>(first, last, out);
}

// Overload for forward iterators. Simple. Used for example when amount is known
// or with pointers.
template <std::forward_iterator FwdIt, std::forward_iterator OutIt>
void decode_base64(
		FwdIt first, FwdIt last, OutIt out, std::forward_iterator_tag) {
	using value_t = typename std::iterator_traits<OutIt>::value_type;
	return decode_base64<value_t>(first, last, out);
}
} // namespace detail

// Note : We need this dispatch overload to support both output and input
// iterators. Output iterators don't have an easily accessible value_type, so we
// need voodoo to deduce it.
template <std::forward_iterator FwdIt, class OutIt>
void from_base64(FwdIt first, FwdIt last, OutIt out) {
	using cat_t = typename std::iterator_traits<OutIt>::iterator_category;
	return detail::decode_base64(first, last, out, cat_t{});
}
} // namespace fea

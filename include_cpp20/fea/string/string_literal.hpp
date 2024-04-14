/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2024, Philippe Groarke
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
#include "fea/utils/platform.hpp"

#include <algorithm>
#include <array>
#include <bitset>
#include <string_view>

namespace fea {
// fnv1a
// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
template <class CharT>
[[nodiscard]] consteval size_t make_cexpr_hash(
		std::basic_string_view<CharT> str) noexcept {
#if FEA_32BIT
	constexpr size_t fnv_offset_basis = 2166136261u;
	constexpr size_t fnv_prime = 16777619u;
#else
	constexpr size_t fnv_offset_basis = 14695981039346656037u;
	constexpr size_t fnv_prime = 1099511628211u;
#endif

	// Perf, consider malformed null-terminated string_view ==
	// non-null-terminated. Differs from std::hash;
	size_t size = str[str.size() - 1] == CharT(0) ? str.size() - 1 : str.size();
	size_t ret = fnv_offset_basis;
	for (size_t i = 0; i < size; ++i) {
		if constexpr (sizeof(CharT) == 1) {
			ret ^= size_t(str[i]);
			ret *= fnv_prime;
		} else {
			// Cannot reinterpret_cast in constexpr context.
			// Shift and mask each subchar instead.
			for (size_t j = 0; j < sizeof(CharT); ++j) {
				size_t d = uint8_t(size_t(str[i]) >> (j * 8));
				ret ^= d;
				ret *= fnv_prime;
			}
		}
	}
	return ret;
}
template <class CharT, size_t N>
[[nodiscard]] consteval size_t make_cexpr_hash(const CharT (&str)[N]) noexcept {
	if (str[N - 1] == CharT(0)) {
		return make_cexpr_hash(std::basic_string_view<CharT>{ str, N - 1 });
	} else {
		return make_cexpr_hash(std::basic_string_view<CharT>{ str, N });
	}
}
template <class CharT, size_t N>
[[nodiscard]] consteval size_t make_cexpr_hash(
		const std::array<CharT, N>& str) noexcept {
	if (str[N - 1] == CharT(0)) {
		return make_cexpr_hash(
				std::basic_string_view<CharT>{ str.data(), N - 1 });
	} else {
		return make_cexpr_hash(std::basic_string_view<CharT>{ str.data(), N });
	}
}


template <class CharT, size_t N>
struct basic_string_literal {
	using value_type = CharT;

	consteval basic_string_literal(const CharT (&str)[N]) {
		std::copy_n(str, N, _data);
		_hash = fea::make_cexpr_hash(_data);
	}

	// Underlying char array, null terminated.
	[[nodiscard]] consteval const CharT* data() const noexcept {
		return _data;
	}

	// Number of chars without null char.
	[[nodiscard]] consteval size_t size() const noexcept {
		return N - 1;
	}

	// The pre-computed hash.
	[[nodiscard]] consteval size_t hash() const noexcept {
		return _hash;
	}

	// Converts to string_view for all the niceties.
	[[nodiscard]] consteval std::basic_string_view<CharT> sv() const noexcept {
		return std::basic_string_view<CharT>(_data, size());
	}

	CharT _data[N]{};
	size_t _hash{};
};


template <size_t N>
struct string_literal : basic_string_literal<char, N> {
	consteval string_literal(const char (&str)[N])
			: basic_string_literal<char, N>(str) {
	}
};
template <size_t N>
struct wstring_literal : basic_string_literal<wchar_t, N> {
	consteval wstring_literal(const wchar_t (&str)[N])
			: basic_string_literal<wchar_t, N>(str) {
	}
};
template <size_t N>
struct u8string_literal : basic_string_literal<char8_t, N> {
	consteval u8string_literal(const char8_t (&str)[N])
			: basic_string_literal<char8_t, N>(str) {
	}
};
template <size_t N>
struct u16string_literal : basic_string_literal<char16_t, N> {
	consteval u16string_literal(const char16_t (&str)[N])
			: basic_string_literal<char16_t, N>(str) {
	}
};
template <size_t N>
struct u32string_literal : basic_string_literal<char32_t, N> {
	consteval u32string_literal(const char32_t (&str)[N])
			: basic_string_literal<char32_t, N>(str) {
	}
};

} // namespace fea

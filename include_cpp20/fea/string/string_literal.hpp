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
#include "fea/meta/traits.hpp"
#include "fea/utils/platform.hpp"

#include <algorithm>
#include <array>
#include <bitset>
#include <string_view>
#include <type_traits>

namespace fea {
template <class CharT, size_t N>
struct basic_string_literal;

// Pass in string_view.size(), string_view must point to string literal (end
// with null char).
template <size_t N, class CharT>
[[nodiscard]]
consteval fea::basic_string_literal<CharT, N + 1> make_string_literal(
		std::basic_string_view<CharT> sv) {
	FEA_CONSTEXPR_ASSERT(*(sv.data() + N) == CharT(0));
	CharT arr[N + 1]{};
	std::copy_n(sv.data(), N + 1, std::begin(arr));
	return fea::basic_string_literal<CharT, N + 1>{ arr };
}

// Makes a fnv1a hash at compile time.
// A null-terminated vs. non-null-terminated string return the same hash.
// fnv1a
// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
template <class CharT>
[[nodiscard]]
consteval size_t cexpr_make_hash(std::basic_string_view<CharT> str) noexcept {
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

// Makes a fnv1a hash at compile time.
// A null-terminated vs. non-null-terminated string return the same hash.
template <class CharT, size_t N>
[[nodiscard]]
consteval size_t cexpr_make_hash(const CharT (&str)[N]) noexcept {
	if (str[N - 1] == CharT(0)) {
		return cexpr_make_hash(std::basic_string_view<CharT>{ str, N - 1 });
	} else {
		return cexpr_make_hash(std::basic_string_view<CharT>{ str, N });
	}
}

// Makes a fnv1a hash at compile time.
// A null-terminated vs. non-null-terminated string return the same hash.
template <class CharT, size_t N>
[[nodiscard]]
consteval size_t cexpr_make_hash(const std::array<CharT, N>& str) noexcept {
	if (str[N - 1] == CharT(0)) {
		return cexpr_make_hash(
				std::basic_string_view<CharT>{ str.data(), N - 1 });
	} else {
		return cexpr_make_hash(std::basic_string_view<CharT>{ str.data(), N });
	}
}

// The total size of concatenation + 1 null char.
template <fea::basic_string_literal... Literals>
[[nodiscard]]
consteval auto cexpr_concat_size() noexcept {
	return (Literals.size() + ...) + 1;
}

// Concatenate multiple string literals.
template <fea::basic_string_literal... Literals>
[[nodiscard]]
consteval fea::basic_string_literal<
		typename fea::front_t<std::decay_t<decltype(Literals)>...>::value_type,
		fea::cexpr_concat_size<Literals...>()> cexpr_concat() noexcept {
	constexpr size_t N = fea::cexpr_concat_size<Literals...>();
	using lit_t = fea::front_t<std::decay_t<decltype(Literals)>...>;
	using CharT = typename lit_t::value_type;

	CharT arr[N]{};
	size_t out_idx = 0;
	auto add = [&](auto sl) noexcept {
		for (size_t i = 0; i < sl.size(); ++i) {
			arr[out_idx++] = sl.data[i];
		}
	};
	(add(Literals), ...);
	arr[out_idx++] = CharT(0);
	return fea::basic_string_literal<CharT, N>{ arr };
}


// A compile time string literal container, permitting typical string operations
// on it.
template <class CharT, size_t N>
struct basic_string_literal {
	using value_type = CharT;

	consteval basic_string_literal(const CharT (&str)[N]) {
		FEA_CONSTEXPR_ASSERT(str[N - 1] == CharT(0));
		std::copy_n(str, N, data);
	}

	// Number of chars without null char.
	[[nodiscard]]
	consteval size_t size() const noexcept {
		return N - 1;
	}

	// Returns the full size, including null char.
	[[nodiscard]]
	consteval size_t capacity() const noexcept {
		return N;
	}

	// Typical operator[]
	[[nodiscard]]
	consteval CharT
	operator[](size_t idx) const noexcept {
		return data[idx];
	}

	// Return a compile-time computed fnv1a hash.
	[[nodiscard]]
	consteval size_t hash() const noexcept {
		return fea::cexpr_make_hash(data);
	}

	// Converts to string_view for all the niceties.
	[[nodiscard]]
	consteval std::basic_string_view<CharT> sv() const noexcept {
		return std::basic_string_view<CharT>(data, size());
	}

	// Syntactically compare string literals.
	template <size_t N2>
	[[nodiscard]]
	consteval bool
	operator==(basic_string_literal<CharT, N2> rhs) const noexcept {
		if constexpr (N == N2) {
			for (size_t i = 0; i < N; ++i) {
				if (data[i] != rhs.data[i]) {
					return false;
				}
			}
			return true;
		} else {
			// Allow different sizes if trailing chars are null.
			size_t min = N < N2 ? N : N2;
			for (size_t i = 0; i < min; ++i) {
				if (data[i] != rhs.data[i]) {
					return false;
				}
			}

			if constexpr (N > N2) {
				for (size_t i = min; i < N; ++i) {
					if (data[i] != CharT(0)) {
						return false;
					}
				}
			} else {
				for (size_t i = min; i < N2; ++i) {
					if (rhs.data[i] != CharT(0)) {
						return false;
					}
				}
			}
			return true;
		}
	}

	// Syntactically compare string literals.
	template <size_t N2>
	[[nodiscard]]
	consteval bool
	operator!=(basic_string_literal<CharT, N2> rhs) const noexcept {
		return !(*this == rhs);
	}

	CharT data[N]{};
};

// Don't use inheritance to allow deduction.
template <size_t N>
using string_literal = basic_string_literal<char, N>;
template <size_t N>
using u8string_literal = basic_string_literal<char8_t, N>;
template <size_t N>
using wstring_literal = basic_string_literal<wchar_t, N>;
template <size_t N>
using u16string_literal = basic_string_literal<char16_t, N>;
template <size_t N>
using u32string_literal = basic_string_literal<char32_t, N>;

} // namespace fea

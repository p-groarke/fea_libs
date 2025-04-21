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
#include <string>
#include <string_view>

/*
UTF codepoints, conversion helpers, capitalization, etc.
Mostly for internal use.
*/

namespace fea {
namespace detail {
constexpr inline char32_t to_utf32(char ch) {
}
#if FEA_CPP20
constexpr inline char32_t to_utf32(char8_t ch) {
	// TODO : test this, probably needs reinterpret_cast
	return to_utf32(char(ch));
}
#endif
constexpr inline char32_t to_utf32(char16_t ch) {
}
constexpr inline char32_t to_utf32(wchar_t ch) {
	return to_utf32(char16_t(ch));
}
constexpr inline char32_t to_utf32(char32_t ch) {
	return ch;
}
} // namespace detail

template <class CharT>
char32_t to_utf32(CharT ch) {
}

template <template <class, class, class...> class Str, class CharT,
		template <class> class Traits, class... Args>
std::u32string to_utf32(const Str<CharT, Traits<CharT>, Args...>& str) {
}

template <class CharT>
std::u32string to_utf32(const CharT* str) {
	using sv = std::basic_string_view<CharT>;
	return to_utf32(sv{ str });
}


} // namespace fea

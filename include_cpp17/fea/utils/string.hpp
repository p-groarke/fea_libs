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
#include "fea/utils/platform.hpp"

#include <algorithm>
#include <cassert>
#include <codecvt>
#include <locale>
#include <sstream>
#include <string>
#include <vector>

#if defined(FEA_WINDOWS)
#include <windows.h>
#endif


// Makes a string literal of type CharType
#define FEA_MAKE_LITERAL_T(CharType, str) \
	[]() { \
		if constexpr (std::is_same_v<CharType, char>) { \
			return str; \
		} else if constexpr (std::is_same_v<CharType, wchar_t>) { \
			return L##str; \
		} else if constexpr (std::is_same_v<CharType, char16_t>) { \
			return u##str; \
		} else if constexpr (std::is_same_v<CharType, char32_t>) { \
			return U##str; \
		} \
	}()

// These are shortcuts if you use CharT as an alias or template.
#define FEA_MAKE_LITERAL(str) FEA_MAKE_LITERAL_T(CharT, str)
#define FEA_ML(str) FEA_MAKE_LITERAL_T(CharT, str)


// Makes a character literal of type CharType
#define FEA_MAKE_CHAR_LITERAL_T(CharType, str) \
	[]() { \
		if constexpr (std::is_same_v<CharType, char>) { \
			return str; \
		} else if constexpr (std::is_same_v<CharType, wchar_t>) { \
			return L##str; \
		} else if constexpr (std::is_same_v<CharType, char16_t>) { \
			return u##str; \
		} else if constexpr (std::is_same_v<CharType, char32_t>) { \
			return U##str; \
		} \
	}()

// These are shortcuts if you use CharT as an alias or template.
#define FEA_MAKE_CHAR_LITERAL(str) FEA_MAKE_CHAR_LITERAL_T(CharT, str)
#define FEA_CH(ch) FEA_MAKE_CHAR_LITERAL_T(CharT, ch)


namespace fea {
template <class CharT>
using m_string = std::basic_string<CharT, std::char_traits<CharT>,
		std::allocator<CharT>>;

template <class CharT>
[[nodiscard]] inline bool contains(
		const m_string<CharT>& str, const m_string<CharT>& search) {
	return str.find(search) != m_string<CharT>::npos;
}
template <class CharT>
[[nodiscard]] inline bool contains(
		const m_string<CharT>& str, const CharT* search) {
	return str.find(search) != m_string<CharT>::npos;
}

template <class CharT>
[[nodiscard]] inline bool starts_with(
		const m_string<CharT>& str, const m_string<CharT>& search) {
	return str.find(search) == 0;
}
template <class CharT>
[[nodiscard]] inline bool starts_with(
		const m_string<CharT>& str, const CharT* search) {
	return str.find(search) == 0;
}

template <class CharT>
[[nodiscard]] inline bool ends_with(
		const m_string<CharT>& str, const m_string<CharT>& search) {
	return str.find_last_of(search) == str.size() - search.size();
}
template <class CharT>
[[nodiscard]] inline bool ends_with(
		const m_string<CharT>& str, const CharT* search) {
	return ends_with(str, m_string<CharT>{ search });
}

template <class CharT>
[[nodiscard]] inline m_string<CharT> to_lower(const m_string<CharT>& str) {
	auto ret = str;
	std::transform(ret.begin(), ret.end(), ret.begin(),
			[](auto c) { return static_cast<CharT>(::tolower(c)); });
	return ret;
}

template <class CharT>
inline void to_lower(m_string<CharT>& out, bool /*inplace*/) {
	std::transform(out.begin(), out.end(), out.begin(),
			[](auto c) { return static_cast<CharT>(::tolower(c)); });
}

[[nodiscard]] inline std::vector<uint8_t> to_lower(
		const std::vector<uint8_t>& str) {
	std::vector<uint8_t> ret = str;
	std::transform(ret.begin(), ret.end(), ret.begin(),
			[](char c) { return static_cast<char>(::tolower(c)); });
	return ret;
}

inline void to_lower(std::vector<uint8_t>& out, bool /*inplace*/) {
	std::transform(out.begin(), out.end(), out.begin(),
			[](char c) { return static_cast<char>(::tolower(c)); });
}

template <class CharT>
[[nodiscard]] inline std::vector<m_string<CharT>> split(
		const m_string<CharT>& str, const CharT* delimiters) {
	std::vector<m_string<CharT>> tokens;
	size_t prev = 0;
	size_t pos;

	while ((pos = str.find_first_of(delimiters, prev))
			!= m_string<CharT>::npos) {
		if (pos > prev) {
			tokens.push_back(str.substr(prev, pos - prev));
		}
		prev = pos + 1;
	}
	if (prev < str.length()) {
		tokens.push_back(str.substr(prev, m_string<CharT>::npos));
	}
	return tokens;
}

template <class CharT>
[[nodiscard]] inline std::vector<m_string<CharT>> split(
		const m_string<CharT>& str, CharT delimiter) {
	m_string<CharT> delim{ delimiter };
	return split(str, delim.c_str());
}


template <class CharT>
inline void replace_all(m_string<CharT>& out, const m_string<CharT>& search,
		const m_string<CharT>& replace, bool /*inplace*/) {

	size_t pos = out.find(search);
	while (pos != m_string<CharT>::npos) {
		out.replace(pos, search.size(), replace);
		--pos;
		pos = out.find(search, pos + search.size());
	}
}
template <class CharT>
inline void replace_all(m_string<CharT>& out, const CharT* search,
		const CharT* replace, bool /*inplace*/) {
	replace_all(
			out, m_string<CharT>{ search }, m_string<CharT>{ replace }, true);
}

template <class CharT>
[[nodiscard]] inline m_string<CharT> replace_all(const m_string<CharT>& str,
		const m_string<CharT>& search, const m_string<CharT>& replace) {
	m_string<CharT> ret = str;
	replace_all(ret, search, replace, true);
	return ret;
}
template <class CharT>
[[nodiscard]] inline m_string<CharT> replace_all(
		const m_string<CharT>& str, const CharT* search, const CharT* replace) {
	return replace_all(
			str, m_string<CharT>{ search }, m_string<CharT>{ replace });
}


// The standard doesn't provide codecvt equivalents. Use the old
// functionality until they do.
#if defined(FEA_WINDOWS)
#pragma warning(push)
#pragma warning(disable : 4996)
#endif

// From UTF8 (multi-byte)

// UTF-8 to UTF-16
inline std::u16string utf8_to_utf16(const std::string& s) {
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
	return convert.from_bytes(s);
}

// UTF-8 to UTF-16, in wstring. Aka Windows "unicode".
inline std::wstring utf8_to_utf16_w(const std::string& s) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
	return convert.from_bytes(s);
}

// UTF-8 to UTF-16, encoded in 32bits. This is dumb, don't use this.
inline std::u32string utf8_to_utf16_32bits(const std::string& s) {
	std::wstring_convert<std::codecvt_utf8_utf16<char32_t>, char32_t> convert;
	return convert.from_bytes(s);
}

// UTF-8 to UCS2, outdated format.
inline std::u16string utf8_to_ucs2(const std::string& s) {
	std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> convert;
	return convert.from_bytes(s);
}

// UTF-8 to UCS2, in wstring. Outdated format.
inline std::wstring utf8_to_ucs2_w(const std::string& s) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> convert;
	return convert.from_bytes(s);
}

// UTF-8 to UTF-32
inline std::u32string utf8_to_utf32(const std::string& s) {
	std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
	return conv.from_bytes(s);
}


// From UTF-16

// UTF-16 to UTF-8
inline std::string utf16_to_utf8(const std::u16string& s) {
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conv;
	return conv.to_bytes(s);
}

// UTF-16 to UTF-8, using wstring.
inline std::string utf16_to_utf8(const std::wstring& s) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> conv;
	return conv.to_bytes(s);
}

// UTF-16 to UTF-8, using 32bit encoded UTF-16 (aka, dumb).
inline std::string utf16_to_utf8(const std::u32string& s) {
	std::wstring_convert<std::codecvt_utf8_utf16<char32_t>, char32_t> conv;
	return conv.to_bytes(s);
}

// UTF-16 to UCS2, outdated format.
inline std::u16string utf16_to_ucs2(const std::u16string& s) {
	return utf8_to_ucs2(utf16_to_utf8(s));
}

// UTF-16 to UCS2, outdated format.
inline std::u16string utf16_to_ucs2(const std::wstring& s) {
	return utf8_to_ucs2(utf16_to_utf8(s));
}

// UTF-16 to UCS2, outdated format.
inline std::wstring utf16_to_ucs2_w(const std::u16string& s) {
	return utf8_to_ucs2_w(utf16_to_utf8(s));
}

// UTF-16 to UCS2, outdated format.
inline std::wstring utf16_to_ucs2_w(const std::wstring& s) {
	return utf8_to_ucs2_w(utf16_to_utf8(s));
}

// UTF-16 to UTF-32.
inline std::u32string utf16_to_utf32(const std::u16string& s) {
	return utf8_to_utf32(utf16_to_utf8(s));
}

// UTF-16 to UTF-32.
inline std::u32string utf16_to_utf32(const std::wstring& s) {
	return utf8_to_utf32(utf16_to_utf8(s));
}


// From UCS (outdated format)

// UCS2 to UTF-8
inline std::string ucs2_to_utf8(const std::u16string& s) {
	std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> conv;
	return conv.to_bytes(s);
}

// UCS2 to UTF-8, using wstring.
inline std::string ucs2_to_utf8(const std::wstring& s) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> conv;
	return conv.to_bytes(s);
}

// UCS2 to UTF-16.
inline std::u16string ucs2_to_utf16(const std::u16string& s) {
	return utf8_to_utf16(ucs2_to_utf8(s));
}

// UCS2 to UTF-16.
inline std::u16string ucs2_to_utf16(const std::wstring& s) {
	return utf8_to_utf16(ucs2_to_utf8(s));
}

// UCS2 to UTF-16.
inline std::wstring ucs2_to_utf16_w(const std::u16string& s) {
	return utf8_to_utf16_w(ucs2_to_utf8(s));
}

// UCS2 to UTF-16.
inline std::wstring ucs2_to_utf16_w(const std::wstring& s) {
	return utf8_to_utf16_w(ucs2_to_utf8(s));
}

// UCS2 to 32bit encoded UTF-16.
inline std::u32string ucs2_to_utf16_32bit(const std::u16string& s) {
	return utf8_to_utf16_32bits(ucs2_to_utf8(s));
}

// UCS2 to 32bit encoded UTF-16.
inline std::u32string ucs2_to_utf16_32bit(const std::wstring& s) {
	return utf8_to_utf16_32bits(ucs2_to_utf8(s));
}

// UCS2 to UTF-32.
inline std::u32string ucs2_to_utf32(const std::u16string& s) {
	return utf8_to_utf32(ucs2_to_utf8(s));
}

// UCS2 to UTF-32.
inline std::u32string ucs2_to_utf32(const std::wstring& s) {
	return utf8_to_utf32(ucs2_to_utf8(s));
}


// From UTF-32

// UTF-32 to UTF-8
inline std::string utf32_to_utf8(const std::u32string& s) {
	std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
	return conv.to_bytes(s);
}

// UTF-32 to UTF-16
inline std::u16string utf32_to_utf16(const std::u32string& s) {
	return utf8_to_utf16(utf32_to_utf8(s));
}

// UTF-32 to UTF-16, using wstring
inline std::wstring utf32_to_utf16_w(const std::u32string& s) {
	return utf8_to_utf16_w(utf32_to_utf8(s));
}

// UTF-32 to 32bit encoded UTF-16
inline std::u32string utf32_to_utf16_32bit(const std::u32string& s) {
	return utf8_to_utf16_32bits(utf32_to_utf8(s));
}

// UTF-32 to UCS2, outdated format.
inline std::u16string utf32_to_ucs2(const std::u32string& s) {
	return utf8_to_ucs2(utf32_to_utf8(s));
}

// UTF-32 to UCS2, using wstring.
inline std::wstring utf32_to_ucs2_w(const std::u32string& s) {
	return utf8_to_ucs2_w(utf32_to_utf8(s));
}


// Useful generalized conversions

template <class CharT>
std::string any_to_utf8(const m_string<CharT>& str) {
	if constexpr (std::is_same_v<CharT, char>) {
		return str;
	} else if constexpr (std::is_same_v<CharT, wchar_t>) {
		return utf16_to_utf8(str);
	} else if constexpr (std::is_same_v<CharT, char16_t>) {
		return utf16_to_utf8(str);
	} else if constexpr (std::is_same_v<CharT, char32_t>) {
		return utf32_to_utf8(str);
	} else {
		assert(false);
		throw std::runtime_error{ "any_to_utf8 : unsupported string type" };
	}
}

template <class CharT>
m_string<CharT> utf8_to_any(const std::string& str) {
	if constexpr (std::is_same_v<CharT, char>) {
		return str;
	} else if constexpr (std::is_same_v<CharT, wchar_t>) {
		return utf8_to_utf16_w(str);
	} else if constexpr (std::is_same_v<CharT, char16_t>) {
		return utf8_to_utf16(str);
	} else if constexpr (std::is_same_v<CharT, char32_t>) {
		return utf8_to_utf32(str);
	} else {
		assert(false);
		throw std::runtime_error{ "utf8_to_any : unsupported string type" };
	}
}

template <class CharT>
std::u32string any_to_utf32(const m_string<CharT>& str) {
	if constexpr (std::is_same_v<CharT, char>) {
		return utf8_to_utf32(str);
	} else if constexpr (std::is_same_v<CharT, wchar_t>) {
		return utf16_to_utf32(str);
	} else if constexpr (std::is_same_v<CharT, char16_t>) {
		return utf16_to_utf32(str);
	} else if constexpr (std::is_same_v<CharT, char32_t>) {
		return str;
	} else {
		assert(false);
		throw std::runtime_error{ "any_to_utf32 : unsupported string type" };
	}
}

template <class CharT>
m_string<CharT> utf32_to_any(const std::u32string& str) {
	if constexpr (std::is_same_v<CharT, char>) {
		return utf32_to_utf8(str);
	} else if constexpr (std::is_same_v<CharT, wchar_t>) {
		return utf32_to_utf16_w(str);
	} else if constexpr (std::is_same_v<CharT, char16_t>) {
		return utf32_to_utf16(str);
	} else if constexpr (std::is_same_v<CharT, char32_t>) {
		return str;
	} else {
		assert(false);
		throw std::runtime_error{ "utf32_to_any : unsupported string type" };
	}
}

// Other Encodings.

inline std::string iso_8859_1_to_utf8(const std::string& str) {
	std::string ret;
	ret.reserve(str.size());

	for (uint8_t ch : str) {
		if (ch < 128u) {
			ret.push_back(ch);
		} else {
			ret.push_back(0b1100'0000 | ch >> 6);
			ret.push_back(0b1000'0000 | (ch & 0b0011'1111));
		}
	}
	return ret;
}


#if defined(FEA_WINDOWS)
// Provide a code page, for example CP_ACP
inline std::wstring codepage_to_utf16_w(
		UINT code_page, const std::string& str) {
	int size = MultiByteToWideChar(
			code_page, 0, str.c_str(), int(str.size()), 0, 0);

	std::wstring ret(size, '\0');
	MultiByteToWideChar(code_page, 0, str.c_str(), int(str.size()), ret.data(),
			int(ret.size()));
	return ret;
}

inline std::string utf16_to_codepage(UINT code_page, const std::wstring& str) {
	int size = WideCharToMultiByte(
			code_page, 0, str.data(), int(str.size()), 0, 0, nullptr, nullptr);

	std::string ret(size, '\0');
	WideCharToMultiByte(code_page, 0, str.c_str(), int(str.size()), ret.data(),
			int(ret.size()), nullptr, nullptr);
	return ret;
}

inline std::wstring current_codepage_to_utf16_w(const std::string& str) {
	return codepage_to_utf16_w(GetACP(), str);
}

inline std::string utf16_to_current_codepage(const std::wstring& str) {
	return utf16_to_codepage(GetACP(), str);
}
#endif

#if defined(FEA_WINDOWS)
#pragma warning(pop)
#endif
} // namespace fea

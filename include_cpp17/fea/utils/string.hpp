﻿/**
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
#include "fea/utils/throw.hpp"

#include <algorithm>
#include <array>
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
using string_t = std::basic_string<CharT, std::char_traits<CharT>,
		std::allocator<CharT>>;

template <class CharT>
[[nodiscard]] bool contains(
		const string_t<CharT>& str, const string_t<CharT>& search) {
	return str.find(search) != string_t<CharT>::npos;
}
template <class CharT>
[[nodiscard]] bool contains(const string_t<CharT>& str, const CharT* search) {
	return str.find(search) != string_t<CharT>::npos;
}

template <class CharT>
[[nodiscard]] bool starts_with(
		const string_t<CharT>& str, const string_t<CharT>& search) {
	return str.find(search) == 0;
}
template <class CharT>
[[nodiscard]] bool starts_with(
		const string_t<CharT>& str, const CharT* search) {
	return str.find(search) == 0;
}

template <class CharT>
[[nodiscard]] bool ends_with(
		const string_t<CharT>& str, const string_t<CharT>& search) {
	return str.find_last_of(search) == str.size() - search.size();
}
template <class CharT>
[[nodiscard]] bool ends_with(const string_t<CharT>& str, const CharT* search) {
	return ends_with(str, string_t<CharT>{ search });
}

template <class CharT>
[[nodiscard]] string_t<CharT> to_lower(const string_t<CharT>& str) {
	auto ret = str;
	std::transform(ret.begin(), ret.end(), ret.begin(),
			[](auto c) { return CharT(::tolower(c)); });
	return ret;
}

template <class CharT>
void to_lower(string_t<CharT>& out, bool /*inplace*/) {
	std::transform(out.begin(), out.end(), out.begin(),
			[](auto c) { return CharT(::tolower(c)); });
}

[[nodiscard]] inline std::vector<uint8_t> to_lower(
		const std::vector<uint8_t>& str) {
	std::vector<uint8_t> ret = str;
	std::transform(ret.begin(), ret.end(), ret.begin(),
			[](char c) { return char(::tolower(c)); });
	return ret;
}

inline void to_lower(std::vector<uint8_t>& out, bool /*inplace*/) {
	std::transform(out.begin(), out.end(), out.begin(),
			[](char c) { return char(::tolower(c)); });
}


template <class CharT>
[[nodiscard]] string_t<CharT> to_upper(const string_t<CharT>& str) {
	auto ret = str;
	std::transform(ret.begin(), ret.end(), ret.begin(),
			[](auto c) { return CharT(::toupper(c)); });
	return ret;
}

template <class CharT>
void to_upper(string_t<CharT>& out, bool /*inplace*/) {
	std::transform(out.begin(), out.end(), out.begin(),
			[](auto c) { return CharT(::toupper(c)); });
}

[[nodiscard]] inline std::vector<uint8_t> to_upper(
		const std::vector<uint8_t>& str) {
	std::vector<uint8_t> ret = str;
	std::transform(ret.begin(), ret.end(), ret.begin(),
			[](char c) { return char(::toupper(c)); });
	return ret;
}

inline void to_upper(std::vector<uint8_t>& out, bool /*inplace*/) {
	std::transform(out.begin(), out.end(), out.begin(),
			[](char c) { return char(::tolower(c)); });
}

template <class CharT>
[[nodiscard]] std::vector<string_t<CharT>> split(
		const string_t<CharT>& str, const CharT* delimiters) {
	std::vector<string_t<CharT>> tokens;
	size_t prev = 0;
	size_t pos;

	while ((pos = str.find_first_of(delimiters, prev))
			!= string_t<CharT>::npos) {
		if (pos > prev) {
			tokens.push_back(str.substr(prev, pos - prev));
		}
		prev = pos + 1;
	}
	if (prev < str.length()) {
		tokens.push_back(str.substr(prev, string_t<CharT>::npos));
	}
	return tokens;
}

template <class CharT>
[[nodiscard]] std::vector<string_t<CharT>> split(
		const string_t<CharT>& str, CharT delimiter) {
	string_t<CharT> delim{ delimiter };
	return split(str, delim.c_str());
}


template <size_t N, class CharT>
[[nodiscard]] std::array<string_t<CharT>, N> split(
		const string_t<CharT>& str, const CharT* delimiters) {
	static_assert(N != 0, "split : array must be bigger than 0");

	std::array<string_t<CharT>, N> tokens;
	size_t idx = 0;
	size_t prev = 0;
	size_t pos;

	while ((pos = str.find_first_of(delimiters, prev))
			!= string_t<CharT>::npos) {
		if (pos > prev) {
			tokens[idx++] = str.substr(prev, pos - prev);

			if (idx >= N) {
				break;
			}
		}
		prev = pos + 1;
	}

	if (prev < str.length() && idx < N) {
		tokens[idx++] = str.substr(prev, string_t<CharT>::npos);
	}
	return tokens;
}

template <size_t N, class CharT>
[[nodiscard]] std::array<string_t<CharT>, N> split(
		const string_t<CharT>& str, CharT delimiter) {
	string_t<CharT> delim{ delimiter };
	return split(str, delim.c_str());
}


template <class CharT>
void replace_all(string_t<CharT>& out, const string_t<CharT>& search,
		const string_t<CharT>& replace, bool /*inplace*/) {

	size_t pos = out.find(search);
	while (pos != string_t<CharT>::npos) {
		out.replace(pos, search.size(), replace);
		--pos;
		pos = out.find(search, pos + search.size());
	}
}
template <class CharT>
void replace_all(string_t<CharT>& out, const CharT* search,
		const CharT* replace, bool /*inplace*/) {
	replace_all(
			out, string_t<CharT>{ search }, string_t<CharT>{ replace }, true);
}

template <class CharT>
[[nodiscard]] string_t<CharT> replace_all(const string_t<CharT>& str,
		const string_t<CharT>& search, const string_t<CharT>& replace) {
	string_t<CharT> ret = str;
	replace_all(ret, search, replace, true);
	return ret;
}
template <class CharT>
[[nodiscard]] string_t<CharT> replace_all(
		const string_t<CharT>& str, const CharT* search, const CharT* replace) {
	return replace_all(
			str, string_t<CharT>{ search }, string_t<CharT>{ replace });
}

// Iterates input string line by line and calls your function with each line.
template <class CharT, class Func>
void for_each_line(const string_t<CharT>& str, Func&& func) {
	std::basic_istringstream iss(str);
	string_t<CharT> line;
	while (std::getline(iss, line)) {
		std::forward<Func>(func)(line);
	}
}

// Removes all leading trim_char.
template <class CharT>
[[nodiscard]] string_t<CharT> trim_leading(
		const string_t<CharT>& str, CharT trim_char) {
	size_t new_begin = str.find_first_not_of(trim_char);
	if (new_begin == string_t<CharT>::npos) {
		return {};
	}
	return str.substr(new_begin);
}

// Removes all leading trim_chars.
template <class CharT>
[[nodiscard]] string_t<CharT> trim_leading(
		const string_t<CharT>& str, const CharT* trim_chars) {
	size_t new_begin = str.find_first_not_of(trim_chars);
	if (new_begin == string_t<CharT>::npos) {
		return {};
	}
	return str.substr(new_begin);
}

// Removes all leading trim_char.
template <class CharT>
[[nodiscard]] string_t<CharT> trim_trailing(
		const string_t<CharT>& str, CharT trim_char) {
	size_t new_end = str.find_last_not_of(trim_char);
	if (new_end == string_t<CharT>::npos) {
		return {};
	}
	return str.substr(0, new_end + 1);
}

// Removes all leading trim_chars.
template <class CharT>
[[nodiscard]] string_t<CharT> trim_trailing(
		const string_t<CharT>& str, const CharT* trim_chars) {
	size_t new_end = str.find_last_not_of(trim_chars);
	if (new_end == string_t<CharT>::npos) {
		return {};
	}
	return str.substr(0, new_end + 1);
}

// Inserts the provided thousand seperator in string.
// https://stackoverflow.com/questions/49319461/how-to-use-custom-thousand-separator-and-decimal-character-with-stdstringstrea

template <class CharT>
string_t<CharT> utf8_to_any(const std::string& str);

template <class CharT>
[[nodiscard]] string_t<CharT> thousand_seperate(
		const string_t<CharT>& str, CharT sep) {
	string_t<CharT> ret = str;

	size_t i = ret.rfind(CharT('.'));
	if (i == ret.npos) {
		i = ret.size();
	}

	const string_t<CharT> nums = fea::utf8_to_any<CharT>("0123456789");
	size_t prefix_pos = ret.find_first_of(nums);
	const size_t digits = 3u + prefix_pos;
	while (i > digits) {
		ret.insert(i -= 3, 1, sep);
	}

	return ret;

	// if (ret.size() <= 3) {
	//	return str;
	//}

	// size_t added_chars = (str.size() / 3u);
	// if (str.size() % 3 == 0) {
	//	--added_chars;
	//}

	// string_t<CharT> ret(str.size() + added_chars, '\0');

	// size_t cnt = 0;
	// int ret_idx = int(ret.size()) - 1;
	// for (int i = int(str.size()) - 1; i >= 0; --i) {
	//	++cnt;
	//	ret[ret_idx--] = str[i];

	//	if (cnt % 3 == 0) {
	//		ret[ret_idx--] = sep;
	//	}
	//}

	// return ret;
}


// The standard doesn't provide codecvt equivalents. Use the old
// functionality until they do.
#if defined(FEA_WINDOWS)
#pragma warning(push)
#pragma warning(disable : 4996)

// VS 2015 and VS 2017 don't export codecvt symbols...
#if (_MSC_VER >= 1900 /* VS 2015*/) && (_MSC_VER <= 1916 /* VS 2017 */)
#define FEA_MSVC_CODECVT_BUG
using u16string_hack = std::basic_string<std::uint_least16_t,
		std::char_traits<std::uint_least16_t>,
		std::allocator<std::uint_least16_t>>;
using u32string_hack = std::basic_string<std::uint_least32_t,
		std::char_traits<std::uint_least32_t>,
		std::allocator<std::uint_least32_t>>;
#endif
#endif


// From UTF8 (multi-byte)

// UTF-8 to UTF-16
inline std::u16string utf8_to_utf16(const std::string& s) {
#if defined(FEA_MSVC_CODECVT_BUG)
	std::wstring_convert<std::codecvt_utf8_utf16<std::uint_least16_t>,
			std::uint_least16_t>
			convert;
	u16string_hack str = convert.from_bytes(s);
	return { reinterpret_cast<const char16_t*>(str.c_str()) };
#else
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
	return convert.from_bytes(s);
#endif
}

// UTF-8 to UTF-16, in wstring. Aka Windows "unicode".
inline std::wstring utf8_to_utf16_w(const std::string& s) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
	return convert.from_bytes(s);
}

// UTF-8 to UTF-16, encoded in 32bits. This is dumb, don't use this.
inline std::u32string utf8_to_utf16_32bits(const std::string& s) {
#if defined(FEA_MSVC_CODECVT_BUG)
	std::wstring_convert<std::codecvt_utf8_utf16<std::uint_least32_t>,
			std::uint_least32_t>
			convert;
	u32string_hack str = convert.from_bytes(s);
	return { reinterpret_cast<const char32_t*>(str.c_str()) };
#else
	std::wstring_convert<std::codecvt_utf8_utf16<char32_t>, char32_t> convert;
	return convert.from_bytes(s);
#endif
}

// UTF-8 to UCS2, outdated format.
inline std::u16string utf8_to_ucs2(const std::string& s) {
#if defined(FEA_MSVC_CODECVT_BUG)
	std::wstring_convert<std::codecvt_utf8<std::uint_least16_t>,
			std::uint_least16_t>
			convert;
	u16string_hack str = convert.from_bytes(s);
	return { reinterpret_cast<const char16_t*>(str.c_str()) };
#else
	std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> convert;
	return convert.from_bytes(s);
#endif
}

// UTF-8 to UCS2, in wstring. Outdated format.
inline std::wstring utf8_to_ucs2_w(const std::string& s) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> convert;
	return convert.from_bytes(s);
}

// UTF-8 to UTF-32
inline std::u32string utf8_to_utf32(const std::string& s) {
#if defined(FEA_MSVC_CODECVT_BUG)
	std::wstring_convert<std::codecvt_utf8<std::uint_least32_t>,
			std::uint_least32_t>
			conv;
	u32string_hack str = conv.from_bytes(s);
	return { reinterpret_cast<const char32_t*>(str.c_str()) };
#else
	std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
	return conv.from_bytes(s);
#endif
}


// From UTF-16

// UTF-16 to UTF-8
inline std::string utf16_to_utf8(const std::u16string& s) {
#if defined(FEA_MSVC_CODECVT_BUG)
	std::wstring_convert<std::codecvt_utf8_utf16<std::uint_least16_t>,
			std::uint_least16_t>
			conv;
	u16string_hack str{ reinterpret_cast<const std::uint_least16_t*>(
			s.c_str()) };
	return conv.to_bytes(str);
#else
	std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> conv;
	return conv.to_bytes(s);
#endif
}

// UTF-16 to UTF-8, using wstring.
inline std::string utf16_to_utf8(const std::wstring& s) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> conv;
	return conv.to_bytes(s);
}

// UTF-16 to UTF-8, using 32bit encoded UTF-16 (aka, dumb).
inline std::string utf16_to_utf8(const std::u32string& s) {
#if defined(FEA_MSVC_CODECVT_BUG)
	std::wstring_convert<std::codecvt_utf8_utf16<std::uint_least32_t>,
			std::uint_least32_t>
			conv;
	u32string_hack str{ reinterpret_cast<const std::uint_least32_t*>(
			s.c_str()) };
	return conv.to_bytes(str);
#else
	std::wstring_convert<std::codecvt_utf8_utf16<char32_t>, char32_t> conv;
	return conv.to_bytes(s);
#endif
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
#if defined(FEA_MSVC_CODECVT_BUG)
	std::wstring_convert<std::codecvt_utf8<std::uint_least16_t>,
			std::uint_least16_t>
			conv;
	u16string_hack str{ reinterpret_cast<const std::uint_least16_t*>(
			s.c_str()) };
	return conv.to_bytes(str);
#else
	std::wstring_convert<std::codecvt_utf8<char16_t>, char16_t> conv;
	return conv.to_bytes(s);
#endif
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
#if defined(FEA_MSVC_CODECVT_BUG)
	std::wstring_convert<std::codecvt_utf8<std::uint_least32_t>,
			std::uint_least32_t>
			conv;
	u32string_hack str{ reinterpret_cast<const std::uint_least32_t*>(
			s.c_str()) };
	return conv.to_bytes(str);
#else
	std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
	return conv.to_bytes(s);
#endif
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
std::string any_to_utf8(const string_t<CharT>& str) {
	if constexpr (std::is_same_v<CharT, char>) {
		return str;
	} else if constexpr (std::is_same_v<CharT, wchar_t>) {
		return utf16_to_utf8(str);
	} else if constexpr (std::is_same_v<CharT, char16_t>) {
		return utf16_to_utf8(str);
	} else if constexpr (std::is_same_v<CharT, char32_t>) {
		return utf32_to_utf8(str);
	} else {
		fea::maybe_throw(__FUNCTION__, __LINE__, "unsupported string type");
	}
}

template <class CharT>
string_t<CharT> utf8_to_any(const std::string& str) {
	if constexpr (std::is_same_v<CharT, char>) {
		return str;
	} else if constexpr (std::is_same_v<CharT, wchar_t>) {
		return utf8_to_utf16_w(str);
	} else if constexpr (std::is_same_v<CharT, char16_t>) {
		return utf8_to_utf16(str);
	} else if constexpr (std::is_same_v<CharT, char32_t>) {
		return utf8_to_utf32(str);
	} else {
		fea::maybe_throw(__FUNCTION__, __LINE__, "unsupported string type");
	}
}

template <class CharT>
std::u32string any_to_utf32(const string_t<CharT>& str) {
	if constexpr (std::is_same_v<CharT, char>) {
		return utf8_to_utf32(str);
	} else if constexpr (std::is_same_v<CharT, wchar_t>) {
		return utf16_to_utf32(str);
	} else if constexpr (std::is_same_v<CharT, char16_t>) {
		return utf16_to_utf32(str);
	} else if constexpr (std::is_same_v<CharT, char32_t>) {
		return str;
	} else {
		fea::maybe_throw(__FUNCTION__, __LINE__, "unsupported string type");
	}
}

template <class CharT>
string_t<CharT> utf32_to_any(const std::u32string& str) {
	if constexpr (std::is_same_v<CharT, char>) {
		return utf32_to_utf8(str);
	} else if constexpr (std::is_same_v<CharT, wchar_t>) {
		return utf32_to_utf16_w(str);
	} else if constexpr (std::is_same_v<CharT, char16_t>) {
		return utf32_to_utf16(str);
	} else if constexpr (std::is_same_v<CharT, char32_t>) {
		return str;
	} else {
		fea::maybe_throw(__FUNCTION__, __LINE__, "unsupported string type");
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

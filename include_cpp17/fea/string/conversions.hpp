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
#include "fea/utility/platform.hpp"

#include <algorithm>
#include <cassert>
#include <codecvt>
#include <cstdlib>
#include <locale>
#include <string>

/*
TODO : Rewrite everything based on std::c8rtomb, std::mbrtoc8,std::c16rtomb,
std::mbrtoc16, etc

Use return type overloading.

wstring -> reinterpret_cast as char16_t on windows, reinterpret_cast as char32_t
on linux.

*/

// Define FEA_CODEPAGE_CONVERSIONS to get windows (only) codepage conversions.
// Those are, helper functions for your current codepage to std::wstring.
#if FEA_WINDOWS && defined(FEA_CODEPAGE_CONVERSIONS_DEF)
#include <windows.h>
#endif

// The standard doesn't provide codecvt equivalents. Use the old
// functionality until they do.
#if FEA_MSVC
#pragma warning(push)
#pragma warning(disable : 4996)
#elif FEA_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

namespace fea {
// VS 2015 and VS 2017 don't export codecvt symbols...
#define FEA_MSVC_CODECVT_BUG 0
#if (_MSC_VER >= 1900 /* VS 2015*/) && (_MSC_VER <= 1916 /* VS 2017 */)
#undef FEA_MSVC_CODECVT_BUG
#define FEA_MSVC_CODECVT_BUG 1
using u16string_hack = std::basic_string<std::uint_least16_t,
		std::char_traits<std::uint_least16_t>,
		std::allocator<std::uint_least16_t>>;
using u32string_hack = std::basic_string<std::uint_least32_t,
		std::char_traits<std::uint_least32_t>,
		std::allocator<std::uint_least32_t>>;
#endif


// From UTF8 (multi-byte)

// UTF-8 to UTF-16
inline std::u16string utf8_to_utf16(const std::string& s) {
#if FEA_MSVC_CODECVT_BUG
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
#if FEA_MSVC_CODECVT_BUG
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
#if FEA_MSVC_CODECVT_BUG
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
#if FEA_MSVC_CODECVT_BUG
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

#if FEA_CPP20
// UTF-8 to UTF-32
inline std::u32string utf8_to_utf32(const std::u8string& s) {
#if FEA_MSVC_CODECVT_BUG
	std::wstring_convert<std::codecvt_utf8<std::uint_least32_t>,
			std::uint_least32_t>
			conv;
	u32string_hack str = conv.from_bytes(s);
	return { reinterpret_cast<const char32_t*>(str.c_str()) };
#else
	std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;
	return conv.from_bytes(reinterpret_cast<const char*>(s.c_str()));
#endif
}
#endif


// From UTF-16

// UTF-16 to UTF-8
inline std::string utf16_to_utf8(const std::u16string& s) {
#if FEA_MSVC_CODECVT_BUG
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
#if FEA_MSVC_CODECVT_BUG
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
#if FEA_MSVC_CODECVT_BUG
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
#if FEA_MSVC_CODECVT_BUG
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
std::string any_to_utf8(const std::basic_string<CharT>& str) {
	if constexpr (std::is_same_v<CharT, char>) {
		return str;
	} else if constexpr (std::is_same_v<CharT, wchar_t>) {
		return utf16_to_utf8(str);
	} else if constexpr (std::is_same_v<CharT, char16_t>) {
		return utf16_to_utf8(str);
	} else if constexpr (std::is_same_v<CharT, char32_t>) {
		return utf32_to_utf8(str);
	} else {
		// Cannot use fea::maybe_throw, the error.hpp header inlcudes us.
		// It causes a circular dependency.
		assert(false);
		std::exit(EXIT_FAILURE);
	}
}

template <class CharT>
std::basic_string<CharT> utf8_to_any(const std::string& str) {
	if constexpr (std::is_same_v<CharT, char>) {
		return str;
	} else if constexpr (std::is_same_v<CharT, wchar_t>) {
		return utf8_to_utf16_w(str);
	} else if constexpr (std::is_same_v<CharT, char16_t>) {
		return utf8_to_utf16(str);
	} else if constexpr (std::is_same_v<CharT, char32_t>) {
		return utf8_to_utf32(str);
#if FEA_CPP20
	} else if constexpr (std::is_same_v<CharT, char8_t>) {
		return std::u8string{ reinterpret_cast<const char8_t*>(str.c_str()) };
#endif
	} else {
		// Cannot use fea::maybe_throw, the error.hpp header inlcudes us.
		// It causes a circular dependency.
		assert(false);
		std::exit(EXIT_FAILURE);
	}
}

template <class CharT>
std::u32string any_to_utf32(const std::basic_string<CharT>& str) {
	if constexpr (std::is_same_v<CharT, char>) {
		return utf8_to_utf32(str);
	} else if constexpr (std::is_same_v<CharT, wchar_t>) {
		return utf16_to_utf32(str);
	} else if constexpr (std::is_same_v<CharT, char16_t>) {
		return utf16_to_utf32(str);
	} else if constexpr (std::is_same_v<CharT, char32_t>) {
		return str;
#if FEA_CPP20
	} else if constexpr (std::is_same_v<CharT, char8_t>) {
		return utf8_to_utf32(str);
#endif
	} else {
		// Cannot use fea::maybe_throw, the error.hpp header inlcudes us.
		// It causes a circular dependency.
		assert(false);
		std::exit(EXIT_FAILURE);
	}
}

template <class CharT>
std::basic_string<CharT> utf32_to_any(const std::u32string& str) {
	if constexpr (std::is_same_v<CharT, char>) {
		return utf32_to_utf8(str);
	} else if constexpr (std::is_same_v<CharT, wchar_t>) {
		return utf32_to_utf16_w(str);
	} else if constexpr (std::is_same_v<CharT, char16_t>) {
		return utf32_to_utf16(str);
	} else if constexpr (std::is_same_v<CharT, char32_t>) {
		return str;
	} else {
		// Cannot use fea::maybe_throw, the error.hpp header inlcudes us.
		// It causes a circular dependency.
		assert(false);
		std::exit(EXIT_FAILURE);
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


#if FEA_WINDOWS && defined(FEA_CODEPAGE_CONVERSIONS_DEF)
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
} // namespace fea

#if FEA_MSVC
#pragma warning(pop)
#elif FEA_CLANG
#pragma clang diagnostic pop
#endif

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
#include "fea/macros/foreach.hpp"
#include "fea/macros/macros.hpp"
#include "fea/utils/platform.hpp"

#include <type_traits>
#if FEA_CPP17
#include <string_view>
#endif

/*
Helpers for string literals.
*/

// Stringify passed in text with appropriate prefix.
// ex : FEA_WSTRINGIFY(test);
#define FEA_STRINGIFY(x) #x
#define FEA_WSTRINGIFY(x) FEA_PASTE(L, #x)
#define FEA_U16STRINGIFY(x) FEA_PASTE(u, #x)
#define FEA_U32STRINGIFY(x) FEA_PASTE(U, #x)

// Stringify passed in text with appropriate prefix.
// Adds a comma after literal.
// ex : FEA_WSTRINGIFY(test);
#define FEA_STRINGIFY_COMMA(x) FEA_STRINGIFY(x),
#define FEA_WSTRINGIFY_COMMA(x) FEA_WSTRINGIFY(x),
#define FEA_U16STRINGIFY_COMMA(x) FEA_U16STRINGIFY(x),
#define FEA_U32STRINGIFY_COMMA(x) FEA_U32STRINGIFY(x),

// Prefix the literal with : nothing, L, u or U.
#define FEA_PREFIX(x) x
#define FEA_WPREFIX(x) L##x
#define FEA_U16PREFIX(x) u##x
#define FEA_U32PREFIX(x) U##x

// Prefix the literal and add trailing comma.
#define FEA_PREFIX_COMMA(x) x,
#define FEA_WPREFIX_COMMA(x) L##x,
#define FEA_U16PREFIX_COMMA(x) u##x,
#define FEA_U32PREFIX_COMMA(x) U##x,

// Prefixes the literals list with : nothing, L, u, or U.
#define FEA_VA_PREFIX(...) FEA_FOR_EACH(FEA_PREFIX_COMMA, __VA_ARGS__)
#define FEA_VA_WPREFIX(...) FEA_FOR_EACH(FEA_WPREFIX_COMMA, __VA_ARGS__)
#define FEA_VA_U16PREFIX(...) FEA_FOR_EACH(FEA_U16PREFIX_COMMA, __VA_ARGS__)
#define FEA_VA_U32PREFIX(...) FEA_FOR_EACH(FEA_U32PREFIX_COMMA, __VA_ARGS__)

// Creates string of literal, prefixed appropriately.
#define FEA_S(x) std::string(x)
#define FEA_WS(x) std::wstring(FEA_WPREFIX(x))
#define FEA_U16S(x) std::u16string(FEA_U16PREFIX(x))
#define FEA_U32S(x) std::u32string(FEA_U32PREFIX(x))

// Creates string of literal with trailing comma.
#define FEA_S_COMMA(x) std::string(x),
#define FEA_WS_COMMA(x) std::wstring(FEA_WPREFIX(x)),
#define FEA_U16S_COMMA(x) std::u16string(FEA_U16PREFIX(x)),
#define FEA_U32S_COMMA(x) std::u32string(FEA_U32PREFIX(x)),

// Creates string_views with literals.
#define FEA_VA_S(...) FEA_FOR_EACH(FEA_S_COMMA, __VA_ARGS__)
#define FEA_VA_WS(...) FEA_FOR_EACH(FEA_WS_COMMA, __VA_ARGS__)
#define FEA_VA_U16S(...) FEA_FOR_EACH(FEA_U16S_COMMA, __VA_ARGS__)
#define FEA_VA_U32S(...) FEA_FOR_EACH(FEA_U32S_COMMA, __VA_ARGS__)

#if FEA_CPP17
// Creates string_view of literal, prefixed appropriately.
#define FEA_SV(x) std::string_view(x)
#define FEA_WSV(x) std::wstring_view(FEA_WPREFIX(x))
#define FEA_U16SV(x) std::u16string_view(FEA_U16PREFIX(x))
#define FEA_U32SV(x) std::u32string_view(FEA_U32PREFIX(x))

// Creates string_view of literal with trailing comma.
#define FEA_SV_COMMA(x) std::string_view(x),
#define FEA_WSV_COMMA(x) std::wstring_view(FEA_WPREFIX(x)),
#define FEA_U16SV_COMMA(x) std::u16string_view(FEA_U16PREFIX(x)),
#define FEA_U32SV_COMMA(x) std::u32string_view(FEA_U32PREFIX(x)),

// Creates string_views with literals.
#define FEA_VA_SV(...) FEA_FOR_EACH(FEA_SV_COMMA, __VA_ARGS__)
#define FEA_VA_WSV(...) FEA_FOR_EACH(FEA_WSV_COMMA, __VA_ARGS__)
#define FEA_VA_U16SV(...) FEA_FOR_EACH(FEA_U16SV_COMMA, __VA_ARGS__)
#define FEA_VA_U32SV(...) FEA_FOR_EACH(FEA_U32SV_COMMA, __VA_ARGS__)


// Makes a string from literal of type CharType
#define FEA_MAKE_STRING_T(CharType, str) \
	[]() { \
		if constexpr (std::is_same_v<CharType, char>) { \
			return std::string{ str }; \
		} else if constexpr (std::is_same_v<CharType, wchar_t>) { \
			return std::wstring{ L##str }; \
		} else if constexpr (std::is_same_v<CharType, char16_t>) { \
			return std::u16string{ u##str }; \
		} else if constexpr (std::is_same_v<CharType, char32_t>) { \
			return std::u32string{ U##str }; \
		} \
	}()

// These are shortcuts if you use CharT as an alias or template.
#define FEA_MAKE_STRING(str) FEA_MAKE_STRING_T(CharT, str)
#define FEA_STR(str) FEA_MAKE_STRING_T(CharT, str)

// Makes a string_view from literal of type CharType
#define FEA_MAKE_STRING_VIEW_T(CharType, str) \
	[]() { \
		if constexpr (std::is_same_v<CharType, char>) { \
			return std::string_view{ str }; \
		} else if constexpr (std::is_same_v<CharType, wchar_t>) { \
			return std::wstring_view{ L##str }; \
		} else if constexpr (std::is_same_v<CharType, char16_t>) { \
			return std::u16string_view{ u##str }; \
		} else if constexpr (std::is_same_v<CharType, char32_t>) { \
			return std::u32string_view{ U##str }; \
		} \
	}()

// These are shortcuts if you use CharT as an alias or template.
#define FEA_MAKE_STRING_VIEW(str) FEA_MAKE_STRING_VIEW_T(CharT, str)
#define FEA_STRV(str) FEA_MAKE_STRING_VIEW_T(CharT, str)

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
#define FEA_LIT(str) FEA_MAKE_LITERAL_T(CharT, str)


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

#endif // FEA_CPP17

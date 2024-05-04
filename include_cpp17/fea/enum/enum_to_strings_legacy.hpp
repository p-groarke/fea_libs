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
#include "fea/enum/enum_array.hpp"
#include "fea/macros/foreach.hpp"
#include "fea/macros/literals.hpp"
#include "fea/macros/macros.hpp"
#include "fea/containers/type_map.hpp"
#include "fea/meta/pack.hpp"
#include "fea/meta/traits.hpp"
#include "fea/string/string.hpp"
#include "fea/utils/platform.hpp"

#include <array>
#include <cstdint>
#include <string>
#include <string_view>
#include <tuple>

/*
FEA_STRING_ENUM_LEGACY creates an enum class with accompanying fea::enum_arrays
of literals and string_views, plus useful accessors.

Call the macro using (enum_name, enum_underlying_type, your, enum, values, ...)
You must always provide an underlying_type.

Example :
FEA_STRING_ENUM_LEGACY(my_enum, unsigned, potato, tomato)

Generates :
enum class my_enum : unsigned { potato, tomato };

constexpr fea::enum_array<const char* const, ...> my_enum_literals;
const fea::enum_array<std::string, ...> my_enum_strings;

constexpr const fea::enum_array<std::string>& strings<my_enum>();

constexpr const std::string& to_string(my_enum e);

template <my_enum E>
constexpr const std::string& to_string();

It generates all of these for all string types :
string, wstring, u16string, u32string.

Prepend "string" with your desired string type. For ex, u32.
const fea::enum_array<std::u32string, ...> my_enum_u32strings;

etc...

*/


// Get the variables' name.
#define FEA_DETAIL_SE_VARNAME(prefix, ename, suffix) \
	FEA_PASTE(ename, FEA_PASTE(_, FEA_PASTE(prefix, suffix)))

// Generates an enum_array of const char* const and one of std::string.
// The arrays are prefixed with the provided string type prefix.
#define FEA_DETAIL_SE_ARRAYS(stringify_macro, chartype, prefix, ename, ...) \
	[[maybe_unused]] inline constexpr fea::enum_array< \
			std::basic_string_view<chartype>, ename, \
			FEA_SIZEOF_VAARGS(__VA_ARGS__)> \
	FEA_DETAIL_SE_VARNAME(prefix, ename, strings) { \
		{ FEA_FOR_EACH(stringify_macro, __VA_ARGS__) } \
	}

// Literals equivalent, currently unused.
#define FEA_DETAIL_SE_LIT_ARRAYS( \
		stringify_macro, chartype, prefix, ename, ...) \
	[[maybe_unused]] inline constexpr fea::enum_array<const chartype* const, \
			ename, FEA_SIZEOF_VAARGS(__VA_ARGS__)> \
			FEA_DETAIL_SE_VARNAME(prefix, ename, literals){ { FEA_FOR_EACH( \
					stringify_macro, __VA_ARGS__) } };


// Declares and implements helper functions.
// strings<enum>(), to_string<enum::val>(), to_string(enum::val)
#define FEA_DETAIL_SE_FUNCS(chartype, prefix, ename) \
	/* Forward declares template functions we specialize. */ \
	template <class> \
	[[maybe_unused]] constexpr const auto& FEA_PASTE(prefix, strings)(); \
	/* Specialize the above function declarations. */ \
	template <> \
	[[maybe_unused]] constexpr const auto& FEA_PASTE( \
			prefix, strings)<ename>() { \
		return FEA_DETAIL_SE_VARNAME(prefix, ename, strings); \
	} \
	/* Non-type compile-time getters. */ \
	template <ename E> \
	[[maybe_unused]] constexpr std::basic_string_view<chartype> \
	FEA_DETAIL_SE_VARNAME(prefix, to, string)() { \
		return FEA_DETAIL_SE_VARNAME(prefix, ename, strings).at<E>(); \
	} \
	/* Non-templated getters, fast O(1). */ \
	[[maybe_unused]] inline constexpr std::basic_string_view<chartype> \
	FEA_DETAIL_SE_VARNAME(prefix, to, string)(ename e) { \
		return FEA_DETAIL_SE_VARNAME(prefix, ename, strings)[e]; \
	}

// Literals equivalent, currently unused.
#define FEA_DETAIL_SE_LIT_FUNCS(chartype, prefix, ename) \
	/* Forward declares template functions we specialize. */ \
	template <class> \
	[[maybe_unused]] constexpr const auto& FEA_PASTE(prefix, literals)(); \
	/* Specialize the above function declarations. */ \
	template <> \
	[[maybe_unused]] constexpr const auto& FEA_PASTE( \
			prefix, literals)<ename>() { \
		return FEA_DETAIL_SE_VARNAME(prefix, ename, literals); \
	} \
	/* Non-type compile-time getters. */ \
	template <ename E> \
	[[maybe_unused]] constexpr const chartype* FEA_DETAIL_SE_VARNAME( \
			prefix, to, literal)() { \
		return FEA_DETAIL_SE_VARNAME(prefix, ename, literals).at<E>(); \
	} \
	/* Non-templated getters, fast O(1). */ \
	[[maybe_unused]] inline constexpr const chartype* FEA_DETAIL_SE_VARNAME( \
			prefix, to, literal)(ename e) { \
		return FEA_DETAIL_SE_VARNAME(prefix, ename, literals)[e]; \
	}


// Generates both an enum and accompanying arrays of enum strings and functions.
#define FEA_STRING_ENUM_LEGACY(ename, utype, ...) \
	/* Declares your enum. */ \
	enum class ename : utype { __VA_ARGS__ }; \
	/* char and std::string */ \
	FEA_DETAIL_SE_ARRAYS(FEA_STRINGIFY_COMMA, char, , ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(char, , ename)

// Generates both an enum and accompanying arrays of enum strings and functions.
#define FEA_WSTRING_ENUM_LEGACY(ename, utype, ...) \
	/* Declares your enum. */ \
	enum class ename : utype { __VA_ARGS__ }; \
	/* wchar_t and std::wstring */ \
	FEA_DETAIL_SE_ARRAYS( \
			FEA_WSTRINGIFY_COMMA, wchar_t, w, ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(wchar_t, w, ename)

// Generates both an enum and accompanying arrays of enum strings and functions.
#define FEA_U16STRING_ENUM_LEGACY(ename, utype, ...) \
	/* Declares your enum. */ \
	enum class ename : utype { __VA_ARGS__ }; \
	/* char16_t and std::u16string */ \
	FEA_DETAIL_SE_ARRAYS( \
			FEA_U16STRINGIFY_COMMA, char16_t, u16, ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(char16_t, u16, ename)

// Generates both an enum and accompanying arrays of enum strings and functions.
#define FEA_U32STRING_ENUM_LEGACY(ename, utype, ...) \
	/* Declares your enum. */ \
	enum class ename : utype { __VA_ARGS__ }; \
	/* char32_t and std::u32string */ \
	FEA_DETAIL_SE_ARRAYS( \
			FEA_U32STRINGIFY_COMMA, char32_t, u32, ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(char32_t, u32, ename)

// Generates both an enum and accompanying arrays of enum strings and functions.
#define FEA_ALLSTRINGS_ENUM_LEGACY(ename, utype, ...) \
	/* Declares your enum. */ \
	enum class ename : utype { __VA_ARGS__ }; \
	/* char and std::string */ \
	FEA_DETAIL_SE_ARRAYS(FEA_STRINGIFY_COMMA, char, , ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(char, , ename) \
	/* wchar_t and std::wstring */ \
	FEA_DETAIL_SE_ARRAYS( \
			FEA_WSTRINGIFY_COMMA, wchar_t, w, ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(wchar_t, w, ename) \
	/* char16_t and std::u16string */ \
	FEA_DETAIL_SE_ARRAYS( \
			FEA_U16STRINGIFY_COMMA, char16_t, u16, ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(char16_t, u16, ename) \
	/* char32_t and std::u32string */ \
	FEA_DETAIL_SE_ARRAYS( \
			FEA_U32STRINGIFY_COMMA, char32_t, u32, ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(char32_t, u32, ename)

// Generates the enum and accompanying helpers, plus,
// calls your provided macro with ename, __VA_ARGS__
#define FEA_STRING_ENUM_LEGACY_CUSTOM(user_macro, ename, utype, ...) \
	/* Declares your enum. */ \
	enum class ename : utype { __VA_ARGS__ }; \
	/* char and std::string */ \
	FEA_DETAIL_SE_ARRAYS(FEA_STRINGIFY_COMMA, char, , ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(char, , ename) \
\
	/* Call user macro once everything is done. */ \
	user_macro(ename, __VA_ARGS__)

// Generates the enum and accompanying helpers, plus,
// calls your provided macro with ename, __VA_ARGS__
#define FEA_WSTRING_ENUM_LEGACY_CUSTOM(user_macro, ename, utype, ...) \
	/* Declares your enum. */ \
	enum class ename : utype { __VA_ARGS__ }; \
	/* wchar_t and std::wstring */ \
	FEA_DETAIL_SE_ARRAYS( \
			FEA_WSTRINGIFY_COMMA, wchar_t, w, ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(wchar_t, w, ename) \
\
	/* Call user macro once everything is done. */ \
	user_macro(ename, __VA_ARGS__)

// Generates the enum and accompanying helpers, plus,
// calls your provided macro with ename, __VA_ARGS__
#define FEA_U16STRING_ENUM_LEGACY_CUSTOM(user_macro, ename, utype, ...) \
	/* Declares your enum. */ \
	enum class ename : utype { __VA_ARGS__ }; \
	/* char16_t and std::u16string */ \
	FEA_DETAIL_SE_ARRAYS( \
			FEA_U16STRINGIFY_COMMA, char16_t, u16, ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(char16_t, u16, ename) \
\
	/* Call user macro once everything is done. */ \
	user_macro(ename, __VA_ARGS__)

// Generates the enum and accompanying helpers, plus,
// calls your provided macro with ename, __VA_ARGS__
#define FEA_U32STRING_ENUM_LEGACY_CUSTOM(user_macro, ename, utype, ...) \
	/* Declares your enum. */ \
	enum class ename : utype { __VA_ARGS__ }; \
	/* char32_t and std::u32string */ \
	FEA_DETAIL_SE_ARRAYS( \
			FEA_U32STRINGIFY_COMMA, char32_t, u32, ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(char32_t, u32, ename) \
\
	/* Call user macro once everything is done. */ \
	user_macro(ename, __VA_ARGS__)


// Generates the enum and accompanying helpers, plus,
// calls your provided macro with ename, __VA_ARGS__
#define FEA_ALLSTRINGS_ENUM_LEGACY_CUSTOM(user_macro, ename, utype, ...) \
	/* Declares your enum. */ \
	enum class ename : utype { __VA_ARGS__ }; \
	/* char and std::string */ \
	FEA_DETAIL_SE_ARRAYS(FEA_STRINGIFY_COMMA, char, , ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(char, , ename) \
	/* wchar_t and std::wstring */ \
	FEA_DETAIL_SE_ARRAYS( \
			FEA_WSTRINGIFY_COMMA, wchar_t, w, ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(wchar_t, w, ename) \
	/* char16_t and std::u16string */ \
	FEA_DETAIL_SE_ARRAYS( \
			FEA_U16STRINGIFY_COMMA, char16_t, u16, ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(char16_t, u16, ename) \
	/* char32_t and std::u32string */ \
	FEA_DETAIL_SE_ARRAYS( \
			FEA_U32STRINGIFY_COMMA, char32_t, u32, ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(char32_t, u32, ename) \
\
	/* Call user macro once everything is done. */ \
	user_macro(ename, __VA_ARGS__)


// Generates both an enum and accompanying arrays of enum strings and functions.
// This version generates a 'count' enum value for you, and places it at the end
// of the enum. No strings are generated for 'count', and array sizes == count.
#define FEA_STRING_ENUM_LEGACY_WITH_COUNT(ename, utype, ...) \
	/* Declares your enum. Adds 'count' at the end. */ \
	enum class ename : utype { __VA_ARGS__, count }; \
	/* char and std::string */ \
	FEA_DETAIL_SE_ARRAYS(FEA_STRINGIFY_COMMA, char, , ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(char, , ename)

// Generates both an enum and accompanying arrays of enum strings and functions.
// This version generates a 'count' enum value for you, and places it at the end
// of the enum. No strings are generated for 'count', and array sizes == count.
#define FEA_WSTRING_ENUM_LEGACY_WITH_COUNT(ename, utype, ...) \
	/* Declares your enum. Adds 'count' at the end. */ \
	enum class ename : utype { __VA_ARGS__, count }; \
	/* wchar_t and std::wstring */ \
	FEA_DETAIL_SE_ARRAYS( \
			FEA_WSTRINGIFY_COMMA, wchar_t, w, ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(wchar_t, w, ename)

// Generates both an enum and accompanying arrays of enum strings and functions.
// This version generates a 'count' enum value for you, and places it at the end
// of the enum. No strings are generated for 'count', and array sizes == count.
#define FEA_U16STRING_ENUM_LEGACY_WITH_COUNT(ename, utype, ...) \
	/* Declares your enum. Adds 'count' at the end. */ \
	enum class ename : utype { __VA_ARGS__, count }; \
	/* char16_t and std::u16string */ \
	FEA_DETAIL_SE_ARRAYS( \
			FEA_U16STRINGIFY_COMMA, char16_t, u16, ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(char16_t, u16, ename)

// Generates both an enum and accompanying arrays of enum strings and functions.
// This version generates a 'count' enum value for you, and places it at the end
// of the enum. No strings are generated for 'count', and array sizes == count.
#define FEA_U32STRING_ENUM_LEGACY_WITH_COUNT(ename, utype, ...) \
	/* Declares your enum. Adds 'count' at the end. */ \
	enum class ename : utype { __VA_ARGS__, count }; \
	/* char32_t and std::u32string */ \
	FEA_DETAIL_SE_ARRAYS( \
			FEA_U32STRINGIFY_COMMA, char32_t, u32, ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(char32_t, u32, ename)

// Generates both an enum and accompanying arrays of enum strings and functions.
// This version generates a 'count' enum value for you, and places it at the end
// of the enum. No strings are generated for 'count', and array sizes == count.
#define FEA_ALLSTRINGS_ENUM_LEGACY_WITH_COUNT(ename, utype, ...) \
	/* Declares your enum. Adds 'count' at the end. */ \
	enum class ename : utype { __VA_ARGS__, count }; \
	/* char and std::string */ \
	FEA_DETAIL_SE_ARRAYS(FEA_STRINGIFY_COMMA, char, , ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(char, , ename) \
	/* wchar_t and std::wstring */ \
	FEA_DETAIL_SE_ARRAYS( \
			FEA_WSTRINGIFY_COMMA, wchar_t, w, ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(wchar_t, w, ename) \
	/* char16_t and std::u16string */ \
	FEA_DETAIL_SE_ARRAYS( \
			FEA_U16STRINGIFY_COMMA, char16_t, u16, ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(char16_t, u16, ename) \
	/* char32_t and std::u32string */ \
	FEA_DETAIL_SE_ARRAYS( \
			FEA_U32STRINGIFY_COMMA, char32_t, u32, ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(char32_t, u32, ename)


// Generates both an enum and accompanying arrays of enum strings and functions.
// This version generates a 'count' enum value for you, and places it at the end
// of the enum. No strings are generated for 'count', and array sizes == count.
//
// In addition, this call will call your 'user_macro' once it is done.
// Your macro must accept : (enum_name, ...)
#define FEA_STRING_ENUM_LEGACY_WITH_COUNT_CUSTOM( \
		user_macro, ename, utype, ...) \
	/* Declares your enum. Adds 'count' at the end. */ \
	enum class ename : utype { __VA_ARGS__, count }; \
	/* char and std::string */ \
	FEA_DETAIL_SE_ARRAYS(FEA_STRINGIFY_COMMA, char, , ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(char, , ename) \
\
	/* Call user macro once everything is done. */ \
	user_macro(ename, __VA_ARGS__)

// Generates both an enum and accompanying arrays of enum strings and functions.
// This version generates a 'count' enum value for you, and places it at the end
// of the enum. No strings are generated for 'count', and array sizes == count.
//
// In addition, this call will call your 'user_macro' once it is done.
// Your macro must accept : (enum_name, ...)
#define FEA_WSTRING_ENUM_LEGACY_WITH_COUNT_CUSTOM( \
		user_macro, ename, utype, ...) \
	/* Declares your enum. Adds 'count' at the end. */ \
	enum class ename : utype { __VA_ARGS__, count }; \
	/* wchar_t and std::wstring */ \
	FEA_DETAIL_SE_ARRAYS( \
			FEA_WSTRINGIFY_COMMA, wchar_t, w, ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(wchar_t, w, ename) \
\
	/* Call user macro once everything is done. */ \
	user_macro(ename, __VA_ARGS__)

// Generates both an enum and accompanying arrays of enum strings and functions.
// This version generates a 'count' enum value for you, and places it at the end
// of the enum. No strings are generated for 'count', and array sizes == count.
//
// In addition, this call will call your 'user_macro' once it is done.
// Your macro must accept : (enum_name, ...)
#define FEA_U16STRING_ENUM_LEGACY_WITH_COUNT_CUSTOM( \
		user_macro, ename, utype, ...) \
	/* Declares your enum. Adds 'count' at the end. */ \
	enum class ename : utype { __VA_ARGS__, count }; \
	/* char16_t and std::u16string */ \
	FEA_DETAIL_SE_ARRAYS( \
			FEA_U16STRINGIFY_COMMA, char16_t, u16, ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(char16_t, u16, ename) \
\
	/* Call user macro once everything is done. */ \
	user_macro(ename, __VA_ARGS__)

// Generates both an enum and accompanying arrays of enum strings and functions.
// This version generates a 'count' enum value for you, and places it at the end
// of the enum. No strings are generated for 'count', and array sizes == count.
//
// In addition, this call will call your 'user_macro' once it is done.
// Your macro must accept : (enum_name, ...)
#define FEA_U32STRING_ENUM_LEGACY_WITH_COUNT_CUSTOM( \
		user_macro, ename, utype, ...) \
	/* Declares your enum. Adds 'count' at the end. */ \
	enum class ename : utype { __VA_ARGS__, count }; \
	/* char32_t and std::u32string */ \
	FEA_DETAIL_SE_ARRAYS( \
			FEA_U32STRINGIFY_COMMA, char32_t, u32, ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(char32_t, u32, ename) \
\
	/* Call user macro once everything is done. */ \
	user_macro(ename, __VA_ARGS__)

// Generates both an enum and accompanying arrays of enum strings and functions.
// This version generates a 'count' enum value for you, and places it at the end
// of the enum. No strings are generated for 'count', and array sizes == count.
//
// In addition, this call will call your 'user_macro' once it is done.
// Your macro must accept : (enum_name, ...)
#define FEA_ALLSTRINGS_ENUM_LEGACY_WITH_COUNT_CUSTOM( \
		user_macro, ename, utype, ...) \
	/* Declares your enum. Adds 'count' at the end. */ \
	enum class ename : utype { __VA_ARGS__, count }; \
	/* char and std::string */ \
	FEA_DETAIL_SE_ARRAYS(FEA_STRINGIFY_COMMA, char, , ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(char, , ename) \
	/* wchar_t and std::wstring */ \
	FEA_DETAIL_SE_ARRAYS( \
			FEA_WSTRINGIFY_COMMA, wchar_t, w, ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(wchar_t, w, ename) \
	/* char16_t and std::u16string */ \
	FEA_DETAIL_SE_ARRAYS( \
			FEA_U16STRINGIFY_COMMA, char16_t, u16, ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(char16_t, u16, ename) \
	/* char32_t and std::u32string */ \
	FEA_DETAIL_SE_ARRAYS( \
			FEA_U32STRINGIFY_COMMA, char32_t, u32, ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(char32_t, u32, ename) \
\
	/* Call user macro once everything is done. */ \
	user_macro(ename, __VA_ARGS__)

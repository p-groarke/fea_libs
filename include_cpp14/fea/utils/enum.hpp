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
#include "fea/containers/enum_array.hpp"
#include "fea/meta/macros.hpp"
#include "fea/utils/platform.hpp"
#include "fea/utils/string.hpp"

#include <array>
#include <string>

/*
FEA_STRING_ENUM creates an enum with accompanying fea::enum_arrays of
strings and useful accessors.

All generated data and functions live in namespace 'enu'.

Call the macro using (enum_name, enum_underlying_type, your, enum, values, ...)
You must always provide an underlying_type.

Example :
FEA_STRING_ENUM(my_enum, unsigned, potato, tomato)

Generates :
enum class my_enum : unsigned { potato, tomato };

namespace enu {
constexpr fea::enum_array<const char* const, ...> my_enum_literals;
const fea::enum_array<std::string, ...> my_enum_strings;

constexpr const fea::enum_array<const char* const>& literals<my_enum>();
constexpr const fea::enum_array<std::string>& strings<my_enum>();

constexpr const char* const to_literal(my_enum e);
constexpr const std::string& to_string(my_enum e);

template <my_enum E>
constexpr const char* const to_literal();
template <my_enum E>
constexpr const std::string& to_string();

} // namespace enu

It generates all of these for all string types :
string, wstring, u16string, u32string.

Prepend "string" with your desired string type. For ex, u32.
constexpr fea::enum_array<const char32_t* const, ...> my_enum_u32literals;
const fea::enum_array<std::u32string, ...> my_enum_u32strings;

etc...

*/

// Get the variables' name.
#define FEA_DETAIL_SE_VARNAME(prefix, ename, suffix) \
	FEA_PASTE(ename, FEA_PASTE(_, FEA_PASTE(prefix, suffix)))

// Generates an enum_array of const char* const and one of std::string.
// The arrays are prefixed with the provided string type prefix.
#define FEA_DETAIL_SE_ARRAYS(stringify_macro, chartype, prefix, ename, ...) \
	FEA_INLINE_VAR constexpr fea::enum_array<const chartype* const, ename, \
			FEA_SIZEOF_VAARGS(__VA_ARGS__)> \
			FEA_DETAIL_SE_VARNAME(prefix, ename, \
					literals){ FEA_FOR_EACH(stringify_macro, __VA_ARGS__) }; \
	FEA_INLINE_VAR const fea::enum_array<fea::m_string<chartype>, ename, \
			FEA_SIZEOF_VAARGS(__VA_ARGS__)> \
	FEA_DETAIL_SE_VARNAME(prefix, ename, strings) { \
		FEA_FOR_EACH(stringify_macro, __VA_ARGS__) \
	}

// Declares and implements helper functions.
// literals<enum>(), to_literal<enum::val>(), to_literal(enum::val) and same for
// strings.
#define FEA_DETAIL_SE_FUNCS(chartype, prefix, ename) \
	/* Forward declares template functions we specialize. */ \
	template <class> \
	constexpr const auto& FEA_PASTE(prefix, literals)(); \
	template <class> \
	constexpr const auto& FEA_PASTE(prefix, strings)(); \
	/* Specialize the above function declarations. */ \
	template <> \
	constexpr const auto& FEA_PASTE(prefix, literals)<ename>() { \
		return FEA_DETAIL_SE_VARNAME(prefix, ename, literals); \
	} \
	template <> \
	constexpr const auto& FEA_PASTE(prefix, strings)<ename>() { \
		return FEA_DETAIL_SE_VARNAME(prefix, ename, strings); \
	} \
	/* Non-type compile-time getters. */ \
	template <ename E> \
	constexpr const chartype* const FEA_DETAIL_SE_VARNAME( \
			prefix, to, literal)() { \
		return fea::get<E>(FEA_DETAIL_SE_VARNAME(prefix, ename, literals)); \
	} \
	template <ename E> \
	constexpr const std::string& FEA_DETAIL_SE_VARNAME(prefix, to, string)() { \
		return fea::get<E>(FEA_DETAIL_SE_VARNAME(prefix, ename, strings)); \
	} \
	/* Non-templated getters, fast O(1). */ \
	inline constexpr const chartype* const FEA_DETAIL_SE_VARNAME( \
			prefix, to, literal)(ename e) { \
		return FEA_DETAIL_SE_VARNAME(prefix, ename, literals)[e]; \
	} \
	inline constexpr const fea::m_string<chartype>& FEA_DETAIL_SE_VARNAME( \
			prefix, to, string)(ename e) { \
		return FEA_DETAIL_SE_VARNAME(prefix, ename, strings)[e]; \
	}


// Generates both an enum and accompanying arrays of enum strings and functions.
#define FEA_STRING_ENUM(ename, utype, ...) \
	/* Declares your enum. */ \
	enum class ename : utype { __VA_ARGS__ }; \
	/* All strings and functions are declared in namespace 'enu'. */ \
	namespace enu { \
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
	} // namespace enu

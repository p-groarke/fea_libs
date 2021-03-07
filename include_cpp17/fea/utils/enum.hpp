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
#include "fea/containers/enum_array.hpp"
#include "fea/maps/type_map.hpp"
#include "fea/meta/macros.hpp"
#include "fea/meta/pack.hpp"
#include "fea/meta/traits.hpp"
#include "fea/utils/platform.hpp"
#include "fea/utils/string.hpp"

#include <array>
#include <string>
#include <tuple>

/*
FEA_STRING_ENUM creates an enum class with accompanying fea::enum_arrays of
literals and strings, plus useful accessors.

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

constexpr const fea::enum_array<std::string>& strings<my_enum>();

constexpr const std::string& to_string(my_enum e);

template <my_enum E>
constexpr const std::string& to_string();

} // namespace enu

It generates all of these for all string types :
string, wstring, u16string, u32string.

Prepend "string" with your desired string type. For ex, u32.
const fea::enum_array<std::u32string, ...> my_enum_u32strings;

etc...


fea::explode_enum calls your lambda with a variadic pack of
std::integral_constant<your_enum, val>...

fea::safe_switch creates a switch-case that will fail (wont compile) if you add
or remove values to an enum.
*/


// Get the variables' name.
#define FEA_DETAIL_SE_VARNAME(prefix, ename, suffix) \
	FEA_PASTE(ename, FEA_PASTE(_, FEA_PASTE(prefix, suffix)))

// Generates an enum_array of const char* const and one of std::string.
// The arrays are prefixed with the provided string type prefix.
#define FEA_DETAIL_SE_ARRAYS(stringify_macro, chartype, prefix, ename, ...) \
	[[maybe_unused]] FEA_INLINE_VAR const fea::enum_array< \
			fea::string_t<chartype>, ename, FEA_SIZEOF_VAARGS(__VA_ARGS__)> \
	FEA_DETAIL_SE_VARNAME(prefix, ename, strings) { \
		{ FEA_FOR_EACH(stringify_macro, __VA_ARGS__) } \
	}

// Literals equivalent, currently unused.
#define FEA_DETAIL_SE_LIT_ARRAYS( \
		stringify_macro, chartype, prefix, ename, ...) \
	[[maybe_unused]] FEA_INLINE_VAR constexpr fea::enum_array< \
			const chartype* const, ename, FEA_SIZEOF_VAARGS(__VA_ARGS__)> \
			FEA_DETAIL_SE_VARNAME(prefix, ename, literals){ { FEA_FOR_EACH( \
					stringify_macro, __VA_ARGS__) } };


// Declares and implements helper functions.
// string<enum>(), to_string<enum::val>(), to_string(enum::val)
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
	[[maybe_unused]] constexpr const fea::string_t<chartype>& \
	FEA_DETAIL_SE_VARNAME(prefix, to, string)() { \
		return FEA_DETAIL_SE_VARNAME(prefix, ename, strings).at<E>(); \
	} \
	/* Non-templated getters, fast O(1). */ \
	[[maybe_unused]] inline constexpr const fea::string_t<chartype>& \
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
#define FEA_STRING_ENUM(ename, utype, ...) \
	/* Declares your enum. */ \
	enum class ename : utype { __VA_ARGS__ }; \
	/* char and std::string */ \
	FEA_DETAIL_SE_ARRAYS(FEA_STRINGIFY_COMMA, char, , ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(char, , ename)

// Generates both an enum and accompanying arrays of enum strings and functions.
#define FEA_WSTRING_ENUM(ename, utype, ...) \
	/* Declares your enum. */ \
	enum class ename : utype { __VA_ARGS__ }; \
	/* wchar_t and std::wstring */ \
	FEA_DETAIL_SE_ARRAYS( \
			FEA_WSTRINGIFY_COMMA, wchar_t, w, ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(wchar_t, w, ename)

// Generates both an enum and accompanying arrays of enum strings and functions.
#define FEA_U16STRING_ENUM(ename, utype, ...) \
	/* Declares your enum. */ \
	enum class ename : utype { __VA_ARGS__ }; \
	/* char16_t and std::u16string */ \
	FEA_DETAIL_SE_ARRAYS( \
			FEA_U16STRINGIFY_COMMA, char16_t, u16, ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(char16_t, u16, ename)

// Generates both an enum and accompanying arrays of enum strings and functions.
#define FEA_U32STRING_ENUM(ename, utype, ...) \
	/* Declares your enum. */ \
	enum class ename : utype { __VA_ARGS__ }; \
	/* char32_t and std::u32string */ \
	FEA_DETAIL_SE_ARRAYS( \
			FEA_U32STRINGIFY_COMMA, char32_t, u32, ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(char32_t, u32, ename)

// Generates both an enum and accompanying arrays of enum strings and functions.
#define FEA_ALLSTRINGS_ENUM(ename, utype, ...) \
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
#define FEA_STRING_ENUM_CUSTOM(user_macro, ename, utype, ...) \
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
#define FEA_WSTRING_ENUM_CUSTOM(user_macro, ename, utype, ...) \
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
#define FEA_U16STRING_ENUM_CUSTOM(user_macro, ename, utype, ...) \
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
#define FEA_U32STRING_ENUM_CUSTOM(user_macro, ename, utype, ...) \
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
#define FEA_ALLSTRINGS_ENUM_CUSTOM(user_macro, ename, utype, ...) \
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
#define FEA_STRING_ENUM_WITH_COUNT(ename, utype, ...) \
	/* Declares your enum. Adds 'count' at the end. */ \
	enum class ename : utype { __VA_ARGS__, count }; \
	/* char and std::string */ \
	FEA_DETAIL_SE_ARRAYS(FEA_STRINGIFY_COMMA, char, , ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(char, , ename)

// Generates both an enum and accompanying arrays of enum strings and functions.
// This version generates a 'count' enum value for you, and places it at the end
// of the enum. No strings are generated for 'count', and array sizes == count.
#define FEA_WSTRING_ENUM_WITH_COUNT(ename, utype, ...) \
	/* Declares your enum. Adds 'count' at the end. */ \
	enum class ename : utype { __VA_ARGS__, count }; \
	/* wchar_t and std::wstring */ \
	FEA_DETAIL_SE_ARRAYS( \
			FEA_WSTRINGIFY_COMMA, wchar_t, w, ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(wchar_t, w, ename)

// Generates both an enum and accompanying arrays of enum strings and functions.
// This version generates a 'count' enum value for you, and places it at the end
// of the enum. No strings are generated for 'count', and array sizes == count.
#define FEA_U16STRING_ENUM_WITH_COUNT(ename, utype, ...) \
	/* Declares your enum. Adds 'count' at the end. */ \
	enum class ename : utype { __VA_ARGS__, count }; \
	/* char16_t and std::u16string */ \
	FEA_DETAIL_SE_ARRAYS( \
			FEA_U16STRINGIFY_COMMA, char16_t, u16, ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(char16_t, u16, ename)

// Generates both an enum and accompanying arrays of enum strings and functions.
// This version generates a 'count' enum value for you, and places it at the end
// of the enum. No strings are generated for 'count', and array sizes == count.
#define FEA_U32STRING_ENUM_WITH_COUNT(ename, utype, ...) \
	/* Declares your enum. Adds 'count' at the end. */ \
	enum class ename : utype { __VA_ARGS__, count }; \
	/* char32_t and std::u32string */ \
	FEA_DETAIL_SE_ARRAYS( \
			FEA_U32STRINGIFY_COMMA, char32_t, u32, ename, __VA_ARGS__); \
	FEA_DETAIL_SE_FUNCS(char32_t, u32, ename)

// Generates both an enum and accompanying arrays of enum strings and functions.
// This version generates a 'count' enum value for you, and places it at the end
// of the enum. No strings are generated for 'count', and array sizes == count.
#define FEA_ALLSTRINGS_ENUM_WITH_COUNT(ename, utype, ...) \
	/* Declares your enum. Adds 'count' at the end. */ \
	enum class ename : utype { __VA_ARGS__, count }; \
	/* All strings and functions are declared in namespace 'enu'. */ \
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
#define FEA_STRING_ENUM_WITH_COUNT_CUSTOM(user_macro, ename, utype, ...) \
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
#define FEA_WSTRING_ENUM_WITH_COUNT_CUSTOM(user_macro, ename, utype, ...) \
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
#define FEA_U16STRING_ENUM_WITH_COUNT_CUSTOM(user_macro, ename, utype, ...) \
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
#define FEA_U32STRING_ENUM_WITH_COUNT_CUSTOM(user_macro, ename, utype, ...) \
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
#define FEA_ALLSTRINGS_ENUM_WITH_COUNT_CUSTOM(user_macro, ename, utype, ...) \
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


namespace fea {
namespace detail {
template <class Enum, class Func, size_t... Idx>
constexpr auto explode_enum(Func&& func, std::index_sequence<Idx...>) {
	return std::forward<Func>(func)(
			std::integral_constant<Enum, Enum(Idx)>{}...);
}
} // namespace detail

// Explodes all enum values into a non-type parameter pack and calls your
// function with it.
// Enum must be from 0 to N.
// You must "extract" the non-type enum as it is passed by
// std::integral_constant, use ::value.
template <class Enum, size_t N, class Func>
constexpr auto explode_enum(Func&& func) {
	return detail::explode_enum<Enum>(
			std::forward<Func>(func), std::make_index_sequence<N>{});
}

// Explodes all enum values into a non-type parameter pack and calls your
// function with it.
// Enum must be from 0 to N.
// Overload for enums that contain a 'count' member.
template <class Enum, class Func>
constexpr auto explode_enum(Func&& func) {
	return detail::explode_enum<Enum>(std::forward<Func>(func),
			std::make_index_sequence<size_t(Enum::count)>{});
}


// Calls your function with each non-type enum values.
// Enum must be from 0 to N.
// Provide N if your enum doesn't have the member 'count'.
template <class Enum, size_t N = size_t(Enum::count), class Func>
constexpr void enum_for_each(Func&& func) {
	fea::explode_enum<Enum, N>(
			[&](auto... constants) { (func(constants), ...); });
}


namespace detail {
template <class Enum, size_t N, class Funcs, Enum... Es>
struct switcher;

template <class Enum, size_t N, class... Funcs, Enum... Es>
struct switcher<Enum, N, std::tuple<Funcs...>, Es...> {

	constexpr switcher() = default;
	constexpr switcher(std::tuple<Funcs...>&& funcs)
			: _funcs(std::move(funcs)) {
	}

	template <Enum E, class NewFunc>
	constexpr auto case_(NewFunc&& func) const&& {
		return switcher<Enum, N, std::tuple<Funcs..., NewFunc>, Es..., E>{
			std::tuple_cat(std::move(_funcs.data()),
					std::make_tuple(std::forward<NewFunc>(func)))
		};
	}

	template <Enum E>
	constexpr auto empty() const&& {
		auto l = []() {};
		return switcher<Enum, N, std::tuple<Funcs..., decltype(l)>, Es..., E>{
			std::tuple_cat(std::move(_funcs.data()), std::make_tuple(l))
		};
	}

	constexpr void operator()(Enum e) const {
		static_assert(sizeof...(Es) == N,
				"safe_switch : missing enum case statement");

#if FEA_DEBUG
		constexpr std::array<Enum, N> arr{ Es... };
		auto it = std::find(arr.begin(), arr.end(), e);
		assert(it != arr.end());
#endif

		fea::static_for<N>([&, this](auto ic) {
			constexpr size_t e_idx = decltype(ic)::value;
			if (e_idx == size_t(e)) {
				return _funcs.template find<Enum(e_idx)>()();
			}
		});
	}


private:
	fea::type_map<fea::pack_nt<Es...>, Funcs...> _funcs;
};
} // namespace detail


// Won't compile if missing a case statement.
// Deduces the size if the enum contains a 'count' member.
template <class Enum, size_t N = size_t(Enum::count)>
constexpr auto safe_switch() {
	return detail::switcher<Enum, N, std::tuple<>>{};
}


} // namespace fea

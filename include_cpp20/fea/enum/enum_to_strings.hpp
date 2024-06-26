﻿/**
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
#include "fea/macros/literals.hpp"
#include "fea/macros/macros.hpp"
#include "fea/meta/return_overload.hpp"
#include "fea/utils/platform.hpp"

#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>

/*
FEA_STRING_ENUM creates to_string and from_string functions to convert your
enums to/from strings.

to_string is return overloaded and can return all std::Xstring_views and all
const std::Xstring&, except c++20 u8.

from_string is overloaded for all std::Xstring_views and const std::Xstring&,
except c++20 u8.

Call the macro using (enum_name, enum_underlying_type, your, enum, values, ...)
You must always provide an underlying_type.

Example :
FEA_STRING_ENUM(my_enum, unsigned, potato, tomato)

Generates :
inline ... to_string(my_enum);
inline my_enum from_string(...);

See unit tests for examples.
*/

// Used internally.
// Creates pairs of { "name", enum_type::name } for unordered_map.
#define FEA_DETAIL_LOOKUP_PAIR(name) { FEA_STRINGIFY(name), enum_type::name },
#define FEA_DETAIL_WLOOKUP_PAIR(name) { FEA_WSTRINGIFY(name), enum_type::name },
#define FEA_DETAIL_U16LOOKUP_PAIR(name) \
	{ FEA_U16STRINGIFY(name), enum_type::name },
#define FEA_DETAIL_U32LOOKUP_PAIR(name) \
	{ FEA_U32STRINGIFY(name), enum_type::name },
#define FEA_DETAIL_U8LOOKUP_PAIR(name) \
	{ FEA_U8STRINGIFY(name), enum_type::name },


// Declare an enum of type enum_t, with underlying type underlying_t.
// to_string and from_string are created for conversions.
// Example : FEA_STRING_ENUM(my_enum, int, potato, tomato)
#define FEA_STRING_ENUM(enum_t, underlying_t, ...) \
	/* Declare your enum. */ \
	enum class enum_t : underlying_t { __VA_ARGS__ }; \
	/* Unfortunately declare inline, cannot use static constexpr in func. */ \
	namespace fea_string_detail##enum_t { \
		inline constexpr size_t N = FEA_SIZEOF_VAARGS(__VA_ARGS__); \
		inline constexpr fea::enum_array<std::string_view, enum_t, N> sv_arr{ \
			FEA_FOR_EACH(FEA_STRINGIFY_COMMA, __VA_ARGS__) \
		}; \
		inline constexpr fea::enum_array<std::wstring_view, enum_t, N> \
				wsv_arr{ FEA_FOR_EACH(FEA_WSTRINGIFY_COMMA, __VA_ARGS__) }; \
		inline constexpr fea::enum_array<std::u16string_view, enum_t, N> \
				u16sv_arr{ FEA_FOR_EACH( \
						FEA_U16STRINGIFY_COMMA, __VA_ARGS__) }; \
		inline constexpr fea::enum_array<std::u32string_view, enum_t, N> \
				u32sv_arr{ FEA_FOR_EACH( \
						FEA_U32STRINGIFY_COMMA, __VA_ARGS__) }; \
		inline constexpr fea::enum_array<std::u8string_view, enum_t, N> \
				u8sv_arr{ FEA_FOR_EACH(FEA_U8STRINGIFY_COMMA, __VA_ARGS__) }; \
	} \
	/* Implement a seperate to_string_view in case you want consteval. */ \
	inline constexpr auto to_string_view(enum_t e__) noexcept { \
		using namespace fea_string_detail##enum_t; \
		return fea::return_overload{ \
			[e__]() -> std::string_view { return sv_arr[e__]; }, \
			[e__]() -> std::wstring_view { return wsv_arr[e__]; }, \
			[e__]() -> std::u16string_view { return u16sv_arr[e__]; }, \
			[e__]() -> std::u32string_view { return u32sv_arr[e__]; }, \
			[e__]() -> std::u8string_view { return u8sv_arr[e__]; }, \
		}; \
	} \
	/* Implement to_string for all supported string types. */ \
	inline auto to_string(enum_t e__) noexcept { \
		static constexpr size_t N = FEA_SIZEOF_VAARGS(__VA_ARGS__); \
		static const fea::enum_array<std::string, enum_t, N> str_arr{ \
			FEA_FOR_EACH(FEA_STRINGIFY_COMMA, __VA_ARGS__) \
		}; \
		static const fea::enum_array<std::wstring, enum_t, N> wstr_arr{ \
			FEA_FOR_EACH(FEA_WSTRINGIFY_COMMA, __VA_ARGS__) \
		}; \
		static const fea::enum_array<std::u16string, enum_t, N> u16str_arr{ \
			FEA_FOR_EACH(FEA_U16STRINGIFY_COMMA, __VA_ARGS__) \
		}; \
		static const fea::enum_array<std::u32string, enum_t, N> u32str_arr{ \
			FEA_FOR_EACH(FEA_U32STRINGIFY_COMMA, __VA_ARGS__) \
		}; \
		static const fea::enum_array<std::u8string, enum_t, N> u8str_arr{ \
			FEA_FOR_EACH(FEA_U8STRINGIFY_COMMA, __VA_ARGS__) \
		}; \
		return fea::return_overload{ \
			[e__]() -> std::string_view { return to_string_view(e__); }, \
			[e__]() -> std::wstring_view { return to_string_view(e__); }, \
			[e__]() -> std::u16string_view { return to_string_view(e__); }, \
			[e__]() -> std::u32string_view { return to_string_view(e__); }, \
			[e__]() -> std::u8string_view { return to_string_view(e__); }, \
			[e__]() -> const std::string& { return str_arr[e__]; }, \
			[e__]() -> const std::wstring& { return wstr_arr[e__]; }, \
			[e__]() -> const std::u16string& { return u16str_arr[e__]; }, \
			[e__]() -> const std::u32string& { return u32str_arr[e__]; }, \
			[e__]() -> const std::u8string& { return u8str_arr[e__]; }, \
		}; \
	} \
	/* Implement from_string for all supported string types. */ \
	namespace detail { \
	template <class StrT, class UMapT> \
	bool from_string(const UMapT& umap__, const StrT& s__, enum_t& out__) { \
		auto it = umap__.find(s__); \
		if (it == umap__.end()) { \
			return false; \
		} \
		out__ = it->second; \
		return true; \
	} \
	} \
	inline bool from_string(std::string_view s__, enum_t& out__) { \
		using enum_type = enum_t; \
		static const std::unordered_map<std::string_view, enum_t> lookup{ \
			FEA_FOR_EACH(FEA_DETAIL_LOOKUP_PAIR, __VA_ARGS__) \
		}; \
		return detail::from_string(lookup, s__, out__); \
	} \
	inline bool from_string(std::wstring_view s__, enum_t& out__) { \
		using enum_type = enum_t; \
		static const std::unordered_map<std::wstring_view, enum_t> lookup{ \
			FEA_FOR_EACH(FEA_DETAIL_WLOOKUP_PAIR, __VA_ARGS__) \
		}; \
		return detail::from_string(lookup, s__, out__); \
	} \
	inline bool from_string(std::u16string_view s__, enum_t& out__) { \
		using enum_type = enum_t; \
		static const std::unordered_map<std::u16string_view, enum_t> lookup{ \
			FEA_FOR_EACH(FEA_DETAIL_U16LOOKUP_PAIR, __VA_ARGS__) \
		}; \
		return detail::from_string(lookup, s__, out__); \
	} \
	inline bool from_string(std::u32string_view s__, enum_t& out__) { \
		using enum_type = enum_t; \
		static const std::unordered_map<std::u32string_view, enum_t> lookup{ \
			FEA_FOR_EACH(FEA_DETAIL_U32LOOKUP_PAIR, __VA_ARGS__) \
		}; \
		return detail::from_string(lookup, s__, out__); \
	} \
	inline bool from_string(std::u8string_view s__, enum_t& out__) { \
		using enum_type = enum_t; \
		static const std::unordered_map<std::u8string_view, enum_t> lookup{ \
			FEA_FOR_EACH(FEA_DETAIL_U8LOOKUP_PAIR, __VA_ARGS__) \
		}; \
		return detail::from_string(lookup, s__, out__); \
	} \
	inline bool from_string(const std::string& s__, enum_t& out__) { \
		using enum_type = enum_t; \
		static const std::unordered_map<std::string, enum_t> lookup{ \
			FEA_FOR_EACH(FEA_DETAIL_LOOKUP_PAIR, __VA_ARGS__) \
		}; \
		return detail::from_string(lookup, s__, out__); \
	} \
	inline bool from_string(const std::wstring& s__, enum_t& out__) { \
		using enum_type = enum_t; \
		static const std::unordered_map<std::wstring, enum_t> lookup{ \
			FEA_FOR_EACH(FEA_DETAIL_WLOOKUP_PAIR, __VA_ARGS__) \
		}; \
		return detail::from_string(lookup, s__, out__); \
	} \
	inline bool from_string(const std::u16string& s__, enum_t& out__) { \
		using enum_type = enum_t; \
		static const std::unordered_map<std::u16string, enum_t> lookup{ \
			FEA_FOR_EACH(FEA_DETAIL_U16LOOKUP_PAIR, __VA_ARGS__) \
		}; \
		return detail::from_string(lookup, s__, out__); \
	} \
	inline bool from_string(const std::u32string& s__, enum_t& out__) { \
		using enum_type = enum_t; \
		static const std::unordered_map<std::u32string, enum_t> lookup{ \
			FEA_FOR_EACH(FEA_DETAIL_U32LOOKUP_PAIR, __VA_ARGS__) \
		}; \
		return detail::from_string(lookup, s__, out__); \
	} \
	inline bool from_string(const std::u8string& s__, enum_t& out__) { \
		using enum_type = enum_t; \
		static const std::unordered_map<std::u8string, enum_t> lookup{ \
			FEA_FOR_EACH(FEA_DETAIL_U8LOOKUP_PAIR, __VA_ARGS__) \
		}; \
		return detail::from_string(lookup, s__, out__); \
	}

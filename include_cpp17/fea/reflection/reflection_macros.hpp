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
#include "fea/enum/enum_array.hpp"
#include "fea/meta/macros.hpp"
#include "fea/utils/platform.hpp"
#include "fea/utils/string.hpp"

#include <array>
#include <string>
#include <tuple>
#include <unordered_map>

#define FEA_REFL_ENAME var

#define FEA_REFL_STRINGIFY_MAP(x) { #x, FEA_REFL_ENAME::x },
#define FEA_REFL_WSTRINGIFY_MAP(x) { FEA_PASTE(L, #x), FEA_REFL_ENAME::x },
#define FEA_REFL_U16STRINGIFY_MAP(x) { FEA_PASTE(u, #x), FEA_REFL_ENAME::x },
#define FEA_REFL_U32STRINGIFY_MAP(x) { FEA_PASTE(U, #x), FEA_REFL_ENAME::x },

// Get the variables' name.
#define FEA_DETAIL_REFL_VARNAME_PRIV(prefix, ename, suffix) \
	FEA_PASTE(_, FEA_PASTE(ename, FEA_PASTE(_, FEA_PASTE(prefix, suffix))))

#define FEA_DETAIL_REFL_VARNAME(prefix, ename, suffix) \
	FEA_PASTE(ename, FEA_PASTE(_, FEA_PASTE(prefix, suffix)))

// Generates an enum_array of const char* const and one of std::string.
// The arrays are prefixed with the provided string type prefix.
#define FEA_DETAIL_REFL_ARRAY(chartype, prefix, ename, ...) \
protected: \
	/* The array of your strings, indexed at the enum position. */ \
	inline static const fea::enum_array<fea::string_t<chartype>, ename, \
			FEA_SIZEOF_VAARGS(__VA_ARGS__)> \
			FEA_DETAIL_REFL_VARNAME_PRIV(prefix, ename, names){ \
				{ FEA_FOR_EACH(FEA_STRINGIFY_COMMA, __VA_ARGS__) } \
			}; \
	/* A reverse-lookup map, to get the enum from a string in O(1). */ \
	inline static const std::unordered_map<fea::string_t<chartype>, ename> \
	FEA_DETAIL_REFL_VARNAME_PRIV(prefix, ename, reverse_lookup) { \
		FEA_FOR_EACH(FEA_REFL_STRINGIFY_MAP, __VA_ARGS__) \
	}


// Declares and implements helper functions.
// string<enum>(), to_string<enum::val>(), to_string(enum::val)
#define FEA_DETAIL_REFL_FUNCS(chartype, prefix, ename) \
public: \
	/* Returns the variable strings */ \
	[[maybe_unused]] static const auto& FEA_DETAIL_REFL_VARNAME( \
			prefix, ename, names)() { \
		return FEA_DETAIL_REFL_VARNAME_PRIV(prefix, ename, names); \
	} \
	/* Non-type compile-time getters. */ \
	template <ename E> \
	[[maybe_unused]] static const fea::string_t<chartype>& \
	FEA_DETAIL_REFL_VARNAME(prefix, ename, name)() { \
		return FEA_DETAIL_REFL_VARNAME_PRIV(prefix, ename, names).at<E>(); \
	} \
	/* Non-templated getters, fast O(1). */ \
	[[maybe_unused]] static const fea::string_t<chartype>& \
	FEA_DETAIL_REFL_VARNAME(prefix, ename, name)(ename e) { \
		return FEA_DETAIL_REFL_VARNAME_PRIV(prefix, ename, names)[e]; \
	} \
	/* Reverse lookup with string, slow O(1). */ \
	[[maybe_unused]] static ename FEA_DETAIL_REFL_VARNAME( \
			prefix, ename, enum)(const fea::string_t<chartype>& str) { \
		return FEA_DETAIL_REFL_VARNAME_PRIV(prefix, ename, reverse_lookup) \
				.at(str); \
	}

#define FEA_REFLECTION_VARNAMES(...) \
	/* Declares your enum. */ \
	enum class FEA_REFL_ENAME : uint16_t { __VA_ARGS__, count }; \
	/* char and std::string */ \
	FEA_DETAIL_REFL_ARRAY(char, , FEA_REFL_ENAME, __VA_ARGS__); \
	FEA_DETAIL_REFL_FUNCS(char, , FEA_REFL_ENAME)

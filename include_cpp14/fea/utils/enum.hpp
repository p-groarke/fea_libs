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
#include "fea/utils/string.hpp"

#include <string>
#include <vector>

/*
FEA_STRING_ENUM creates an accompanying vector of enum strings and some
accessors. It only works on enums that start at 0 to N!

Generates :
enum class ename : type {...};
const std::vector<std::string> ename_strings;
const std::vector<std::string>& enum_strings<ename>();
const std::string& to_string(ename e);

Plus, it generates all of these for all string types :
string, wstring, u16string, u32string.

Prepend "string" with your desired string type. For ex, u32.
const std::vector<std::u32string> ename_u32strings;
const std::vector<std::u32string>& enum_u32strings<ename>();
const std::u32string& to_u32string(ename e);
*/
// clang-format off
#define FEA_STRING_ENUM(ename, utype, ...) \
	template <class> \
	const std::vector<std::string>& enum_strings(); \
	template <class> \
	const std::vector<std::wstring>& enum_wstrings(); \
	template <class> \
	const std::vector<std::u16string>& enum_u16strings(); \
	template <class> \
	const std::vector<std::u32string>& enum_u32strings(); \
	\
	enum class ename : utype { __VA_ARGS__ }; \
	\
	namespace ename##_detail { \
		FEA_INLINE_VAR const std::string ename##_string = #__VA_ARGS__; \
		FEA_INLINE_VAR const std::wstring ename##_wstring = L#__VA_ARGS__; \
		FEA_INLINE_VAR const std::u16string ename##_u16string = u#__VA_ARGS__; \
		FEA_INLINE_VAR const std::u32string ename##_u32string = U#__VA_ARGS__; \
	} \
	\
	FEA_INLINE_VAR const std::vector<std::string> ename##_strings \
			= fea::split(ename##_detail::ename##_string, ", "); \
	FEA_INLINE_VAR const std::vector<std::wstring> ename##_wstrings \
			= fea::split(ename##_detail::ename##_wstring, L", "); \
	FEA_INLINE_VAR const std::vector<std::u16string> ename##_u16strings \
			= fea::split(ename##_detail::ename##_u16string, u", "); \
	FEA_INLINE_VAR const std::vector<std::u32string> ename##_u32strings \
			= fea::split(ename##_detail::ename##_u32string, U", "); \
	\
	template <> \
	inline const std::vector<std::string>& enum_strings<ename>() { \
		return ename##_strings; \
	} \
	template <> \
	inline const std::vector<std::wstring>& enum_wstrings<ename>() { \
		return ename##_wstrings; \
	} \
	template <> \
	inline const std::vector<std::u16string>& enum_u16strings<ename>() { \
		return ename##_u16strings; \
	} \
	template <> \
	inline const std::vector<std::u32string>& enum_u32strings<ename>() { \
		return ename##_u32strings; \
	} \
	\
	inline const std::string& to_string(ename e) { \
		return ename##_strings[size_t(e)]; \
	}\
	inline const std::wstring& to_wstring(ename e) { \
		return ename##_wstrings[size_t(e)]; \
	}\
	inline const std::u16string& to_u16string(ename e) { \
		return ename##_u16strings[size_t(e)]; \
	}\
	inline const std::u32string& to_u32string(ename e) { \
		return ename##_u32strings[size_t(e)]; \
	}
// clang-format on

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
#include "fea/maps/flat_unsigned_map.hpp"
#include "fea/string/conversions.hpp"
#include "fea/string/string.hpp"
#include "fea/utils/file.hpp"
#include "fea/utils/platform.hpp"

#include <cassert>
#include <cctype>
#include <charconv>
#include <cstdint>
#include <filesystem>
#include <format>
#include <fstream>
#include <limits>
#include <string>
#include <string_view>
#include <system_error>
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <variant>

/* ini.hpp internals */

namespace fea {
struct ini;
namespace detail {
namespace ini {
using entry_id_t = uint16_t;
using section_id_t = uint16_t;
using float_t = std::conditional_t<sizeof(intmax_t) == 8, double, float>;
using variant_t
		= std::variant<bool, intmax_t, float_t, std::string, std::nullptr_t>;

inline constexpr std::string_view general_help = R"(; INI Help
; An INI file stores user settings in a simple format.
; It consists of sections and entries (and sometimes comments).
; Entries are meant to be changed.
; Sections shouldn't be changed.
;
; Sections
;   Sections are used to organize the entries.
;   They are encapsulated by brackets [].
;   For example :
;     [This is a section]
;     [This.is.also.a.section]
;
; Comments
;   Lines that start with ';' are comments, like this one!
;   They are completely ignored.
;
; Entries
;   Entries hold values.
;   You set the values on the right side of the equal sign.
;   Do not change the name of the entry, only its value.
;   For example :
;     an_entry = 42
;     another_entry = "Another value"
;     never_change_this = "Change this."
;
; Values
;   Values can be either a boolean, a number, a decimal number or a string.
;
; Booleans
;   Booleans are either true or false.
;   They are on/off switches.
;   For example :
;     a_bool = true
;     another_bool = false
;
; Numbers
;   Numbers are exactly that, but they do not have a decimal point.
;   For example :
;     a_number = 42
;     another_number = -35
;     not_a_number = 1.0
;
; Decimal Numbers
;   Decimal numbers are numbers that contain decimal points.
;   For example :
;     a_decimal_number = 42.0
;     another_decimal_number = -35.55
;     not_a_decimal_number = 1
;
; Strings
;   Strings are sentences.
;   They are surrounded by either single or double quotes.
;   For exmple :
;     a_string = "I am a string."
;     another_string = 'I am also a string.'
)";

// https://www.cppstories.com/2021/heterogeneous-access-cpp20/
struct string_hash {
	using is_transparent = void;
	[[nodiscard]] size_t operator()(const char* txt) const {
		return std::hash<std::string_view>{}(txt);
	}
	[[nodiscard]] size_t operator()(std::string_view txt) const {
		return std::hash<std::string_view>{}(txt);
	}
	[[nodiscard]] size_t operator()(const std::string& txt) const {
		return std::hash<std::string>{}(txt);
	}
};

struct entry {
	// The entry name.
	std::string entry_name;

	// Optional entry comment.
	std::string comment;

	// The parsed value.
	variant_t value;
};

struct section {
	// Section name.
	std::string section_name;

	// Optional comment.
	std::string comment;

	// Used when generating entry ids.
	entry_id_t next_entry_id = entry_id_t(0);

	// Our entries.
	fea::flat_unsigned_map<entry_id_t, entry> entry_map;

	// Section id to section lookup of entry name to entry id.
	std::unordered_map<std::string, entry_id_t, string_hash, std::equal_to<>>
			entry_name_to_id;

	// Used to store invalid variant.
	static constexpr variant_t invalid_variant = std::nullptr_t{};
};

struct ini_data {
	// Our entries, sorted in order of appearance.
	fea::flat_unsigned_map<section_id_t, section> section_map;

	// Section name to section id.
	std::unordered_map<std::string, section_id_t, string_hash, std::equal_to<>>
			section_name_to_id;
};

inline std::string variant_to_helpstr(const entry& e) {
	constexpr std::string_view variable_help = "  ; Expects a {}.\n";

	if (std::holds_alternative<bool>(e.value)) {
		// return std::format(variable_help, e.entry_name, "boolean");
		return std::format(variable_help, "boolean");
	}
	if (std::holds_alternative<intmax_t>(e.value)) {
		// return std::format(variable_help, e.entry_name, "number");
		return std::format(variable_help, "number");
	}
	if (std::holds_alternative<float_t>(e.value)) {
		// return std::format(variable_help, e.entry_name, "decimal number");
		return std::format(variable_help, "decimal number");
	}
	if (std::holds_alternative<std::string>(e.value)) {
		// return std::format(variable_help, e.entry_name, "string");
		return std::format(variable_help, "string");
	}
	return "";
}

// Parse a string value to expected type.
[[nodiscard]] variant_t from_string(std::string_view str) {
	size_t single_idx = str.find('\'');
	size_t double_idx = str.find('"');
	if (single_idx != str.npos && double_idx == str.npos) {
		size_t single_end = str.find_last_of('\'');
		return variant_t{ std::string(
				str.begin() + single_idx + 1, str.begin() + single_end) };
	} else if (double_idx != str.npos && single_idx == str.npos) {
		size_t double_end = str.find_last_of('\"');
		return variant_t{ std::string(
				str.begin() + double_idx + 1, str.begin() + double_end) };
	} else if (single_idx != str.npos && double_idx != str.npos) {
		size_t begin = single_idx < double_idx ? single_idx : double_idx;
		size_t single_end = str.find_last_of('\'');
		size_t double_end = str.find_last_of('"');
		size_t end = single_end > double_end ? single_end : double_end;
		assert(end != str.npos);
		return variant_t{ std::string(
				str.begin() + begin + 1, str.begin() + end) };
	} else if (str == "true") {
		return variant_t{ true };
	} else if (str == "false") {
		return variant_t{ false };
	} else if (str.find('.') != str.npos) {
		float_t val = float_t(0);
		std::from_chars_result res
				= std::from_chars(str.data(), str.data() + str.size(), val);
		if (res.ec == std::errc{}) {
			return variant_t{ val };
		} else {
			return variant_t{ std::nullptr_t{} };
		}
	} else {
		intmax_t val = intmax_t(0);
		std::from_chars_result res
				= std::from_chars(str.data(), str.data() + str.size(), val);
		if (res.ec == std::errc{}) {
			return variant_t{ val };
		} else {
			return variant_t{ std::nullptr_t{} };
		}
	}
}

inline std::string to_string(const variant_t& v) {
	if (std::holds_alternative<bool>(v)) {
		return std::get<bool>(v) ? "true" : "false";
	}
	if (std::holds_alternative<intmax_t>(v)) {
		return std::to_string(std::get<intmax_t>(v));
	}
	if (std::holds_alternative<float_t>(v)) {
		std::string ret = std::to_string(std::get<float_t>(v));
		size_t idx = ret.find_last_not_of('0');
		if (idx != ret.npos) {
			if (ret[idx] == '.' && idx < ret.size() - 2) {
				// Keep 1 zero.
				ret.erase(idx + 2, ret.npos);
			} else {
				ret.erase(idx + 1, ret.npos);
			}
		}
		return ret;
	}
	if (std::holds_alternative<std::string>(v)) {
		return "\"" + std::get<std::string>(v) + "\"";
	}
	return "INTERNAL_ERROR";
}

inline std::string to_string(const entry& e, bool var_help) {
	constexpr std::string_view endline = "\n";
	constexpr std::string_view comment_fmt = "  ; {}\n";
	constexpr std::string_view val_fmt = "{} = {}\n";

	assert(!e.entry_name.empty());
	assert(!std::holds_alternative<std::nullptr_t>(e.value));

	std::string ret;
	if (!e.comment.empty()) {
		ret += std::format(comment_fmt, e.comment);
	}

	if (var_help) {
		ret += variant_to_helpstr(e);
	}

	ret += std::format(val_fmt, e.entry_name, to_string(e.value));
	return ret;
}

inline std::string to_string(const section& s, bool var_help) {
	constexpr std::string_view endline = "\n";
	constexpr std::string_view comment_fmt = "\n; {}";
	constexpr std::string_view s_fmt = "\n[{}]\n";

	std::string ret;
	if (!s.comment.empty()) {
		ret += std::format(comment_fmt, s.comment);
	}

	if (!s.section_name.empty()) {
		// Not global section.
		ret += std::format(s_fmt, s.section_name);
	}

	for (const entry& e : s.entry_map) {
		ret += to_string(e, var_help);
	}

	return ret;
}


// Return funkyness begins.
// Because we can!
template <class EntryT>
struct return_overload {
	return_overload(EntryT* mentry)
			: _entry(mentry)
			, _user_variant(std::nullptr_t{}) {
	}
	return_overload(variant_t&& mvariant)
			: _entry(nullptr)
			, _user_variant(std::move(mvariant)) {
	}

	template <class T>
	[[nodiscard]] static auto to_variant_type() {
		using m_t = std::decay_t<T>;
		if constexpr (std::is_same_v<m_t, bool>) {
			return bool{};
		} else if constexpr (std::is_integral_v<m_t>
				|| std::is_unsigned_v<m_t>) {
			return intmax_t{};
		} else if constexpr (std::is_floating_point_v<m_t>) {
			return float_t{};
		} else if constexpr (std::is_convertible_v<m_t, std::string>
				|| std::is_constructible_v<std::string, m_t>) {
			return std::string{};
		} else {
			return std::nullptr_t{};
		}
	}

	template <class ToT>
	[[nodiscard]] static bool can_cast(const variant_t& from) {
		if (std::is_same_v<ToT, std::string_view>) {
			// Cannot cast, needs back reference.
			return false;
		}

		switch (from.index()) {
		case 0: {
			using inner_t = std::decay_t<decltype(std::get<0>(from))>;
			return std::is_convertible_v<inner_t, ToT>
					|| std::is_constructible_v<ToT, inner_t>;
		} break;
		case 1: {
			using inner_t = std::decay_t<decltype(std::get<1>(from))>;
			return std::is_convertible_v<inner_t, ToT>
					|| std::is_constructible_v<ToT, inner_t>;
		} break;
		case 2: {
			using inner_t = std::decay_t<decltype(std::get<2>(from))>;
			return std::is_convertible_v<inner_t, ToT>
					|| std::is_constructible_v<ToT, inner_t>;
		} break;
		case 3: {
			using inner_t = std::decay_t<decltype(std::get<3>(from))>;
			static_assert(std::is_same_v<inner_t, std::string>,
					"can_cast : Need to update switch case.");

			return std::is_convertible_v<inner_t, ToT>
					|| std::is_constructible_v<ToT, inner_t>;
		} break;
		case 4: {
			using inner_t = std::decay_t<decltype(std::get<4>(from))>;
			static_assert(std::is_same_v<inner_t, std::nullptr_t>,
					"can_cast : Need to update switch case.");
			return false;
		} break;
		default: {
			assert(false);
			return false;
		} break;
		}
	}

	template <class ToT>
	[[nodiscard]] static bool can_convert(const variant_t& from) {
		if (std::is_same_v<ToT, std::string_view>) {
			// Cannot cast, needs back reference.
			return false;
		}

		if (std::is_same_v<ToT, std::string>) {
			return from.index() != 4;
		}

		switch (from.index()) {
		case 0:
			[[fallthrough]];
		case 1:
			[[fallthrough]];
		case 2: {
			return false;
		} break;
		case 3: {
			return true;
		} break;
		case 4: {
			return false;
		} break;
		default: {
			assert(false);
			return false;
		} break;
		}
	}

	template <class ToT>
	[[nodiscard]] static ToT cast(const variant_t& from) {
		switch (from.index()) {
		case 0: {
			using inner_t = std::decay_t<decltype(std::get<0>(from))>;
			if constexpr (std::is_convertible_v<inner_t, ToT>
					|| std::is_constructible_v<ToT, inner_t>) {
				return ToT(std::get<0>(from));
			}
		} break;
		case 1: {
			using inner_t = std::decay_t<decltype(std::get<1>(from))>;
			if constexpr (std::is_convertible_v<inner_t, ToT>
					|| std::is_constructible_v<ToT, inner_t>) {
				return ToT(std::get<1>(from));
			}
		} break;
		case 2: {
			using inner_t = std::decay_t<decltype(std::get<2>(from))>;
			if constexpr (std::is_convertible_v<inner_t, ToT>
					|| std::is_constructible_v<ToT, inner_t>) {
				return ToT(std::get<2>(from));
			}
		} break;
		case 3: {
			using inner_t = std::decay_t<decltype(std::get<3>(from))>;
			if constexpr (std::is_convertible_v<inner_t, ToT>
					|| std::is_constructible_v<ToT, inner_t>) {
				return ToT(std::get<3>(from));
			}
		} break;
		case 4: {
			using inner_t = std::decay_t<decltype(std::get<4>(from))>;
			if constexpr (std::is_convertible_v<inner_t, ToT>
					|| std::is_constructible_v<ToT, inner_t>) {
				return ToT(std::get<4>(from));
			}
		} break;
		default: {
			assert(false);
		} break;
		}
		return ToT{};
	}

	template <class ToT>
	[[nodiscard]] static ToT convert(const variant_t& from) {
		if constexpr (std::is_same_v<ToT, std::string>) {
			return to_string(from);
		} else {
			variant_t new_v = from_string(std::get<std::string>(from));
			assert(can_cast<ToT>(new_v));
			return cast<ToT>(new_v);
		}
	}

	template <class T>
	[[nodiscard]] static variant_t make_variant(T&& t) {
		using m_t = std::decay_t<T>;
		using inner_t = decltype(to_variant_type<T>());
		return variant_t{ inner_t(std::forward<T>(t)) };
	}

	// User provided default.
	template <class T>
	[[nodiscard]] return_overload operator|(const T& t) const&& {
		if (_entry != nullptr
				&& !(_entry->value.valueless_by_exception()
						|| std::holds_alternative<std::nullptr_t>(
								_entry->value))) {
			// We contain a valid value, ignore user default.
			return *this;
		}

		// Return temporary pointing to user default.
		return return_overload{ make_variant(t) };
	}

	// On non-const access, assign default if entry is invalid.
	template <class T>
	[[nodiscard]] return_overload operator|(const T& t) && {
		if (_entry == nullptr) {
			// Return temporary pointing to user default.
			return return_overload{ make_variant(t) };
		}

		if (_entry->value.valueless_by_exception()
				|| std::holds_alternative<std::nullptr_t>(_entry->value)) {
			// We contain an invalid value, set user default.
			return this->operator=(t);
		}

		using user_t = decltype(to_variant_type<T>());
		if (!std::holds_alternative<user_t>(_entry->value)) {
			// Mismatch between held type and user provided type.
			if (can_cast<user_t>(_entry->value)) {
				// If possible, cast to user demanded type.
				_entry->value = cast<user_t>(_entry->value);
				return *this;
			}

			if (can_convert<user_t>(_entry->value)) {
				// Try to convert (to / from string).
				_entry->value = convert<user_t>(_entry->value);
				return *this;
			}
		}

		// We contain a value of expected type, ignore user default.
		return *this;
	}

	// User assignement.
	template <class T>
	return_overload& operator=(T&& t) {
		assert(_entry != nullptr);
		_entry->value = make_variant<T>(std::forward<T>(t));
		return *this;
	}

	return_overload& operator,(std::string_view comment) {
		assert(_entry != nullptr);
		_entry->comment = comment;
		return *this;
	}

	[[nodiscard]] operator bool() const&& {
		return doit<bool>();
	}

	[[nodiscard]] operator signed char() const&& {
		return (signed char)(doit<intmax_t>());
	}
	[[nodiscard]] operator unsigned char() const&& {
		return (unsigned char)(doit<intmax_t>());
	}

	[[nodiscard]] operator short() const&& {
		return short(doit<intmax_t>());
	}
	[[nodiscard]] operator unsigned short() const&& {
		return (unsigned short)(doit<intmax_t>());
	}

	[[nodiscard]] operator int() const&& {
		return int(doit<intmax_t>());
	}
	[[nodiscard]] operator unsigned() const&& {
		return unsigned(doit<intmax_t>());
	}

	[[nodiscard]] operator float() const&& {
		return float(doit<float_t>());
	}

#if FEA_64BIT
	[[nodiscard]] operator long long() const&& {
		return (long long)(doit<intmax_t>());
	}

	[[nodiscard]] operator unsigned long long() const&& {
		return (unsigned long long)(doit<intmax_t>());
	}

	[[nodiscard]] operator double() const&& {
		return doit<float_t>();
	}
#endif

	[[nodiscard]] operator std::string() const&& {
		return doit<std::string>();
	}

	// No can do because of casting / conversions.
	//[[nodiscard]] operator std::string_view() const&& {
	//	const std::string& str = doit<std::string>();
	//	return std::string_view(str.begin(), str.end());
	//}
	//[[nodiscard]] operator const char*() const&& {
	//	const std::string& str = doit<std::string>();
	//	return str.c_str();
	//}

	template <class T>
	[[nodiscard]] bool is_valid(const variant_t& mvariant) const {
		return !mvariant.valueless_by_exception()
				&& !std::holds_alternative<std::nullptr_t>(mvariant)
				&& std::holds_alternative<T>(mvariant);
	}

	template <class T>
	[[nodiscard]] T doit() const {
		if (_entry != nullptr) {
			// Try as best we can to return stored value.
			if (is_valid<T>(_entry->value)) {
				// Everythin gucci.
				return std::get<T>(_entry->value);
			}

			if (can_cast<T>(_entry->value)) {
				// Will be re-cast in conversion function.
				return cast<T>(_entry->value);
			}

			if (can_convert<T>(_entry->value)) {
				// Try to convert (to / from string).
				return convert<T>(_entry->value);
			}
		}

		if (is_valid<T>(_user_variant)) {
			// Invalid entry variant, defer to user provided default.
			return std::get<T>(_user_variant);
		}

		// No default provided and value doesn't exist or couldn't be
		// parsed or isn't correct type.
		return std::get<T>(default_vals);
	}

	template <class T>
	[[nodiscard]] auto& doit() {
		assert(_entry != nullptr);
		_entry->value = T{};
		return std::get<T>(_entry->value);
	}

	EntryT* _entry = nullptr;
	variant_t _user_variant = std::nullptr_t{};

	inline static const std::tuple<bool, intmax_t, float_t, std::string>
			default_vals{};
};

template <class SectionT>
struct section_ret {
	[[nodiscard]] return_overload<const entry> operator[](
			std::string_view entry_name) const&& {
		if (s == nullptr) {
			return { &s->invalid_variant };
		}

		auto eid_it = s->entry_name_to_id.find(entry_name);
		if (eid_it == s->entry_name_to_id.end()) {
			return { &s->invalid_variant };
		}
		return return_overload{ &s->entry_map.at(eid_it->second) };
	}

	[[nodiscard]] return_overload<entry> operator[](
			std::string_view entry_name) && {
		assert(s != nullptr);

		auto eid_it = s->entry_name_to_id.find(entry_name);
		if (eid_it == s->entry_name_to_id.end()) {
			entry_id_t new_id = s->next_entry_id++;
			eid_it = s->entry_name_to_id
							 .insert({ std::string{ entry_name }, new_id })
							 .first;
			s->entry_map.insert(new_id,
					entry{
							.entry_name = std::string{ entry_name },
							//.str_value = "",
							.comment = "",
							.value = std::nullptr_t{},
					});
		}
		return return_overload{ &s->entry_map.at(eid_it->second) };
	}

	void operator,(std::string_view comment) && {
		s->comment = comment;
	}

	SectionT* s = nullptr;
};

// Sanitize user text, both from a security perspective and ini perspective.
[[nodiscard]] std::string sanitize(const std::u32string& text) {
	std::u32string sanitized;
	sanitized.reserve(text.size());

	// constexpr std::u32string_view allowed_ascii_extra = U"=.!?_-[];'\" \t\v";

	// Character is allowed.
	auto is_allowed = [&](char32_t c) {
		// In unicode range, allow anything.
		if (c > 127u) {
			return true;
		}
		return bool(std::isprint(c)) || bool(std::isspace(c));
	};

	// Character is a space.
	static constexpr std::u32string_view whitespace_chars = U" \t\n\v\f\r";
	auto is_space = [](char32_t c) {
		if (c > 127u) {
			return false;
		}
		return whitespace_chars.find(c) != whitespace_chars.npos;
	};

	// Clean start and end, remove comments.
	auto trim = [](std::u32string_view& line) {
		line = fea::trim_leading(line, whitespace_chars.data());
		line = fea::trim_trailing(line, whitespace_chars.data());
		size_t comment_idx = line.find(U';');
		if (comment_idx == line.npos) {
			return;
		}
		if (comment_idx == 0) {
			line = U"";
			return;
		}
		line = std::u32string_view(line.begin(), line.begin() + comment_idx);
		line = fea::trim_trailing(line, whitespace_chars.data());
	};

	// Remove anything before or after section brackets.
	auto clean_outer_sections = [](std::u32string_view& line,
										size_t& section_begin,
										size_t& section_end) {
		section_begin = line.find(U'[');
		if (section_begin != line.npos && section_begin != 0) {
			line = std::u32string_view(
					line.begin() + section_begin, line.end());
			section_begin = 0;
		}

		section_end = line.find_last_of(U']');
		if (section_end != line.npos && section_end < line.size() - 1) {
			line = std::u32string_view(
					line.begin(), line.begin() + section_end + 1);
		}
	};

	// Remove completely malformed lines.
	auto clean_unsaveable = [](size_t section_begin, size_t section_end,
									std::u32string_view& line) {
		size_t equal_idx = line.find(U'=');
		if (section_begin == line.npos && section_end == line.npos
				&& equal_idx == line.npos) {
			line = U"";
		}
	};

	fea::for_each_line(text, [&](std::u32string_view line) {
		trim(line);
		if (line.empty()) {
			return;
		}

		size_t section_begin = line.npos;
		size_t section_end = line.npos;
		clean_outer_sections(line, section_begin, section_end);

		clean_unsaveable(section_begin, section_end, line);
		if (line.empty()) {
			return;
		}

		std::u32string str;
		str.reserve(line.size() + 1);

		// Add missing section brackets.
		if (section_begin == line.npos && section_end != line.npos) {
			// Missing start [
			str.push_back(U'[');
			section_begin = 0;
		}

		// Will remove all spaces except those inside string literals.
		size_t single_lit_idx = 0;
		size_t double_lit_idx = 0;

		// Will also clean brackets other than outer brackets.
		// Inner brackets are changed to '.', for example :
		// [test][test][test] -> [test.test.test]

		for (size_t i = 0; i < line.size(); ++i) {
			char32_t c = line[i];
			if (!is_allowed(c)) {
				continue;
			}

			// Pretty dumb but good enough for sanitization.
			if (c == U'\'') {
				if (single_lit_idx != 0) {
					single_lit_idx = 0;
				} else {
					single_lit_idx = i;
				}
			}
			if (c == U'"') {
				if (double_lit_idx != 0) {
					double_lit_idx = 0;
				} else {
					double_lit_idx = i;
				}
			}
			if (!(single_lit_idx || double_lit_idx) && is_space(c)) {
				// if (single_lit_idx || double_lit_idx) {
				//	str.push_back(U' ');
				// }
				continue;
			}

			// Convert inner sections to dot.
			if (c == U'[' && i != section_begin) {
				if (str.back() == U'.' || str.back() == U'[') {
					continue;
				}
				str.push_back(U'.');
				continue;
			}
			if (c == U']' && i != section_end) {
				if (str.back() == U'.') {
					continue;
				}
				str.push_back(U'.');
				continue;
			}

			// Could happen with really malformed sections.
			if (c == U']' && i == section_end && str.back() == U'.') {
				str.back() = U']';
				continue;
			}

			str.push_back(c);
		}

		if (str.empty()) {
			return;
		}

		// Unclosed quotes.
		if (single_lit_idx != 0 || double_lit_idx != 0) {
			if (single_lit_idx != 0 && double_lit_idx == 0) {
				str.push_back(U'\'');
			} else if (double_lit_idx != 0 && single_lit_idx == 0) {
				str.push_back(U'"');
			} else if (single_lit_idx > double_lit_idx) {
				str.push_back(U'\'');
				str.push_back(U'"');
			} else {
				str.push_back(U'"');
				str.push_back(U'\'');
			}
		}

		if (section_begin != line.npos && section_end == line.npos) {
			// Missing end ]
			if (str.back() == U'.') {
				// Could happen with really malformed sections.
				str.back() = U']';
			} else {
				str.push_back(U']');
			}
		}

		str.push_back(U'\n');
		sanitized.insert(sanitized.end(), str.begin(), str.end());
	});

	return fea::utf32_to_utf8(sanitized);
}

// Read data, returns utf8 string.
[[nodiscard]] std::string read_data(const std::filesystem::path& filepath) {
	std::ifstream ifs{ filepath };
	if (!ifs.is_open()) {
		return {};
	}

	// Open any of ascii or unicode (utfN).
	std::u32string text = fea::open_text_file_with_bom(ifs);

	// Clean it as best we can.
	return sanitize(text);
}


// Parse data and fill map.
[[nodiscard]] ini_data make_data(
		const std::string& data, section_id_t& next_section_id) {
	if (data.empty()) {
		return {};
	}

	ini_data ret{};
	next_section_id = section_id_t(0);
	section_id_t current_section_id
			= (std::numeric_limits<section_id_t>::max)();

	// Keep track of current section to minimize lookups.
	section* current_section = nullptr;

	auto add_section = [&](std::string_view name) {
		current_section_id = next_section_id++;
		ret.section_name_to_id.insert(
				{ std::string{ name }, current_section_id });
		auto p = ret.section_map.insert(current_section_id,
				section{
						.section_name = std::string{ name },
				});
		current_section = &(*p.first);
	};

	// Prime it to allow unsectioned global entries.
	add_section("");

	fea::for_each_line(data, [&](std::string_view line) {
		if (line.front() == '[') {
			// Potentially new section.
			assert(line.find(']') == line.size() - 1);
			line = std::string_view{ line.data() + 1,
				line.data() + line.size() - 1 };

			auto it = ret.section_name_to_id.find(line);
			if (it != ret.section_name_to_id.end()) {
				// Existing section, merge.
				current_section_id = it->second;
				current_section = &ret.section_map.at(current_section_id);
				return;
			} else {
				// New section.
				add_section(line);
			}
		} else {
			// Potentially new entry.
			size_t equal_idx = line.find('=');
			assert(equal_idx != line.npos);
			std::string_view entry_name
					= std::string_view(line.begin(), line.begin() + equal_idx);
			if (current_section->entry_name_to_id.contains(entry_name)) {
				// Nothing to do, skip duplicate.
				return;
			}

			std::string_view str_value = std::string_view(
					line.begin() + equal_idx + 1, line.end());

			variant_t value = from_string(str_value);
			entry_id_t new_entry_id = current_section->next_entry_id++;
			current_section->entry_name_to_id.insert({
					std::string{ entry_name },
					new_entry_id,
			});
			current_section->entry_map.insert(new_entry_id,
					entry{
							.entry_name = std::string{ entry_name },
							//.str_value = std::string{ str_value },
							.comment = "",
							.value = value,
					});
		}
	});

	return ret;
}
} // namespace ini
} // namespace detail
} // namespace fea

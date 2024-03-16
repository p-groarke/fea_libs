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
#include <fstream>
#include <limits>
#include <string>
#include <string_view>
#include <system_error>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>

/*
A simple ini parser.

Entries are ordered by declaration order, not sorted alphabetically. Supports
output comments. Supports automatic comments.
*/

namespace fea {
struct basic_fini_read;
struct basic_fini_write;
namespace detail {
namespace fini {
using entry_id_t = uint16_t;
using section_id_t = uint16_t;
using float_t = std::conditional_t<sizeof(intmax_t) == 8, double, float>;
using variant_t = std::variant<bool, intmax_t, float_t, std::nullptr_t,
		std::string_view>;

struct entry {
	std::string_view entry_name;
	std::string_view str_value;
	variant_t value;
};

struct section {
	// Section name.
	std::string_view section_name;

	// Used when generating entry ids.
	entry_id_t next_entry_id = entry_id_t(0);

	// Our entries.
	fea::flat_unsigned_map<entry_id_t, entry> entry_map;

	// Section id to section lookup of entry name to entry id.
	std::unordered_map<std::string_view, entry_id_t> entry_name_to_id;
};

struct ini_data {
	// Our entries, sorted in order of appearance.
	fea::flat_unsigned_map<section_id_t, section> section_map;

	// Section name to section id.
	std::unordered_map<std::string_view, section_id_t> section_name_to_id;
};

// Return funkyness begins.
// Because we can!
struct return_overload {

	template <class T>
	return_overload operator|(const T& t) const&& {
		if (!std::holds_alternative<std::nullptr_t>(val)) {
			// We contain a valid value, return it.
			return *this;
		}

		// Replace the bad value.
		return return_overload{ variant_t{ t } };
	}

	operator bool() const&& {
		return doit<bool>();
	}

	operator signed char() const&& {
		return (signed char)(doit<intmax_t>());
	}
	operator unsigned char() const&& {
		return (unsigned char)(doit<intmax_t>());
	}

	operator short() const&& {
		return short(doit<intmax_t>());
	}
	operator unsigned short() const&& {
		return (unsigned short)(doit<intmax_t>());
	}

	operator int() const&& {
		return int(doit<intmax_t>());
	}
	operator unsigned() const&& {
		return unsigned(doit<intmax_t>());
	}

	operator float() const&& {
		return float(doit<float_t>());
	}

#if FEA_64BIT
	operator long long() const&& {
		return (long long)(doit<intmax_t>());
	}

	operator unsigned long long() const&& {
		return (unsigned long long)(doit<intmax_t>());
	}

	operator double() const&& {
		return doit<float_t>();
	}
#endif

	operator std::string() const&& {
		return std::string(doit<std::string_view>());
	}

	operator std::string_view() const&& {
		return doit<std::string_view>();
	}

	template <class T>
	auto doit() const {
		if (val.valueless_by_exception()
				|| std::holds_alternative<std::nullptr_t>(val)
				|| !std::holds_alternative<T>(val)) {
			// No default provided and value doesn't exist or couldn't be
			// parsed or isn't correct type.
			return T{};
		}
		return std::get<T>(val);
	}

	// template <class T>
	// operator T() const&& {
	//	using m_t = std::decay_t<T>;

	//	if (val.valueless_by_exception()) {
	//		// No default provided and value doesn't exist.
	//		return m_t{};
	//	}

	//	if (std::holds_alternative<std::nullptr_t>(val)) {
	//		// No default provided and value couldn't be parsed.
	//		return m_t{};
	//	}

	//	if constexpr (std::is_same_v<m_t, bool>) {
	//		if (!std::holds_alternative<bool>(val)) {
	//			return m_t{};
	//		} else {
	//			return std::get<bool>(val);
	//		}
	//	} else if constexpr (std::is_integral_v<m_t>) {
	//		if (!std::holds_alternative<intmax_t>(val)) {
	//			return m_t{};
	//		} else {
	//			return m_t(std::get<intmax_t>(val));
	//		}
	//	} else if constexpr (std::is_floating_point_v<m_t>) {
	//		if (!std::holds_alternative<float_t>(val)) {
	//			return m_t{};
	//		} else {
	//			return m_t(std::get<float_t>(val));
	//		}
	//	} else if constexpr (std::is_convertible_v<std::string_view, m_t>
	//			|| std::is_constructible_v<m_t, std::string_view>) {
	//		if (!std::holds_alternative<std::string_view>(val)) {
	//			return m_t{};
	//		} else {
	//			return m_t(std::get<std::string_view>(val));
	//		}
	//	} else {
	//		return m_t{};
	//	}
	//}

	variant_t val = std::nullptr_t{};
};

struct section_ret {
	[[nodiscard]] return_overload operator[](
			std::string_view entry_name) const&& {
		if (s == nullptr) {
			return {};
		}

		auto eid_it = s->entry_name_to_id.find(entry_name);
		if (eid_it == s->entry_name_to_id.end()) {
			return {};
		}
		return return_overload{ s->entry_map.at(eid_it->second).value };
	}
	const detail::fini::section* s = nullptr;
};

// Parse a string value to expected type.
[[nodiscard]] variant_t parse_value(std::string_view str) {
	size_t single_idx = str.find('\'');
	size_t double_idx = str.find('"');
	if (single_idx != str.npos && double_idx == str.npos) {
		size_t single_end = str.find_last_of('\'');
		return variant_t{ std::string_view(
				str.begin() + single_idx + 1, str.begin() + single_end) };
	} else if (double_idx != str.npos && single_idx == str.npos) {
		size_t double_end = str.find_last_of('\"');
		return variant_t{ std::string_view(
				str.begin() + double_idx + 1, str.begin() + double_end) };
	} else if (single_idx != str.npos && double_idx != str.npos) {
		size_t begin = single_idx < double_idx ? single_idx : double_idx;
		size_t single_end = str.find_last_of('\'');
		size_t double_end = str.find_last_of('"');
		size_t end = single_end > double_end ? single_end : double_end;
		assert(end != str.npos);
		return variant_t{ std::string_view(
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
[[nodiscard]] ini_data make_data(const std::string& data) {
	if (data.empty()) {
		return {};
	}

	ini_data ret{};
	section_id_t next_section_id = section_id_t(0);
	section_id_t current_section_id
			= (std::numeric_limits<section_id_t>::max)();

	// Keep track of current section to minimize lookups.
	section* current_section = nullptr;

	auto add_section = [&](std::string_view name) {
		current_section_id = next_section_id++;
		ret.section_name_to_id.insert({ name, current_section_id });
		auto p = ret.section_map.insert(current_section_id,
				section{
						.section_name = name,
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

			variant_t value = parse_value(str_value);
			entry_id_t new_entry_id = current_section->next_entry_id++;
			current_section->entry_name_to_id.insert({
					entry_name,
					new_entry_id,
			});
			current_section->entry_map.insert(new_entry_id,
					entry{
							.entry_name = entry_name,
							.str_value = str_value,
							.value = value,
					});
		}
	});

	return ret;
}
} // namespace fini
} // namespace detail

struct basic_fini_read {
	using section_id_t = detail::fini::section_id_t;
	using entry_id_t = detail::fini::entry_id_t;
	using entry_t = detail::fini::entry;
	using variant_t = detail::fini::variant_t;

	basic_fini_read(const std::filesystem::path& filepath)
			: _string_data(detail::fini::read_data(filepath))
			, _ini_data(detail::fini::make_data(_string_data)) {
	}
	basic_fini_read(const std::string& data)
			: _string_data(detail::fini::sanitize(fea::utf8_to_utf32(data)))
			, _ini_data(detail::fini::make_data(_string_data)) {
	}
	basic_fini_read(std::string_view data)
			: basic_fini_read(std::string{ data }) {
	}

	// File was opened succesfully / data was parsed and we contain data.
	[[nodiscard]] bool is_open() const noexcept {
		return !_string_data.empty();
	}

	// Does the ini file contain a specific section.
	[[nodiscard]] bool contains(std::string_view section_name) const {
		return _ini_data.section_name_to_id.contains(section_name);
	}

	// Does the ini file contain a specific entry in that section.
	[[nodiscard]] bool contains(
			std::string_view section_name, std::string_view entry_name) const {
		auto it = _ini_data.section_name_to_id.find(section_name);
		if (it == _ini_data.section_name_to_id.end()) {
			return false;
		}
		return _ini_data.section_map.at(it->second)
				.entry_name_to_id.contains(entry_name);
	}

	// Unlike usual std operator[], doesn't create values on read-only ini.
	// Non-throwing.
	// On failure, either returns the type's default value or your provided
	// default value. Provide a default value using tortellini-style operator|
	// Because pasta is good.
	// For example :
	// bool b = ini["bla"]["bla"] | true;
	[[nodiscard]] detail::fini::section_ret operator[](
			std::string_view section_name) const {
		auto sid_it = _ini_data.section_name_to_id.find(section_name);
		if (sid_it == _ini_data.section_name_to_id.end()) {
			return {};
		}
		return detail::fini::section_ret{ &_ini_data.section_map.at(
				sid_it->second) };
	}

	//[[nodiscard]] detail::fini::return_overload operator[](
	//		std::string_view section_name, std::string_view entry_name) const {

	//	const detail::fini::section& s
	//			= _ini_data.section_map.at(sid_it->second);
	//	auto eid_it = s.entry_name_to_id.find(entry_name);
	//	if (eid_it == s.entry_name_to_id.end()) {
	//		return detail::fini::return_overload{};
	//	}

	//	return {};
	//}

private:
	// Our loaded text data.
	const std::string _string_data;

	// The maps of data.
	const detail::fini::ini_data _ini_data;
};

struct basic_fini_write {};


// Friendly aliases.
using ifini = basic_fini_read;
using ofini = basic_fini_write;
using fini = basic_fini_write;

} // namespace fea

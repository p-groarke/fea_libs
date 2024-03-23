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
#include "fea/serialize/ini_details.hpp"

#include <filesystem>
#include <fstream>
#include <string>
#include <string_view>

/*
A simple ini parser.

TODO : Deal with const better : |= etc.

Entries are ordered by declaration order, not sorted alphabetically. Supports
output comments. Supports automatic comments. When parsing an ini file, comments
are dropped.

Since ini files are expected to be edited by users, this format tries everything
in its power not to fail or throw.

Reading
fea::ini uses return type overloads to read values. It uses tortellini style
operator| for read default values.
WARNING : The type provided by operator| is assumed to be the expected type.
Conversions may occur on incorrect user data.

Writing
When writing, use operator ',' to add comments.

Supported types :
- bool (true, false)
- int (intmax_t)
- floats (floatmax_t)
- std::string

Example :
fea::ini f{ "file.ini" };
bool b = f["section"]["a_bool"] | default_bool_val;
f["section"]["an_int"] = 42, "an_int comment";
f.write();
*/

namespace fea {
struct ini {
	using section_id_t = detail::ini::section_id_t;
	using entry_id_t = detail::ini::entry_id_t;
	using entry_t = detail::ini::entry;
	using section_t = detail::ini::section;
	using variant_t = detail::ini::variant_t;

	ini(const std::filesystem::path& filepath)
			: _next_section_id(0)
			, _ini_data(detail::ini::make_data(
					  detail::ini::read_data(filepath), _next_section_id))
			, _filepath(filepath) {
	}
	ini(const std::string& data)
			: _next_section_id(0)
			, _ini_data(detail::ini::make_data(
					  detail::ini::sanitize(fea::utf8_to_utf32(data)),
					  _next_section_id)) {
	}
	ini(std::string_view data)
			: ini(std::string{ data }) {
	}

	// File was opened succesfully / data was parsed and we contain data.
	//[[nodiscard]] bool is_open() const noexcept {
	//	return !_string_data.empty();
	//}

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

	// Reads ini values.
	// Non-throwing.
	// On failure, either returns the type's default value or your provided
	// default value. Provide a default value using tortellini-style operator|
	// Because pasta is good.
	// For example :
	// bool b = ini["bla"]["bla"] | true;
	[[nodiscard]] detail::ini::section_ret<const detail::ini::section>
	operator[](std::string_view section_name) const {
		auto sid_it = _ini_data.section_name_to_id.find(section_name);
		if (sid_it == _ini_data.section_name_to_id.end()) {
			return {};
		}
		return detail::ini::section_ret{ &_ini_data.section_map.at(
				sid_it->second) };
	}

	// Write ini values.
	// Non-throwing.
	// Missing sections or entries are created on demand.
	// Use operator, to add comments.
	detail::ini::section_ret<detail::ini::section> operator[](
			std::string_view section_name) {
		auto sid_it = _ini_data.section_name_to_id.find(section_name);
		if (sid_it == _ini_data.section_name_to_id.end()) {
			section_id_t new_id = _next_section_id++;
			sid_it = _ini_data.section_name_to_id
							 .insert({ std::string{ section_name }, new_id })
							 .first;
			_ini_data.section_map.insert(new_id,
					detail::ini::section{
							.section_name = std::string{ section_name },
					});
		}
		return detail::ini::section_ret{ &_ini_data.section_map.at(
				sid_it->second) };
	}

	// Writes to file provided in constructor.
	void write() const {
		assert(!_filepath.empty());
		std::ofstream ofs{ _filepath };
		if (!ofs.is_open()) {
			return;
		}
		ofs << to_string(*this);
	}

	// Writes to file.
	void write(const std::filesystem::path& filepath) const {
		assert(!filepath.empty());
		std::ofstream ofs{ filepath };
		if (!ofs.is_open()) {
			return;
		}
		ofs << to_string(*this);
	}

	// Prints a generalized help at the top of the INI file,
	// for users who aren't familiar with the format.
	void general_help(bool b) noexcept {
		_print_general_help = b;
	}

	// For each variable, prints a comment denoting what data type it expects.
	// Has no effect on user comments, which are always output if they have been
	// set.
	void variable_help(bool b) noexcept {
		_print_var_help = b;
	}

private:
	friend std::string to_string(const ini&);

	// Tracks section ids.
	section_id_t _next_section_id = section_id_t(0);

	// The maps of data.
	detail::ini::ini_data _ini_data;

	// Opened with file.
	std::filesystem::path _filepath;

	// Prints a generalized help message at the top of the ini file.
	bool _print_general_help = true;

	// Prints a help comment for each variable, explaining their type.
	bool _print_var_help = false;
};

inline std::string to_string(const ini& mini) {
	std::string ret;

	// TODO : loop and find approximate reserve size.

	if (mini._print_general_help) {
		ret += detail::ini::general_help;
	}

	for (const detail::ini::section& s : mini._ini_data.section_map) {
		ret += detail::ini::to_string(s, mini._print_var_help);
	}
	return ret;
}

} // namespace fea

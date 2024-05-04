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
#include "fea/utils/platform.hpp"

#if !FEA_MACOS // Missing std::from_chars float.
#include "fea/containers/unsigned_hole_hashmap.hpp"
#include "fea/string/string.hpp"
#include "fea/string/string_literal.hpp"
#include "fea/utils/file.hpp"
#include "fea/utils/throw.hpp"

#include <cassert>
#include <charconv>
#include <filesystem>
#include <fstream>
#include <source_location>

/*
Tweak Values are constant values which can be updated and reloaded at runtime.

In release, the macro simply pastes the value.
In debug, the macro checks if the current source file has been modified.
If so, it parses the source file and updates the value.

The first I heard of this was from Joel David.
*/

#if FEA_RELEASE
#define FEA_TWEAK(val) val
#else
// Must use __FILE__ macro to get full filepath.
#define FEA_TWEAK(val) \
	fea::detail::tweak_value<(fea::detail::src_stamp{ \
			__FILE__, std::source_location::current() })>(val)
#endif

namespace fea {
namespace detail {
template <size_t N>
struct src_stamp {
	consteval src_stamp(const char (&path)[N], std::source_location loc)
			: file_path(path)
			, file_hash(file_path.hash())
			, line(loc.line())
			, counter(loc.column()) {
	}

	fea::string_literal<N> file_path;
	size_t file_hash = 0;
	uint32_t line = 0;
	uint32_t counter = 0;
};

struct tweak_file {
	void load_data() {
		data.clear();
		std::ifstream ifs{ file_path };
		if (!ifs.is_open()) {
			assert(false);
			return;
		}
		std::string blob = fea::any_to_utf8(fea::open_text_file_with_bom(ifs));
		fea::for_each_line(blob,
				[this](std::string_view l) { data.push_back(std::string(l)); });
	}

	bool needs_update = false;
	std::filesystem::file_time_type last_modified{};
	std::filesystem::path file_path;
	std::vector<std::string> data;
};

// Files that contain tweak values and their accompanying data.
inline fea::unsigned_hole_hashmap<size_t, tweak_file> tweak_files;

template <src_stamp loc, class T>
T tweak_value(T&& val) {
	static bool first_call = true;
	static T stored_value;

	if (first_call) {
		first_call = false;
		stored_value = std::forward<T>(val);
		if (!tweak_files.contains(loc.file_hash)) {
			// First call, just store and init data.
			tweak_file f;
			f.needs_update = false;
			f.file_path = loc.file_path.sv();
			f.last_modified = std::filesystem::last_write_time(f.file_path);
			f.load_data();
			tweak_files.insert(loc.file_hash, std::move(f));
		}
		return stored_value;
	}

	const tweak_file& f = tweak_files.at_unchecked(loc.file_hash);
	if (!f.needs_update) {
		return stored_value;
	}

	// Needs update. Find our tweak macro and load data.
	assert(loc.line - 1 < f.data.size());
	const std::string& my_line = f.data[loc.line - 1];

	size_t start = my_line.find("FEA_TWEAK(");
	if (start == my_line.npos) {
		fea::print_error_message(__FUNCTION__, __LINE__,
				"Couldn't find tweak macro at expected line, returning "
				"previously stored value.");
		return stored_value;
	}
	start += 10;

	size_t end = my_line.find(")", start);
	if (end == my_line.npos) {
		fea::print_error_message(__FUNCTION__, __LINE__,
				"Couldn't find tweak macro end at expected line, returning "
				"previously stored value.");
		return stored_value;
	}

	T result{};
	auto [ptr, ec] = std::from_chars(
			my_line.data() + start, my_line.data() + end, result);

	if (ec == std::errc{}) {
		stored_value = result;
	} else {
		fea::print_error_message(__FUNCTION__, __LINE__,
				"Failed to parse tweak value, returning previously stored "
				"value.");
	}

	return stored_value;
}
} // namespace detail

inline void tweak_update() {
	for (detail::tweak_file& f : detail::tweak_files) {
		std::filesystem::file_time_type last_mod
				= std::filesystem::last_write_time(f.file_path);
		if (last_mod > f.last_modified) {
			f.needs_update = true;
			f.load_data();
			f.last_modified = last_mod;
		}
	}
}
} // namespace fea
#endif // !FEA_MACOS

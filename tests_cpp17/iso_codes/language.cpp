#include <algorithm>
#include <cassert>
#include <fea/iso_codes/language.hpp>
#include <fea/string/string.hpp>
#include <fea/utils/file.hpp>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <sstream>
#include <unordered_map>

extern const char* argv0;
namespace {

// Parses the iso 639 dataset and generates a header.
// Expects dataset to be in bin folder, named :
// 'iso-639-3_Code_Tables_dddddddd/'
#if 1
const std::string file_header = R"xx(
/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2022, Philippe Groarke
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
#include "fea/iso_codes/language_types.hpp"

#include <array>

namespace fea {
namespace detail {

// All codes and information come from :
// https://iso639-3.sil.org/code_tables/download_tables
)xx";

const std::string file_footer = R"xx(
} // namespace detail
} // namespace fea
)xx";

struct lang {
	fea::iso_639_scope scope = fea::iso_639_scope::count;
	fea::iso_639_type type = fea::iso_639_type::count;
	fea::iso_639_status status = fea::iso_639_status::count;
	fea::iso_639_deprecation_reason reason
			= fea::iso_639_deprecation_reason::count;

	uint16_t runtime_id = (std::numeric_limits<uint16_t>::max)();
	uint16_t parent_macro_id = (std::numeric_limits<uint16_t>::max)();

	std::string code_3;
	std::string code_2b;
	std::string code_2t;
	std::string code_1;

	std::string name;
	std::string inverted_name;

	std::string macro_code;
	std::string changed_to;
};

std::filesystem::path get_iso_folderpath(const std::filesystem::path& search) {
	std::filesystem::path ret;
	for (const std::filesystem::path& filepath :
			std::filesystem::directory_iterator(search)) {
		if (!std::filesystem::is_directory(filepath)) {
			continue;
		}

		if (fea::starts_with(
					filepath.filename().c_str(), L"iso-639-3_Code_Tables_")) {
			ret = filepath;
			break;
		}
	}
	return ret;
}

std::unordered_map<std::string, lang> parse_main(
		const std::filesystem::path& main_filepath) {
	std::ifstream ifs{ main_filepath };
	if (!ifs.is_open()) {
		fwprintf(stderr, L"Couldn't open file : '%s'\n", main_filepath.c_str());
		return {};
	}

	std::u32string main_fileutf32 = fea::open_text_file_with_bom(ifs);
	std::string main_file = fea::any_to_utf8(main_fileutf32);

	// First row is title.
	bool skip_first = true;

	std::unordered_map<std::string, lang> ret;
	ret.reserve(10000);

	uint16_t runtime_index = 0;
	fea::for_each_line(main_file, [&](const std::string& line) {
		if (skip_first) {
			skip_first = false;
			return;
		}

		lang new_lang;
		new_lang.runtime_id = runtime_index++;

		std::istringstream iss(line);
		std::string word;
		size_t idx = 0;
		while (std::getline(iss, word, '\t')) {
			++idx;
			if (word.empty()) {
				assert(idx != 1);
				continue;
			}

			switch (idx) {
			case 1: {
				// id3
				assert(word.size() == 3);
				new_lang.code_3 = word;
				new_lang.status = fea::iso_639_status::active;
			} break;
			case 2: {
				// id2b
				assert(word.size() == 3);
				new_lang.code_2b = word;
			} break;
			case 3: {
				// id2t
				assert(word.size() == 3);
				new_lang.code_2t = word;
			} break;
			case 4: {
				// id1
				assert(word.size() == 2);
				new_lang.code_1 = word;
			} break;
			case 5: {
				// scope
				assert(word.size() == 1);
				char s = word[0];
				switch (s) {
				case 'I': {
					new_lang.scope = fea::iso_639_scope::individual;
				} break;
				case 'M': {
					new_lang.scope = fea::iso_639_scope::macrolanguage;
				} break;
				case 'S': {
					new_lang.scope = fea::iso_639_scope::special;
				} break;
				default: {
				} break;
				}
			} break;
			case 6: {
				// type
				assert(word.size() == 1);
				char s = word[0];
				switch (s) {
				case 'A': {
					new_lang.type = fea::iso_639_type::ancient;
				} break;
				case 'C': {
					new_lang.type = fea::iso_639_type::constructed;
				} break;
				case 'E': {
					new_lang.type = fea::iso_639_type::extinct;
				} break;
				case 'H': {
					new_lang.type = fea::iso_639_type::historical;
				} break;
				case 'L': {
					new_lang.type = fea::iso_639_type::living;
				} break;
				case 'S': {
					new_lang.type = fea::iso_639_type::special;
				} break;
				default: {
				} break;
				}
			} break;
			case 7: {
				// name
				new_lang.name = word;
			} break;
			// case 8: {
			//	// comment
			//} break;
			default: {
			} break;
			}
		}

		std::string key = new_lang.code_3;
		ret.insert({ std::move(key), std::move(new_lang) });
	});

	return ret;
}

void parse_retirements(const std::filesystem::path& path,
		std::unordered_map<std::string, lang>& map) {
	std::ifstream ifs{ path };
	if (!ifs.is_open()) {
		fwprintf(stderr, L"Couldn't open file : '%s'\n", path.c_str());
		return;
	}

	std::u32string utf_text = fea::open_text_file_with_bom(ifs);
	std::string text = fea::any_to_utf8(utf_text);

	// First row is title.
	bool skip_first = true;

	uint16_t runtime_index = uint16_t(map.size());
	fea::for_each_line(text, [&](const std::string& line) {
		if (skip_first) {
			skip_first = false;
			return;
		}

		lang new_lang;
		new_lang.runtime_id = runtime_index++;

		std::istringstream iss(line);
		std::string word;
		size_t idx = 0;

		while (std::getline(iss, word, '\t')) {
			++idx;
			if (word.empty()) {
				assert(idx != 1);
				continue;
			}

			switch (idx) {
			case 1: {
				// id3
				assert(word.size() == 3);
				if (map.count(word) == 1) {
					// duplicate
					--runtime_index;
					return;
				}
				new_lang.code_3 = word;
				new_lang.status = fea::iso_639_status::deprecated;
			} break;
			case 2: {
				// name
				new_lang.name = word;
			} break;
			case 3: {
				// reason
				assert(word.size() == 1);
				char s = word[0];
				switch (s) {
				case 'C': {
					new_lang.reason = fea::iso_639_deprecation_reason::change;
				} break;
				case 'D': {
					new_lang.reason
							= fea::iso_639_deprecation_reason::duplicate;
				} break;
				case 'N': {
					new_lang.reason
							= fea::iso_639_deprecation_reason::non_existent;
				} break;
				case 'S': {
					new_lang.reason = fea::iso_639_deprecation_reason::split;
				} break;
				case 'M': {
					new_lang.reason = fea::iso_639_deprecation_reason::merge;
				} break;
				default: {
				} break;
				}
			} break;
			case 4: {
				// changed to
				assert(word.size() == 3);
				new_lang.changed_to = word;
			} break;
			// case 5: {
			//	// remedy
			//} break;
			// case 6: {
			//	// effective date
			//} break;
			default: {
			} break;
			}
		}

		std::string code = new_lang.code_3;
		map.insert({ std::move(code), std::move(new_lang) });
	});
}

void parse_name_index(const std::filesystem::path& path,
		std::unordered_map<std::string, lang>& map) {
	std::ifstream ifs{ path };
	if (!ifs.is_open()) {
		fwprintf(stderr, L"Couldn't open file : '%s'\n", path.c_str());
		return;
	}

	std::u32string utf_text = fea::open_text_file_with_bom(ifs);
	std::string text = fea::any_to_utf8(utf_text);

	// First row is title.
	bool skip_first = true;

	fea::for_each_line(text, [&](const std::string& line) {
		if (skip_first) {
			skip_first = false;
			return;
		}

		std::istringstream iss(line);
		std::string word;
		size_t idx = 0;
		lang* current_lang = nullptr;
		while (std::getline(iss, word, '\t')) {
			++idx;
			if (word.empty()) {
				if (idx == 1) {
					// just in case
					current_lang = nullptr;
				}
				continue;
			}

			switch (idx) {
			case 1: {
				// id3
				assert(word.size() == 3);
				current_lang = &map.at(word);
			} break;
			case 2: {
				// extra name
				// We ignore extra names for now, and only extract the
				// inverted_name.
				if (word != current_lang->name) {
					return;
				}
			} break;
			case 3: {
				// inverted_name
				current_lang->inverted_name = word;
			} break;
			default: {
			} break;
			}
		}
	});
}

void parse_macrolanguage(const std::filesystem::path& path,
		std::unordered_map<std::string, lang>& map) {
	std::ifstream ifs{ path };
	if (!ifs.is_open()) {
		fwprintf(stderr, L"Couldn't open file : '%s'\n", path.c_str());
		return;
	}

	std::u32string utf_text = fea::open_text_file_with_bom(ifs);
	std::string text = fea::any_to_utf8(utf_text);

	// First row is title.
	bool skip_first = true;

	fea::for_each_line(text, [&](const std::string& line) {
		if (skip_first) {
			skip_first = false;
			return;
		}

		std::istringstream iss(line);
		std::string word;
		size_t idx = 0;
		lang* current_macro_lang = nullptr;
		lang* current_lang = nullptr;
		while (std::getline(iss, word, '\t')) {
			++idx;
			if (word.empty()) {
				if (idx == 1 || idx == 2) {
					// just in case
					current_macro_lang = nullptr;
					current_lang = nullptr;
				}
				continue;
			}

			switch (idx) {
			case 1: {
				// macro id
				assert(word.size() == 3);
				current_macro_lang = &map.at(word);
				assert(current_macro_lang->code_3.size() == 3);
			} break;
			case 2: {
				// sublang id
				assert(word.size() == 3);
				current_lang = &map.at(word);
				current_lang->macro_code = current_macro_lang->code_3;
				current_lang->parent_macro_id = current_macro_lang->runtime_id;
			} break;
			case 3: {
				// status
				assert(word.size() == 1);
				char s = word[0];
				switch (s) {
				case 'A': {
					current_lang->status = fea::iso_639_status::active;
				} break;
				case 'R': {
					current_lang->status = fea::iso_639_status::deprecated;
				} break;
				default: {
				} break;
				}
			} break;
			default: {
			} break;
			}
		}
	});
}

void gen_header(const std::filesystem::path& path,
		const std::string& dataset_name,
		std::unordered_map<std::string, lang>& map) {
	std::ofstream ofs{ path };
	if (!ofs.is_open()) {
		fwprintf(stderr, L"Couldn't output file : '%s'\n", path.c_str());
		return;
	}

	// Move to sorted vector for final output.
	std::vector<lang> langs_vec;
	langs_vec.reserve(map.size());
	for (std::pair<const std::string, lang>& p : map) {
		langs_vec.push_back(std::move(p.second));
	}
	std::sort(langs_vec.begin(), langs_vec.end(),
			[](const lang& lhs, const lang& rhs) {
				return lhs.runtime_id < rhs.runtime_id;
			});

	// Make sure we don't have duplicate ids.
	auto a_it = std::adjacent_find(langs_vec.begin(), langs_vec.end(),
			[](const lang& lhs, const lang& rhs) {
				return lhs.runtime_id == rhs.runtime_id;
			});
	assert(a_it == langs_vec.end());

	// Make sure the ids are correct (they match the index).
	for (size_t i = 0; i < langs_vec.size(); ++i) {
		assert(size_t(langs_vec[i].runtime_id) == i);
	}

	ofs << file_header;
	ofs << "// Dataset : " << dataset_name << std::endl;
	ofs << std::endl;

	// Main language array.
	// Indexed at runtime index.
	ofs << "inline constexpr auto iso_639_languages = std::array{" << std::endl;

	auto output_enum = [&](auto e) {
		std::string e_name = typeid(e).name();
		e_name = e_name.substr(5);
		ofs << "\t\t" << e_name << "::" << fea::to_string(e) << ","
			<< std::endl;
	};

	auto output_string = [&](const std::string& str) {
		ofs << "\t\t\"" << str << "\"," << std::endl;
	};

	auto output_id = [&](uint16_t id) {
		ofs << "\t\t{" << std::to_string(id) << "}," << std::endl;
	};

	for (const lang& l : langs_vec) {
		// const lang& l = p.second;
		ofs << "\tiso_639_lang{" << std::endl;

		output_enum(l.scope);
		output_enum(l.type);
		output_enum(l.status);
		output_enum(l.reason);

		output_id(l.runtime_id);
		output_id(l.parent_macro_id);

		output_string(l.code_3);
		output_string(l.code_2b);
		output_string(l.code_2t);
		output_string(l.code_1);

		output_string(l.name);
		output_string(l.inverted_name);

		output_string(l.macro_code);
		output_string(l.changed_to);

		ofs << "\t}," << std::endl;
	}

	ofs << "};" << std::endl;

	ofs << file_footer;
}

TEST(languages, generate_header) {
	std::filesystem::path exe_path = fea::executable_dir(argv0);
	std::filesystem::path iso_dir = get_iso_folderpath(exe_path);
	ASSERT_FALSE(iso_dir.empty());

	std::filesystem::path main_filepath;
	std::filesystem::path retirements_filepath;
	std::filesystem::path nameindex_filepath;
	std::filesystem::path macrolanguages_filepath;

	for (const std::filesystem::path& filepath :
			std::filesystem::directory_iterator(iso_dir)) {
		if (fea::contains(filepath.c_str(), L"iso-639-3.tab")) {
			main_filepath = filepath;
		} else if (fea::contains(filepath.c_str(), L"Retirements")) {
			retirements_filepath = filepath;
		} else if (fea::contains(filepath.c_str(), L"Name_Index")) {
			nameindex_filepath = filepath;
		} else if (fea::contains(filepath.c_str(), L"macrolanguages")) {
			macrolanguages_filepath = filepath;
		}
	}

	std::unordered_map<std::string, lang> map = parse_main(main_filepath);
	parse_retirements(retirements_filepath, map);
	parse_name_index(nameindex_filepath, map);
	parse_macrolanguage(macrolanguages_filepath, map);

	gen_header(exe_path / "language_database.hpp", iso_dir.filename().string(),
			map);
}
#endif
} // namespace

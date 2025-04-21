#if 0
#include <algorithm>
#include <cassert>
#include <fea/string/string.hpp>
#include <fea/utility/file.hpp>
#include <filesystem>
#include <format>
#include <fstream>
#include <gtest/gtest.h>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

extern const char* argv0;
namespace {
/*
Parses a dictionary and generates a header.
Expects text file to be in bin folder, named :
'english.txt'

For exact repro, use this dictionary :
https://norvig.com/google-books-common-words.txt
*/

// The google corpus dictionary is tab seperated with word count.
constexpr bool tsv = true;

// Only keep 'count' words.
constexpr size_t final_count = 10'000u;

constexpr std::string_view file_header = R"xx(
/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2025, Philippe Groarke
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
namespace fea {
namespace en {
namespace detail {
// clang-format off
)xx";

constexpr std::string_view array_decl_open_fmt = R"xx(
 inline constexpr std::array<std::string_view, {}> english_dic{{
)xx";
constexpr std::string_view vec_decl_open_fmt = R"xx(
inline const std::vector<std::string_view> english_dic{
)xx";

constexpr std::string_view decl_close = R"xx(
};
)xx";

constexpr std::string_view file_footer = R"xx(
// clang-format on
} // namespace detail
} // namespace en
} // namespace fea
)xx";


TEST(language, gen_english_dic) {
	std::filesystem::path exe_path = fea::executable_dir(argv0);
	std::filesystem::path dic_filepath = exe_path / "english.txt";
	std::filesystem::path out_dic_filepath = exe_path / "english_dic.imp.hpp";

	std::vector<std::string> words;
	fea::read_text_file(dic_filepath, [&](std::string&& str) {
		if (words.size() == final_count) {
			return;
		}

		size_t pos = str.find('\t');
		assert(pos != str.npos);
		str = str.substr(0u, pos);

		fea::to_lower_ascii_inplace(str);
		str.insert(str.begin(), '"');
		str += "\",";
		words.push_back(std::move(str));
	});

	// Considering the words are sorted by popularity,
	// we don't want to sort them. But we still need to make sure they are
	// unique.
	// Copy the data, find non-unique words. Then, remove them from original
	// vec.
	{
		std::vector<std::string> words_orig = words;
		std::vector<std::string> words_cpy = words;
		std::sort(words_cpy.begin(), words_cpy.end());
		auto new_end = std::unique(words_cpy.begin(), words_cpy.end());

		if (new_end != words_cpy.end()) {
			std::unordered_set<std::string_view> uset;
			uset.reserve(std::distance(words_cpy.begin(), new_end));
			words.clear();
			for (const std::string& word : words_orig) {
				if (uset.contains(word)) {
					continue;
				}
				uset.insert(word);
				words.push_back(word);
			}
		}
	}

	assert(std::none_of(words.begin(), words.end(),
			[](const std::string& str) { return str.find('\t') != str.npos; }));

	std::ofstream ofs{ out_dic_filepath };
	if (!ofs.is_open()) {
		assert(false);
		return;
	}

	ofs << file_header;
	ofs << std::format(array_decl_open_fmt, words.size());
	// ofs << array_decl_open_fmt;

	constexpr size_t words_per_line = 8u;
	size_t newline_count = 0u;
	for (const std::string& word : words) {
		ofs << word;
		++newline_count;
		if (newline_count == words_per_line) {
			ofs << '\n';
			newline_count = 0u;
		}
	}
	ofs << decl_close;
	ofs << file_footer;
}
} // namespace
#endif

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
#include <array>
#include <fea/containers/span.hpp>
#include <string>
#include <string_view>
#include <vector>

namespace fea {
// Split string using any of the provided delimiters.
// Returns std::vector of tokens.
// If keep_delims is true, tokens start with delimiters.
template <class CharT>
[[nodiscard]] std::vector<std::basic_string_view<CharT>> split(
		std::basic_string_view<CharT> str, const CharT* delimiters,
		bool keep_delims = false) {
	std::vector<std::basic_string_view<CharT>> tokens;
	size_t prev = 0;
	size_t pos;

	while ((pos = str.find_first_of(delimiters, prev))
			!= std::basic_string_view<CharT>::npos) {
		if (pos > prev) {
			if (keep_delims && prev != 0) {
				tokens.push_back(str.substr(prev - 1, pos - prev));
			} else {
				tokens.push_back(str.substr(prev, pos - prev));
			}
		}
		prev = pos + 1;
	}

	if (prev < str.length()) {
		if (keep_delims && prev != 0) {
			tokens.push_back(
					str.substr(prev - 1, std::basic_string_view<CharT>::npos));
		} else {
			tokens.push_back(
					str.substr(prev, std::basic_string_view<CharT>::npos));
		}
	}

	return tokens;
}

// Overload for string.
// Calls the string view version and converts back to string.
template <class CharT>
[[nodiscard]] std::vector<std::basic_string<CharT>> split(
		const std::basic_string<CharT>& str, const CharT* delimiters,
		bool keep_delims = false) {
	std::vector<std::basic_string_view<CharT>> views = fea::split(
			std::basic_string_view<CharT>{ str }, delimiters, keep_delims);

	std::vector<std::basic_string<CharT>> ret;
	ret.reserve(views.size());
	for (auto v : views) {
		ret.push_back(std::basic_string<CharT>{ v });
	}
	return ret;
}

// Split and return results in std::array.
// Fills up array with tokens, then exits.
// If keep_delims is true, tokens start with delimiters.
template <size_t N, class CharT>
[[nodiscard]] std::array<std::basic_string<CharT>, N> split(
		const std::basic_string<CharT>& str, const CharT* delimiters,
		bool keep_delims = false) {
	static_assert(N != 0, "split : array must be bigger than 0");

	std::array<std::basic_string<CharT>, N> tokens;
	size_t idx = 0;
	size_t prev = 0;
	size_t pos;

	while ((pos = str.find_first_of(delimiters, prev))
			!= std::basic_string<CharT>::npos) {
		if (pos > prev) {
			if (keep_delims && prev != 0) {
				tokens[idx++] = str.substr(prev - 1, pos - prev);
			} else {
				tokens[idx++] = str.substr(prev, pos - prev);
			}

			if (idx >= N) {
				break;
			}
		}
		prev = pos + 1;
	}

	if (prev < str.length() && idx < N) {
		if (keep_delims && prev != 0) {
			tokens[idx++]
					= str.substr(prev - 1, std::basic_string<CharT>::npos);
		} else {
			tokens[idx++] = str.substr(prev, std::basic_string<CharT>::npos);
		}
	}
	return tokens;
}


// Split string using any of the provided words.
// Returns std::vector of std::string_view tokens.
// If keep_delims is true, tokens start with delimiters.
template <class CharT>
[[nodiscard]] std::vector<std::basic_string_view<CharT>> split(
		const std::basic_string<CharT>& str,
		fea::span<const std::basic_string<CharT>> multi_char_delimiters,
		bool keep_delims = false) {
	std::vector<std::basic_string_view<CharT>> tokens;
	size_t prev = 0;
	size_t pos;
	size_t length = 0;

	auto find_words
			= [](const std::basic_string<CharT>& str,
					  fea::span<const std::basic_string<CharT>> searches,
					  size_t pos, size_t& length) {
				  if (pos >= str.size()) {
					  return std::basic_string<CharT>::npos;
				  }

				  for (size_t i = pos; i < str.size(); ++i) {
					  for (const std::basic_string<CharT>& s : searches) {
						  size_t mi = i;
						  bool found = true;
						  for (size_t j = 0; j < s.size() && mi < str.size();
								  ++j, ++mi) {
							  if (str[mi] != s[j]) {
								  found = false;
								  break;
							  }
						  }

						  if (found) {
							  length = s.size();
							  return i;
						  }
					  }
				  }
				  return std::basic_string<CharT>::npos;
			  };

	while ((pos = find_words(str, multi_char_delimiters, prev, length))
			!= std::basic_string<CharT>::npos) {
		if (pos > prev) {
			if (keep_delims && prev != 0) {
				tokens.push_back(
						{ str.begin() + prev - length, str.begin() + pos });
			} else {
				tokens.push_back({ str.begin() + prev, str.begin() + pos });
			}
		}
		prev = pos + length;
	}

	if (prev < str.length()) {
		if (keep_delims && prev != 0) {
			tokens.push_back({ str.begin() + prev - length, str.end() });
		} else {
			tokens.push_back({ str.begin() + prev, str.end() });
		}
	}

	return tokens;
}
} // namespace fea

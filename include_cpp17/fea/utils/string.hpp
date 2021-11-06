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
#include "fea/utils/string_conversions.hpp"

#include <vector>

// Makes a string literal of type CharType
#define FEA_MAKE_LITERAL_T(CharType, str) \
	[]() { \
		if constexpr (std::is_same_v<CharType, char>) { \
			return str; \
		} else if constexpr (std::is_same_v<CharType, wchar_t>) { \
			return L##str; \
		} else if constexpr (std::is_same_v<CharType, char16_t>) { \
			return u##str; \
		} else if constexpr (std::is_same_v<CharType, char32_t>) { \
			return U##str; \
		} \
	}()

// These are shortcuts if you use CharT as an alias or template.
#define FEA_MAKE_LITERAL(str) FEA_MAKE_LITERAL_T(CharT, str)
#define FEA_ML(str) FEA_MAKE_LITERAL_T(CharT, str)


// Makes a character literal of type CharType
#define FEA_MAKE_CHAR_LITERAL_T(CharType, str) \
	[]() { \
		if constexpr (std::is_same_v<CharType, char>) { \
			return str; \
		} else if constexpr (std::is_same_v<CharType, wchar_t>) { \
			return L##str; \
		} else if constexpr (std::is_same_v<CharType, char16_t>) { \
			return u##str; \
		} else if constexpr (std::is_same_v<CharType, char32_t>) { \
			return U##str; \
		} \
	}()

// These are shortcuts if you use CharT as an alias or template.
#define FEA_MAKE_CHAR_LITERAL(str) FEA_MAKE_CHAR_LITERAL_T(CharT, str)
#define FEA_CH(ch) FEA_MAKE_CHAR_LITERAL_T(CharT, ch)


namespace fea {
template <class CharT>
[[nodiscard]] bool contains(
		const string_t<CharT>& str, const string_t<CharT>& search) {
	return str.find(search) != string_t<CharT>::npos;
}
template <class CharT>
[[nodiscard]] bool contains(const string_t<CharT>& str, const CharT* search) {
	return str.find(search) != string_t<CharT>::npos;
}

template <class CharT>
[[nodiscard]] bool starts_with(
		const string_t<CharT>& str, const string_t<CharT>& search) {
	return str.find(search) == 0;
}
template <class CharT>
[[nodiscard]] bool starts_with(
		const string_t<CharT>& str, const CharT* search) {
	return str.find(search) == 0;
}

template <class CharT>
[[nodiscard]] bool ends_with(
		const string_t<CharT>& str, const string_t<CharT>& search) {
	return str.find_last_of(search) == str.size() - search.size();
}
template <class CharT>
[[nodiscard]] bool ends_with(const string_t<CharT>& str, const CharT* search) {
	return ends_with(str, string_t<CharT>{ search });
}

// Constexpr character to_lower (lower case).
template <class CharT>
[[nodiscard]] constexpr CharT to_lower(CharT ch) {
	return (ch >= FEA_CH('A') && ch <= FEA_CH('Z'))
			? ch + (FEA_CH('a') - FEA_CH('A'))
			: ch;
}

template <class CharT>
[[nodiscard]] string_t<CharT> to_lower(const string_t<CharT>& str) {
	auto ret = str;
	std::transform(ret.begin(), ret.end(), ret.begin(),
			[](auto c) { return std::tolower(c, std::locale()); });
	return ret;
}

template <class CharT>
void to_lower(string_t<CharT>& out, bool /*inplace*/) {
	std::transform(out.begin(), out.end(), out.begin(),
			[](auto c) { return std::tolower(c, std::locale()); });
}

[[nodiscard]] inline std::vector<uint8_t> to_lower(
		const std::vector<uint8_t>& str) {
	std::vector<uint8_t> ret = str;
	std::transform(ret.begin(), ret.end(), ret.begin(),
			[](auto c) { return std::tolower(c, std::locale()); });
	return ret;
}

inline void to_lower(std::vector<uint8_t>& out, bool /*inplace*/) {
	std::transform(out.begin(), out.end(), out.begin(),
			[](auto c) { return std::tolower(c, std::locale()); });
}


template <class CharT>
[[nodiscard]] string_t<CharT> to_upper(const string_t<CharT>& str) {
	auto ret = str;
	std::transform(ret.begin(), ret.end(), ret.begin(),
			[](auto c) { return CharT(::toupper(c)); });
	return ret;
}

template <class CharT>
void to_upper(string_t<CharT>& out, bool /*inplace*/) {
	std::transform(out.begin(), out.end(), out.begin(),
			[](auto c) { return CharT(::toupper(c)); });
}

[[nodiscard]] inline std::vector<uint8_t> to_upper(
		const std::vector<uint8_t>& str) {
	std::vector<uint8_t> ret = str;
	std::transform(ret.begin(), ret.end(), ret.begin(),
			[](char c) { return char(::toupper(c)); });
	return ret;
}

inline void to_upper(std::vector<uint8_t>& out, bool /*inplace*/) {
	std::transform(out.begin(), out.end(), out.begin(),
			[](char c) { return char(::tolower(c)); });
}

template <class CharT>
[[nodiscard]] std::vector<string_t<CharT>> split(
		const string_t<CharT>& str, const CharT* delimiters) {
	std::vector<string_t<CharT>> tokens;
	size_t prev = 0;
	size_t pos;

	while ((pos = str.find_first_of(delimiters, prev))
			!= string_t<CharT>::npos) {
		if (pos > prev) {
			tokens.push_back(str.substr(prev, pos - prev));
		}
		prev = pos + 1;
	}
	if (prev < str.length()) {
		tokens.push_back(str.substr(prev, string_t<CharT>::npos));
	}
	return tokens;
}

template <class CharT>
[[nodiscard]] std::vector<string_t<CharT>> split(
		const string_t<CharT>& str, CharT delimiter) {
	string_t<CharT> delim{ delimiter };
	return split(str, delim.c_str());
}


template <size_t N, class CharT>
[[nodiscard]] std::array<string_t<CharT>, N> split(
		const string_t<CharT>& str, const CharT* delimiters) {
	static_assert(N != 0, "split : array must be bigger than 0");

	std::array<string_t<CharT>, N> tokens;
	size_t idx = 0;
	size_t prev = 0;
	size_t pos;

	while ((pos = str.find_first_of(delimiters, prev))
			!= string_t<CharT>::npos) {
		if (pos > prev) {
			tokens[idx++] = str.substr(prev, pos - prev);

			if (idx >= N) {
				break;
			}
		}
		prev = pos + 1;
	}

	if (prev < str.length() && idx < N) {
		tokens[idx++] = str.substr(prev, string_t<CharT>::npos);
	}
	return tokens;
}

template <size_t N, class CharT>
[[nodiscard]] std::array<string_t<CharT>, N> split(
		const string_t<CharT>& str, CharT delimiter) {
	string_t<CharT> delim{ delimiter };
	return split(str, delim.c_str());
}


template <class CharT>
void replace_all(string_t<CharT>& out, const string_t<CharT>& search,
		const string_t<CharT>& replace, bool /*inplace*/) {

	size_t pos = out.find(search);
	while (pos != string_t<CharT>::npos) {
		out.replace(pos, search.size(), replace);
		pos = out.find(search, pos + replace.size());
	}
}
template <class CharT>
void replace_all(string_t<CharT>& out, const CharT* search,
		const CharT* replace, bool /*inplace*/) {
	replace_all(
			out, string_t<CharT>{ search }, string_t<CharT>{ replace }, true);
}

template <class CharT>
[[nodiscard]] string_t<CharT> replace_all(const string_t<CharT>& str,
		const string_t<CharT>& search, const string_t<CharT>& replace) {
	string_t<CharT> ret = str;
	replace_all(ret, search, replace, true);
	return ret;
}
template <class CharT>
[[nodiscard]] string_t<CharT> replace_all(
		const string_t<CharT>& str, const CharT* search, const CharT* replace) {
	return replace_all(
			str, string_t<CharT>{ search }, string_t<CharT>{ replace });
}

// Iterates input string line by line and calls your function with each line.
template <class CharT, class Func>
void for_each_line(const string_t<CharT>& str, Func&& func) {
	std::basic_istringstream iss(str);
	string_t<CharT> line;
	while (std::getline(iss, line)) {
		std::forward<Func>(func)(line);
	}
}

// Removes all leading trim_char.
template <class CharT>
[[nodiscard]] string_t<CharT> trim_leading(
		const string_t<CharT>& str, CharT trim_char) {
	size_t new_begin = str.find_first_not_of(trim_char);
	if (new_begin == string_t<CharT>::npos) {
		return {};
	}
	return str.substr(new_begin);
}

// Removes all leading trim_chars.
template <class CharT>
[[nodiscard]] string_t<CharT> trim_leading(
		const string_t<CharT>& str, const CharT* trim_chars) {
	size_t new_begin = str.find_first_not_of(trim_chars);
	if (new_begin == string_t<CharT>::npos) {
		return {};
	}
	return str.substr(new_begin);
}

// Removes all leading trim_char.
template <class CharT>
[[nodiscard]] string_t<CharT> trim_trailing(
		const string_t<CharT>& str, CharT trim_char) {
	size_t new_end = str.find_last_not_of(trim_char);
	if (new_end == string_t<CharT>::npos) {
		return {};
	}
	return str.substr(0, new_end + 1);
}

// Removes all leading trim_chars.
template <class CharT>
[[nodiscard]] string_t<CharT> trim_trailing(
		const string_t<CharT>& str, const CharT* trim_chars) {
	size_t new_end = str.find_last_not_of(trim_chars);
	if (new_end == string_t<CharT>::npos) {
		return {};
	}
	return str.substr(0, new_end + 1);
}

// Inserts the provided thousand seperator in string.
// https://stackoverflow.com/questions/49319461/how-to-use-custom-thousand-separator-and-decimal-character-with-stdstringstrea

template <class CharT>
[[nodiscard]] string_t<CharT> thousand_seperate(
		const string_t<CharT>& str, CharT sep) {
	string_t<CharT> ret = str;

	size_t i = ret.rfind(CharT('.'));
	if (i == ret.npos) {
		i = ret.size();
	}

	const string_t<CharT> nums = fea::utf8_to_any<CharT>("0123456789");
	size_t prefix_pos = ret.find_first_of(nums);
	const size_t digits = 3u + prefix_pos;
	while (i > digits) {
		ret.insert(i -= 3, 1, sep);
	}

	return ret;
}

#if FEA_CPP20
template <class InputIt1, class InputIt2>
[[nodiscard]] constexpr std::strong_ordering lexicographical_compare(
		InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2) {
	for (; (first1 != last1) && (first2 != last2); ++first1, (void)++first2) {
		auto c1 = fea::to_lower(*first1);
		auto c2 = fea::to_lower(*first2);
		if (c1 != c2) {
			return c1 <=> c2;
		}
	}
	return std::distance(first1, last1) <=> std::distance(first2, last2);
}
#endif

} // namespace fea

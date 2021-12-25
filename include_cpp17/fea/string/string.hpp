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
#include "fea/containers/span.hpp"
#include "fea/macros/literals.hpp"
#include "fea/string/conversions.hpp"
#include "fea/string/split.hpp"
#include "fea/utils/platform.hpp"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <locale>
#include <string>
#include <string_view>
#include <vector>

#if FEA_CPP20
#include <compare>
#endif

/*
Various helper functions for strings.
TODO : Integrate std::string_view.
*/

namespace fea {
// Returns the size of a given string, string_view or c_string.
template <template <class, class, class...> class Str, class CharT,
		template <class> class Traits, class... Args>
[[nodiscard]] size_t size(const Str<CharT, Traits<CharT>, Args...>& str) {
	return str.size();
}
template <class CharT>
[[nodiscard]] size_t size(const CharT* str) {
	return std::char_traits<CharT>::length(str);
}


// Returns true if provided string contains 'search'.
template <template <class, class, class...> class Str, class CharT,
		template <class> class Traits, class... Args, class... Args2>
[[nodiscard]] bool contains(const Str<CharT, Traits<CharT>, Args...>& str,
		const Str<CharT, Traits<CharT>, Args2...>& search) {
	return str.find(search) != str.npos;
}
template <template <class, class, class...> class Str, class CharT,
		template <class> class Traits, class... Args>
[[nodiscard]] bool contains(
		const Str<CharT, Traits<CharT>, Args...>& str, const CharT* search) {
	return str.find(search) != str.npos;
}
template <template <class, class, class...> class Str, class CharT,
		template <class> class Traits, class... Args>
[[nodiscard]] bool contains(
		const Str<CharT, Traits<CharT>, Args...>& str, CharT search) {
	return str.find(search) != str.npos;
}

template <class CharT>
[[nodiscard]] bool contains(const CharT* str, const CharT* search) {
	using sv = std::basic_string_view<CharT>;
	return contains(sv{ str }, search);
}
template <class CharT>
[[nodiscard]] bool contains(const CharT* str, CharT search) {
	using sv = std::basic_string_view<CharT>;
	return contains(sv{ str }, search);
}


// Returns true if provided string starts with 'search'.
template <template <class, class, class...> class Str, class CharT,
		template <class> class Traits, class... Args, class... Args2>
[[nodiscard]] bool starts_with(const Str<CharT, Traits<CharT>, Args...>& str,
		const Str<CharT, Traits<CharT>, Args2...>& search) {
	return str.size() >= search.size()
			&& Traits<CharT>::compare(str.data(), search.data(), search.size())
			== 0;
}
template <template <class, class, class...> class Str, class CharT,
		template <class> class Traits, class... Args>
[[nodiscard]] bool starts_with(
		const Str<CharT, Traits<CharT>, Args...>& str, const CharT* search) {
	using sv = std::basic_string_view<CharT>;
	sv _search = sv{ search };

	return str.size() >= _search.size()
			&& Traits<CharT>::compare(
					   str.data(), _search.data(), _search.size())
			== 0;
}
template <template <class, class, class...> class Str, class CharT,
		template <class> class Traits, class... Args>
[[nodiscard]] bool starts_with(
		const Str<CharT, Traits<CharT>, Args...>& str, CharT search) {
	return str.size() >= 1
			&& Traits<CharT>::compare(str.data(), &search, 1) == 0;
}

template <class CharT>
[[nodiscard]] bool starts_with(const CharT* str, const CharT* search) {
	using sv = std::basic_string_view<CharT>;
	return starts_with(sv{ str }, search);
}
template <class CharT>
[[nodiscard]] bool starts_with(const CharT* str, CharT search) {
	using sv = std::basic_string_view<CharT>;
	return starts_with(sv{ str }, search);
}

template <template <class, class, class...> class Str, class CharT,
		template <class> class Traits, class... Args, class... Args2>
[[nodiscard]] bool ends_with(const Str<CharT, Traits<CharT>, Args...>& str,
		const Str<CharT, Traits<CharT>, Args2...>& search) {
	return str.size() >= search.size()
			&& Traits<CharT>::compare(str.data() + (str.size() - search.size()),
					   search.data(), search.size())
			== 0;
}
template <template <class, class, class...> class Str, class CharT,
		template <class> class Traits, class... Args>
[[nodiscard]] bool ends_with(
		const Str<CharT, Traits<CharT>, Args...>& str, const CharT* search) {
	using sv = std::basic_string_view<CharT>;
	sv _search = sv{ search };

	return str.size() >= _search.size()
			&& Traits<CharT>::compare(
					   str.data() + (str.size() - _search.size()),
					   _search.data(), _search.size())
			== 0;
}
template <template <class, class, class...> class Str, class CharT,
		template <class> class Traits, class... Args>
[[nodiscard]] bool ends_with(
		const Str<CharT, Traits<CharT>, Args...>& str, CharT search) {
	return str.size() >= 1
			&& Traits<CharT>::compare(str.data() + (str.size() - 1), &search, 1)
			== 0;
}

template <class CharT>
[[nodiscard]] bool ends_with(const CharT* str, const CharT* search) {
	using sv = std::basic_string_view<CharT>;
	return ends_with(sv{ str }, search);
}
template <class CharT>
[[nodiscard]] bool ends_with(const CharT* str, CharT search) {
	using sv = std::basic_string_view<CharT>;
	return ends_with(sv{ str }, search);
}


// Constexpr lower case ASCII character.
template <class CharT>
[[nodiscard]] constexpr CharT to_lower_ascii(CharT ch) {
	constexpr auto diff = FEA_CH('a') - FEA_CH('A');
	if (ch >= FEA_CH('A') && ch <= FEA_CH('Z')) {
		return ch + diff;
	}
	return ch;
}

template <class CharT>
[[nodiscard]] std::basic_string<CharT> to_lower(
		const std::basic_string<CharT>& str) {
	auto ret = str;
	std::transform(ret.begin(), ret.end(), ret.begin(),
			[](auto c) { return std::tolower(c, std::locale()); });
	return ret;
}

[[nodiscard]] inline std::vector<uint8_t> to_lower(
		const std::vector<uint8_t>& str) {
	std::vector<uint8_t> ret = str;
	std::transform(ret.begin(), ret.end(), ret.begin(),
			[](auto c) { return uint8_t(std::tolower(int(c))); });
	return ret;
}

// Lower-case passed in string.
// No copies.
template <class CharT>
void to_lower_inplace(std::basic_string<CharT>& out) {
	std::transform(out.begin(), out.end(), out.begin(),
			[](auto c) { return std::tolower(c, std::locale()); });
}
inline void to_lower_inplace(std::vector<uint8_t>& out) {
	std::transform(out.begin(), out.end(), out.begin(),
			[](auto c) { return uint8_t(std::tolower(int(c))); });
}


template <class CharT>
[[nodiscard]] std::basic_string<CharT> to_upper(
		const std::basic_string<CharT>& str) {
	auto ret = str;
	std::transform(ret.begin(), ret.end(), ret.begin(),
			[](auto c) { return CharT(::toupper(c)); });
	return ret;
}

template <class CharT>
void to_upper(std::basic_string<CharT>& out, bool /*inplace*/) {
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


// Replaces all 'search' occurences with 'replace'.
// Modifies string out.
template <class CharT>
void replace_all_inplace(std::basic_string<CharT>& out,
		const std::basic_string<CharT>& search,
		const std::basic_string<CharT>& replace) {
	size_t pos = out.find(search);
	while (pos != std::basic_string<CharT>::npos) {
		out.replace(pos, search.size(), replace);
		pos = out.find(search, pos + replace.size());
	}
}

// Replaces all 'search' occurences with 'replace'.
// Modifies 'out' string.
template <class CharT>
void replace_all_inplace(std::basic_string<CharT>& out, const CharT* search,
		const CharT* replace) {
	// TODO : This should be the base function.
	replace_all_inplace(out, std::basic_string<CharT>{ search },
			std::basic_string<CharT>{ replace });
}

// Replaces all 'search' occurences with 'replace'.
// Returns modified string.
template <class CharT>
[[nodiscard]] std::basic_string<CharT> replace_all(
		const std::basic_string<CharT>& str, const CharT* search,
		const CharT* replace) {
	std::basic_string<CharT> ret = str;
	replace_all_inplace(ret, search, replace);
	return ret;
}

// Replaces all 'search' occurences with 'replace'.
// Returns modified string.
template <class CharT>
[[nodiscard]] std::basic_string<CharT> replace_all(
		const std::basic_string<CharT>& str,
		const std::basic_string<CharT>& search,
		const std::basic_string<CharT>& replace) {
	return replace_all(str, search.c_str(), replace.c_str());
}


// Iterates input string line by line and calls your function with each line.
template <class CharT, class Func>
void for_each_line(const std::basic_string<CharT>& str, Func&& func) {
	std::basic_istringstream iss(str);
	std::basic_string<CharT> line;
	while (std::getline(iss, line)) {
		std::forward<Func>(func)(line);
	}
}

// Removes all leading trim_chars.
template <class CharT>
[[nodiscard]] std::basic_string<CharT> trim_leading(
		const std::basic_string<CharT>& str, const CharT* trim_chars) {
	size_t new_begin = str.find_first_not_of(trim_chars);
	if (new_begin == std::basic_string<CharT>::npos) {
		return {};
	}
	return str.substr(new_begin);
}

// Removes all leading characters and returns an appropriately sized
// string_view.
template <class CharT>
[[nodiscard]] std::basic_string_view<CharT> trim_leading(
		std::basic_string_view<CharT> str, const CharT* trim_chars) {
	size_t new_begin = str.find_first_not_of(trim_chars);
	if (new_begin == std::basic_string_view<CharT>::npos) {
		return {};
	}
	return { str.begin() + new_begin, str.end() };
}

// Removes all trailing trim_chars.
template <class CharT>
[[nodiscard]] std::basic_string<CharT> trim_trailing(
		const std::basic_string<CharT>& str, const CharT* trim_chars) {
	size_t new_end = str.find_last_not_of(trim_chars);
	if (new_end == std::basic_string<CharT>::npos) {
		return {};
	}
	return str.substr(0, new_end + 1);
}

// Removes all trailing trim_chars and returns appropriately sized
// string_view.
template <class CharT>
[[nodiscard]] std::basic_string_view<CharT> trim_trailing(
		std::basic_string_view<CharT> str, const CharT* trim_chars) {
	size_t new_end = str.find_last_not_of(trim_chars);
	if (new_end == std::basic_string_view<CharT>::npos) {
		return {};
	}
	return { str.begin(), str.begin() + new_end + 1 };
}

// Replaces conflicting html characters with entities.
template <class CharT>
[[nodiscard]] std::basic_string<CharT> html_escape(
		std::basic_string_view<CharT> str) {
	std::basic_string<CharT> ret;
	ret.reserve(str.size());
	for (size_t i = 0; i < str.size(); ++i) {
		switch (str[i]) {
		case FEA_CH('&'): {
			ret.append(FEA_ML("&amp;"));
		} break;
		case FEA_CH('\"'): {
			ret.append(FEA_ML("&quot;"));
		} break;
		case FEA_CH('\''): {
			ret.append(FEA_ML("&apos;"));
		} break;
		case FEA_CH('<'): {
			ret.append(FEA_ML("&lt;"));
		} break;
		case FEA_CH('>'): {
			ret.append(FEA_ML("&gt;"));
		} break;
		default: {
			ret.push_back(str[i]);
		} break;
		}
	}
	return ret;
}

// Overload for strings.
template <class CharT>
[[nodiscard]] std::basic_string<CharT> html_escape(
		const std::basic_string<CharT>& str) {
	return html_escape(std::basic_string_view<CharT>{ str });
}

// Inserts the provided thousand seperator in string.
// https://stackoverflow.com/questions/49319461/how-to-use-custom-thousand-separator-and-decimal-character-with-stdstringstrea

template <class CharT>
[[nodiscard]] std::basic_string<CharT> thousand_seperate(
		const std::basic_string<CharT>& str, CharT sep) {
	std::basic_string<CharT> ret = str;

	size_t i = ret.rfind(CharT('.'));
	if (i == ret.npos) {
		i = ret.size();
	}

	const std::basic_string<CharT> nums = fea::utf8_to_any<CharT>("0123456789");
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
		auto c1 = fea::to_lower_ascii(*first1);
		auto c2 = fea::to_lower_ascii(*first2);
		if (c1 != c2) {
			return c1 <=> c2;
		}
	}
	return std::distance(first1, last1) <=> std::distance(first2, last2);
}
#endif

} // namespace fea

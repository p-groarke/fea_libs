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
#include "fea/containers/span.hpp"
#include "fea/macros/literals.hpp"
#include "fea/string/conversions.hpp"
#include "fea/string/details.hpp"
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
template <class Str>
[[nodiscard]] constexpr size_t size(const Str& str) {
	return detail::str_view<Str>{ str }.size();
}

// Returns true if provided string 'str' contains 'search'.
template <class Str1, class Str2>
[[nodiscard]] constexpr bool contains(const Str1& str, const Str2& search) {
	return detail::str_view<Str1>{ str }.find(detail::str_view<Str2>{ search })
			!= detail::str_view<Str1>::npos;
}

// Returns true if provided string 'str' starts with 'search'.
template <class Str1, class Str2>
[[nodiscard]] constexpr bool starts_with(const Str1& str, const Str2& search) {
	return detail::str_view<Str1>{ str }.starts_with(
			detail::str_view<Str2>{ search });
}

// Returns true if provided string 'str' ends with 'search'.
template <class Str1, class Str2>
[[nodiscard]] constexpr bool ends_with(const Str1& str, const Str2& search) {
	return detail::str_view<Str1>{ str }.ends_with(
			detail::str_view<Str2>{ search });
}

// Replaces all 'search' occurences with 'replace'.
// Modifies std::string 'out' in place (no copies).
template <template <class, class, class...> class Str, class CharT,
		template <class> class Traits, class... Args, class Str2, class Str3>
void replace_all_inplace(Str<CharT, Traits<CharT>, Args...>& out,
		const Str2& search, const Str3& replace) {
	detail::str_view<Str2> search_v{ search };
	detail::str_view<Str3> replace_v{ replace };

	auto pos = out.find(search_v.data(), 0, search_v.size());
	while (pos != out.npos) {
		out.replace(pos, search_v.size(), replace_v.data(), replace_v.size());
		pos = out.find(
				search_v.data(), pos + replace_v.size(), search_v.size());
	}
}

// Replaces all 'search' occurences with 'replace'.
// Returns new modified std::string.
template <class Str1, class Str2, class Str3>
[[nodiscard]] auto replace_all(
		const Str1& str, const Str2& search, const Str3& replace) {
	using CharT = typename detail::str_view<Str1>::char_type;
	using Traits = typename detail::str_view<Str1>::traits_type;
	std::basic_string<CharT, Traits> ret{ str };

	replace_all_inplace(ret, search, replace);
	return ret;
}

// Lower case ASCII character.
// C's tolower(int) is undefined for characters unrepresentable by unsigned
// char.
template <class CharT, class = std::enable_if_t<std::is_integral_v<CharT>>>
[[nodiscard]] constexpr CharT to_lower_ascii(CharT ch) {
	constexpr auto diff = FEA_CH('a') - FEA_CH('A');
	if (ch <= FEA_CH('Z') && ch >= FEA_CH('A')) {
		return ch + diff;
	}
	return ch;
}

// Lower case ASCII string (without copying).
template <template <class, class, class...> class Str, class CharT,
		template <class> class Traits, class... Args>
constexpr void to_lower_ascii_inplace(Str<CharT, Traits<CharT>, Args...>& out) {
	for (CharT& c : out) {
		c = to_lower_ascii(c);
	}
}

// Lower case ASCII string, returns new modified std::string.
template <class Str, class = std::enable_if_t<!std::is_integral_v<Str>>>
[[nodiscard]] auto to_lower_ascii(const Str& str) {
	using CharT = typename detail::str_view<Str>::char_type;
	using Traits = typename detail::str_view<Str>::traits_type;
	std::basic_string<CharT, Traits> ret{ str };

	to_lower_ascii_inplace(ret);
	return ret;
}

/* TODO : Real utf to_lower, forget std::locale. */

// Upper case ASCII character.
// C's toupper(int) is undefined for characters unrepresentable by unsigned
// char.
template <class CharT, class = std::enable_if_t<std::is_integral_v<CharT>>>
[[nodiscard]] constexpr CharT to_upper_ascii(CharT ch) {
	constexpr auto diff = FEA_CH('a') - FEA_CH('A');
	if (ch >= FEA_CH('a') && ch <= FEA_CH('z')) {
		return ch - diff;
	}
	return ch;
}

// Upper case ASCII string without copying.
template <template <class, class, class...> class Str, class CharT,
		template <class> class Traits, class... Args>
constexpr void to_upper_ascii_inplace(Str<CharT, Traits<CharT>, Args...>& out) {
	for (CharT& c : out) {
		c = to_upper_ascii(c);
	}
}

// Upper case ASCII string.
template <class Str, class = std::enable_if_t<!std::is_integral_v<Str>>>
[[nodiscard]] auto to_upper_ascii(const Str& str) {
	using CharT = typename detail::str_view<Str>::char_type;
	using Traits = typename detail::str_view<Str>::traits_type;
	std::basic_string<CharT, Traits> ret{ str };

	to_upper_ascii_inplace(ret);
	return ret;
}

/* TODO : Real utf to_upper. */


// Removes any of the leading trim_chars.
template <class CharT>
[[nodiscard]] std::basic_string<CharT> trim_leading(
		const std::basic_string<CharT>& str, const CharT* trim_chars) {
	size_t new_begin = str.find_first_not_of(trim_chars);
	if (new_begin == std::basic_string<CharT>::npos) {
		return {};
	}
	return str.substr(new_begin);
}

// Removes any of the leading characters and returns an appropriately sized
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

// Iterates input string line by line and calls your function with a string_view
// of each line.
// Pass false to 'Strip' template to keep delimiter.
template <bool Strip, class Str, class InCharT, class Func>
void for_each_line(const Str& str, InCharT delim, Func&& func) {
	using CharT = typename detail::str_view<Str>::char_type;
	static_assert(std::is_same_v<CharT, InCharT>,
			"fea::for_each_line : wrong delimiter type");

	auto sv = detail::str_view<Str>{ str }.sv();
	using str_view_t = decltype(sv);

	// TODO : benchmark simd
	const auto* begin = sv.data();
	size_t sub_size = 0;
	for (const auto& c : sv) {
		if (c != delim) {
			++sub_size;
			continue;
		}

		str_view_t line;
		if constexpr (Strip) {
			line = str_view_t{ begin, sub_size };
		} else {
			line = str_view_t{ begin, sub_size + 1 };
		}
		begin = &c + 1;
		sub_size = 0;
		func(line);
	}

	// Leftover that doesn't end with delim.
	if (sub_size != 0) {
		str_view_t line{ begin, sub_size };
		func(line);
	}
}

// Iterates input string line by line and calls your function with a string_view
// of each line. Strips the provided delimiter char.
template <class Str, class InCharT, class Func>
void for_each_line(const Str& str, InCharT delim, Func&& func) {
	for_each_line<true>(str, delim, std::forward<Func>(func));
}

// Iterates input string line by line and calls your function with a string_view
// of each line. Splits with '\n'.
// Enable/disable stripping with Strip template.
template <bool Strip, class Str, class Func>
void for_each_line(const Str& str, Func&& func) {
	using CharT = typename detail::str_view<Str>::char_type;
	constexpr CharT end_ch = FEA_CH('\n');
	for_each_line<Strip>(str, end_ch, std::forward<Func>(func));
}

// Iterates input string line by line and calls your function with a string_view
// of each line. Splits with and strips out '\n'.
template <class Str, class Func>
void for_each_line(const Str& str, Func&& func) {
	for_each_line<true>(str, std::forward<Func>(func));
}


// Get a specific line in string, using line_endings search.
// Returns empty string_view on failure.
template <class Str, class Str2>
auto get_line(const Str& str, size_t line_num, const Str2& line_endings) {
	detail::str_view<Str> str_v{ str };
	using CharT = typename detail::str_view<Str>::char_type;
	detail::str_view<Str2> search_v{ line_endings };

	size_t line = 0;
	size_t prev = 0;
	size_t pos;

	while ((pos = str_v.find(search_v, prev)) != str_v.npos) {
		if (line == line_num) {
			assert(prev < pos);
			return std::basic_string_view<CharT>(
					str_v.data() + prev, pos - prev);
		}

		prev = pos + search_v.size();
		++line;
	}

	return std::basic_string_view<CharT>{};
}

// Get a specific line in string, uses \n for search.
// Returns empty string_view on failure.
template <class Str>
auto get_line(const Str& str, size_t line_num) {
	detail::str_view<Str> str_v{ str };

	using CharT = typename detail::str_view<Str>::char_type;
	const CharT* endings = FEA_LIT("\n");

	return get_line(str, line_num, endings);
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
			ret.append(FEA_LIT("&amp;"));
		} break;
		case FEA_CH('\"'): {
			ret.append(FEA_LIT("&quot;"));
		} break;
		case FEA_CH('\''): {
			ret.append(FEA_LIT("&apos;"));
		} break;
		case FEA_CH('<'): {
			ret.append(FEA_LIT("&lt;"));
		} break;
		case FEA_CH('>'): {
			ret.append(FEA_LIT("&gt;"));
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

// Returns true if the character is an ascii digit.
// std::isdigit is undefined for types that cannot by represented by unsigned
// char.
template <class CharT>
[[nodiscard]] constexpr bool is_digit_ascii(CharT c) {
	return c <= FEA_CH('9') && c >= FEA_CH('0');
}

// Returns true if the string is an ascii number.
template <template <class, class, class...> class Str, class CharT,
		template <class> class Traits, class... Args>
[[nodiscard]] constexpr bool is_number_ascii(
		const Str<CharT, Traits<CharT>, Args...>& str) {
	for (CharT c : str) {
		if (!is_digit_ascii(c)) {
			return false;
		}
	}
	return true;
}
template <class CharT>
[[nodiscard]] constexpr bool is_number_ascii(const CharT* str) {
	size_t i = 0;
	while (CharT c = str[i++]) {
		if (!is_digit_ascii(c)) {
			return false;
		}
	}
	return true;
}

#if FEA_CPP20
// Compares ranges and sorts them lexicographically.
// Given : a, A, aa, AA, b, B, 1
// Produces : 1, A, AA, a, aa, B, b
template <class InputIt1, class InputIt2>
[[nodiscard]] constexpr std::strong_ordering lexicographical_compare(
		InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2) {
	for (; (first1 != last1) && (first2 != last2); ++first1, (void)++first2) {
		// Lower check (alpha grouping).
		auto c1 = fea::to_lower_ascii(*first1);
		auto c2 = fea::to_lower_ascii(*first2);
		if (c1 != c2) {
			return c1 <=> c2;
		}

		// Upper first in identical chars.
		c1 = *first1;
		c2 = *first2;
		if (c1 != c2) {
			return c1 <=> c2;
		}
	}
	return std::distance(first1, last1) <=> std::distance(first2, last2);
}
#endif


// Helper structure which accepts a lambda
// and calls it with a return overloaded string output.
// Return overloads your function with value type string returns.
template <class Func>
struct str_return_overload {
	str_return_overload(Func&& func)
			: _func(std::forward<Func>(func)) {
	}

	operator std::string() const {
		std::string ret;
		_func(ret);
		return ret;
	}
	operator std::wstring() const {
		std::wstring ret;
		_func(ret);
		return ret;
	}
	operator std::u16string() const {
		std::u16string ret;
		_func(ret);
		return ret;
	}
	operator std::u32string() const {
		std::u32string ret;
		_func(ret);
		return ret;
	}

private:
	Func _func;
};


// Helper structure which accepts a lambda
// and calls it with a return overloaded string pointer.
// Return overloads your function with const ref string returns.
template <class Func>
struct str_cr_return_overload {
	str_cr_return_overload(Func&& func)
			: _func(std::forward<Func>(func)) {
	}

	operator const std::string&() const {
		const std::string* ret = nullptr;
		_func(ret);
		return *ret;
	}
	operator const std::wstring&() const {
		const std::wstring* ret = nullptr;
		_func(ret);
		return *ret;
	}
	operator const std::u16string&() const {
		const std::u16string* ret = nullptr;
		_func(ret);
		return *ret;
	}
	operator const std::u32string&() const {
		const std::u32string* ret = nullptr;
		_func(ret);
		return *ret;
	}

private:
	Func _func;
};
} // namespace fea

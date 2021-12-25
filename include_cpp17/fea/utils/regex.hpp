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
#include <regex>
#include <string>
#include <string_view>

namespace fea {

using svmatch = std::match_results<std::string_view::const_iterator>;
using svsub_match = std::sub_match<std::string_view::const_iterator>;
using svregex_iterator = std::regex_iterator<std::string_view::const_iterator>;

using wsvmatch = std::match_results<std::wstring_view::const_iterator>;
using wsvsub_match = std::sub_match<std::wstring_view::const_iterator>;
using wsvregex_iterator
		= std::regex_iterator<std::wstring_view::const_iterator>;

[[nodiscard]] inline std::string_view to_sv(const svsub_match& m) {
	return std::string_view(&(*m.first), m.length());
}

[[nodiscard]] inline std::wstring_view to_wsv(const wsvsub_match& m) {
	return std::wstring_view(&(*m.first), m.length());
}


template <class CharT, class Traits>
bool regex_match(const std::basic_string_view<CharT>& s,
		std::match_results<
				typename std::basic_string_view<CharT>::const_iterator>& m,
		const std::basic_regex<CharT, Traits>& e,
		std::regex_constants::match_flag_type flags
		= std::regex_constants::match_default) {
	return std::regex_match(s.begin(), s.end(), m, e, flags);
}

template <class CharT, class Traits>
bool regex_match(const std::basic_string_view<CharT>& s,
		const std::basic_regex<CharT, Traits>& e,
		std::regex_constants::match_flag_type flags
		= std::regex_constants::match_default) {
	return std::regex_match(s.begin(), s.end(), e, flags);
}

template <class CharT, class Traits>
bool regex_match(const std::basic_string_view<CharT>&&,
		std::match_results<
				typename std::basic_string_view<CharT>::const_iterator>&,
		const std::basic_regex<CharT, Traits>&,
		std::regex_constants::match_flag_type flags
		= std::regex_constants::match_default)
		= delete;


template <class CharT, class Traits>
bool regex_search(const std::basic_string_view<CharT>& s,
		std::match_results<
				typename std::basic_string_view<CharT>::const_iterator>& m,
		const std::basic_regex<CharT, Traits>& e,
		std::regex_constants::match_flag_type flags
		= std::regex_constants::match_default) {
	return std::regex_search(s.begin(), s.end(), m, e, flags);
}
template <class STraits, class SAlloc, class CharT, class Traits>
bool regex_search(const std::basic_string_view<CharT>& s,
		const std::basic_regex<CharT, Traits>& e,
		std::regex_constants::match_flag_type flags
		= std::regex_constants::match_default) {
	return std::regex_search(s.begin(), s.end(), e, flags);
}

template <class CharT, class Traits>
bool regex_search(const std::basic_string_view<CharT>&&,
		std::match_results<
				typename std::basic_string_view<CharT>::const_iterator>&,
		const std::basic_regex<CharT, Traits>&,
		std::regex_constants::match_flag_type flags
		= std::regex_constants::match_default)
		= delete;

// TODO : return std::string, use output iterator overload?
//
// template <class Traits, class CharT>
// std::basic_string<CharT> regex_replace(const std::basic_string_view<CharT>&
// s, 		const std::basic_regex<CharT, Traits>& re, 		const
// std::basic_string_view<CharT>& fmt, std::regex_constants::match_flag_type
// flags 		= std::regex_constants::match_default) { 	return
// std::regex_replace(s.begin(), s.end(), re, fmt, flags);
//}
//
// template <class Traits, class CharT>
// std::basic_string<CharT> regex_replace(const std::basic_string_view<CharT>&
// s, 		const std::basic_regex<CharT, Traits>& re, const CharT* fmt,
//		std::regex_constants::match_flag_type flags
//		= std::regex_constants::match_default) {
//	std::basic_string<CharT> ret;
//	return std::regex_replace(s.begin(), s.end(), re, fmt, flags);
//}
} // namespace fea

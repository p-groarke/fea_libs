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
#include <string>
#include <string_view>

namespace fea {

// Replaces all 'search' occurences with 'replace'.
// Modifies string out.
template <template <class, class, class...> class Str,
		template <class, class, class...> class Str2, class CharT,
		template <class> class Traits, class... Args, class... Args2,
		class... Args3>
void replace_all_inplace(std::basic_string<CharT, Traits<CharT>, Args...>& out,
		const Str<CharT, Traits<CharT>, Args2...>& search,
		const Str2<CharT, Traits<CharT>, Args3...>& replace) {
	auto pos = out.find(search.data(), 0, search.size());
	while (pos != out.npos) {
		out.replace(pos, search.size(), replace.data(), replace.size());
		pos = out.find(search.data(), pos + replace.size(), search.size());
	}
}

// Replaces all 'search' occurences with 'replace'.
// Modifies 'out' string.
template <class CharT, template <class> class Traits, class... Args>
void replace_all_inplace(std::basic_string<CharT, Traits<CharT>, Args...>& out,
		const CharT* search, const CharT* replace) {
	using sv = std::basic_string_view<CharT, Traits<CharT>>;
	replace_all_inplace(out, sv{ search }, sv{ replace });
}

// Replaces all 'search' occurences with 'replace'.
// Modifies 'out' string.
template <template <class, class, class...> class Str, class CharT,
		template <class> class Traits, class... Args, class... Args2>
void replace_all_inplace(std::basic_string<CharT, Traits<CharT>, Args...>& out,
		const Str<CharT, Traits<CharT>, Args2...>& search,
		const CharT* replace) {
	using sv = std::basic_string_view<CharT, Traits<CharT>>;
	replace_all_inplace(out, search, sv{ replace });
}

// Replaces all 'search' occurences with 'replace'.
// Modifies 'out' string.
template <template <class, class, class...> class Str, class CharT,
		template <class> class Traits, class... Args, class... Args2>
void replace_all_inplace(std::basic_string<CharT, Traits<CharT>, Args...>& out,
		const CharT* search,
		const Str<CharT, Traits<CharT>, Args2...>& replace) {
	using sv = std::basic_string_view<CharT, Traits<CharT>>;
	replace_all_inplace(out, sv{ search }, replace);
}

// Replaces all 'search' occurences with 'replace'.
// Returns modified string.
template <template <class, class, class...> class Str,
		template <class, class, class...> class Str2,
		template <class, class, class...> class Str3, class CharT,
		template <class> class Traits, class... Args, class... Args2,
		class... Args3>
[[nodiscard]] std::basic_string<CharT, Traits<CharT>, Args...> replace_all(
		const Str<CharT, Traits<CharT>, Args...>& str,
		const Str2<CharT, Traits<CharT>, Args2...>& search,
		const Str3<CharT, Traits<CharT>, Args3...>& replace) {
	std::basic_string<CharT, Traits<CharT>, Args...> ret{ str };
	replace_all_inplace(ret, search, replace);
	return ret;
}

// Replaces all 'search' occurences with 'replace'.
// Returns modified string.
template <template <class, class, class...> class Str, class CharT,
		template <class> class Traits, class... Args>
[[nodiscard]] std::basic_string<CharT, Traits<CharT>, Args...> replace_all(
		const Str<CharT, Traits<CharT>, Args...>& str, const CharT* search,
		const CharT* replace) {
	std::basic_string<CharT, Traits<CharT>, Args...> ret{ str };
	replace_all_inplace(ret, search, replace);
	return ret;
}

// Replaces all 'search' occurences with 'replace'.
// Returns modified string.
template <template <class, class, class...> class Str,
		template <class, class, class...> class Str2, class CharT,
		template <class> class Traits, class... Args, class... Args2>
[[nodiscard]] std::basic_string<CharT, Traits<CharT>, Args...> replace_all(
		const Str<CharT, Traits<CharT>, Args...>& str,
		const Str2<CharT, Traits<CharT>, Args2...>& search,
		const CharT* replace) {
	std::basic_string<CharT, Traits<CharT>, Args...> ret{ str };
	replace_all_inplace(ret, search, replace);
	return ret;
}

// Replaces all 'search' occurences with 'replace'.
// Returns modified string.
template <template <class, class, class...> class Str,
		template <class, class, class...> class Str2, class CharT,
		template <class> class Traits, class... Args, class... Args2>
[[nodiscard]] std::basic_string<CharT, Traits<CharT>, Args...> replace_all(
		const Str<CharT, Traits<CharT>, Args...>& str, const CharT* search,
		const Str2<CharT, Traits<CharT>, Args2...>& replace) {
	std::basic_string<CharT, Traits<CharT>, Args...> ret{ str };
	replace_all_inplace(ret, search, replace);
	return ret;
}


} // namespace fea

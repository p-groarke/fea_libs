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
#include <cassert>
#include <string>
#include <string_view>

namespace fea {
namespace detail {
template <class>
struct str_view;

template <class CharT, class Traits>
struct str_view_base {
	static constexpr size_t npos = std::basic_string_view<CharT>::npos;

	using char_type = CharT;
	using traits_type = Traits;

	template <class Str>
	constexpr str_view_base(const Str& in) noexcept
			: _sv(in) {
	}

	[[nodiscard]] constexpr auto begin() const noexcept {
		return _sv.begin();
	}
	[[nodiscard]] constexpr auto cbegin() const noexcept {
		return _sv.begin();
	}

	[[nodiscard]] constexpr auto end() const noexcept {
		return _sv.end();
	}
	[[nodiscard]] constexpr auto cend() const noexcept {
		return _sv.end();
	}

	[[nodiscard]] constexpr const CharT* data() const noexcept {
		return _sv.data();
	}

	[[nodiscard]] constexpr auto sv() const noexcept {
		return _sv;
	}

	[[nodiscard]] constexpr size_t size() const noexcept {
		return _sv.size();
	}

	template <class InStr>
	[[nodiscard]] constexpr size_t find(
			str_view<InStr> search, size_t pos = 0) const noexcept {
		return _sv.find(search.sv(), pos);
	}

	template <class InStr>
	[[nodiscard]] constexpr size_t find_first_of(
			str_view<InStr> search, size_t pos = 0) const noexcept {
		return _sv.find_first_of(search.sv(), pos);
	}

	template <class InStr>
	[[nodiscard]] constexpr bool starts_with(
			str_view<InStr> search) const noexcept {
		return size() >= search.size()
				&& traits_type::compare(data(), search.data(), search.size())
				== 0;
	}

	template <class InStr>
	[[nodiscard]] constexpr bool ends_with(
			str_view<InStr> search) const noexcept {
		return size() >= search.size()
				&& traits_type::compare(data() + (size() - search.size()),
						   search.data(), search.size())
				== 0;
	}

protected:
	std::basic_string_view<char_type, traits_type> _sv;
};

// Only specialization that doesn't inherit base (single char special
// snowflake).
template <class CharT>
struct str_view {
	static constexpr size_t npos = std::basic_string_view<CharT>::npos;

	using is_char = std::true_type;
	using is_ptr = std::false_type;
	using char_type = CharT;
	using traits_type = std::char_traits<CharT>;

	constexpr str_view(CharT ch) noexcept
			: _char(ch) {
	}

	[[nodiscard]] constexpr const CharT* begin() const noexcept {
		return data();
	}
	[[nodiscard]] constexpr const CharT* cbegin() const noexcept {
		return data();
	}

	[[nodiscard]] constexpr const CharT* end() const noexcept {
		return begin() + 1;
	}
	[[nodiscard]] constexpr const CharT* cend() const noexcept {
		return cbegin() + 1;
	}

	[[nodiscard]] constexpr const CharT* data() const noexcept {
		return &_char;
	}

	[[nodiscard]] constexpr CharT sv() const noexcept {
		return _char;
	}

	[[nodiscard]] constexpr size_t size() const noexcept {
		return 1;
	}

	template <class InStr>
	[[nodiscard]] constexpr size_t find(
			str_view<InStr> search, size_t pos = 0) const noexcept {
		// This is a char, only valid pos is 0.
		assert(pos == 0);

		if (size() >= search.size() && _char == search.data()[0]) {
			return 0;
		}
		return npos;
	}

	template <class InStr>
	[[nodiscard]] constexpr size_t find_first_of(
			str_view<InStr> search, size_t pos = 0) const noexcept {
		for (auto c : search) {
			if (_char == c) {
				return 0;
			}
		}
		return npos;
	}

	template <class InStr>
	[[nodiscard]] constexpr bool starts_with(
			str_view<InStr> search) const noexcept {
		return size() >= search.size() && _char == search.data()[0];
	}

	template <class InStr>
	[[nodiscard]] constexpr bool ends_with(
			str_view<InStr> search) const noexcept {
		return size() >= search.size() && _char == search.data()[0];
	}

private:
	CharT _char;
};

template <class CharT>
struct str_view<const CharT*> : str_view_base<CharT, std::char_traits<CharT>> {
	using is_char = std::false_type;
	using is_ptr = std::true_type;

	constexpr str_view(const CharT* str) noexcept
			: str_view_base<CharT, std::char_traits<CharT>>(str) {
	}
};

template <class CharT, size_t N>
struct str_view<CharT[N]> : str_view_base<CharT, std::char_traits<CharT>> {
	using is_char = std::false_type;
	using is_ptr = std::true_type;

	constexpr str_view(const CharT str[N]) noexcept
			: str_view_base<CharT, std::char_traits<CharT>>(str) {
	}
};

template <template <class, class> class Str, class CharT,
		template <class> class Traits>
struct str_view<Str<CharT, Traits<CharT>>>
		: str_view_base<CharT, Traits<CharT>> {
	using is_char = std::false_type;
	using is_ptr = std::false_type;

	constexpr str_view(const Str<CharT, Traits<CharT>>& str) noexcept
			: str_view_base<CharT, Traits<CharT>>(str) {
	}
};

template <template <class, class, class...> class Str, class CharT,
		template <class> class Traits, class... Extra>
struct str_view<Str<CharT, Traits<CharT>, Extra...>>
		: str_view_base<CharT, Traits<CharT>> {
	using is_char = std::false_type;
	using is_ptr = std::false_type;

	constexpr str_view(const Str<CharT, Traits<CharT>, Extra...>& str) noexcept
			: str_view_base<CharT, Traits<CharT>>(str) {
	}
};

} // namespace detail
} // namespace fea

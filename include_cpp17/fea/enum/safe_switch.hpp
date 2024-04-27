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
#include "fea/containers/type_map.hpp"
#include "fea/meta/traits.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <tuple>

/*
fea::safe_switch creates a switch-case that will fail (wont compile) if you add
or remove values to an enum.
*/

namespace fea {
namespace detail {
template <class Enum, size_t N, class Funcs, Enum... Es>
struct switcher;

template <class Enum, size_t N, class... Funcs, Enum... Es>
struct switcher<Enum, N, std::tuple<Funcs...>, Es...> {

	constexpr switcher() = default;
	constexpr switcher(std::tuple<Funcs...>&& funcs)
			: _funcs(std::move(funcs)) {
	}

	template <Enum E, class NewFunc>
	constexpr auto case_(NewFunc&& func) const&& {
		return switcher<Enum, N, std::tuple<Funcs..., NewFunc>, Es..., E>{
			std::tuple_cat(std::move(_funcs.data()),
					std::make_tuple(std::forward<NewFunc>(func)))
		};
	}

	template <Enum E>
	constexpr auto empty() const&& {
		auto l = []() {};
		return switcher<Enum, N, std::tuple<Funcs..., decltype(l)>, Es..., E>{
			std::tuple_cat(std::move(_funcs.data()), std::make_tuple(l))
		};
	}

	constexpr void operator()(Enum e) const {
		static_assert(
				sizeof...(Es) == N, "safe_switch : missing case statement");

#if FEA_DEBUG
		constexpr std::array<Enum, N> arr{ Es... };
		auto it = std::find(arr.begin(), arr.end(), e);
		assert(it != arr.end());
#endif

		bool found = false;
		fea::static_for<N>([&, this](auto e_idx) {
			if (found) {
				return;
			}

			if (e_idx == size_t(e)) {
				found = true;
				return _funcs.template find<Enum(e_idx())>()();
			}
		});
	}


private:
	fea::type_map<fea::pack_nt<Es...>, Funcs...> _funcs;
};
} // namespace detail


// Won't compile if missing a case statement.
// Deduces the size if the enum contains a 'count' member.
template <class Enum, size_t N = size_t(Enum::count)>
constexpr auto safe_switch() {
	return detail::switcher<Enum, N, std::tuple<>>{};
}
} // namespace fea

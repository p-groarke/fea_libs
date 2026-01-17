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
#pragma once
#include "fea/meta/traits.hpp"
#include "fea/numerics/numerics.hpp"
// #include "fea/performance/constants.hpp"
// #include "fea/performance/tls.hpp"

#include <array>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <vector>

namespace fea {
/*
A collection of sorts.
*/

namespace detail {
template <class IndexT>
struct radix_data {
	std::array<IndexT, 256> counts{};
	std::array<IndexT, 256> jmp_table;
};
} // namespace detail

// Radix sort.
// Thread-safe.
// Allocates memory to fit 'count' items.
//
// Also allocates histograms and extra required data on first call from thread.
// Subsequent thread calls do not (uses a thread-safe cache when possible).
template <class FwdIt>
void radix_sort(FwdIt first, FwdIt last) {
	static_assert(std::is_base_of_v<std::forward_iterator_tag,
						  fea::iterator_category_t<FwdIt>>,
			"Iterators must at least be forward iterators.");

	using value_t = fea::iterator_value_t<FwdIt>;
	static_assert(std::is_arithmetic_v<value_t>,
			"Radix sort only works on arithmetic types.");

	size_t count = size_t(std::distance(first, last));
	if (count <= 1) {
		return;
	}

	using index_t = fea::best_index_t<value_t>;
	detail::radix_data<index_t> rad_data{};

	std::vector<value_t> sorted_values(count);
	if constexpr (std::is_unsigned_v<value_t>) {
		for (FwdIt it = first; it != last; ++it) {
			rad_data.counts[index_t(*it)]++;
		}

		rad_data.jmp_table[0] = 0;
		for (size_t i = 1; i < rad_data.jmp_table.size(); ++i) {
			rad_data.jmp_table[i] = rad_data.jmp_table[i - 1] + rad_data.counts[i - 1];
		}

		for (FwdIt it = first; it != last; ++it) {
			index_t off = rad_data.jmp_table[index_t(*it)]++;
			sorted_values[off] = *it;
		}
	} else {
		// TODO : handle the signed and floats.
		assert(false);
	}

	std::copy(sorted_values.begin(), sorted_values.end(), first);
}
} // namespace fea

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
#include "fea/meta/static_for.hpp"
#include "fea/meta/traits.hpp"
#include "fea/numerics/numerics.hpp"
#include "fea/performance/intrinsics.hpp"
#include "fea/utility/error.hpp"
// #include "fea/performance/constants.hpp"
#include "fea/performance/tls.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <iterator>
#include <limits>
#include <numeric>
#include <vector>

namespace fea {
/*
A collection of sorts.
*/

// Radix sort.
// Thread-safe.
// Allocates thread caches on first call only.
template <class FwdIt>
void radix_sort(FwdIt first, FwdIt last);
} // namespace fea


// Implementation
namespace fea {
namespace detail {
template <class IndexT>
struct radix_data {
	std::array<std::array<IndexT, 256>, sizeof(void*)> counts{};
	std::array<std::array<IndexT, 256>, sizeof(void*)> jmp_table; // uninit
};

// Caches, 1 per index type * number of caller threads.
inline fea::tls<radix_data<uint8_t>> radix_data_cache8;
inline fea::tls<radix_data<uint16_t>> radix_data_cache16;
inline fea::tls<radix_data<uint32_t>> radix_data_cache32;
inline fea::tls<radix_data<uint64_t>> radix_data_cache64;

template <class IndexT, class FwdIt>
void radix_sort(
		FwdIt first, FwdIt last, size_t count, radix_data<IndexT>& rad_data) {
	using value_t = fea::iterator_value_t<FwdIt>;

	// Reset counts.
	// for (size_t pass_idx = 0; pass_idx < sizeof(value_t); ++pass_idx) {
	fea::static_for<sizeof(value_t)>([&](auto const_pass_idx) {
		constexpr size_t pass_idx = const_pass_idx;
		std::get<pass_idx>(rad_data.counts) = {};
	});
	//}

	if constexpr (std::is_unsigned_v<value_t>) {
		// Compute counters / histograms.
		// Performance Q : Faster to loop on each counter?
		for (FwdIt it = first; it != last; ++it) {
			const uint8_t* radixes_ptr
					= reinterpret_cast<const uint8_t*>(&(*it));

			fea::static_for<sizeof(value_t)>([&](auto const_pass_idx) {
				constexpr size_t pass_idx = const_pass_idx;
				std::array<IndexT, 256>& counts
						= std::get<pass_idx>(rad_data.counts);
				counts[radixes_ptr[pass_idx]]++;
			});
		}

		// Compute offsets / jump tables / lookup tables.
		// for (size_t pass_idx = 0; pass_idx < sizeof(value_t); ++pass_idx) {
		fea::static_for<sizeof(value_t)>([&](auto const_pass_idx) {
			constexpr size_t pass_idx = const_pass_idx;
			const std::array<IndexT, 256>& counts
					= std::get<pass_idx>(rad_data.counts);
			std::array<IndexT, 256>& jmp_table
					= std::get<pass_idx>(rad_data.jmp_table);
			jmp_table[0] = IndexT(0);
			for (size_t i = 1; i < jmp_table.size(); ++i) {
				jmp_table[i] = jmp_table[i - 1] + counts[i - 1];
			}
		});
		//}

		// TODO : fea::static_for
		// Performance Q : To simplify and minimize allocations, we copy
		// values to a temporary scratch storage.
		// Another technique is to use 2 vectors of indices and work with those
		// instead. This will still incur the value copy at the very end,
		// but maybe it accelerates everything since indexes can be very small.
		std::vector<value_t> scratch(count);
		FwdIt write_first;
		FwdIt write_last;
		// for (size_t pass_idx = 0; pass_idx < sizeof(value_t); ++pass_idx) {
		fea::static_for<sizeof(value_t)>([&](auto const_pass_idx) {
			constexpr size_t pass_idx = const_pass_idx;
			std::array<IndexT, 256>& jmp_table
					= std::get<pass_idx>(rad_data.jmp_table);

			FwdIt read_first = (pass_idx % 2) == 0 ? first : scratch.begin();
			FwdIt read_last = (pass_idx % 2) == 0 ? last : scratch.end();

			write_first = (pass_idx % 2) == 0 ? scratch.begin() : first;
			write_last = (pass_idx % 2) == 0 ? scratch.end() : last;

			assert(&(*read_first) != &(*write_first));
			for (FwdIt it = read_first; it != read_last; ++it) {
				const uint8_t* radixes_ptr
						= reinterpret_cast<const uint8_t*>(&(*it));
				size_t off = size_t(jmp_table[radixes_ptr[pass_idx]]++);
				FwdIt cpy_it = std::next(write_first, off);
				*cpy_it = *it;
			}
		});
		//}

		if constexpr (sizeof(value_t) == 1) {
			// We were a char (non-even byte size).
			// The output is in the wrong storage.
			assert(&(*write_first) != &(*first));
			std::copy(write_first, write_last, first);
		}
	} else {
		// TODO : handle the signed and floats.
		fea::maybe_throw(__FUNCTION__, __LINE__, "TODO");
	}
}
} // namespace detail

// Radix sort.
// Thread-safe.
// Allocates memory to fit 'count' items.
//
// Also allocates histograms and extra required data on first call from thread.
// Subsequent thread calls do not (uses a thread-safe cache when possible).
template <class FwdIt>
void radix_sort(FwdIt first, FwdIt last) {
	static_assert(
			std::is_unsigned_v<size_t>, "Unsupported c++ implementation.");
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

	// Dispatch to the most appropriate index type to optimize memory usage.
	// Overall, we'll use more memory in total, since we have 1 cache per index
	// size. However for any given sort, the memory will be as compressed as
	// possible, accelerating the loops.
	// Absolutely take the tradeoff!
	if (count < size_t((std::numeric_limits<uint8_t>::max)())) {
		using index_t = uint8_t;
		fea::tls_lock<detail::radix_data<index_t>> lock
				= detail::radix_data_cache8.lock();
		return detail::radix_sort<index_t>(first, last, count, lock.local());
	}
	if (count < size_t((std::numeric_limits<uint16_t>::max)())) {
		using index_t = uint16_t;
		fea::tls_lock<detail::radix_data<index_t>> lock
				= detail::radix_data_cache16.lock();
		return detail::radix_sort<index_t>(first, last, count, lock.local());
	}
	if (count < size_t((std::numeric_limits<uint32_t>::max)())) {
		using index_t = uint32_t;
		fea::tls_lock<detail::radix_data<index_t>> lock
				= detail::radix_data_cache32.lock();
		return detail::radix_sort<index_t>(first, last, count, lock.local());
	}

	static_assert(sizeof(void*) <= 8, "TODO : Update if statement + cache.");
	using index_t = uint64_t;
	fea::tls_lock<detail::radix_data<index_t>> lock
			= detail::radix_data_cache64.lock();
	return detail::radix_sort<index_t>(first, last, count, lock.local());
}
} // namespace fea

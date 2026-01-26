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
#include "fea/performance/tls.hpp"
#include "fea/utility/error.hpp"
#include "fea/utility/platform.hpp"

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
// Radix sort.
// Values pointed to must be arithmetic (integers or floats).
// Performance wise : unsigned > signed > floats
//
// Thread-safe.
// Allocates thread caches on first call.
// Allocates values every call.
template <class FwdIt>
void radix_sort(FwdIt first, FwdIt last);

// Radix sort.
// See above for more details.
//
// This overload sorts indexes that are provided through first_idx and last_idx.
// The indexes must be comprised of values from 0-N, but don't need to be
// pre-sorted.
template <class FwdIt, class FwdIt2>
void radix_sort_idxes(
		FwdIt first, FwdIt last, FwdIt2 first_idx, FwdIt2 last_idx);

//// Radix sort.
//// This overload uses a getter callback to evaluate values.
//// Callback must return an arithmetic type to sort.
//// Callback may be called multiple times.
//// See 'radix_sort' for more details.
// template <class FwdIt, class Getter>
// void radix_sort(FwdIt first, FwdIt last, Getter&& get_func);
} // namespace fea


// Implementation
namespace fea {
namespace detail {
template <class IndexT>
struct radix_data {
	using index_t = IndexT;
	std::array<std::array<IndexT, 256>, sizeof(void*)> counts{};
	std::array<std::array<IndexT, 256>, sizeof(void*)> jmp_table; // uninit
};

// Caches, 1 per index type * number of caller threads.
inline fea::tls<radix_data<uint8_t>> radix_data_cache8;
inline fea::tls<radix_data<uint16_t>> radix_data_cache16;
inline fea::tls<radix_data<uint32_t>> radix_data_cache32;
#if FEA_ARCH >= 64
inline fea::tls<radix_data<uint64_t>> radix_data_cache64;
#endif

// Checks basics before running sort.
template <class FwdIt>
void radix_checks() {
	static_assert(
			std::is_unsigned_v<size_t>, "Unsupported c++ implementation.");
	static_assert(std::is_base_of_v<std::forward_iterator_tag,
						  fea::iterator_category_t<FwdIt>>,
			"Iterators must at least be forward iterators.");

	using value_t = fea::iterator_value_t<FwdIt>;
	static_assert(std::is_arithmetic_v<value_t>,
			"Radix sort only works on arithmetic types.");
}

// Returns the right cache which uses an unsigned type able to fit 'count'
// elements.
template <class Func>
auto radix_get_tls_cache(size_t count, Func&& func) {
	if (count < size_t((std::numeric_limits<uint8_t>::max)())) {
		fea::tls_lock<detail::radix_data<uint8_t>> lock
				= detail::radix_data_cache8.lock();
		return func(lock.local());
	} else if (count < size_t((std::numeric_limits<uint16_t>::max)())) {
		fea::tls_lock<detail::radix_data<uint16_t>> lock
				= detail::radix_data_cache16.lock();
		return func(lock.local());
	} else if (count < size_t((std::numeric_limits<uint32_t>::max)())) {
		fea::tls_lock<detail::radix_data<uint32_t>> lock
				= detail::radix_data_cache32.lock();
		return func(lock.local());
	}
#if FEA_ARCH >= 64
	else if (count < size_t((std::numeric_limits<uint64_t>::max)())) {
		static_assert(
				sizeof(uintptr_t) <= 8, "TODO : Update if statement + cache.");
		fea::tls_lock<detail::radix_data<uint64_t>> lock
				= detail::radix_data_cache64.lock();
		return func(lock.local());
	}
#endif

	// Should never get here.
	fea::maybe_throw<std::invalid_argument>(
			__FUNCTION__, __LINE__, "Something went horribly wrong.");
}

// Returns true if presorted.
template <class FwdIt, class IndexT>
bool radix_precompute(
		FwdIt first, FwdIt last, size_t, radix_data<IndexT>* rad_data_ptr) {
	radix_data<IndexT>& rad_data = *rad_data_ptr;
	using value_t = fea::iterator_value_t<FwdIt>;

	// Reset counts.
	fea::static_for<sizeof(value_t)>([&](auto const_pass_idx) {
		constexpr size_t pass_idx = const_pass_idx;
		std::get<pass_idx>(rad_data.counts) = {};
	});

	// Compute counters / histograms.
	// Performance : Much faster to loop on values only once.
	{
		bool pre_sorted = true;
		value_t prev_val = *first;
		for (FwdIt it = first; it != last; ++it) {
			const value_t& val = *it;
			const uint8_t* radixes_ptr
					= reinterpret_cast<const uint8_t*>(&(val));

			if (pre_sorted) {
				if (prev_val > val) {
					pre_sorted = false;
				} else {
					prev_val = val;
				}
			}

			fea::static_for<sizeof(value_t)>([&](auto const_pass_idx) {
				constexpr size_t pass_idx = const_pass_idx;
				std::array<IndexT, 256>& counts
						= std::get<pass_idx>(rad_data.counts);
				++counts[radixes_ptr[pass_idx]];
			});
		}

		if (pre_sorted) {
			return true;
		}
	}

	// Compute offsets / jump tables / lookup tables.
	fea::static_for<sizeof(value_t)>([&](auto const_pass_idx) {
		constexpr size_t pass_idx = const_pass_idx;
		const std::array<IndexT, 256>& counts
				= std::get<pass_idx>(rad_data.counts);
		std::array<IndexT, 256>& jmp_table
				= std::get<pass_idx>(rad_data.jmp_table);

		if constexpr (std::is_signed_v<value_t> && std::is_integral_v<value_t>
					  && pass_idx == sizeof(value_t) - 1) {
			// Signed integer at last pass, fix lookups.
			// Negatives (128+) must start at offset zero, positives after.
			jmp_table[128] = 0;
			for (size_t i = 129; i < 256; ++i) {
				jmp_table[i] = jmp_table[i - 1] + counts[i - 1];
			}

			jmp_table[0] = jmp_table.back() + counts.back();
			for (size_t i = 1; i < 128; ++i) {
				jmp_table[i] = jmp_table[i - 1] + counts[i - 1];
			}
		} else if constexpr (std::is_floating_point_v<value_t>
							 && pass_idx == sizeof(value_t) - 1) {
			// Floats at last pass, fix lookups.
			// Negatives (127+) must start at offset zero, positives after.
			// We also must reverse the order of negatives.

			// Like singed ints, store the negatives before positives.
			jmp_table[255] = 0;
			for (size_t i = 254; i > 127; --i) {
				jmp_table[i] = jmp_table[i + 1] + counts[i + 1];
			}
			jmp_table[0] = jmp_table[128] + counts[128];
			for (size_t i = 1; i < 128; ++i) {
				jmp_table[i] = jmp_table[i - 1] + counts[i - 1];
			}

			// In our sort loop, we'll flip the order of final radixes.
			// To do that, we need the first hit count to be the last.
			for (size_t i = 128; i < 256; ++i) {
				jmp_table[i] += counts[i];
			}

		} else {
			// Straightforward lookups.
			jmp_table[0] = IndexT(0);
			for (size_t i = 1; i < 256; ++i) {
				jmp_table[i] = jmp_table[i - 1] + counts[i - 1];
			}
		}
	});

	return false;
}

// Notes :
// Signed negative values are at the wrong position but correct order.
// Float negative values are at the wrong position and worng order.
template <class FwdIt, class IndexT>
 __declspec(noinline) void radix_sort(
		FwdIt first, FwdIt last, size_t count, radix_data<IndexT>& rad_data) {
	using value_t = fea::iterator_value_t<FwdIt>;

	if (fea::detail::radix_precompute(first, last, count, &rad_data)) {
		// pre-sorted
		return;
	}

	// Performance Q : To simplify and minimize allocations, we copy
	// values to a temporary scratch storage.
	// Another technique is to use 2 vectors of indices and work with those
	// instead. This will still incur the value copy at the very end,
	// but maybe it accelerates everything since indexes can be very small.
	std::vector<value_t> scratch(count);
	FwdIt write_first;
	FwdIt write_last;
	fea::static_for<sizeof(value_t)>([&](auto const_pass_idx)  __declspec(noinline) {
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
			const uint8_t& radix = radixes_ptr[pass_idx];

			ptrdiff_t off;
			if constexpr (std::is_floating_point_v<value_t>
						  && pass_idx == sizeof(value_t) - 1) {
				if (radix < 128) {
					off = ptrdiff_t(jmp_table[radix]++);
				} else {
					// Dealing with negative floats,
					// the lookup is prepped from last to first.
					off = ptrdiff_t(--jmp_table[radix]);
				}
			} else {
				off = ptrdiff_t(jmp_table[radix]++);
			}

			FwdIt cpy_it = std::next(write_first, off);
			*cpy_it = *it;
		}
	});

	if constexpr (sizeof(value_t) == 1) {
		// We were a char (non-even byte size).
		// The output is in the wrong storage.
		assert(&(*write_first) != &(*first));
		std::copy(write_first, write_last, first);
	}
}

template <class FwdIt, class FwdIt2, class IndexT>
void radix_sort(FwdIt first, FwdIt last, FwdIt2 idx_first,
		[[maybe_unused]] FwdIt2 idx_last,
		size_t count, radix_data<IndexT>& rad_data) {
	using value_t = fea::iterator_value_t<FwdIt>;
	using user_index_t = fea::iterator_value_t<FwdIt2>;

	if (fea::detail::radix_precompute(first, last, count, &rad_data)) {
		// pre-sorted
		return;
	}

#if 1
	// Optimize TLB thrashing.
	struct data {
		IndexT idx1;
		IndexT idx2;
		//value_t value;
	};

	std::vector<data> idxes(count);

	// Initialize compute data.
	// We'll alternate between indexes in it.
	{
		auto it = first;
		for (size_t i = 0; i < count; ++i, ++it) {
			idxes[i].idx1 = IndexT(i);
			//data_vec[i].value = *it;
		}
	}

	fea::static_for<sizeof(value_t)>(
			[&](auto const_pass_idx) __declspec(noinline) {
				constexpr size_t pass_idx = const_pass_idx;
				std::array<IndexT, 256>& jmp_table
				= std::get<pass_idx>(rad_data.jmp_table);

				auto read_idx = []() {};
				auto write_idx = []() {};

		//// Flip-flop our index containers.
		//auto read_first = (pass_idx % 2) == 0 ? idxes1.begin() : idxes2.begin();
		//auto read_last = (pass_idx % 2) == 0 ? idxes1.end() : idxes2.end();
		//write_first = (pass_idx % 2) == 0 ? idxes2.begin() : idxes1.begin();
		//write_last = (pass_idx % 2) == 0 ? idxes2.end() : idxes1.end();
		//assert(&(*read_first) != &(*write_first));

		for (data& d : idxes) {
			IndexT&;

			// Get our pointed too value.
			IndexT idx = *idx_it;
			auto it = std::next(first, size_t(idx));
			const uint8_t* radixes_ptr
					= reinterpret_cast<const uint8_t*>(&(*it));
			const uint8_t& radix = radixes_ptr[pass_idx];

			// Compute the position it should be in.
			// Also increment / decrement counters for that radix.
			IndexT off;
			if constexpr (std::is_floating_point_v<value_t>
						  && pass_idx == sizeof(value_t) - 1) {
				if (radix < 128) {
					off = jmp_table[radix]++;
				} else {
					// Dealing with negative floats,
					// the lookup is prepped from last to first.
					off = --jmp_table[radix];
				}
			} else {
				off = jmp_table[radix]++;
			}

			// At position 'off', we should be item 'idx'.
			auto idx_cpy_it = std::next(write_first, ptrdiff_t(off));
			*idx_cpy_it = idx;
		}
	});

	// And finally, copy our indexes to the output.
	if constexpr (sizeof(value_t) == 1) {
		// We were a char (non-even byte size).
		// The output is in the wrong storage.
		assert(&(*write_first) != &(*idxes1.begin()));
		//std::copy(write_first, write_last, idxes1.begin());
		for (const IndexT& idx : idxes2)
		{
			assert(idx_first != idx_last);
			*idx_first = user_index_t(idx);
			++idx_first;
		}

	} else {
		assert(&(*write_first) != &(*idxes2.begin()));
		for (const IndexT& idx : idxes1)
		{
			assert(idx_first != idx_last);
			*idx_first = user_index_t(idx);
			++idx_first;
		}
	}
#else
	// 2 vectors of indexes we'll alternate between.
	// We don't use the user index type, we use are (potentially) smaller type.
	std::vector<IndexT> idxes1(count);
	std::iota(idxes1.begin(), idxes1.end(), IndexT(0));
	std::vector<IndexT> idxes2(count);

	typename std::vector<IndexT>::iterator write_first;
	typename std::vector<IndexT>::iterator write_last;
	fea::static_for<sizeof(value_t)>(
			[&](auto const_pass_idx) __declspec(noinline) {
				constexpr size_t pass_idx = const_pass_idx;
				std::array<IndexT, 256>& jmp_table
				= std::get<pass_idx>(rad_data.jmp_table);

		// Flip-flop our index containers.
		auto read_first = (pass_idx % 2) == 0 ? idxes1.begin() : idxes2.begin();
		auto read_last = (pass_idx % 2) == 0 ? idxes1.end() : idxes2.end();
		write_first = (pass_idx % 2) == 0 ? idxes2.begin() : idxes1.begin();
		write_last = (pass_idx % 2) == 0 ? idxes2.end() : idxes1.end();
		assert(&(*read_first) != &(*write_first));

		for (auto idx_it = read_first; idx_it != read_last; ++idx_it) {
			// Get our pointed too value.
			IndexT idx = *idx_it;
			auto it = std::next(first, size_t(idx));
			const uint8_t* radixes_ptr
					= reinterpret_cast<const uint8_t*>(&(*it));
			const uint8_t& radix = radixes_ptr[pass_idx];

			// Compute the position it should be in.
			// Also increment / decrement counters for that radix.
			IndexT off;
			if constexpr (std::is_floating_point_v<value_t>
						  && pass_idx == sizeof(value_t) - 1) {
				if (radix < 128) {
					off = jmp_table[radix]++;
				} else {
					// Dealing with negative floats,
					// the lookup is prepped from last to first.
					off = --jmp_table[radix];
				}
			} else {
				off = jmp_table[radix]++;
			}

			// At position 'off', we should be item 'idx'.
			auto idx_cpy_it = std::next(write_first, ptrdiff_t(off));
			*idx_cpy_it = idx;
		}
	});

	// And finally, copy our indexes to the output.
	if constexpr (sizeof(value_t) == 1) {
		// We were a char (non-even byte size).
		// The output is in the wrong storage.
		assert(&(*write_first) != &(*idxes1.begin()));
		//std::copy(write_first, write_last, idxes1.begin());
		for (const IndexT& idx : idxes2)
		{
			assert(idx_first != idx_last);
			*idx_first = user_index_t(idx);
			++idx_first;
		}

	} else {
		assert(&(*write_first) != &(*idxes2.begin()));
		for (const IndexT& idx : idxes1)
		{
			assert(idx_first != idx_last);
			*idx_first = user_index_t(idx);
			++idx_first;
		}
	}
#endif
}
} // namespace detail


template <class FwdIt>
void radix_sort(FwdIt first, FwdIt last) {
	fea::detail::radix_checks<FwdIt>();

	size_t count = size_t(std::distance(first, last));
	if (count <= 1) {
		return;
	}

	// Dispatch to the most appropriate index type to optimize memory usage.
	// Overall, we'll use more memory in total, since we have 1 cache per index
	// size. However for any given sort, the memory will be as compressed as
	// possible, accelerating the loops.
	// Absolutely take the tradeoff!
	fea::detail::radix_get_tls_cache(count, [&](auto& local_cache) {
		return fea::detail::radix_sort(first, last, count, local_cache);
	});
}

template <class FwdIt, class FwdIt2>
void radix_sort_idxes(
		FwdIt first, FwdIt last, FwdIt2 idx_first, FwdIt2 idx_last) {
	fea::detail::radix_checks<FwdIt>();

	size_t count = size_t(std::distance(first, last));
	size_t idx_count = size_t(std::distance(idx_first, idx_last));
	if (count != idx_count) {
		fea::maybe_throw(
				__FUNCTION__, __LINE__, "Mismatch element and index count.");
	}

	if (count <= 1) {
		return;
	}

	if constexpr (fea::debug_build) {
		// Check that we contain 0-N indexes.
		// Too heavy, only check in debug.
		using index_t = fea::iterator_value_t<FwdIt2>;
		std::vector<index_t> cpy(idx_first, idx_last);
		fea::radix_sort(cpy.begin(), cpy.end());
		assert(std::adjacent_find(cpy.begin(), cpy.end()) == cpy.end());
		assert(*cpy.begin() == index_t(0));
		assert(*(cpy.begin() + (idx_count - 1)) == index_t(idx_count - 1));
	}

	fea::detail::radix_get_tls_cache(count, [&](auto& local_cache) {
		return fea::detail::radix_sort(first, last, idx_first, idx_last, count, local_cache);
	});
}


// template <class FwdIt, class Getter>
// void radix_sort(FwdIt first, FwdIt last, Getter&& getter_func) {
// static_assert(
//		std::is_unsigned_v<size_t>, "Unsupported c++ implementation.");
// static_assert(std::is_base_of_v<std::forward_iterator_tag,
//					  fea::iterator_category_t<FwdIt>>,
//		"Iterators must at least be forward iterators.");

// using value_t = std::decay_t<decltype(getter_func(std::declval<FwdIt>()))>;
////using value_t = fea::iterator_value_t<FwdIt>;
// static_assert(std::is_arithmetic_v<value_t>,
//		"Radix sort only works on arithmetic types.");

// size_t count = size_t(std::distance(first, last));
// if (count <= 1) {
//	return;
// }

//// Dispatch to the most appropriate index type to optimize memory usage.
//// Overall, we'll use more memory in total, since we have 1 cache per index
//// size. However for any given sort, the memory will be as compressed as
//// possible, accelerating the loops.
//// Absolutely take the tradeoff!
// if (count < size_t((std::numeric_limits<uint8_t>::max)())) {
//	using index_t = uint8_t;
//	fea::tls_lock<detail::radix_data<index_t>> lock
//			= detail::radix_data_cache8.lock();
//	return detail::radix_sort(first, last, count, lock.local());
// }
// if (count < size_t((std::numeric_limits<uint16_t>::max)())) {
//	using index_t = uint16_t;
//	fea::tls_lock<detail::radix_data<index_t>> lock
//			= detail::radix_data_cache16.lock();
//	return detail::radix_sort(first, last, count, lock.local());
// }
// if (count < size_t((std::numeric_limits<uint32_t>::max)())) {
//	using index_t = uint32_t;
//	fea::tls_lock<detail::radix_data<index_t>> lock
//			= detail::radix_data_cache32.lock();
//	return detail::radix_sort(first, last, count, lock.local());
// }

// static_assert(sizeof(void*) <= 8, "TODO : Update if statement + cache.");
// using index_t = uint64_t;
// fea::tls_lock<detail::radix_data<index_t>> lock
//		= detail::radix_data_cache64.lock();
// return detail::radix_sort(first, last, count, lock.local());
//}

} // namespace fea

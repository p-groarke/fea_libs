/*
BSD 3-Clause License

Copyright (c) 2024, Philippe Groarke
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>

/*
fea::unsigned_set is an ordered set interface for unsigned numbers.
The memory backing grows as large as biggest key!

See fea::id_set for a more generic equivalent capable of storing struct keys.
*/

/*
REMINDER
to try hash algo
modulo -> lookup number of collisions -> ++collisions
	if collisions > max collisions -> rehash
modulo = index start -> std::find -> index start + collisions
*/

namespace fea {
// TODO : Specialize for pure unsigned. Specialize for ordered/unordered.
// Currently unsafe as it drops potential id extra data.

template <class MySet>
struct uss_const_iterator {
	// Typedefs
	using difference_type = typename MySet::difference_type;
	using key_type = typename MySet::key_type;
	using value_type = key_type;
	using pointer = void;
	using reference = key_type;
	using iterator_category = std::bidirectional_iterator_tag;

	// Internals.
	using bool_const_iterator = typename MySet::bool_const_iterator;

	// Ctors
	constexpr uss_const_iterator() noexcept = default;
	~uss_const_iterator() noexcept = default;
	constexpr uss_const_iterator(const uss_const_iterator&) noexcept = default;
	constexpr uss_const_iterator(uss_const_iterator&&) noexcept = default;
	constexpr uss_const_iterator& operator=(const uss_const_iterator&) noexcept
			= default;
	constexpr uss_const_iterator& operator=(uss_const_iterator&&) noexcept
			= default;

	// Returns a constructed key.
	[[nodiscard]]
	constexpr key_type
	operator*() const noexcept;

	// Unavailable, keys aren't actually stored in container.
	[[nodiscard]]
	constexpr key_type
	operator->() const noexcept
			= delete;

	// Pre-fix ++operator.
	constexpr uss_const_iterator& operator++() noexcept;

	// Post-fix operator++.
	constexpr uss_const_iterator& operator++(int) noexcept;

	// Pre-fix --operator.
	constexpr uss_const_iterator& operator--() noexcept;

	// Post-fix operator--.
	constexpr uss_const_iterator& operator--(int) noexcept;

	// Comparison.
	[[nodiscard]]
	bool
	operator==(const uss_const_iterator& rhs) const noexcept;

	// Comparison.
	[[nodiscard]]
	bool
	operator!=(const uss_const_iterator& rhs) const noexcept;

	// Comparison.
	[[nodiscard]]
	bool
	operator<(const uss_const_iterator& rhs) const noexcept;

	// Comparison.
	[[nodiscard]]
	bool
	operator>(const uss_const_iterator& rhs) const noexcept;

	// Comparison.
	[[nodiscard]]
	bool
	operator<=(const uss_const_iterator& rhs) const noexcept;

	// Comparison.
	[[nodiscard]]
	bool
	operator>=(const uss_const_iterator& rhs) const noexcept;

protected:
	friend MySet;

	constexpr uss_const_iterator(bool_const_iterator first,
			bool_const_iterator last, bool_const_iterator ptr) noexcept;

	bool_const_iterator _first = {};
	bool_const_iterator _last = {};
	bool_const_iterator _current = {};
};

} // namespace fea

#include "unsigned_slotset.iterators.imp.hpp"

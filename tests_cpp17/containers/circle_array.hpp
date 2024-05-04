/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2024, Philippe Groarke
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 this
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
#include "fea/memory/memory.hpp"
#include "fea/utils/platform.hpp"

#include <array>
#include <cassert>
#include <initializer_list>
#include <type_traits>
#include <vector>

namespace fea {
namespace experimental {
template <class T, size_t N>
struct circle_array {
	circle_array() noexcept = default;

	template <class FwdIt>
	circle_array(FwdIt first, FwdIt last) {
		assert(std::distance(first, last) <= _arr.size());
		fea::copy_or_move(first, last, _arr.begin());
	}

	circle_array(std::initializer_list<T>&& list) {
		assert(list.size() <= _arr.size());
		fea::copy_or_move(list.begin(), list.end(), _arr.begin());
	}

	// Pushes a new value at the write playhead.
	void push(const T& value) {
		_arr[_write_idx] = value;
		advance_write(1);
	}
	void push(T&& value) {
		_arr[_write_idx] = std::move(value);
		advance_write(1);
	}

	// Gets the value at the read playhead, doesn't affect playhead positions.
	const T& get() const {
		return _arr[_read_idx];
	}

	// template <class FwdIt>
	// void push(FwdIt first, FwdIt last) {
	// }

	// T pop() {
	// }

	// void pop(size_t count, FwdIt out) {
	// }

private:
	// If the write playhead would surpass the read playhead,
	// pushes the read playhead forward.
	void advance_write(size_t count) noexcept {
		// using ssize_t = typename std::make_signed<size_t>::type;
		assert(_write_idx < N);
		assert(_read_idx < N);
		assert(_read_idx != _write_idx);

		size_t old_write_idx = _write_idx;
		_write_idx += count;

		auto push_read = [&, this](size_t pcount) {
			_read_idx += pcount;
			if (_read_idx >= N) {
				_read_idx %= N;
			}

			assert(_read_idx != _write_idx);
#if FEA_DEBUG
			if (_read_idx > _write_idx) {
				assert(_read_idx - _write_idx == 1);
			} else {
				assert(_write_idx - _read_idx == N - 1);
			}
#endif
		};

		if (old_write_idx > _read_idx && _write_idx >= _read_idx + N) {
			push_read(count);
		} else if (old_write_idx < _read_idx && _write_idx >= _read_idx) {
			push_read(count);
		}

		if (_write_idx >= N) {
			_write_idx %= N;
		}
	}

	// The read playhead is never allowed to surpass the write playhead,
	// so this may do nothing.
	void advance_read(size_t) noexcept {
	}

	// The write index "pushes" the read index if you loop around.
	size_t _write_idx = 0;
	size_t _read_idx = N - 1;

	// todo : _read_idx = 0;

	std::array<T, N> _arr{};
};
} // namespace experimental
} // namespace fea

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
#include "fea/containers/deque_list.hpp"
#include "fea/meta/traits.hpp"
#include "fea/utility/error.hpp"

#include <cassert>
#include <cstdint>
#include <deque>
#include <forward_list>
#include <limits>
#include <memory>
#include <shared_mutex>
#include <thread>
#include <unordered_map>
#include <utility>

/*
fea::tls is a safe implementation of a thread local storage type,
with a few differences to thread_local and tbb's offerings.

It requires a lock on the tls storage, and with it, can throw / assert
guaranteed problematic behaviors. Furthermore, the storage creation is
recursive. A single thread can create more than 1 stored type if it
already has a lock on the storage, it is not an error condition.

If the underlying thread local storage has already been initialized for a
thread, locking and unlocking doesn't actually lock.

General Usage
- Types are constructed as threads require them.
- Type T must be default constructible.
- Get a scoped lock from the tls storage, then call local on it
	to access your thread data.

Unique Behavior
- fea::tls will throw if 2 threads try to access the same data.
- fea::tls does NOT destroy objects on thread destruction.
- fea::tls is recursive, allowing storage to be used in
	nested tbb calls.
*/

namespace fea {
using std_thread_id_t = decltype(std::this_thread::get_id());

template <class T, class = std::allocator<T>>
struct tls;

// A lock to thread data.
// Construct by providing the tls storage.
template <class T, class Alloc = std::allocator<T>>
struct tls_lock {
	using size_type = typename tls<T, Alloc>::size_type;

	// Create a lock for tls storage.
	// std::lock_guard symmetry.
	explicit tls_lock(tls<T, Alloc>& storage);

	// Move lock ownership.
	explicit tls_lock(tls_lock&& other) noexcept;

	// Release our lock.
	~tls_lock();

	tls_lock(const tls_lock&) = delete;
	tls_lock& operator=(const tls_lock&) = delete;
	tls_lock& operator=(tls_lock&&) = delete;

	// Prevent transient access.
	T& local() const&& = delete;
	T& local() && = delete;

	// Access thread data.
	const T& local() const&;

	// Access thread data.
	T& local() &;

private:
	template <class, class>
	friend struct tls;

	// Creates a lock, used internally.
	tls_lock(std_thread_id_t tid, size_type data_idx, T& value,
			tls<T, Alloc>& storage) noexcept;

	std_thread_id_t _tid = (std::numeric_limits<std_thread_id_t>::max)();
	size_type _idx = (std::numeric_limits<size_type>::max)();
	T& _value;
	tls<T, Alloc>& _storage;
};

template <class T, class Alloc /*= std::allocator<T>*/>
struct tls {
	using value_type = T;
	using allocator_type = Alloc;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using reference = T&;
	using pointer = typename std::allocator_traits<Alloc>::pointer;

	// Ctors
	tls() = default;
	~tls();

	tls(const tls&) = delete;
	tls(tls&&) = delete;
	tls& operator=(const tls&) = delete;
	tls& operator=(tls&&) = delete;

	// Lock this thread's storage for use.
	// Returns a RAII type, which handles unlocking to enable safe operation.
	[[nodiscard]]
	tls_lock<T, Alloc> lock();

	// Unlocks the thread.
	void unlock(std_thread_id_t tid, size_type data_idx);

	// Do we contain any thread data?
	[[nodiscard]]
	bool empty() const;

	// How many thread datas do we contain?
	[[nodiscard]]
	size_type size() const;

	// Clear contents of the storage.
	// All threads must have released their locks before calling this.
	void clear();

	// Calls your callback with a reference to each stored T.
	// All Ts' are initialized, but may have never been used.
	// All threads must have released their locks before calling this.
	template <class Func>
	void combine_each(Func&& func) const;

	// Calls your callback with a reference to each stored T.
	// All Ts' are initialized, but may have never been used.
	// All threads must have released their locks before calling this.
	template <class Func>
	void combine_each(Func&& func);

private:
	struct thread_info {
		std_thread_id_t thread_id
				= (std::numeric_limits<std_thread_id_t>::max)();
		bool locked = false;
	};

	// Used only in exclusive mode currently.
	mutable std::shared_mutex _mutex{};

	// The thread's values, stored in a stable container to prevent
	// invalidating references.
	fea::deque_list<T, 128> _datas{};

	// Stores the lock state of a given thread data, and its index in the
	// stable container.
	// Threads have pre-assigned indexes in our data deque.
	// This allows us to search for free data quickly, without locking.
	// The same thread_id can recursively lock data, so there may be more than 1
	// thread_info for a tid in this container.
	fea::deque_list<thread_info, 128> _locks{};
};

} // namespace fea


// Implementation
namespace fea {
template <class T, class Alloc>
tls_lock<T, Alloc>::tls_lock(tls<T, Alloc>& storage)
		: tls_lock(std::move(storage.lock())) {
}

template <class T, class Alloc>
tls_lock<T, Alloc>::tls_lock(tls_lock&& other) noexcept
		: _tid(other._tid)
		, _idx(other._idx)
		, _value(other._value)
		, _storage(other._storage) {
	other._tid = std_thread_id_t{};
	other._idx = (std::numeric_limits<uint32_t>::max)();
	assert(_tid == std::this_thread::get_id());
	assert(_tid != std_thread_id_t{});
	assert(_idx != (std::numeric_limits<uint32_t>::max)());
}

template <class T, class Alloc>
tls_lock<T, Alloc>::tls_lock(std_thread_id_t tid, size_type idx, T& value,
		tls<T, Alloc>& storage) noexcept
		: _tid(tid)
		, _idx(idx)
		, _value(value)
		, _storage(storage) {
	assert(_tid == std::this_thread::get_id());
	assert(_tid != std_thread_id_t{});
	assert(_idx != (std::numeric_limits<uint32_t>::max)());
}

template <class T, class Alloc>
tls_lock<T, Alloc>::~tls_lock() {
	if (_tid != std_thread_id_t{}) {
		assert(_tid == std::this_thread::get_id());
		assert(_idx != (std::numeric_limits<uint32_t>::max)());
		_storage.unlock(_tid, _idx);
	}
}


template <class T, class Alloc>
const T& tls_lock<T, Alloc>::local() const& {
	assert(_tid == std::this_thread::get_id());
	assert(_tid != std_thread_id_t{});
	assert(_idx != (std::numeric_limits<uint32_t>::max)());
	return _value;
}

template <class T, class Alloc>
T& tls_lock<T, Alloc>::local() & {
	assert(_tid == std::this_thread::get_id());
	assert(_tid != std_thread_id_t{});
	assert(_idx != (std::numeric_limits<uint32_t>::max)());
	return _value;
}

template <class T, class Alloc>
tls<T, Alloc>::~tls() {
	std::lock_guard<std::shared_mutex> g{ _mutex };
	assert(_locks.size() == _datas.size());

	for (const thread_info& ti : _locks) {
		if (ti.locked) {
			fea::maybe_throw<std::runtime_error>(__FUNCTION__, __LINE__,
					"Destroying storage with unreleased locks. Make sure "
					"all your threads are done working before destroying "
					"storage.");
		}
	}
}

template <class T, class Alloc>
tls_lock<T, Alloc> tls<T, Alloc>::lock() {
	std_thread_id_t tid = std::this_thread::get_id();

	// Optimization.
	// We are guaranteed atomic for this thread id.
	// That is, the same thread cannot re-enter this function (or any other),
	// so we do not have a race condition for unlocking.
	//
	// Also, std::deque doesn't invalidate pre-existing iterators.
	//
	// This means we can search for a free slot for this thread without locking.
	// However, we use our "valid" size instead of the deque size,
	// in case it is being modified.
	{
		size_type size = _locks.size();
		auto begin_it = _locks.begin();

		size_type i = 0;
		for (; i < size; ++i) {
			const thread_info& info = *begin_it;
			if (info.thread_id == tid && info.locked == false) {
				break;
			}
			++begin_it;
		}

		// auto end_it = std::next(begin_it, size);

		// auto it = std::find_if(begin_it, end_it, [&](const thread_info& info)
		// { 	return info.thread_id == tid && info.locked == false;
		// });

		// End iterator could have changed, compare with our cached size.
		// if (idx != size) {
		if (i < size) {
			// Found free thread slot.
			thread_info& info = *begin_it;
			assert(!info.locked);

			info.locked = true;
			return tls_lock<T, Alloc>{
				tid,
				uint32_t(i),
				*std::next(_datas.begin(), i),
				*this,
			};
		}
	}

	// If we reach here, we must initialize data and modify the locks deque.
	// We lock.
	std::lock_guard<std::shared_mutex> g{ _mutex };
	assert(_locks.size() == _datas.size());

	// Create new lock + data slot, then update valid size.
	size_t idx = _locks.size();
	_datas.push_back({});
	_locks.push_back(thread_info{ tid, true });
	assert(_locks.size() == _datas.size());

	return tls_lock<T, Alloc>{
		tid,
		idx,
		*std::next(_datas.begin(), idx),
		*this,
	};
}

template <class T, class Alloc>
void tls<T, Alloc>::unlock(
		[[maybe_unused]] std_thread_id_t tid, size_type idx) {
	// Since the lock should have been created already, no need for locking.
	// Just use our cached valid size.
	size_type size = _locks.size();
	if (idx >= size) {
		fea::maybe_throw<std::runtime_error>(__FUNCTION__, __LINE__,
				"Trying to unlock tls that doesn't exist.");
	}

	thread_info& info = *std::next(_locks.begin(), idx);
	assert(info.thread_id == tid);

	// thread_info& info = _locks[idx];
	if (!info.locked) {
		fea::maybe_throw<std::runtime_error>(__FUNCTION__, __LINE__,
				"Trying to unlock tls that isn't locked.");
	}

	// Unlock it.
	info.locked = false;
}

template <class T, class Alloc>
bool tls<T, Alloc>::empty() const {
	assert(_locks.size() == _datas.size());
	return _locks.empty();
}

template <class T, class Alloc>
auto tls<T, Alloc>::size() const -> size_type {
	assert(_locks.size() == _datas.size());
	return _locks.size();
}

template <class T, class Alloc>
void tls<T, Alloc>::clear() {
	std::lock_guard<std::shared_mutex> g{ _mutex };
	assert(_locks.size() == _datas.size());

	for (const thread_info& info : _locks) {
		if (info.locked) {
			fea::maybe_throw<std::runtime_error>(__FUNCTION__, __LINE__,
					"Cannot clear storage, at least 1 thread currently "
					"owns a lock on storage.");
		}
	}

	_datas.clear();
	_locks.clear();
	assert(_locks.size() == _datas.size());
}

template <class T, class Alloc>
template <class Func>
void tls<T, Alloc>::combine_each(Func&& func) const {
	std::lock_guard<std::shared_mutex> g{ _mutex };
	assert(_locks.size() == _datas.size());

	for (const thread_info& info : _locks) {
		if (info.locked) {
			fea::maybe_throw<std::runtime_error>(__FUNCTION__, __LINE__,
					"Cannot combine storage, at least 1 thread still holds "
					"a lock.");
		}
	}

	for (const T& t : _datas) {
		func(t);
	}
}

template <class T, class Alloc>
template <class Func>
void tls<T, Alloc>::combine_each(Func&& func) {
	std::lock_guard<std::shared_mutex> g{ _mutex };
	assert(_locks.size() == _datas.size());

	for (const thread_info& info : _locks) {
		if (info.locked) {
			fea::maybe_throw<std::runtime_error>(__FUNCTION__, __LINE__,
					"Cannot combine storage, at least 1 thread still holds "
					"a lock.");
		}
	}

	for (T& t : _datas) {
		func(t);
	}
}
} // namespace fea

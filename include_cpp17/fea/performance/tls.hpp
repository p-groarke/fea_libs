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
#include "fea/meta/traits.hpp"
#include "fea/utils/throw.hpp"

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

Warning : The lock call is SLOW and meant to be called once.
No effort has gone into making the lock faster.
*/

namespace fea {
using std_thread_id_t = decltype(std::this_thread::get_id());

template <class T, class = std::allocator<T>>
struct tls;

// A lock to thread data.
// Construct by providing the tls storage.
template <class T, class Alloc = std::allocator<T>>
struct tls_lock {
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
	tls_lock(std_thread_id_t tid, uint32_t data_idx, T& value,
			tls<T, Alloc>& storage) noexcept;

	std_thread_id_t _tid = (std::numeric_limits<std_thread_id_t>::max)();
	uint32_t _data_idx = (std::numeric_limits<uint32_t>::max)();
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
	tls();
	~tls();

	tls(const tls&) = delete;
	tls(tls&&) = delete;
	tls& operator=(const tls&) = delete;
	tls& operator=(tls&&) = delete;

	// Used internally. Also called by clear.
	void init();

	// Lock this thread's storage for use.
	// Returns a RAII type, which handles unlocking to enable safe operation.
	[[nodiscard]]
	tls_lock<T, Alloc> lock();

	// Unlocks the thread.
	void unlock(std_thread_id_t tid, uint32_t data_idx);

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
		thread_info* next = nullptr;
		std_thread_id_t thread_id
				= (std::numeric_limits<std_thread_id_t>::max)();
		uint32_t data_idx = (std::numeric_limits<uint32_t>::max)();
		bool locked = false;
	};

	using map_alloctor_type = fea::rebind_alloc_t<Alloc,
			std::pair<const std_thread_id_t, thread_info>>;
	using lock_allocator_type = fea::rebind_alloc_t<Alloc, thread_info>;

	template <class FuncT>
	thread_info* find_if(thread_info* first, size_type count, FuncT&&);

	// Used only in exclusive mode currently.
	mutable std::shared_mutex _mutex{};

	// The thread's values, stored in a stable container to prevent
	// invalidating references.
	std::deque<T, allocator_type> _datas{};

	// Stores the lock state of a given thread data, and its index in the
	// stable container.
	// The same thread_id can recursively lock data, so there may be more than 1
	// thread_info for a tid in this container.
	std::deque<thread_info, lock_allocator_type> _locks{};

	// Since the deque only garauntees stable pointers, but NOT iterators,
	// we implement our own linked list. This is the first thread_info.
	// Traversal we be relatively fast, since our data is mostly linear.
	// Note : Iterating on the list, we must stop at _valid_locks_size,
	// not next == nullptr.
	thread_info* _first_lock = nullptr;

	//// Since the deque may invalidate iterators (but not pointers), we
	/// implement / a forward list on top of the deque. This guarantees
	/// reasonable linear / travesal speed, all the while being stable and
	/// thread safe.
	// std::forward_list<thread_info*> _locks_traversal;

	// Only updated once everything is initialized for a new thread.
	size_type _valid_locks_size = 0u;
};

} // namespace fea

#include "imp/tls.imp.hpp"

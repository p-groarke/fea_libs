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
#include "fea/utils/throw.hpp"

#include <cassert>
#include <cstdint>
#include <deque>
#include <limits>
#include <memory>
#include <shared_mutex>
#include <thread>
#include <unordered_map>
#include <utility>

/*
safe_tls is a safe implementation of a thread local storage type,
with a few differences to thread_local and tbb::enumrable_thread_specific.

General Usage
- Types are constructed as threads require them.
- Type T must be default constructible.
- Get a scoped lock from the safe_tls storage, then call local on it
	to access your thread data.

Unique Behavior
- safe_tls will throw if 2 threads try to access the same data.
- safe_tls does NOT destroy objects on thread destruction.
- The lock call is SLOW and meant to be called rarely.
*/

namespace fea {
template <class T, class = std::allocator<T>>
struct tls;

template <class T, class Alloc = std::allocator<T>>
struct tls_lock {
	using thread_id_t = decltype(std::this_thread::get_id());

	tls_lock(thread_id_t tid, T& value, tls<T, Alloc>& storage) noexcept
			: _tid(tid)
			, _value(value)
			, _storage(storage) {
		assert(_tid == std::this_thread::get_id());
		assert(_tid != thread_id_t{});
	}

	explicit tls_lock(tls_lock&& other) noexcept
			: _tid(other._tid)
			, _value(other._value)
			, _storage(other._storage) {
		other._tid = thread_id_t{};
		assert(_tid == std::this_thread::get_id());
		assert(_tid != thread_id_t{});
	}

	// std::lock_guard symmetry.
	explicit tls_lock(tls<T, Alloc>& storage)
			: tls_lock(std::move(storage.lock())) {
	}

	// Release our lock.
	~tls_lock() {
		if (_tid != thread_id_t{}) {
			assert(_tid == std::this_thread::get_id());
			_storage.unlock(_tid);
		}
	}

	tls_lock(const tls_lock&) = delete;
	tls_lock& operator=(const tls_lock&) = delete;
	tls_lock& operator=(tls_lock&&) = delete;

	// Prevent transient access.
	T& local() const&& = delete;
	T& local() && = delete;

	// Access thread data.
	const T& local() const& {
		assert(_tid == std::this_thread::get_id());
		assert(_tid != thread_id_t{});
		return _value;
	}

	// Access thread data.
	T& local() & {
		assert(_tid == std::this_thread::get_id());
		assert(_tid != thread_id_t{});
		return _value;
	}

private:
	thread_id_t _tid{};
	T& _value;
	tls<T, Alloc>& _storage;
};

template <class T, class Alloc /*= std::allocator<T>*/>
struct tls {
	struct thread_info {
		bool locked = false;
		size_t data_idx = (std::numeric_limits<size_t>::max)();
	};
	using thread_id_t = decltype(std::this_thread::get_id());

	using value_type = T;
	using allocator_type = Alloc;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using reference = T&;
	using pointer = std::allocator_traits<Alloc>::pointer;

	using map_alloctor_type =
			typename std::allocator_traits<Alloc>::template rebind_alloc<
					std::pair<const thread_id_t, thread_info>>;

	tls() {
		int num_threads = std::thread::hardware_concurrency();
		if (num_threads > 0) {
			_thread_info.reserve(size_t(num_threads));
		}
	}

	~tls() {
		std::lock_guard<std::shared_mutex> g{ _mutex };
		assert(_thread_info.size() == _thread_data.size());

		for (const std::pair<thread_id_t, thread_info>& p : _thread_info) {
			if (p.second.locked) {
				fea::maybe_throw<std::runtime_error>(__FUNCTION__, __LINE__,
						"Destroying storage with unreleased locks. Make sure "
						"all your threads are done working before destroying "
						"storage.");
			}
		}
	}

	tls(const tls&) = delete;
	tls(tls&&) = delete;
	tls& operator=(const tls&) = delete;
	tls& operator=(tls&&) = delete;

	// Lock this thread's storage for use.
	// Returns a RAII type, which handles unlocking to enable safe operation.
	[[nodiscard]] tls_lock<T, Alloc> lock() {
		thread_id_t tid = std::this_thread::get_id();

		// Prefer safety over speed.
		// This could be acheived with reader / writer paradigm,
		// but standard adopt lock doesn't mention supporting promotion
		// from reader to writer.
		std::lock_guard<std::shared_mutex> g{ _mutex };

		// Create thread data if we don't recognize this thread id.
		if (_thread_info.count(tid) == 0) {
			size_t idx = _thread_data.size();
			_thread_data.emplace_back();
			assert(idx < _thread_data.size());
			_thread_info.insert({ tid, thread_info{ false, idx } });
		}
		assert(_thread_info.size() == _thread_data.size());

		// Lock data and return RAII helper.
		thread_info& tinfo = _thread_info.at(tid);
		if (tinfo.locked) {
			fea::maybe_throw<std::runtime_error>(__FUNCTION__, __LINE__,
					"Trying to access data locked by an other thread.");
		}

		assert(tinfo.data_idx < _thread_data.size());
		assert(!tinfo.locked);
		tinfo.locked = true;
		return tls_lock<T, Alloc>{ tid, _thread_data[tinfo.data_idx], *this };
	}

	void unlock(thread_id_t tid) {
		std::lock_guard<std::shared_mutex> g{ _mutex };
		assert(_thread_info.at(tid).locked);
		_thread_info.at(tid).locked = false;
	}

	bool empty() const noexcept {
		std::lock_guard<std::shared_mutex> g{ _mutex };
		assert(_thread_info.size() == _thread_data.size());
		return _thread_data.empty();
	}

	size_type size() const noexcept {
		std::lock_guard<std::shared_mutex> g{ _mutex };
		assert(_thread_info.size() == _thread_data.size());
		return _thread_data.size();
	}

	// Clear contents of the storage.
	// All threads must have released their locks before calling this.
	void clear() {
		std::lock_guard<std::shared_mutex> g{ _mutex };
		assert(_thread_info.size() == _thread_data.size());

		for (const std::pair<thread_id_t, thread_info>& p : _thread_info) {
			if (p.second.locked) {
				fea::maybe_throw<std::runtime_error>(__FUNCTION__, __LINE__,
						"Cannot clear storage, at least 1 thread currently "
						"owns a lock on storage.");
			}
		}
		_thread_info.clear();
		_thread_data.clear();
	}

	// Calls your callback with a reference to each stored T.
	// All Ts' are initialized, but may have never been used.
	// All threads must have released their locks before calling this.
	template <class Func>
	void combine_each(Func&& func) const {
		std::lock_guard<std::shared_mutex> g{ _mutex };
		assert(_thread_info.size() == _thread_data.size());

		for (const std::pair<thread_id_t, thread_info>& p : _thread_info) {
			if (p.second.locked) {
				fea::maybe_throw<std::runtime_error>(__FUNCTION__, __LINE__,
						"Cannot combine storage, at least 1 thread still holds "
						"a lock.");
			}
		}

		for (const T& t : _thread_data) {
			func(t);
		}
	}

	// Calls your callback with a reference to each stored T.
	// All Ts' are initialized, but may have never been used.
	// All threads must have released their locks before calling this.
	template <class Func>
	void combine_each(Func&& func) {
		std::lock_guard<std::shared_mutex> g{ _mutex };
		assert(_thread_info.size() == _thread_data.size());

		for (const std::pair<thread_id_t, thread_info>& p : _thread_info) {
			if (p.second.locked) {
				fea::maybe_throw<std::runtime_error>(__FUNCTION__, __LINE__,
						"Cannot combine storage, at least 1 thread still holds "
						"a lock.");
			}
		}

		for (T& t : _thread_data) {
			func(t);
		}
	}

private:
	// Used only in exclusive mode currently.
	mutable std::shared_mutex _mutex{};

	// Stores the lock state of a given thread data, and its index in the
	// stable container.
	std::unordered_map<thread_id_t, thread_info, std::hash<thread_id_t>,
			std::equal_to<thread_id_t>, map_alloctor_type>
			_thread_info{};

	// The thread's values, stored in a stable container to prevent
	// invalidating references.
	std::deque<T, allocator_type> _thread_data{};
};
} // namespace fea

/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2023, Philippe Groarke
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
#include "fea/memory/memory.hpp"

#include <algorithm>
#include <functional>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

// If you don't feel like linking to tbb.
namespace fea {
[[nodiscard]] inline size_t num_threads() {
	size_t concurrency = std::thread::hardware_concurrency();
	return concurrency <= 0 ? 1 : concurrency;
}

inline void parallel_for(size_t loop_count,
		const std::function<void(const std::pair<size_t, size_t>&, size_t)>&
				func) {

	const size_t num_t = num_threads();

	std::vector<std::pair<size_t, size_t>> index_ranges(num_t, { 0, 0 });
	size_t chunk_size = loop_count / num_t;
	size_t chunk_extra = loop_count % num_t;
	for (size_t i = 0; i < num_t; ++i) {
		index_ranges[i].first = i * chunk_size;
		index_ranges[i].second = (i * chunk_size) + chunk_size;
		if (i == num_t - 1) {
			index_ranges[i].second += chunk_extra;
		}
	}

	std::vector<std::thread> threads;
	threads.reserve(num_t);
	for (size_t i = 0; i < num_t; ++i) {
		threads.emplace_back(func, index_ranges[i], i);
	}

	for (auto& t : threads) {
		t.join();
	}
}

inline void parallel_tasks(std::vector<std::function<void()>>&& tasks) {
	using namespace std::chrono_literals;
	if (tasks.empty())
		return;

	size_t thread_count = std::min(num_threads(), tasks.size());
	std::vector<std::thread> threads;
	threads.reserve(thread_count);

	for (size_t i = 0; i < thread_count; ++i) {
		threads.emplace_back(std::move(tasks.back()));
		tasks.pop_back();
	}

	while (threads.size() > 0) {
		std::this_thread::sleep_for(0.01s);

		for (size_t i = 0; i < threads.size(); ++i) {
			if (!threads[i].joinable())
				continue;

			threads[i].join();

			if (tasks.size() > 0) {
				threads[i] = std::thread{ std::move(tasks.back()) };
				tasks.pop_back();
			} else if (i == threads.size() - 1) {
				// edge case: pop last
				threads.pop_back();
			} else {
				// erase thread : swap & pop
				threads[i] = std::move(threads.back());
				threads.pop_back();
			}
		}
	}
}

template <class T>
struct mtx_safe {
	mtx_safe(const T& obj)
			: _obj(obj) {
	}
	mtx_safe(T&& obj)
			: _obj(std::move(obj)) {
	}
	template <class... CtorArgs>
	mtx_safe(CtorArgs&&... ctor_args)
			: _obj(std::forward<CtorArgs>(ctor_args)...) {
	}
	mtx_safe() = default;

	template <class Func>
	auto read(Func&& func) const {
		std::shared_lock l{ _mutex };
		return std::forward<Func>(func)(_obj);
	}

	template <class Func>
	auto write(Func&& func) {
		std::unique_lock l{ _mutex };
		return std::forward<Func>(func)(_obj);
	}

	template <class... CtorArgs>
	T extract(CtorArgs&&... replacement_ctor_args) {
		std::unique_lock l{ _mutex };
		T ret{ fea::maybe_move(_obj) };
		_obj = T{ std::forward<CtorArgs>(replacement_ctor_args)... };
		return ret;
	}

private:
	mutable std::shared_mutex _mutex;
	T _obj{};
};

template <class T>
struct mtx_safe<T*> {
	mtx_safe(T* obj)
			: _obj(obj) {
	}
	mtx_safe() = default;

	template <class Func>
	auto read(Func&& func) const {
		std::shared_lock l{ _mutex };
		return std::invoke(std::forward<Func>(func), *_obj);
	}

	template <class Func>
	auto write(Func&& func) {
		std::unique_lock l{ _mutex };
		return std::invoke(std::forward<Func>(func), *_obj);
	}

	T* extract(T* replacement = nullptr) {
		std::unique_lock l{ _mutex };
		T* ret = _obj;
		_obj = replacement;
		return ret;
	}

private:
	mutable std::shared_mutex _mutex;
	T* _obj{ nullptr };
};

template <class T>
struct mtx_safe<T&> {
	mtx_safe(T& obj)
			: _obj(obj) {
	}

	template <class Func>
	auto read(Func&& func) const {
		std::shared_lock l{ _mutex };
		return std::invoke(std::forward<Func>(func), _obj);
	}

	template <class Func>
	auto write(Func&& func) {
		std::unique_lock l{ _mutex };
		return std::invoke(std::forward<Func>(func), _obj);
	}

	T& extract(T& replacement) {
		std::unique_lock l{ _mutex };
		T& ret = _obj;
		_obj = replacement;
		return ret;
	}

private:
	mutable std::shared_mutex _mutex;
	T& _obj{ nullptr };
};
} // namespace fea

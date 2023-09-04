///**
// * BSD 3-Clause License
// *
// * Copyright (c) 2023, Philippe Groarke
// * All rights reserved.
// *
// * Redistribution and use in source and binary forms, with or without
// * modification, are permitted provided that the following conditions are met:
// *
// * * Redistributions of source code must retain the above copyright notice,
// this
// *   list of conditions and the following disclaimer.
// *
// * * Redistributions in binary form must reproduce the above copyright notice,
// *   this list of conditions and the following disclaimer in the documentation
// *   and/or other materials provided with the distribution.
// *
// * * Neither the name of the copyright holder nor the names of its
// *   contributors may be used to endorse or promote products derived from
// *   this software without specific prior written permission.
// *
// * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// * POSSIBILITY OF SUCH DAMAGE.
// **/
//#pragma once
//#include <fea/containers/stack_vector.hpp>
//#include <fea/utils/throw.hpp>
//
//#include <array>
//#include <cassert>
//#include <memory>
//#include <vector>
//
///*
// growing_array allocates a given amount of objects on the stack. If you
// push_back too many items, it will reallocate on the heap. Allowing you to
// grow the vector as big as you wish.
//
// You can think of it as a small object optimization vector, where you control
// the amount of objects stored on the stack.
//
// The downside is an extra if statement pretty much everywhere.
//*/
//
// namespace fea {
// template <class T, size_t StackSize = 128>
// struct growing_array {
//	growing_array() = default;
//
//	growing_array(size_t count, const T& value) {
//		if (count > StackSize) {
//			_heap_data = std::vector<T>(count, value);
//		} else {
//			std::fill_n(_stack_data.begin(), count, value);
//		}
//	}
//
//	growing_array(size_t count)
//			: growing_array(count, T{}) {
//	}
//
//	size_t size() const noexcept {
//		return select([](const auto& v) { return v.size(); });
//	}
//
//	size_t max_size() const noexcept {
//		return select([](const auto& v) { return v.max_size(); });
//	}
//
//	constexpr size_t capacity() const noexcept {
//		return select([](const auto& v) { return v.capacity(); });
//	}
//
//	bool empty() const noexcept {
//		return select([](const auto& v) { return v.empty(); });
//	}
//
//	const T* begin() const noexcept {
//		return select([](const auto& v) { return v.begin(); });
//	}
//	T* begin() noexcept {
//		return select([](auto& v) { return v.begin(); });
//	}
//
//	const T* end() const noexcept {
//		return select([](const auto& v) { return v.end(); });
//	}
//	T* end() noexcept {
//		return select([](auto& v) { return v.end(); });
//	}
//
//	const T* rbegin() const noexcept {
//		return select([](const auto& v) { return v.rbegin(); });
//	}
//	T* rbegin() noexcept {
//		return select([](auto& v) { return v.rbegin(); });
//	}
//
//	const T* rend() const noexcept {
//		return select([](const auto& v) { return v.rend(); });
//	}
//	T* rend() noexcept {
//		return select([](auto& v) { return v.rend(); });
//	}
//
//	const T* cbegin() const noexcept {
//		return select([](const auto& v) { return v.cbegin(); });
//	}
//	const T* cend() const noexcept {
//		return select([](const auto& v) { return v.cend(); });
//	}
//
//	const T* crbegin() const noexcept {
//		return select([](const auto& v) { return v.crbegin(); });
//	}
//	const T* crend() const noexcept {
//		return select([](const auto& v) { return v.crend(); });
//	}
//
//	const T& front() const noexcept {
//		return select([](const auto& v) { return v.front(); });
//	}
//	T& front() noexcept {
//		return select([](auto& v) { return v.front(); });
//	}
//
//	const T& back() const noexcept {
//		return select([](const auto& v) { return v.back(); });
//	}
//	T& back() noexcept {
//		return select([](auto& v) { return v.back(); });
//	}
//
//	const T& operator[](size_t i) const noexcept {
//		return select([&](const auto& v) -> const T& { return v[i]; });
//	}
//	T& operator[](size_t i) noexcept {
//		return select([&](auto& v) -> T& { return v[i]; });
//	}
//
//	const T& at(size_t i) const {
//		return select([&](const auto& v) -> const T& { return v.at(i); });
//	}
//	T& at(size_t i) {
//		return select([&](auto& v) -> T& { return v.at(i); });
//	}
//
//	const T* data() const noexcept {
//		return select([](const auto& v) { return v.data(); });
//	}
//	T* data() noexcept {
//		return select([](auto& v) { return v.data(); });
//	}
//
//	void swap(growing_array& other) {
//		select([&](auto& v) {
//			using std::swap;
//			swap(v, other);
//		});
//	}
//
//	void push_back(const T& value) {
//		select([&](auto& v) { v.push_back(value); });
//	}
//	void push_back(T&& value) {
//		select([&](auto& v) { v.push_back(std::move(value)); });
//	}
//
//	void pop_back() {
//		select([](auto& v) { v.pop_back(); });
//	}
//
//	void resize(size_t new_size) {
//		resize(new_size, T{});
//	}
//
//	void resize(size_t new_size, const T& value) {
//		select([&](auto& v) { v.resize(new_size, value); });
//	}
//
//	void clear() {
//		select([](auto& v) { v.clear(); });
//	}
//
// private:
//	template <class Func>
//	auto select(Func func) const
//			-> decltype(func(std::declval<std::vector<T>&>())) {
//		if (_heap_data.empty()) {
//			return func(_stack_data);
//		}
//		return func(_heap_data);
//	}
//
//	template <class Func>
//	auto select(Func func) -> decltype(func(std::declval<std::vector<T>&>())) {
//		if (_heap_data.empty()) {
//			return func(_stack_data);
//		}
//		return func(_heap_data);
//	}
//
//	fea::stack_vector<T, StackSize> _stack_data;
//	std::vector<T> _heap_data;
//};
//} // namespace fea

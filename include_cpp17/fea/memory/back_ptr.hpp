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
#include <cstddef>

/*
A non-owning pointer which enforces updating a "back pointer" to the
current structure or children structures.

Clears the pointer for all constructors.
*/

namespace fea {
template <class T>
struct back_ptr {
	// Initialized to null.
	back_ptr() noexcept = default;

	// Assigns pointer.
	explicit back_ptr(T* ptr) noexcept;

	// Assigns nullptr.
	back_ptr(std::nullptr_t) noexcept;

	// Resets back pointer.
	back_ptr(const back_ptr&) noexcept;

	// Resets back pointer and other's back pointer.
	back_ptr(back_ptr&&) noexcept;

	// Resets back pointer.
	back_ptr& operator=(const back_ptr&) noexcept;

	// Resets back pointer and other's back pointer.
	back_ptr& operator=(back_ptr&&) noexcept;

	// Assigns pointer.
	back_ptr& operator=(T* ptr) noexcept;

	// Access pointer.
	T& operator*() const noexcept;

	// Access pointer.
	T* operator->() const noexcept;

	// Access pointer.
	T* get() const noexcept;

	// Replaces pointer.
	void reset(T* ptr = nullptr) noexcept;

	// Bool conversion.
	explicit operator bool() const noexcept;

private:
	T* _ptr = nullptr;
};
} // namespace fea


// Implementation
namespace fea {
template <class T>
back_ptr<T>::back_ptr(T* ptr) noexcept
		: _ptr(ptr) {
}

template <class T>
back_ptr<T>::back_ptr(std::nullptr_t) noexcept
		: _ptr(nullptr) {
}

template <class T>
back_ptr<T>::back_ptr(const back_ptr&) noexcept
		: _ptr(nullptr) {
}

template <class T>
back_ptr<T>::back_ptr(back_ptr&& other) noexcept
		: _ptr(nullptr) {
	other._ptr = nullptr;
}

template <class T>
auto back_ptr<T>::operator=(const back_ptr& other) noexcept -> back_ptr& {
	if (this != &other) {
		_ptr = nullptr;
	}
	return *this;
}

template <class T>
auto back_ptr<T>::operator=(back_ptr&& other) noexcept -> back_ptr& {
	if (this != &other) {
		_ptr = nullptr;
		other._ptr = nullptr;
	}
	return *this;
}

template <class T>
auto back_ptr<T>::operator=(T* ptr) noexcept -> back_ptr& {
	_ptr = ptr;
	return *this;
}

template <class T>
T& back_ptr<T>::operator*() const noexcept {
	return *_ptr;
}

template <class T>
T* back_ptr<T>::operator->() const noexcept {
	return _ptr;
}

template <class T>
T* back_ptr<T>::get() const noexcept {
	return _ptr;
}

template <class T>
void back_ptr<T>::reset(T* ptr /*= nullptr*/) noexcept {
	_ptr = ptr;
}

template <class T>
back_ptr<T>::operator bool() const noexcept {
	return _ptr != nullptr;
}

} // namespace fea

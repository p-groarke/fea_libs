﻿/**
 * BSD 3-Clause License
 *
 * Copyright (c) 2020, Philippe Groarke
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
#include <memory>

/*
fea::pimpl_ptr is a wrapper for const std::unique_ptr<>. It implements all
constructors and assignement operators for you.

Remember to implement your class destructor in its cpp file.
for ex in cpp : my_class::~my_class() = default;
*/

namespace fea {
template <class T, class Deleter = std::default_delete<T>>
struct pimpl_ptr {
	using pointer = T*;
	using element_type = T;
	using deleter_type = Deleter;

	pimpl_ptr()
			: _ptr(std::make_unique<T>()) {
	}

	pimpl_ptr(const pimpl_ptr& other)
			: _ptr(std::make_unique<T>(*other)) {
	}
	pimpl_ptr(pimpl_ptr&& other)
			: _ptr(std::make_unique<T>(std::move(*other))) {
		*other = {};
	}
	pimpl_ptr& operator=(const pimpl_ptr& other) {
		if (this != &other) {
			*_ptr = *other;
		}
		return *this;
	}
	pimpl_ptr& operator=(pimpl_ptr&& other) {
		if (this != &other) {
			*_ptr = std::move(*other);
			*other = {};
		}
		return *this;
	}

	~pimpl_ptr() = default;

	pointer get() const noexcept {
		return _ptr.get();
	}

	pointer operator->() const noexcept {
		return _ptr.get();
	}

	const std::add_lvalue_reference_t<T> operator*() const {
		return *_ptr;
	}
	std::add_lvalue_reference_t<T> operator*() {
		return *_ptr;
	}

protected:
	const std::unique_ptr<element_type, deleter_type> _ptr;
};

} // namespace fea

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
#include "fea/memory/back_ptr.hpp"
#include "fea/memory/memory.hpp"
#include "fea/meta/traits.hpp"

#include <array>
#include <cassert>
#include <memory>
#include <utility>

/*
fea::deque_list is a deque with linked list iterators.
The buckets are connected together as a linked list.
Forward iteration is linear on the std::deque memory buckets.

This container is usefull in a multi-threaded contexts.
It allows iterating on deque items while another thread inserts,
as long as the iteration is limited to the items contained in the forward_deque
BEFORE the insertion. Aka, cache your size and limit your iteration to that
size.

Iterators and pointers / references aren't invalidated on growth.
*/

namespace fea {
namespace detail {
template <class, size_t>
struct dl_bucket;
template <class>
struct dl_const_iter;
template <class>
struct dl_iter;
} // namespace detail

template <class T, size_t BucketSize = 32>
struct deque_list {
	using value_type = T;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	// using allocator_type = Alloc;
	using const_reference = const value_type&;
	using reference = value_type&;
	using const_iterator = detail::dl_const_iter<deque_list>;
	using iterator = detail::dl_iter<deque_list>;

	static constexpr size_type bucket_size = BucketSize;
	using bucket = detail::dl_bucket<value_type, bucket_size>;

	// Ctors.
	deque_list() noexcept;
	~deque_list() = default;
	deque_list(const deque_list&);
	deque_list(deque_list&&);
	deque_list& operator=(const deque_list&);
	deque_list& operator=(deque_list&&);

	// Element access

	// Access the first element. Error if empty.
	[[nodiscard]]
	const_reference front() const;

	// Access the first element. Error if empty.
	[[nodiscard]]
	reference front();

	// Access the last element. Error if empty.
	[[nodiscard]]
	const_reference back() const;

	// Access the last element. Error if empty.
	[[nodiscard]]
	reference back();

	// Iterators

	// Const begin iterator.
	[[nodiscard]]
	const_iterator begin() const noexcept;

	// Const begin iterator.
	[[nodiscard]]
	const_iterator cbegin() const noexcept;

	// Begin iterator.
	[[nodiscard]]
	iterator begin() noexcept;

	// Const end iterator O(1).
	[[nodiscard]]
	const_iterator end() const noexcept;

	// Const end iterator O(1).
	[[nodiscard]]
	const_iterator cend() const noexcept;

	// End iterator O(1).
	[[nodiscard]]
	iterator end() noexcept;

	// Capacity

	// Is container empty? O(1)
	[[nodiscard]]
	bool empty() const noexcept;

	// Size of container.
	[[nodiscard]]
	size_type size() const noexcept;

	//// Max storage capacity.
	//[[nodiscard]]
	// size_type max_size() const noexcept;

	// Shrinks the underlying storage (frees buckets).
	void shrink_to_fit();

	// Modifiers

	// Clear all items and reset container.
	void clear();

	// Append an element to the end of the deque.
	void push_back(const value_type& value);

	// Append an element to the end of the deque.
	void push_back(value_type&& value);

	//// Append an element to the end of the deque.
	// template <class... Args>
	// reference emplace_back(Args&&... args);

	// Remove the last element in the deque.
	void pop_back();

private:
	void maybe_grow();

	// Our first bucket, allocated in place.
	bucket _first_bucket{};

	// Number of elements.
	size_type _size = size_type(0);

	// Pointer to the last bucket. May be == first bucket.
	fea::back_ptr<bucket> _last_bucket = nullptr;
};
} // namespace fea


// Implementation
namespace fea {
namespace detail {
template <class T, size_t BucketSize>
struct dl_bucket {
	using size_type = std::size_t;
	using aligned_storage_t = fea::aligned_storage_t<sizeof(T), alignof(T)>;

	dl_bucket() = default;

	const T* begin() const noexcept {
		return reinterpret_cast<const T*>(data.data());
	}
	T* begin() noexcept {
		return const_cast<T*>(std::as_const(*this).begin());
	}

	const T* end() const noexcept {
		assert(size <= BucketSize);
		return reinterpret_cast<const T*>(data.data() + size);
	}
	T* end() noexcept {
		return const_cast<T*>(std::as_const(*this).end());
	}

	// Our raw data.
	std::array<aligned_storage_t, BucketSize> data;
	size_type size = size_type(0);
	std::unique_ptr<dl_bucket> next = nullptr;
	fea::back_ptr<dl_bucket> prev = nullptr;
};

template <class MyList>
struct dl_const_iter {
	// Typedefs
	using value_type = typename MyList::value_type;
	using difference_type = typename MyList::difference_type;
	using size_type = typename MyList::size_type;
	using pointer = const value_type*;
	using reference = const value_type&;
	using iterator_category = std::bidirectional_iterator_tag;
	using bucket_type = typename MyList::bucket;
	static constexpr size_type bucket_size = MyList::bucket_size;

	// Internals.

	// Ctors
	constexpr dl_const_iter() noexcept = default;
	constexpr dl_const_iter(const bucket_type* bucket, size_type idx,
			const bucket_type* last_bucket) noexcept
			: _bucket(bucket)
			, _last_bucket(last_bucket)
			, _idx(idx) {
	}

	~dl_const_iter() noexcept = default;
	constexpr dl_const_iter(const dl_const_iter&) noexcept = default;
	constexpr dl_const_iter(dl_const_iter&&) noexcept = default;
	constexpr dl_const_iter& operator=(const dl_const_iter&) noexcept = default;
	constexpr dl_const_iter& operator=(dl_const_iter&&) noexcept = default;

	[[nodiscard]]
	constexpr pointer operator->() const noexcept {
		assert(_bucket != nullptr);
		assert(_idx < bucket_size);
		return _bucket->begin() + _idx;
	}

	[[nodiscard]]
	constexpr reference operator*() const noexcept {
		return *this->operator->();
	}

	// Pre-fix ++operator.
	constexpr dl_const_iter& operator++() noexcept {
		assert(_bucket != nullptr);
		++_idx;
		if (_idx >= bucket_size && _bucket != _last_bucket) {
			_idx = size_type(0);
			_bucket = _bucket->next.get();
		}
		return *this;
	}

	// Post-fix operator++.
	constexpr dl_const_iter operator++(int) noexcept {
		dl_const_iter tmp = *this;
		++*this;
		return tmp;
	}

	// Pre-fix --operator.
	constexpr dl_const_iter& operator--() noexcept {
		assert(_bucket != nullptr);
		if (_idx == size_type(0)) {
			assert(_bucket->prev.get() != nullptr);
			_bucket = _bucket->prev.get();
			_idx = MyList::bucket_size;
		}
		--_idx;
		return *this;
	}

	// Post-fix operator--.
	constexpr dl_const_iter operator--(int) noexcept {
		dl_const_iter tmp = *this;
		--*this;
		return tmp;
	}

	// Comparison.
	[[nodiscard]]
	bool operator==(const dl_const_iter& rhs) const noexcept {
		return _bucket == rhs._bucket && _idx == rhs._idx;
	}

	// Comparison.
	[[nodiscard]]
	bool operator!=(const dl_const_iter& rhs) const noexcept {
		return !(*this == rhs);
	}

	//// Comparison.
	//[[nodiscard]]
	// bool
	// operator<(const fd_const_iter& rhs) const noexcept {
	//	if (_bucket == nullptr) {
	//		return false
	//	}
	//	return (_bucket == nullptr) || (_bucket == rhs._bucket && idx <
	// rhs._idx)
	//		|| _bucket < rhs._bucket;
	//}

	//// Comparison.
	//[[nodiscard]]
	// bool
	// operator>(const fd_const_iter& rhs) const noexcept {
	//	return rhs < *this;
	//}

	//// Comparison.
	//[[nodiscard]]
	// bool
	// operator<=(const fd_const_iter& rhs) const noexcept {
	//	return !(rhs < *this);
	//}

	//// Comparison.
	//[[nodiscard]]
	// bool
	// operator>=(const fd_const_iter& rhs) const noexcept {
	//	return !(*this < rhs);
	//}

	const bucket_type* _bucket = nullptr;
	const bucket_type* _last_bucket = nullptr;
	size_type _idx = 0u;
};

template <class MyList>
struct dl_iter : dl_const_iter<MyList> {
	using base_t = dl_const_iter<MyList>;

	// Typedefs
	using value_type = typename base_t::value_type;
	using difference_type = typename base_t::difference_type;
	using size_type = typename base_t::size_type;
	using pointer = value_type*;
	using reference = value_type&;
	using iterator_category = typename base_t::iterator_category;
	using bucket_type = typename base_t::bucket_type;

	using base_t::base_t;

	[[nodiscard]]
	constexpr reference operator*() const noexcept {
		return const_cast<reference>(base_t::operator*());
	}

	[[nodiscard]]
	constexpr pointer operator->() const noexcept {
		assert(base_t::_ptr != nullptr);
		return &base_t::_ptr->value;
	}

	// Pre-fix ++operator.
	constexpr dl_iter& operator++() noexcept {
		base_t::operator++();
		return *this;
	}

	// Post-fix operator++.
	constexpr dl_iter operator++(int) noexcept {
		dl_const_iter tmp = *this;
		base_t::operator++();
		return tmp;
	}

	// Pre-fix --operator.
	constexpr dl_iter& operator--() noexcept {
		base_t::operator--();
		return *this;
	}

	// Post-fix operator--.
	constexpr dl_iter operator--(int) noexcept {
		dl_const_iter tmp = *this;
		base_t::operator--();
		return tmp;
	}
};
} // namespace detail


template <class T, size_t BucketSize /*= 32*/>
deque_list<T, BucketSize>::deque_list() noexcept
		: _first_bucket()
		, _size(0)
		, _last_bucket(&_first_bucket) {
}

template <class T, size_t BucketSize /*= 32*/>
deque_list<T, BucketSize>::deque_list(const deque_list& other) {
	this->operator=(other);
}

template <class T, size_t BucketSize /*= 32*/>
deque_list<T, BucketSize>::deque_list(deque_list&& other) {
	this->operator=(std::move(other));
}

template <class T, size_t BucketSize /*= 32*/>
auto deque_list<T, BucketSize>::operator=(const deque_list& other)
		-> deque_list& {
	if (this == &other) {
		return *this;
	}

	_last_bucket = &_first_bucket;
	{
		bucket* my_b = &_first_bucket;
		const bucket* other_b = &other._first_bucket;
		while (other_b != nullptr && other_b->size > size_type(0)) {
			_last_bucket = my_b;

			std::uninitialized_copy(
					other_b->begin(), other_b->end(), my_b->begin());
			my_b->size = other_b->size;

			if (other_b->next && other_b->next->size > size_type(0)) {
				my_b->next = std::make_unique<bucket>();
				my_b->next->prev = my_b;
			}

			my_b = my_b->next.get();
			other_b = other_b->next.get();
		}
	}

	_size = other._size;
	return *this;
}

template <class T, size_t BucketSize /*= 32*/>
auto deque_list<T, BucketSize>::operator=(deque_list&& other) -> deque_list& {
	if (this == &other) {
		return *this;
	}

	_last_bucket = &_first_bucket;
	{
		bucket* my_b = &_first_bucket;
		bucket* other_b = &other._first_bucket;
		while (other_b != nullptr && other_b->size > size_type(0)) {
			_last_bucket = my_b;

			std::uninitialized_move(
					other_b->begin(), other_b->end(), my_b->begin());
			my_b->size = other_b->size;

			if (other_b->next && other_b->next->size > size_type(0)) {
				my_b->next = std::make_unique<bucket>();
				my_b->next->prev = my_b;
			}

			my_b = my_b->next.get();
			other_b->size = size_type(0);
			other_b = other_b->next.get();
		}
	}

	size_type msize = other._size; // threading, always update our size last.
	other._size = size_type(0);
	other._last_bucket = &other._first_bucket;
	_size = msize;
	return *this;
}

template <class T, size_t BucketSize>
auto deque_list<T, BucketSize>::front() const -> const_reference {
	assert(!empty());
	assert(_first_bucket.size != size_type(0));
	return *_first_bucket.begin();
}

template <class T, size_t BucketSize>
auto deque_list<T, BucketSize>::front() -> reference {
	return const_cast<reference>(std::as_const(*this).front());
}

template <class T, size_t BucketSize>
auto deque_list<T, BucketSize>::back() const -> const_reference {
	assert(!empty());
	assert(_last_bucket->size != size_type(0));
	return *(_last_bucket->begin() + (_last_bucket->size - 1));
}

template <class T, size_t BucketSize>
auto deque_list<T, BucketSize>::back() -> reference {
	return const_cast<reference>(std::as_const(*this).back());
}

template <class T, size_t BucketSize>
auto deque_list<T, BucketSize>::begin() const noexcept -> const_iterator {
	assert(_last_bucket);
	return const_iterator{ &_first_bucket, size_type(0), _last_bucket.get() };
}

template <class T, size_t BucketSize>
auto deque_list<T, BucketSize>::cbegin() const noexcept -> const_iterator {
	return begin();
}

template <class T, size_t BucketSize>
auto deque_list<T, BucketSize>::begin() noexcept -> iterator {
	assert(_last_bucket);
	return iterator{ &_first_bucket, size_type(0), _last_bucket.get() };
}

template <class T, size_t BucketSize>
auto deque_list<T, BucketSize>::end() const noexcept -> const_iterator {
	assert(_last_bucket);
	return const_iterator{ _last_bucket.get(), _last_bucket->size,
		_last_bucket.get() };
}

template <class T, size_t BucketSize>
auto deque_list<T, BucketSize>::cend() const noexcept -> const_iterator {
	return end();
}

template <class T, size_t BucketSize>
auto deque_list<T, BucketSize>::end() noexcept -> iterator {
	assert(_last_bucket);
	return iterator{ _last_bucket.get(), _last_bucket->size,
		_last_bucket.get() };
}

template <class T, size_t BucketSize>
bool deque_list<T, BucketSize>::empty() const noexcept {
	assert(_last_bucket);
	return _size == size_type(0);
}

template <class T, size_t BucketSize>
auto fea::deque_list<T, BucketSize>::size() const noexcept -> size_type {
	assert(_last_bucket);
	return _size;
}

// template <class T, size_t BucketSize>
// auto forward_deque<T, BucketSize>::max_size() const noexcept -> size_type {
//	return _data.max_size();
// }

template <class T, size_t BucketSize /*= 32*/>
void fea::deque_list<T, BucketSize>::shrink_to_fit() {
	assert(_last_bucket);
	if (_last_bucket->next) {
		assert(_last_bucket->next->size == 0u);
		_last_bucket->next.reset(nullptr);
	}
}

template <class T, size_t BucketSize>
void deque_list<T, BucketSize>::clear() {
	assert(_last_bucket);
	{
		bucket* b = &_first_bucket;
		while (b != nullptr) {
			fea::destroy(b->begin(), b->end());
			b->size = size_type(0);
			b = b->next.get();
		}
	}

	_last_bucket = &_first_bucket;
	_size = 0;
	assert(empty());
}

template <class T, size_t BucketSize>
void deque_list<T, BucketSize>::push_back(const value_type& value) {
	maybe_grow();
	assert(_last_bucket->size < bucket_size);

	std::uninitialized_copy_n(
			&value, 1u, _last_bucket->begin() + _last_bucket->size++);
	++_size;
}

template <class T, size_t BucketSize>
void deque_list<T, BucketSize>::push_back(value_type&& value) {
	maybe_grow();
	assert(_last_bucket->size < bucket_size);

	std::uninitialized_move_n(
			&value, 1u, _last_bucket->begin() + _last_bucket->size++);
	++_size;
}

template <class T, size_t BucketSize>
void deque_list<T, BucketSize>::pop_back() {
	assert(!empty());
	assert(_last_bucket->size > size_type(0)
			&& _last_bucket->size <= bucket_size);

	value_type* last_ptr = &back();
	fea::destroy_at(last_ptr);

	--_last_bucket->size;
	if (_last_bucket->size == size_type(0) && _last_bucket->prev) {
		_last_bucket = _last_bucket->prev.get();
	}
	--_size;
}

template <class T, size_t BucketSize /*= 32*/>
void fea::deque_list<T, BucketSize>::maybe_grow() {
	assert(_last_bucket);
	if (_last_bucket->size == bucket_size) {
		if (_last_bucket->next == nullptr) {
			_last_bucket->next = std::make_unique<bucket>();
			_last_bucket->next->prev = _last_bucket.get();
		}

		_last_bucket = _last_bucket->next.get();
	}
}

} // namespace fea

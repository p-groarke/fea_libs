#include "global.hpp"

#include <gtest/gtest.h>
#include <iterator>

template <class T>
struct input_it {
	using value_type = T; // almost always T
	using pointer = value_type*; // almost always T*
	using reference = value_type&; // almost always T&
	using iterator_category = std::input_iterator_tag;
	using difference_type = std::ptrdiff_t; // almost always ptrdiff

	input_it(T* beg)
			: _t(beg) {
	}
	input_it() = default;

	bool operator==(const input_it& other) const {
		return _t == other._t;
	}
	bool operator!=(const input_it& other) const {
		return !(*this == other);
	}

	// use a ref for the unit tests, they compare addresses
	T& operator*() const {
		return *_t;
	}
	T* operator->() const {
		return _t;
	}

	input_it operator++() {
		++_t;
		return *this;
	}
	input_it operator++(int) {
		input_it i = *this;
		++*this;
		return i;
	}

	friend void swap(input_it& lhs, input_it& rhs) {
		using std::swap;
		swap(lhs, rhs);
	}

protected:
	T* _t = nullptr;
};

template <class T>
struct bidir_it : public input_it<T> {
	using iterator_category = std::bidirectional_iterator_tag;

	using input_it<T>::input_it;

	bidir_it operator++() {
		++input_it<T>::_t;
		return *this;
	}
	bidir_it operator++(int) {
		bidir_it i = *this;
		++*this;
		return i;
	}
	bidir_it operator--() {
		--input_it<T>::_t;
		return *this;
	}
	bidir_it operator--(int) {
		bidir_it i = *this;
		--*this;
		return i;
	}
};

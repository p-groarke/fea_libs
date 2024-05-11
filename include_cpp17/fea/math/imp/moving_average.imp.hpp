namespace fea {
namespace detail {
template <class T>
struct moving_avg_base {
	static constexpr bool is_int_v = std::is_integral_v<T>;
	using mfloat_t = std::conditional_t<sizeof(T) == 8, double, float>;

	// Prime the average to an initial value.
	void prime(const T& v) {
		_last = mfloat_t(v);
	}

	// Get the latest average.
	T get() const {
		if constexpr (is_int_v) {
			return T(std::round(_last));
		} else {
			return T(_last);
		}
	}

protected:
	mfloat_t _last = mfloat_t(0);
};
} // namespace detail


template <class T>
cumulative_average<T>::cumulative_average(size_t n) noexcept
		: _n(n) {
}

template <class T>
T cumulative_average<T>::operator()(const T& in) {
	// Unbounded.
	if (_n == 0 || _size != _n) {
		_last = (mfloat_t(in) + mfloat_t(_size) * _last) / mfloat_t(_size + 1);
		++_size;
		if constexpr (is_int_v) {
			return T(std::round(_last));
		} else {
			return T(_last);
		}
	}

	// Bounded.
	_last -= _last / _n;
	_last += mfloat_t(in) / _n;

	return get();
}

template <class T, size_t N>
T simple_moving_average<T, N>::operator()(const T& in) {
	if (_size != _circle_buf.size()) {
		// cumulative
		_circle_buf[_size++] = mfloat_t(in);
		_last = fea::mean(_circle_buf.begin(), _circle_buf.begin() + _size);

		if constexpr (is_int_v) {
			return T(std::round(_last));
		} else {
			return T(_last);
		}
	}

	// 1/k * (pn+1 - p1)
	mfloat_t new_v = (mfloat_t(in) - _circle_buf[_playhead]) * _divider;
	_last = _last + new_v;

	_circle_buf[_playhead] = mfloat_t(in);
	_playhead = _playhead + 1 == _circle_buf.size() ? 0 : _playhead + 1;

	return get();
}

template <class T>
exponential_moving_average<T>::exponential_moving_average(
		mfloat_t alpha) noexcept
		: _alpha(alpha) {
	assert(_alpha > mfloat_t(0) && _alpha < mfloat_t(1));
}

template <class T>
T exponential_moving_average<T>::operator()(const T& in) {
	mfloat_t in_f = mfloat_t(in);
	_last = (in_f * _alpha) + _last * _alpha_inv;
	if constexpr (is_int_v) {
		return T(std::round(_last));
	} else {
		return T(_last);
	}
}

template <class T, size_t N>
T weighted_moving_average<T, N>::operator()(const T& in) {
	using msize_t = std::make_signed_t<size_t>;

	if (_size != _circle_buf.size()) {
		_circle_buf[_size++] = mfloat_t(in);

		_last = mfloat_t(0);
		mfloat_t denom = (_size * (_size + 1)) / mfloat_t(2);
		for (msize_t i = _size - 1; i >= 0; --i) {
			_last += _circle_buf[i] * mfloat_t(i + 1);
		}
		_last /= denom;

		if constexpr (is_int_v) {
			return T(std::round(_last));
		} else {
			return T(_last);
		}
	}

	_circle_buf[_playhead] = mfloat_t(in);
	_playhead = _playhead + 1 == _circle_buf.size() ? 0 : _playhead + 1;

	_last = mfloat_t(0);
	mfloat_t w = mfloat_t(1);
	for (size_t i = _playhead; i < _circle_buf.size(); ++i) {
		_last += _circle_buf[i] * w++;
	}
	for (size_t i = 0; i < _playhead; ++i) {
		_last += _circle_buf[i] * w++;
	}
	_last /= _denom;

	return get();
}

template <class T, size_t N>
T moving_median<T, N>::operator()(const T& in) {
	if (_size != N) {
		_circle_buf[_size++] = mfloat_t(in);
		std::copy(_circle_buf.begin(), _circle_buf.begin() + _size,
				_sorted.begin());
		std::sort(_sorted.begin(), _sorted.begin() + _size);

		if (_size % 2 == 0) {
			mfloat_t v1 = _sorted[(_size / 2) - 1];
			mfloat_t v2 = _sorted[_size / 2];
			_last = (v1 + v2) / mfloat_t(2);
		} else {
			_last = _sorted[(_size - 1) / 2];
		}

		if constexpr (is_int_v) {
			return T(std::round(_last));
		} else {
			return T(_last);
		}
	}

	_circle_buf[_playhead] = mfloat_t(in);
	_playhead = _playhead + 1 == _circle_buf.size() ? 0 : _playhead + 1;

	std::copy(_circle_buf.begin(), _circle_buf.end(), _sorted.begin());
	std::sort(_sorted.begin(), _sorted.end());

	if constexpr (N % 2 == 0) {
		mfloat_t v1 = _sorted[(_size / 2) - 1];
		mfloat_t v2 = _sorted[_size / 2];
		_last = (v1 + v2) / mfloat_t(2);
	} else {
		_last = _sorted[(_size - 1) / 2];
	}

	return get();
}
} // namespace fea
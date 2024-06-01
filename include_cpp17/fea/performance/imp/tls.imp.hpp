namespace fea {
template <class T, class Alloc>
tls_lock<T, Alloc>::tls_lock(tls<T, Alloc>& storage)
		: tls_lock(std::move(storage.lock())) {
}

template <class T, class Alloc>
tls_lock<T, Alloc>::tls_lock(tls_lock&& other) noexcept
		: _tid(other._tid)
		, _data_idx(other._data_idx)
		, _value(other._value)
		, _storage(other._storage) {
	other._tid = std_thread_id_t{};
	other._data_idx = (std::numeric_limits<uint32_t>::max)();
	assert(_tid == std::this_thread::get_id());
	assert(_tid != std_thread_id_t{});
	assert(_data_idx != (std::numeric_limits<uint32_t>::max)());
}

template <class T, class Alloc>
tls_lock<T, Alloc>::tls_lock(std_thread_id_t tid, uint32_t data_idx, T& value,
		tls<T, Alloc>& storage) noexcept
		: _tid(tid)
		, _data_idx(data_idx)
		, _value(value)
		, _storage(storage) {
	assert(_tid == std::this_thread::get_id());
	assert(_tid != std_thread_id_t{});
	assert(_data_idx != (std::numeric_limits<uint32_t>::max)());
}

template <class T, class Alloc>
tls_lock<T, Alloc>::~tls_lock() {
	if (_tid != std_thread_id_t{}) {
		assert(_tid == std::this_thread::get_id());
		assert(_data_idx != (std::numeric_limits<uint32_t>::max)());
		_storage.unlock(_tid, _data_idx);
	}
}


template <class T, class Alloc>
const T& tls_lock<T, Alloc>::local() const& {
	assert(_tid == std::this_thread::get_id());
	assert(_tid != std_thread_id_t{});
	assert(_data_idx != (std::numeric_limits<uint32_t>::max)());
	return _value;
}

template <class T, class Alloc>
T& tls_lock<T, Alloc>::local() & {
	assert(_tid == std::this_thread::get_id());
	assert(_tid != std_thread_id_t{});
	assert(_data_idx != (std::numeric_limits<uint32_t>::max)());
	return _value;
}


template <class T, class Alloc>
tls<T, Alloc>::tls() {
	init();
}

template <class T, class Alloc>
void tls<T, Alloc>::init() {
	std_thread_id_t tid = std::this_thread::get_id();

	// We MUST initialize the locks deque to guarantee the begin
	// iterator is always valid.
	std::lock_guard<std::shared_mutex> g{ _mutex };
	assert(_locks.size() == _valid_locks_size);
	assert(_datas.size() == _valid_locks_size);

	// Create new lock + data slot.
	_datas.emplace_back();
	_locks.push_back(thread_info{ tid, 0u, false });
	_first_lock = &_locks.front();
	_valid_locks_size = 1u;

	assert(_locks.size() == _valid_locks_size);
	assert(_datas.size() == _valid_locks_size);
	assert(_first_lock != nullptr);
}

template <class T, class Alloc>
tls<T, Alloc>::~tls() {
	std::lock_guard<std::shared_mutex> g{ _mutex };
	assert(_locks.size() == _valid_locks_size);
	assert(_datas.size() == _valid_locks_size);
	assert(_first_lock != nullptr);

	for (const thread_info& ti : _locks) {
		if (ti.locked) {
			fea::maybe_throw<std::runtime_error>(__FUNCTION__, __LINE__,
					"Destroying storage with unreleased locks. Make sure "
					"all your threads are done working before destroying "
					"storage.");
		}
	}
}

// Threads have pre-assigned indexes in our data deque.
// This allows us to search for free data quickly, without locking.
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
		size_type size = _valid_locks_size;

		auto it = std::find_if(_locks.begin(), _locks.begin() + size,
				[&](const thread_info& ti) {
					return ti.thread_id == tid && ti.locked == false;
				});

		// End iterator could have changed, compare with our cached size.
		if (it != _locks.begin() + size) {
			// Found free thread slot.
			assert(it->data_idx < size);
			assert(!it->locked);

			// Make sure only 1 thread points to the data idx.
			assert(std::count_if(_locks.begin(), _locks.begin() + size,
						   [&](const thread_info& ti) {
							   return ti.data_idx == it->data_idx;
						   })
					== ptrdiff_t(1));

			it->locked = true;
			return tls_lock<T, Alloc>{
				tid,
				it->data_idx,
				_datas[it->data_idx],
				*this,
			};
		}
	}

	// If we reach here, we must initialize data and modify the locks deque.
	// We lock.
	std::lock_guard<std::shared_mutex> g{ _mutex };
	assert(_locks.size() == _valid_locks_size);
	assert(_datas.size() == _valid_locks_size);
	// assert(_locks_traversal.size() == _valid_locks_size);

	// Create new (locked) thread info.
	thread_info new_ti{ tid, uint32_t(_datas.size()), true };

	// Create new lock + data slot, then update valid size.
	_datas.emplace_back();
	_locks.push_back(new_ti);
	_valid_locks_size = _locks.size();
	assert(_locks.size() == _valid_locks_size);
	assert(_datas.size() == _valid_locks_size);
	// assert(_locks_traversal.size() == _valid_locks_size);

	// Only 1 lock points to data.
	assert(std::count_if(_locks.begin(), _locks.end(),
				   [&, this](const thread_info& ti) {
					   return ti.data_idx == _locks.back().data_idx;
				   })
			== ptrdiff_t(1));

	return tls_lock<T, Alloc>{
		tid,
		new_ti.data_idx,
		_datas[new_ti.data_idx],
		*this,
	};

	//// Create thread data if we don't recognize this thread id
	//// or if this thread already has a lock (recursive).
	// auto it = find_first_unlocked(tid);
	// if (it == _locks.end()) {
	//	assert(_locks.size() == _datas.size());
	//	// assert(_thread_info.count(tid) == 0);

	//	uint32_t idx = uint32_t(_datas.size());
	//	_datas.emplace_back();
	//	assert(idx < _datas.size());

	//	auto it = _locks.insert({ tid, thread_info{ false, idx } });
	//	assert(_locks.size() == _datas.size());
	//}

	//// Lock data and return RAII helper.
	// thread_info& tinfo = it->second;
	// if (tinfo.locked) {
	//	fea::maybe_throw<std::runtime_error>(__FUNCTION__, __LINE__,
	//			"Trying to access data locked by an other thread.");
	// }

	// assert(tinfo.data_idx < _datas.size());
	// assert(!tinfo.locked);
	// tinfo.locked = true;
	// return tls_lock<T, Alloc>{
	//	tid,
	//	tinfo.data_idx,
	//	_datas[tinfo.data_idx],
	//	*this,
	// };
}

template <class T, class Alloc>
void tls<T, Alloc>::unlock(std_thread_id_t tid, uint32_t data_idx) {
	// Since the lock should have been created already, no need for locking.
	// Just use our cached valid size.
	size_type size = _valid_locks_size;

	auto it = std::find_if(
			_locks.begin(), _locks.begin() + size, [&](const thread_info& ti) {
				return ti.thread_id == tid && ti.data_idx == data_idx;
			});

	if (it == _locks.begin() + size) {
		fea::maybe_throw<std::runtime_error>(__FUNCTION__, __LINE__,
				"Trying to unlock tls that doesn't exist.");
	}

	if (!it->locked) {
		fea::maybe_throw<std::runtime_error>(__FUNCTION__, __LINE__,
				"Trying to unlock tls that isn't locked.");
	}

	// Only 1 lock points to data.
	assert(std::count_if(_locks.begin(), _locks.begin() + size,
				   [&, this](const thread_info& ti) {
					   return ti.data_idx == it->data_idx;
				   })
			== ptrdiff_t(1));

	// Unlock it.
	it->locked = false;
}

template <class T, class Alloc>
bool tls<T, Alloc>::empty() const {
	return _valid_locks_size == 0u;
}

template <class T, class Alloc>
auto tls<T, Alloc>::size() const -> size_type {
	return _valid_locks_size;
}

template <class T, class Alloc>
void tls<T, Alloc>::clear() {
	std::lock_guard<std::shared_mutex> g{ _mutex };
	assert(_locks.size() == _valid_locks_size);
	assert(_datas.size() == _valid_locks_size);
	// assert(_locks_traversal.size() == _valid_locks_size);

	for (const thread_info& ti : _locks) {
		if (ti.locked) {
			fea::maybe_throw<std::runtime_error>(__FUNCTION__, __LINE__,
					"Cannot clear storage, at least 1 thread currently "
					"owns a lock on storage.");
		}
	}
	_locks.clear();
	_datas.clear();
	_valid_locks_size = 0u;
}

template <class T, class Alloc>
template <class Func>
void tls<T, Alloc>::combine_each(Func&& func) const {
	std::lock_guard<std::shared_mutex> g{ _mutex };
	assert(_locks.size() == _valid_locks_size);
	assert(_datas.size() == _valid_locks_size);
	// assert(_locks_traversal.size() == _valid_locks_size);

	for (const thread_info& ti : _locks) {
		if (ti.locked) {
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
	assert(_locks.size() == _valid_locks_size);
	assert(_datas.size() == _valid_locks_size);
	// assert(_locks_traversal.size() == _valid_locks_size);

	for (const thread_info& ti : _locks) {
		if (ti.locked) {
			fea::maybe_throw<std::runtime_error>(__FUNCTION__, __LINE__,
					"Cannot combine storage, at least 1 thread still holds "
					"a lock.");
		}
	}

	for (T& t : _datas) {
		func(t);
	}
}

template <class T, class Alloc>
template <class FuncT>
auto tls<T, Alloc>::find_if(thread_info* first, size_type count, FuncT&& func)
		-> thread_info* {
	for (size_type i = 0; i < count; ++i) {
		assert(first != nullptr);
		bool r = func(*first);
		if (r) {
			return first;
		}

		first = first->next;
	}
	return nullptr;
}
} // namespace fea

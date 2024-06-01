namespace fea {

template <class EventEnum, EventEnum e>
struct event_id {
	event_id() = default;
	~event_id() = default;
	event_id(const event_id&) = default;
	event_id(event_id&&) = default;
	event_id& operator=(const event_id&) = default;
	event_id& operator=(event_id&&) = default;

private:
	template <class, class...>
	friend struct event_stack;

	event_id(size_t id)
			: _id(id) {
	}

	size_t _id = (std::numeric_limits<size_t>::max)();
};

// template <class EventEnum, EventEnum e>
// fea::event_id<EventEnum, e>::event_id(size_t id)
//		: _id(id) {
// }


template <class EventEnum, class... FuncTypes>
template <EventEnum e>
bool event_stack<EventEnum, FuncTypes...>::contains(event_id<EventEnum, e> id) {
	return std::get<size_t(e)>(_stacks).contains(id._id);
}

template <class EventEnum, class... FuncTypes>
template <EventEnum e>
const auto& event_stack<EventEnum, FuncTypes...>::at(
		event_id<EventEnum, e> id) const {
	return std::get<size_t(e)>(_stacks).at(id._id);
}

template <class EventEnum, class... FuncTypes>
template <EventEnum e>
auto& event_stack<EventEnum, FuncTypes...>::at(event_id<EventEnum, e> id) {
	return std::get<size_t(e)>(_stacks).at(id._id);
}

template <class EventEnum, class... FuncTypes>
template <EventEnum e>
const auto& event_stack<EventEnum, FuncTypes...>::at_unchecked(
		event_id<EventEnum, e> id) const {
	assert(contains<e>(id));
	return std::get<size_t(e)>(_stacks).at_unchecked(id._id);
}

template <class EventEnum, class... FuncTypes>
template <EventEnum e>
auto& event_stack<EventEnum, FuncTypes...>::at_unchecked(
		event_id<EventEnum, e> id) {
	assert(contains<e>(id));
	return std::get<size_t(e)>(_stacks).at_unchecked(id._id);
}

template <class EventEnum, class... FuncTypes>
bool event_stack<EventEnum, FuncTypes...>::empty() const noexcept {
	bool ret = true;
	tuple_for_each([&](const auto& map) { ret &= map.empty(); }, _stacks);
	return ret;
}

template <class EventEnum, class... FuncTypes>
template <EventEnum e>
bool event_stack<EventEnum, FuncTypes...>::empty() const noexcept {
	return std::get<size_t(e)>(_stacks).empty();
}

template <class EventEnum, class... FuncTypes>
size_t event_stack<EventEnum, FuncTypes...>::size() const noexcept {
	size_t ret = 0;
	tuple_for_each([&](const auto& map) { ret += map.size(); }, _stacks);
	return ret;
}

template <class EventEnum, class... FuncTypes>
template <EventEnum e>
size_t event_stack<EventEnum, FuncTypes...>::size() const noexcept {
	return std::get<size_t(e)>(_stacks).size();
}

template <class EventEnum, class... FuncTypes>
void event_stack<EventEnum, FuncTypes...>::reserve(size_t new_cap) {
	tuple_for_each([&](auto& map) { map.reserve(new_cap); }, _stacks);
}

template <class EventEnum, class... FuncTypes>
template <EventEnum e>
void event_stack<EventEnum, FuncTypes...>::reserve(size_t new_cap) {
	std::get<size_t(e)>(_stacks).reserve(new_cap);
}

template <class EventEnum, class... FuncTypes>
template <EventEnum e>
size_t event_stack<EventEnum, FuncTypes...>::capacity() const noexcept {
	return std::get<size_t(e)>(_stacks).capacity();
}

template <class EventEnum, class... FuncTypes>
template <EventEnum e>
void event_stack<EventEnum, FuncTypes...>::clear() {
	std::get<size_t(e)>(_stacks).clear();
}

template <class EventEnum, class... FuncTypes>
void event_stack<EventEnum, FuncTypes...>::clear() {
	tuple_for_each([](auto& map) { map.clear(); }, _stacks);
}

template <class EventEnum, class... FuncTypes>
template <EventEnum e, class Func>
fea::event_id<EventEnum, e> event_stack<EventEnum, FuncTypes...>::subscribe(
		Func&& callback) {
	size_t& id_generator = std::get<size_t(e)>(_id_generators);
	assert(id_generator != (std::numeric_limits<size_t>::max)());

	// 0 is never used, reserved for future.
	++id_generator;

	// Insert callback.
	std::get<size_t(e)>(_stacks).insert(
			{ id_generator, std::forward<Func>(callback) });

	// Return id.
	return { id_generator };
}

template <class EventEnum, class... FuncTypes>
template <EventEnum e>
void event_stack<EventEnum, FuncTypes...>::unsubscribe(
		event_id<EventEnum, e> id) {
	std::get<size_t(e)>(_stacks).erase(id._id);
}

template <class EventEnum, class... FuncTypes>
template <EventEnum e, class... FuncArgs>
void event_stack<EventEnum, FuncTypes...>::trigger(
		FuncArgs&&... func_args) const {
	for (const auto& func_pair : std::get<size_t(e)>(_stacks)) {
		// std::invoke is not compile time, plus it makes debugging
		// difficult.
		func_pair.second(std::forward<FuncArgs>(func_args)...);
	}
}

template <class EventEnum, class... FuncTypes>
template <EventEnum e, class... FuncArgs>
void event_stack<EventEnum, FuncTypes...>::trigger(FuncArgs&&... func_args) {
	for (auto& func_pair : std::get<size_t(e)>(_stacks)) {
		func_pair.second(std::forward<FuncArgs>(func_args)...);
	}
}

#if FEA_WITH_TBB
template <class EventEnum, class... FuncTypes>
template <EventEnum e, class... FuncArgs>
void event_stack<EventEnum, FuncTypes...>::trigger_mt(
		FuncArgs&&... func_args) const {
	const auto& map = std::get<size_t(e)>(_stacks);
	auto eval = [&, this](const tbb::blocked_range<size_t>& range) {
		for (size_t i = range.begin(); i < range.end(); ++i) {
			map.data()[i].second(std::forward<FuncArgs>(func_args)...);
		}
	};

	tbb::blocked_range<size_t> range{
		0,
		map.size(),
		fea::default_grainsize_small_v<true>,
	};
	tbb::parallel_for(range, eval, fea::default_partitioner_t<true>{});
}

template <class EventEnum, class... FuncTypes>
template <EventEnum e, class... FuncArgs>
void event_stack<EventEnum, FuncTypes...>::trigger_mt(FuncArgs&&... func_args) {
	auto& map = std::get<size_t(e)>(_stacks);
	auto eval = [&](const tbb::blocked_range<size_t>& range) {
		for (size_t i = range.begin(); i < range.end(); ++i) {
			map.data()[i].second(std::forward<FuncArgs>(func_args)...);
		}
	};
	tbb::blocked_range<size_t> range{
		0,
		map.size(),
		fea::default_grainsize_small_v<true>,
	};
	tbb::parallel_for(range, eval, fea::default_partitioner_t<true>{});
}

#endif

} // namespace fea

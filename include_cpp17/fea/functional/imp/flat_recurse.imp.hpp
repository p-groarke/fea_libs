namespace fea {
template <class InputIt, class StatePtr /* = const void */>
std::pair<InputIt, InputIt> children_range(InputIt parent, StatePtr*) {
	return { parent->begin(), parent->end() };
}

template <class InputIt, class Func, class CullPredicate,
		class StatePtr /* = const void */>
void for_each_depthfirst(InputIt root, Func&& func, CullPredicate cull_pred,
		StatePtr* state_ptr /* = nullptr */) {
	// Traditional depth-first recursion.
	if (cull_pred(root)) {
		return;
	}

	func(root);

	using fea::children_range;
	std::pair<InputIt, InputIt> range = children_range(root, state_ptr);

	for (auto it = range.first; it != range.second; ++it) {
		for_each_depthfirst(it, func, cull_pred, state_ptr);
	}
}

template <class InputIt, class Func, class StatePtr /* = const void */>
void for_each_depthfirst(
		InputIt root, Func&& func, StatePtr* state_ptr /* = nullptr */) {

	return for_each_depthfirst(
			root, func, [](InputIt) { return false; }, state_ptr);
}

template <class BidirIt, class Func, class CullPredicate,
		class StatePtr /* = const void */>
void for_each_depthfirst_flat(BidirIt root, Func&& func,
		CullPredicate cull_pred, StatePtr* state_ptr /* = nullptr */) {
	static_assert(
			!std::is_same_v<std::input_iterator_tag,
					typename std::iterator_traits<BidirIt>::iterator_category>,
			"for_each_flat_depth : iterators must be at minimum bidirectional");
	static_assert(
			!std::is_same_v<std::output_iterator_tag,
					typename std::iterator_traits<BidirIt>::iterator_category>,
			"for_each_flat_depth : iterators must be at minimum bidirectional");
	static_assert(
			!std::is_same_v<std::forward_iterator_tag,
					typename std::iterator_traits<BidirIt>::iterator_category>,
			"for_each_flat_depth : iterators must be at minimum bidirectional");

	// Uses a "rolling vector" to flatten out graph and execute function on
	// those nodes.
	// For performance reasons, the children are inversed and the vector acts as
	// a stack.
	// Theoretical algo : Take stack front node, remove from stack,
	// execute func, gather its children, pushfront in stack. Rince-repeat until
	// vector empty.

	if (cull_pred(root)) {
		return;
	}

	std::vector<BidirIt> stack;
	stack.push_back(root);

	while (true) {
		if (stack.empty()) {
			// We are done.
			break;
		}

		// Get next in line.
		// Don't need to check cull_pred here, culled nodes are never
		// pushed back in waiting.
		BidirIt current_node = stack.back();
		stack.pop_back();
		func(current_node);

		using fea::children_range;
		std::pair<BidirIt, BidirIt> range
				= children_range(current_node, state_ptr);

		// Find first non-culled child.
		while (range.first != range.second && cull_pred(range.first)) {
			++range.first;
		}

		// All children culled, evaluate next waiting.
		if (range.first == range.second) {
			continue;
		}

		// Cull remaining children and enqueue in the stack back to front.
		while (--range.second != range.first) {
			if (cull_pred(range.second)) {
				continue;
			}
			stack.push_back(range.second);
		}

		if (!cull_pred(range.first)) {
			stack.push_back(range.first);
		}
	}
}

template <class BidirIt, class Func, class StatePtr /* = const void */>
void for_each_depthfirst_flat(
		BidirIt root, Func&& func, StatePtr* state_ptr /* = nullptr */) {

	return for_each_depthfirst_flat(
			root, func, [](BidirIt) { return false; }, state_ptr);
}

template <class InputIt, class Func, class CullPredicate,
		class StatePtr /* = const void */>
void for_each_breadthfirst(InputIt root, Func&& func, CullPredicate cull_pred,
		StatePtr* state_ptr /* = nullptr */) {
	std::vector<InputIt> graph;
	gather_breadthfirst(root, cull_pred, &graph, state_ptr);

	for (InputIt it : graph) {
		func(it);
	}
}

template <class InputIt, class Func, class StatePtr /* = const void */>
void for_each_breadthfirst(
		InputIt root, Func&& func, StatePtr* state_ptr /* = nullptr */) {
	std::vector<InputIt> graph;
	gather_breadthfirst(root, &graph, state_ptr);

	for (InputIt it : graph) {
		func(it);
	}
}

template <class InputIt, class CullPredicate, class StatePtr /* = const void */>
void gather_depthfirst(InputIt root, std::vector<InputIt>* out,
		CullPredicate cull_pred, StatePtr* state_ptr /* = nullptr */) {
	return for_each_depthfirst(
			root, [&](InputIt node) { out->push_back(node); }, cull_pred,
			state_ptr);
}

template <class InputIt, class StatePtr /* = const void */>
void gather_depthfirst(InputIt root, std::vector<InputIt>* out,
		StatePtr* state_ptr /* = nullptr */) {
	return gather_depthfirst(
			root, out, [](InputIt) { return false; }, state_ptr);
}

template <class BidirIt, class CullPredicate, class StatePtr /* = const void */>
void gather_depthfirst_flat(BidirIt root, CullPredicate cull_pred,
		std::vector<BidirIt>* out, StatePtr* state_ptr /* = nullptr */) {
	return for_each_depthfirst_flat(
			root, [&](BidirIt node) { out->push_back(node); }, cull_pred,
			state_ptr);
}

template <class BidirIt, class StatePtr /* = const void */>
void gather_depthfirst_flat(BidirIt root, std::vector<BidirIt>* out,
		StatePtr* state_ptr /* = nullptr */) {
	return gather_depthfirst_flat(
			root, [](BidirIt) { return false; }, out, state_ptr);
}

template <class InputIt, class CullPredicate, class StatePtr /* = const void */>
void gather_breadthfirst(InputIt root, CullPredicate cull_pred,
		std::vector<InputIt>* out, StatePtr* state_ptr /* = nullptr */) {
	// Grab children, pushback range if not culled, rince-repeat.
	// Continue looping the vector until you reach end.
	if (cull_pred(root)) {
		return;
	}

	out->push_back(root);
	for (size_t i = 0; i < out->size(); ++i) {
		using fea::children_range;
		std::pair<InputIt, InputIt> range
				= children_range((*out)[i], state_ptr);

		for (InputIt it = range.first; it != range.second; ++it) {
			if (cull_pred(it)) {
				continue;
			}

			out->push_back(it);
		}
	}
}

template <class InputIt, class StatePtr /* = const void */>
void gather_breadthfirst(InputIt root, std::vector<InputIt>* out,
		StatePtr* state_ptr /* = nullptr */) {
	return gather_breadthfirst(
			root, [](InputIt) { return false; }, out, state_ptr);
}

template <class InputIt, class CullPredicate, class StatePtr /* = const void */>
void gather_breadthfirst_staged(InputIt root, CullPredicate cull_pred,
		std::vector<std::vector<InputIt>>* out,
		StatePtr* state_ptr /* = nullptr */) {
	if (cull_pred(root)) {
		return;
	}

	out->push_back({ root });
	for (size_t i = 0; i < out->size(); ++i) {
		for (size_t j = 0; j < (*out)[i].size(); ++j) {
			using fea::children_range;
			std::pair<InputIt, InputIt> range
					= children_range((*out)[i][j], state_ptr);

			if (out->size() == i + 1 && range.first != range.second) {
				out->push_back({});
				// Expect at least as much as previous.
				out->back().reserve((*out)[i].size());
			}

			for (InputIt it = range.first; it != range.second; ++it) {
				if (cull_pred(it)) {
					continue;
				}
				(*out)[i + 1].push_back(it);
			}
		}
	}
}

template <class InputIt, class StatePtr /* = const void */>
void gather_breadthfirst_staged(InputIt root,
		std::vector<std::vector<InputIt>>* out,
		StatePtr* state_ptr /* = nullptr */) {
	gather_breadthfirst_staged(
			root, [](InputIt) { return false; }, out, state_ptr);
}
} // namespace fea

#pragma once
namespace fea {
/**
 * utility_ai_function
 */
template <class FunctionEnum, class PredicateEnum, class... PredArgs,
		class ActionReturn, class... ActionArgs>
void utility_ai_function<FunctionEnum, PredicateEnum, float(PredArgs...),
		ActionReturn(ActionArgs...)>::
		add_predicates(fea::span<const PredicateEnum> preds) {
	if (preds.size() + _predicates.size() > _max_predicates) {
		fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
				"Too many predicates provided, do you have duplicates?");
	}
	_predicates.insert(_predicates.end(), preds.begin(), preds.end());
}

template <class FunctionEnum, class PredicateEnum, class... PredArgs,
		class ActionReturn, class... ActionArgs>
void utility_ai_function<FunctionEnum, PredicateEnum, float(PredArgs...),
		ActionReturn(ActionArgs...)>::
		add_predicates(std::initializer_list<PredicateEnum>&& preds) {
	add_predicates({ preds.begin(), preds.end() });
}

template <class FunctionEnum, class PredicateEnum, class... PredArgs,
		class ActionReturn, class... ActionArgs>
void utility_ai_function<FunctionEnum, PredicateEnum, float(PredArgs...),
		ActionReturn(ActionArgs...)>::add_predicate(PredicateEnum pred) {
	if (_predicates.size() + 1 > _max_predicates) {
		fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
				"Too many predicates provided, do you have duplicates?");
	}
	_predicates.push_back(pred);
}

template <class FunctionEnum, class PredicateEnum, class... PredArgs,
		class ActionReturn, class... ActionArgs>
template <class ActionFunc>
void utility_ai_function<FunctionEnum, PredicateEnum, float(PredArgs...),
		ActionReturn(ActionArgs...)>::add_action(ActionFunc&& func) {
	_action = std::forward<ActionFunc>(func);
	assert(has_action());
}

template <class FunctionEnum, class PredicateEnum, class... PredArgs,
		class ActionReturn, class... ActionArgs>
fea::span<const PredicateEnum> utility_ai_function<FunctionEnum, PredicateEnum,
		float(PredArgs...), ActionReturn(ActionArgs...)>::predicates() const {
	return { _predicates.begin(), _predicates.end() };
}

template <class FunctionEnum, class PredicateEnum, class... PredArgs,
		class ActionReturn, class... ActionArgs>
bool utility_ai_function<FunctionEnum, PredicateEnum, float(PredArgs...),
		ActionReturn(ActionArgs...)>::has_action() const {
	return bool(_action);
}

template <class FunctionEnum, class PredicateEnum, class... PredArgs,
		class ActionReturn, class... ActionArgs>
size_t utility_ai_function<FunctionEnum, PredicateEnum, float(PredArgs...),
		ActionReturn(ActionArgs...)>::size() const {
	return _predicates.size();
}

template <class FunctionEnum, class PredicateEnum, class... PredArgs,
		class ActionReturn, class... ActionArgs>
ActionReturn
utility_ai_function<FunctionEnum, PredicateEnum, float(PredArgs...),
		ActionReturn(ActionArgs...)>::execute(ActionArgs... args) const {
	assert(has_action());
	return _action(std::forward<ActionArgs>(args)...);
}


/**
 * utility_ai
 */
template <class FunctionEnum, class PredicateEnum, class... PredArgs,
		class ActionReturn, class... ActionArgs>
constexpr auto utility_ai<FunctionEnum, PredicateEnum, float(PredArgs...),
		ActionReturn(ActionArgs...)>::make_function() {
	return utility_func_t{};
}

template <class FunctionEnum, class PredicateEnum, class... PredArgs,
		class ActionReturn, class... ActionArgs>
template <FunctionEnum F>
void utility_ai<FunctionEnum, PredicateEnum, float(PredArgs...),
		ActionReturn(ActionArgs...)>::add_function(utility_func_t&&
				utility_function) {
	if (utility_function.size() == 0 || !utility_function.has_action()) {
		fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
				"Misconfigured utility function provided.");
	}

	assert(!utility_function.predicates().empty());
	std::get<size_t(F)>(_utility_functions) = std::move(utility_function);
}

template <class FunctionEnum, class PredicateEnum, class... PredArgs,
		class ActionReturn, class... ActionArgs>
template <PredicateEnum P, class PredFunc>
void utility_ai<FunctionEnum, PredicateEnum, float(PredArgs...),
		ActionReturn(ActionArgs...)>::add_predicate(PredFunc&& pred) {
	std::get<size_t(P)>(_predicates) = std::forward<PredFunc>(pred);
}

template <class FunctionEnum, class PredicateEnum, class... PredArgs,
		class ActionReturn, class... ActionArgs>
ActionReturn utility_ai<FunctionEnum, PredicateEnum, float(PredArgs...),
		ActionReturn(ActionArgs...)>::trigger(ActionArgs... action_args,
		PredArgs... predicate_args) {

	// Don't use std::max_element, it would compute score twice for each
	// utility func.

	size_t winner_idx = (std::numeric_limits<size_t>::max)();
	float max_score = (std::numeric_limits<float>::lowest)();

	for (size_t i = 0; i < size_t(FunctionEnum::count); ++i) {
		// predicate_args are not forwarded, as they are used for multiple
		// predicates.
		fea::span<const PredicateEnum> preds
				= _utility_functions[i].predicates();
		float score = evaluate_score(preds, predicate_args...);

		// TODO : What happens when predicates return negative, should
		// guarantee skip?
		if (score > max_score) {
			winner_idx = i;
			max_score = score;
		}
	}

	// Something went horribly wrong.
	assert(winner_idx != (std::numeric_limits<size_t>::max)());
	return _utility_functions[winner_idx].execute(
			std::forward<ActionArgs>(action_args)...);
}

#if FEA_WITH_TBB
template <class FunctionEnum, class PredicateEnum, class... PredArgs,
		class ActionReturn, class... ActionArgs>
ActionReturn utility_ai<FunctionEnum, PredicateEnum, float(PredArgs...),
		ActionReturn(ActionArgs...)>::trigger_mt(ActionArgs... action_args,
		PredArgs... predicate_args) {
	std::array<float, size_t(FunctionEnum::count)> scores;
	auto eval = [&, this](const tbb::blocked_range<size_t>& range) {
		for (size_t i = range.begin(); i < range.end(); ++i) {
			fea::span<const PredicateEnum> preds
					= _utility_functions[i].predicates();
			scores[i] = evaluate_score(preds, predicate_args...);
		}
	};
	tbb::blocked_range<size_t> range{
		0,
		size_t(FunctionEnum::count),
		fea::default_grainsize_small_v<true>,
	};
	tbb::parallel_for(range, eval, fea::default_partitioner_t<true>{});

	size_t winner_idx = (std::numeric_limits<size_t>::max)();
	float max_score = (std::numeric_limits<float>::lowest)();

	// TODO : What happens when predicates return negative,
	// should guarantee skip?
	for (size_t i = 0; i < size_t(FunctionEnum::count); ++i) {
		if (scores[i] > max_score) {
			winner_idx = i;
			max_score = scores[i];
		}
	}

	// Something went horribly wrong.
	assert(winner_idx != (std::numeric_limits<size_t>::max)());
	return _utility_functions[winner_idx].execute(
			std::forward<ActionArgs>(action_args)...);
}
#endif

template <class FunctionEnum, class PredicateEnum, class... PredArgs,
		class ActionReturn, class... ActionArgs>
float utility_ai<FunctionEnum, PredicateEnum, float(PredArgs...),
		ActionReturn(ActionArgs...)>::
		evaluate_score(fea::span<const PredicateEnum> preds,
				PredArgs... pred_args) const {

	assert(!preds.empty());

	// Compute the functions score and average it.
	float ret = 0.f;
	for (PredicateEnum pred : preds) {
		ret += _predicates[size_t(pred)](pred_args...);
	}
	return ret / float(preds.size());
}
} // namespace fea
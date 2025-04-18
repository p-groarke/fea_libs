/**
 * Copyright (c) 2024, Philippe Groarke
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once
#include "fea/containers/span.hpp"
#include "fea/containers/stack_vector.hpp"
#include "fea/functional/function.hpp"
#include "fea/meta/static_for.hpp"
#include "fea/performance/constants.hpp"
#include "fea/utils/platform.hpp"
#include "fea/utils/throw.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdio>
#include <functional>
#include <limits>
#include <vector>

#if FEA_WITH_TBB
#if FEA_WINDOWS
#pragma warning(push)
#pragma warning(disable : 4459)
#include <tbb/parallel_for.h>
#pragma warning(pop)
#else
#include <tbb/parallel_for.h>
#endif
#endif

/*
utility_ai stores predicates and actions as "functions" and executes the highest
desirable action when asked to.

A function is a combo of multiple predicates and an action. utility_ai calls an
appropriate action if its predicates "wins" over others. When multiple
predicates are provide for an action, their return values are averaged.

Behaviors
- A function must contain at minimum one predicate.
- All utility functions must be valid when you call the first trigger.
QUESTION : When a predicate returns negative, should the action be ignore
completely, or should it still compute average.

When creating a utility_ai, you must provide your action and predicate
arguments as you would to a std::function. The return type must be float for
predicates.
For example,

enum class function {
	eat,
	sleep,
	count // count is mandatory
};

enum class predicate {
	wants_to_eat,
	wants_to_sleep,
	count // count is mandatory
};

fea::utility_ai<function, predicate, float(int), void(int&, double)> ai;

The predicate signature is : float(int)
The action signature is : void(int&, double)

When evaluating the utility_ai and triggering an action, provide action
arguments first and then the predicate arguments. For example :

ai.trigger(int&, double, int);

TODO :
CaptureLess Mode
By default, the container stores your actions and predicates in std::function.
This can be an issue for various reasons. To enable storing your callbacks as
raw function pointers, use the _cl alias. For example,

fea::utility_ai_cl<utility_function, void(int&, double), float(int)> ai;
*/

namespace fea {
template <class, class, class, class>
struct utility_ai;
template <class, class, class, class>
struct utility_ai_function;

template <class FunctionEnum, class PredicateEnum, class... PredArgs,
		class ActionReturn, class... ActionArgs>
struct utility_ai_function<FunctionEnum, PredicateEnum, float(PredArgs...),
		ActionReturn(ActionArgs...)> {
	// Typedefs
	using action_t = std::function<ActionReturn(ActionArgs...)>;

	// Ctors
	utility_ai_function() = default;
	~utility_ai_function() = default;
	utility_ai_function(const utility_ai_function&) = default;
	utility_ai_function(utility_ai_function&&) = default;
	utility_ai_function& operator=(const utility_ai_function&) = default;
	utility_ai_function& operator=(utility_ai_function&&) = default;

	// Enables the provided predicates on this utility function.
	void add_predicates(fea::span<const PredicateEnum> preds);

	// Enables the provided predicates on this utility function.
	void add_predicates(std::initializer_list<PredicateEnum>&& preds);

	// Enables the provided predicate on this utility function.
	void add_predicate(PredicateEnum pred);

	// Adds an action to execute.
	template <class ActionFunc>
	void add_action(ActionFunc&& func);

	// The predicates to use.
	fea::span<const PredicateEnum> predicates() const;

	// Has an action.
	bool has_action() const;

	// Number of predicates.
	size_t size() const;

	ActionReturn execute(ActionArgs... args) const;

private:
	static constexpr size_t _max_predicates = size_t(PredicateEnum::count);

	fea::stack_vector<PredicateEnum, _max_predicates> _predicates;
	action_t _action;
};


template <class FunctionEnum, class PredicateEnum, class... PredArgs,
		class ActionReturn, class... ActionArgs>
struct utility_ai<FunctionEnum, PredicateEnum, float(PredArgs...),
		ActionReturn(ActionArgs...)> {
	// Typedefs
	using utility_func_t = utility_ai_function<FunctionEnum, PredicateEnum,
			float(PredArgs...), ActionReturn(ActionArgs...)>;
	using action_t = typename utility_func_t::action_t;
	using predicate_func_t = std::function<float(PredArgs...)>;

	// Ctors
	utility_ai() = default;
	~utility_ai() = default;
	utility_ai(const utility_ai&) = default;
	utility_ai(utility_ai&&) = default;
	utility_ai& operator=(const utility_ai&) = default;
	utility_ai& operator=(utility_ai&&) = default;

	// Helper so you don't have to type all the template parameters.
	// Returns a new state to be filled in and later re-added through
	// add_function.
	static constexpr auto make_function();

	// Adds the utility function F.
	// Must be configured appropriately, with at minimum 1 predicate and an
	// action.
	template <FunctionEnum F>
	void add_function(utility_func_t&& utility_function);

	// Adds the given predicate and assigns it to provided enum value.
	template <PredicateEnum P, class PredFunc>
	void add_predicate(PredFunc&& pred);

	// Evaluates all utility functions, picks the function with the
	// highest predicate score and executes it.
	ActionReturn trigger(ActionArgs... action_args, PredArgs... predicate_args);

	// Same as trigger, but evaluates scores in multiple threads.
	// Your predicates must be thread safe.
	// The action is executed on the caller thread.
#if FEA_WITH_TBB
	ActionReturn trigger_mt(
			ActionArgs... action_args, PredArgs... predicate_args);
#endif

private:
	static_assert(std::is_enum_v<FunctionEnum>,
			"fea::utility_ai : The first template parameter must be an enum of "
			"your functions. The enum must end with the member 'count'.");
	static_assert(size_t(FunctionEnum::count) != 0,
			"fea::utility_ai : You must provide a 'count' member in your "
			"function enum, and it must not be equal to 0.");

	float evaluate_score(
			fea::span<const PredicateEnum> preds, PredArgs... pred_args) const;

	// The utility functions.
	std::array<utility_func_t, size_t(FunctionEnum::count)>
			_utility_functions{};

	// The predicates.
	std::array<predicate_func_t, size_t(PredicateEnum::count)> _predicates{};
};
} // namespace fea


// Implmentation
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
ActionReturn utility_ai_function<FunctionEnum, PredicateEnum,
		float(PredArgs...),
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

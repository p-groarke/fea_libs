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

#include "imp/utility_ai.imp.hpp"

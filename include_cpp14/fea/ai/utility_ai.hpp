#pragma once
#include "fea/functional/function.hpp"
#include "fea/meta/static_for.hpp"
#include "fea/utils/platform.hpp"
#include "fea/utils/throw.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <functional>
#include <tbb/parallel_for.h>
#include <vector>

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
predicates. Specify the action signature first, then the predicate signature.
For example,

enum class utility_function {
	eat,
	sleep,
	count // count is mandatory
};

fea::utility_ai<utility_function, void(int&, double), float(int)> ai;

The action signature is : void(int&, double)
The predicate signature is : float(int)

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
template <class, class, class>
struct utility_ai;

template <class FunctionEnum, class ActionReturn, class... ActionArgs,
		class PredReturn, class... PredArgs>
struct utility_ai<FunctionEnum, ActionReturn(ActionArgs...),
		PredReturn(PredArgs...)> {
	using action_t = std::function<ActionReturn(ActionArgs...)>;
	using predicate_t = std::function<PredReturn(ActionArgs...)>;

	utility_ai() = default;
	utility_ai(const utility_ai&) = default;
	utility_ai(utility_ai&&) = default;
	utility_ai& operator=(const utility_ai&) = default;
	utility_ai& operator=(utility_ai&&) = default;

	// Create the utility function F.
	// You must provide a predicate and an action. The signatures must match the
	// created struct.
	// Remember predicates always return float.
	template <FunctionEnum F, class ActionFunc, class... PredFuncs>
	void create_function(
			ActionFunc&& action_func, PredFuncs&&... predicate_funcs) {
		static_assert(sizeof...(PredFuncs) != 0,
				"fea::utility_ai : must provide at least 1 predicate for "
				"utility function");

		std::get<size_t(F)>(_utility_functions) = { F,
			std::vector<predicate_t>{
					std::forward<PredFuncs>(predicate_funcs)... },
			std::forward<ActionFunc>(action_func) };
	}

	// Add multiple predicates to an existing utility function.
	// You must have created the function before calling this.
	template <FunctionEnum F, class... PredFuncs>
	void add_predicates(PredFuncs&&... predicate_functions) {
		utility_function& u_func = std::get<size_t(F)>(_utility_functions);
		if (u_func.id != F) {
			fea::maybe_throw<std::invalid_argument>(__FUNCTION__, __LINE__,
					" : Adding predicate to non-initialized utility function."
					" Remember to call create_function before adding extra "
					"predicates.");
		}

		fold(
				[&](auto&& f) {
					u_func.predicates.push_back(std::forward<decltype(f)>(f));
				},
				std::forward<PredFuncs>(predicate_functions)...);
	}

	// Add a predicate to an existing utility function.
	// You must have created the function before calling this.
	template <FunctionEnum F, class PredFunc>
	void add_predicate(PredFunc&& predicate_function) {
		add_predicates<F>(std::forward<PredFunc>(predicate_function));
	}

	// The first time execute is called, a somewhat heavy verification step is
	// executed. You can call this step manually if you with to control when
	// the validation happens.
	void validate() {
		if (_validated) {
			return;
		}

		for (const utility_function& u_func : _utility_functions) {
			if (u_func.id == FunctionEnum::count) {
				fea::maybe_throw(__FUNCTION__, __LINE__,
						" : One or more utility functions are uninitialized.");
			}
			if (u_func.predicates.empty()) {
				fea::maybe_throw(__FUNCTION__, __LINE__,
						" : One or more utility function has no predicates.");
			}
			if (!u_func.action) {
				fea::maybe_throw(__FUNCTION__, __LINE__,
						" : One or more utility function has no action.");
			}
		}

		_validated = true;
	}

	// Evaluates all utility functions, picks the function with the highest
	// predicate score and executes it.
	ActionReturn trigger(
			ActionArgs... action_args, PredArgs... predicate_args) {

		validate();

		// Don't use std::max_element, it would compute score twice for each
		// utility func.

		size_t winner_idx = (std::numeric_limits<size_t>::max)();
		float max_score = (std::numeric_limits<float>::lowest)();

		for (size_t i = 0; i < size_t(FunctionEnum::count); ++i) {
			// predicate_args are not forwarded, as they are used for multiple
			// predicates.
			float score = evaluate_score(i, predicate_args...);

			// TODO : What happens when predicates return negative, should
			// guarantee skip?
			if (score > max_score) {
				winner_idx = i;
				max_score = score;
			}
		}

		// Something went horribly wrong.
		assert(winner_idx != (std::numeric_limits<size_t>::max)());
		return _utility_functions[winner_idx].action(
				std::forward<ActionArgs>(action_args)...);
	}

	// Same as trigger, but evaluates scores in multiple threads.
	// Your predicates must be thread safe.
	// The action is executed on the caller thread.
	ActionReturn trigger_mt(
			ActionArgs... action_args, PredArgs... predicate_args) {

		validate();

		std::array<float, size_t(FunctionEnum::count)> scores;
		tbb::parallel_for(
				tbb::blocked_range<size_t>{ 0, size_t(FunctionEnum::count) },
				[&, this](const tbb::blocked_range<size_t>& range) {
					for (size_t i = range.begin(); i < range.end(); ++i) {
						scores[i] = evaluate_score(i, predicate_args...);
					}
				});

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
		return _utility_functions[winner_idx].action(
				std::forward<ActionArgs>(action_args)...);
	}

private:
	static_assert(std::is_same<PredReturn, float>::value,
			"fea::utility_ai : Predicate return type must be float.");
	static_assert(std::is_enum<FunctionEnum>::value,
			"fea::utility_ai : The first template parameter must be an enum of "
			"your functions. The enum must end with the member 'count'.");
	static_assert(size_t(FunctionEnum::count) != 0,
			"fea::utility_ai : You must provide a 'count' member in your "
			"function enum, and it must not be equal to 0.");

	struct utility_function {
		// utility_function() = default;
		// utility_function(FunctionEnum id_,
		//		std::vector<predicate_t>&& predicates_, action_t&& action_)
		//		: id(id_)
		//		, predicates(std::move(predicates_))
		//		, action(std::move(action_)) {
		//}

		FunctionEnum id = FunctionEnum::count;
		std::vector<predicate_t> predicates{};
		action_t action{};
	};

	float evaluate_score(size_t i, PredArgs... pred_args) const {
		const utility_function& u_func = _utility_functions[i];

		// These are just sanity checks, everything should have been verified
		// before.
		assert(!u_func.predicates.empty());
		assert(u_func.id == FunctionEnum(i));
		assert(bool(u_func.action));

		// Compute the functions score and average it.
		float ret = 0.f;
		for (const predicate_t& pred : u_func.predicates) {
			ret += pred(pred_args...);
		}
		return ret / float(u_func.predicates.size());
	}

	// The utility functions.
	std::array<utility_function, size_t(FunctionEnum::count)>
			_utility_functions{};

	// Did we run the validation step?
	bool _validated = false;
};


// TODO
//// Alias for captureless utility_ai.
//// Captureless versions of classes do not use std::function, they store
//// callbacks as raw function pointers.
// template <class UtilityFunctionEnum, class ActionSignature,
//		class PredicateSignature>
// using utility_ai_cl = utility_ai<UtilityFunctionEnum, ActionSignature,
//		PredicateSignature, true>;
} // namespace fea

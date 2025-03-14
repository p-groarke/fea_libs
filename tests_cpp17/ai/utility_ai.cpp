﻿#include <chrono>
#include <fea/ai/utility_ai.hpp>
#include <fea/utils/platform.hpp>
#include <gtest/gtest.h>

namespace {
using mdseconds = std::chrono::duration<double>;
using mdhours = std::chrono::duration<double, std::chrono::hours::period>;

// TODO : Needs way more tests. Needs an answer for negative predicates (do they
// invalidate the function or we average as usual).
TEST(utility_ai, basics) {
	bool test_passed = false;

	enum class ufunc {
		pass,
		fail,
		count, // count is mandatory
	};

	enum class upred {
		always_true,
		always_false,
		count, // count is mandatory
	};

	// Create a utility ai with :
	// - utility functions ufunc
	// - predicates upred
	// - action signature void()
	// - predicate signature float()
	fea::utility_ai<ufunc, upred, float(), void()> ai;

	// Add predicates.
	ai.add_predicate<upred::always_true>([]() { return 1.f; });
	ai.add_predicate<upred::always_false>([]() { return 0.f; });

	// Create a function and set it up.
	{
		auto pass_func = ai.make_function();
		EXPECT_EQ(pass_func.size(), 0u);
		EXPECT_TRUE(pass_func.predicates().empty());
		EXPECT_FALSE(pass_func.has_action());

		pass_func.add_predicate(upred::always_true);
		EXPECT_EQ(pass_func.size(), 1u);
		EXPECT_FALSE(pass_func.predicates().empty());
		EXPECT_FALSE(pass_func.has_action());

		// for testing
		pass_func.add_predicate(upred::always_false);
		EXPECT_EQ(pass_func.size(), 2u);
		EXPECT_EQ(pass_func.predicates()[0], upred::always_true);
		EXPECT_EQ(pass_func.predicates()[1], upred::always_false);

#if FEA_DEBUG || FEA_NOTHROW
		EXPECT_DEATH(pass_func.add_predicate(upred::always_true), "");
#else
		EXPECT_THROW(pass_func.add_predicate(upred::always_true),
				std::invalid_argument);
#endif


		pass_func.add_action([&]() { test_passed = true; });
		EXPECT_EQ(pass_func.size(), 2u);
		EXPECT_FALSE(pass_func.predicates().empty());
		EXPECT_TRUE(pass_func.has_action());

		ai.add_function<ufunc::pass>(std::move(pass_func));
	}

	// Should throw or assert, missing 1 utility function.
#if FEA_DEBUG
	EXPECT_DEATH(ai.trigger(), "");
#if FEA_WITH_TBB
	EXPECT_DEATH(ai.trigger_mt(), "");
#endif
#endif

	{
		auto fail_func = ai.make_function();
		fail_func.add_predicate(upred::always_false);
		fail_func.add_action([&]() { test_passed = false; });

		ai.add_function<ufunc::fail>(std::move(fail_func));
	}

	ai.trigger();
	EXPECT_TRUE(test_passed);

#if FEA_WITH_TBB
	ai.trigger_mt();
	EXPECT_TRUE(test_passed);
#endif
}

struct cat {
	enum class util_func {
		sleep,
		idle,
		count,
	};

	enum class pred {
		wants_sleep,
		wants_idle,
		count,
	};

	cat(const char* name_, float sleepy_head_)
			: name(name_)
			, sleepy_head(sleepy_head_) {

		// Initialize utility_ai.
		// Creates the 2 utility functions with 1 predicate and an action each.
		// Uses member functions because pretty.

		ai.add_predicate<pred::wants_sleep>(&cat::wants_sleep);
		ai.add_predicate<pred::wants_idle>(&cat::wants_idle);

		// Sleep
		{
			auto sleep_func = ai.make_function();
			sleep_func.add_predicate(pred::wants_sleep);
			sleep_func.add_action(&cat::do_sleep);
			ai.add_function<util_func::sleep>(std::move(sleep_func));
		}

		// Idle
		{
			auto idle_func = ai.make_function();
			idle_func.add_predicate(pred::wants_idle);
			idle_func.add_action(&cat::do_idle);
			ai.add_function<util_func::idle>(std::move(idle_func));
		}
	}

	void update(mdseconds dt) {
		awake_hours += dt * 60.f * 60.f;

		// print();

		ai.trigger(this, this);
	}

	void update_mt(mdseconds dt) {
		awake_hours += dt * 60.f * 60.f;

		// print();

#if FEA_WITH_TBB
		ai.trigger_mt(this, this);
#else
		ai.trigger(this, this);
#endif
	}

	void print() const {
		printf("\n{ id : %zu, name : %.*s, awake for : %fh, sleeping : %s }\n",
				id, 32, name, awake_hours.count(), sleeping ? "true" : "false");
	}

	// The utility_ai functions can be member functions if you want.
	float wants_sleep() const {
		const mdhours average_awake_hours{ 15.f };
		const mdhours sleepy_delta{ 6.f };

		mdhours target_awake_hours = average_awake_hours;
		if (sleepy_head != 0.f) {
			mdhours cat_delta = sleepy_head * sleepy_delta;
			target_awake_hours -= cat_delta;
		}

		if (awake_hours >= target_awake_hours)
			return 1.f;

		return 0.f;
	}

	void do_sleep() {
		sleeping = true;
	}

	float wants_idle() const {
		return .8f;
	}

	void do_idle() {
	}

	const char* name = "";
	float sleepy_head = 0.f; // personality

	bool sleeping = false; // state
	mdhours awake_hours = mdhours{ 0.f };

	static size_t cat_id_counter;
	size_t id = cat_id_counter++;

	// Utility AI
	fea::utility_ai<util_func, pred, float(cat*), void(cat*)> ai;
};

size_t cat::cat_id_counter = 0;

TEST(utility_ai, meow) {
	// single-thread
	{
		std::vector<cat> cats{
			{ "little_shit", 0.f },
			{ "fluffy", 0.5f },
			{ "fatty", 1.f },
		};

		for (cat& c : cats) {
			c.update(mdseconds{ 9.f });
		}
		EXPECT_FALSE(cats[0].sleeping);
		EXPECT_FALSE(cats[1].sleeping);
		EXPECT_TRUE(cats[2].sleeping);

		for (cat& c : cats) {
			c.update(mdseconds{ 3.f });
		}
		EXPECT_FALSE(cats[0].sleeping);
		EXPECT_TRUE(cats[1].sleeping);
		EXPECT_TRUE(cats[2].sleeping);

		for (cat& c : cats) {
			c.update(mdseconds{ 3.f });
		}
		EXPECT_TRUE(cats[0].sleeping);
		EXPECT_TRUE(cats[1].sleeping);
		EXPECT_TRUE(cats[2].sleeping);
	}

	// multi-thread
	{
		std::vector<cat> cats{
			{ "little_shit", 0.f },
			{ "fluffy", 0.5f },
			{ "fatty", 1.f },
		};

		for (cat& c : cats) {
			c.update_mt(mdseconds{ 9.f });
		}
		EXPECT_FALSE(cats[0].sleeping);
		EXPECT_FALSE(cats[1].sleeping);
		EXPECT_TRUE(cats[2].sleeping);

		for (cat& c : cats) {
			c.update_mt(mdseconds{ 3.f });
		}
		EXPECT_FALSE(cats[0].sleeping);
		EXPECT_TRUE(cats[1].sleeping);
		EXPECT_TRUE(cats[2].sleeping);

		for (cat& c : cats) {
			c.update_mt(mdseconds{ 3.f });
		}
		EXPECT_TRUE(cats[0].sleeping);
		EXPECT_TRUE(cats[1].sleeping);
		EXPECT_TRUE(cats[2].sleeping);
	}
}

} // namespace

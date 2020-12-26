#include <fea/ai/utility_ai.hpp>
#include <fea/time/delta_time.hpp>
#include <fea/time/time.hpp>
#include <gtest/gtest.h>

namespace {
// TODO : Needs way more tests. Needs an answer for negative predicates (do they
// invalidate the function or we average as usual).
TEST(utility_ai, basics) {
	bool test_passed = false;

	enum class ufunc {
		pass,
		fail,
		count, // count is mandatory
	};

	// Create a utility ai with :
	// - utility functions ufunc
	// - action signature void()
	// - predicate signature float()
	fea::utility_ai<ufunc, void(), float()> ai;

	// Create the function.
	ai.create_function<ufunc::pass>(
			[&]() { test_passed = true; }, []() { return 1.f; });

	// Add predicates.
	ai.add_predicate<ufunc::pass>([]() { return 1.f; });
	ai.add_predicates<ufunc::pass>([]() { return 1.f; }, []() { return 1.f; },
			[]() { return 1.f; }, []() { return 1.f; });

	// Should throw or assert, missing 1 utility function.
#if FEA_DEBUG
	EXPECT_DEATH(ai.validate(), "");
	EXPECT_DEATH(ai.trigger(), "");
	EXPECT_DEATH(ai.trigger_mt(), "");
#else
	EXPECT_THROW(ai.validate(), std::runtime_error);
	EXPECT_THROW(ai.trigger(), std::runtime_error);
	EXPECT_THROW(ai.trigger_mt(), std::runtime_error);
#endif

	ai.create_function<ufunc::fail>(
			[&]() { test_passed = false; }, []() { return 0.f; });

	ai.trigger();
	EXPECT_TRUE(test_passed);

	ai.trigger_mt();
	EXPECT_TRUE(test_passed);
}

struct cat {
	cat(const char* name_, float sleepy_head_)
			: name(name_)
			, sleepy_head(sleepy_head_) {

		// Initialize utility_ai.
		// Creates the 2 utility functions with 1 predicate and an action each.
		// Uses member functions because pretty.
		ai.create_function<util_func::sleep>(
				&cat::do_sleep, &cat::wants_sleep, &cat::wants_sleep /*etc*/);
		ai.add_predicate<util_func::sleep>(&cat::wants_sleep);

		ai.create_function<util_func::idle>(&cat::do_idle, &cat::wants_idle);
	}

	void update(fea::dseconds dt) {
		awake_hours += dt * 60.f * 60.f;

		// print();

		ai.trigger(this, this);
	}

	void update_mt(fea::dseconds dt) {
		awake_hours += dt * 60.f * 60.f;

		// print();

		ai.trigger_mt(this, this);
	}

	void print() const {
		printf("\n{ id : %zu, name : %.*s, awake for : %fh, sleeping : %s }\n",
				id, 32, name, awake_hours.count(), sleeping ? "true" : "false");
	}

	// The utility_ai functions can be member functions if you want.
	float wants_sleep() const {
		const fea::dhours average_awake_hours{ 15.f };
		const fea::dhours sleepy_delta{ 6.f };

		fea::dhours target_awake_hours = average_awake_hours;
		if (sleepy_head != 0.f) {
			fea::dhours cat_delta = sleepy_head * sleepy_delta;
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
	fea::dhours awake_hours = fea::dhours{ 0.f };

	static size_t cat_id_counter;
	size_t id = cat_id_counter++;

	// Utility AI
	enum class util_func {
		sleep,
		idle,
		count,
	};

	fea::utility_ai<util_func, void(cat*), float(cat*)> ai;
};

size_t cat::cat_id_counter = 0;

TEST(utility_ai, meow) {
	fea::delta_time dtc;

	// single-thread
	{
		std::vector<cat> cats{
			{ "little_shit", 0.f },
			{ "fluffy", 0.5f },
			{ "fatty", 1.f },
		};

		for (cat& c : cats) {
			c.update(fea::dseconds{ 9.f });
		}
		EXPECT_FALSE(cats[0].sleeping);
		EXPECT_FALSE(cats[1].sleeping);
		EXPECT_TRUE(cats[2].sleeping);

		for (cat& c : cats) {
			c.update(fea::dseconds{ 3.f });
		}
		EXPECT_FALSE(cats[0].sleeping);
		EXPECT_TRUE(cats[1].sleeping);
		EXPECT_TRUE(cats[2].sleeping);

		for (cat& c : cats) {
			c.update(fea::dseconds{ 3.f });
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
			c.update_mt(fea::dseconds{ 9.f });
		}
		EXPECT_FALSE(cats[0].sleeping);
		EXPECT_FALSE(cats[1].sleeping);
		EXPECT_TRUE(cats[2].sleeping);

		for (cat& c : cats) {
			c.update_mt(fea::dseconds{ 3.f });
		}
		EXPECT_FALSE(cats[0].sleeping);
		EXPECT_TRUE(cats[1].sleeping);
		EXPECT_TRUE(cats[2].sleeping);

		for (cat& c : cats) {
			c.update_mt(fea::dseconds{ 3.f });
		}
		EXPECT_TRUE(cats[0].sleeping);
		EXPECT_TRUE(cats[1].sleeping);
		EXPECT_TRUE(cats[2].sleeping);
	}
}

} // namespace

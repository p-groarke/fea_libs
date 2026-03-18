#include <fea/state_machines/fsm.hpp>
#include <fea/utility/platform.hpp>
#include <gtest/gtest.h>


namespace {
#define FAIL_MSG "fsm.cpp : Failed test."

TEST(fsm, example) {
	struct test_data {
		bool walk_enter = false;
		bool walk_update = false;

		size_t num_onenterfrom_calls = 0;
		size_t num_onenter_calls = 0;
		size_t num_onupdate_calls = 0;
		size_t num_onexit_calls = 0;
		size_t num_onexitto_calls = 0;
	};
	test_data mtest_data;

	// Create your enums. They MUST end with 'count'.
	enum class state {
		walk,
		run,
		jump,
		count,
	};
	enum class transition {
		do_walk,
		do_run,
		do_jump,
		count,
	};

	// Used for callbacks
	using machine_t = fea::fsm<transition, state, void(test_data&)>;

	// Create your state machine.
	fea::fsm_builder<transition, state, void(test_data&)> builder;
	auto machine = builder.make_machine();

	// Create your states.
	// Walk
	{
		auto walk_state = machine.make_state();

		// Add allowed transitions.
		walk_state.add_transition<transition::do_run, state::run>();

		// Add state events.
		walk_state.add_event<fea::fsm_event::on_enter>(
				[](test_data& t, machine_t& /*machine*/) {
					t.walk_enter = true;
					++t.num_onenter_calls;
				});
		walk_state.add_event<fea::fsm_event::on_update>(
				[](test_data& t, machine_t& /*machine*/) {
					t.walk_update = true;
					++t.num_onupdate_calls;
				});

		machine.add_state<state::walk>(std::move(walk_state));
	}

	// Run
	{
		auto run_state = builder.make_state();
		run_state.add_transition<transition::do_walk, state::walk>();
		run_state.add_transition<transition::do_jump, state::jump>();
		run_state.add_event<fea::fsm_event::on_enter_from, state::walk>(
				[](test_data& t, machine_t& machine) {
					++t.num_onenterfrom_calls;
					// This is OK.
					machine.trigger<transition::do_walk>(t);
				});
		run_state.add_event<fea::fsm_event::on_update>(
				[](test_data& t, machine_t&) { ++t.num_onupdate_calls; });
		run_state.add_event<fea::fsm_event::on_exit>(
				[](test_data& t, machine_t& machine) {
					++t.num_onexit_calls;

					// This is also OK, though probably not recommended from a
					// "design" standpoint.
					machine.trigger<transition::do_jump>(t);
				});
		machine.add_state<state::run>(std::move(run_state));
	}

	// Jump
	{
		auto jump_state = builder.make_state();
		jump_state.add_transition<transition::do_walk, state::walk>();
		jump_state.add_transition<transition::do_run, state::run>();

		jump_state.add_event<fea::fsm_event::on_enter_from, state::run>(
				[](test_data& t, machine_t&) { ++t.num_onenterfrom_calls; });

		jump_state.add_event<fea::fsm_event::on_exit_to, state::walk>(
				[](test_data& t, machine_t&) { ++t.num_onexitto_calls; });

		machine.add_state<state::jump>(std::move(jump_state));
	}


	// Init and update default state (walk).
	machine.update(mtest_data);
	EXPECT_TRUE(mtest_data.walk_enter);
	EXPECT_TRUE(mtest_data.walk_update);
	EXPECT_EQ(mtest_data.num_onenterfrom_calls, 0u);
	EXPECT_EQ(mtest_data.num_onenter_calls, 1u);
	EXPECT_EQ(mtest_data.num_onupdate_calls, 1u);
	EXPECT_EQ(mtest_data.num_onexit_calls, 0u);
	EXPECT_EQ(mtest_data.num_onexitto_calls, 0u);

	// Currently doesn't handle walk to jump transition.
#if FEA_DEBUG || FEA_NOTHROW
	EXPECT_DEATH(machine.trigger<transition::do_jump>(mtest_data), "");
#else
	EXPECT_THROW(machine.trigger<transition::do_jump>(mtest_data),
			std::invalid_argument);
#endif

	// Go to jump.
	machine.state<state::walk>()
			.add_transition<transition::do_jump, state::jump>();
	EXPECT_NO_THROW(machine.trigger<transition::do_jump>(mtest_data));

	// Nothing should have changed.
	EXPECT_EQ(mtest_data.num_onenterfrom_calls, 0u);
	EXPECT_EQ(mtest_data.num_onenter_calls, 1u);
	EXPECT_EQ(mtest_data.num_onupdate_calls, 1u);
	EXPECT_EQ(mtest_data.num_onexit_calls, 0u);
	EXPECT_EQ(mtest_data.num_onexitto_calls, 0u);

	// Go back to walk.
	machine.trigger<transition::do_walk>(mtest_data);

	// Should get on exit to walk + on enter walk.
	EXPECT_EQ(mtest_data.num_onenterfrom_calls, 0u);
	EXPECT_EQ(mtest_data.num_onenter_calls, 2u);
	EXPECT_EQ(mtest_data.num_onupdate_calls, 1u);
	EXPECT_EQ(mtest_data.num_onexit_calls, 0u);
	EXPECT_EQ(mtest_data.num_onexitto_calls, 1u);

	// Update walk.
	machine.update(mtest_data);
	EXPECT_EQ(mtest_data.num_onenterfrom_calls, 0u);
	EXPECT_EQ(mtest_data.num_onenter_calls, 2u);
	EXPECT_EQ(mtest_data.num_onupdate_calls, 2u);
	EXPECT_EQ(mtest_data.num_onexit_calls, 0u);
	EXPECT_EQ(mtest_data.num_onexitto_calls, 1u);

	// Test retrigger in on_enter and in on_exit.
	machine.trigger<transition::do_run>(mtest_data);
	// run on_enter_from -> run on_exit -> jump on_enter_from
	EXPECT_EQ(mtest_data.num_onenterfrom_calls, 2u);
	EXPECT_EQ(mtest_data.num_onenter_calls, 2u);
	EXPECT_EQ(mtest_data.num_onupdate_calls, 2u);
	EXPECT_EQ(mtest_data.num_onexit_calls, 1u);
	EXPECT_EQ(mtest_data.num_onexitto_calls, 1u);

	// Does nothing, no jump update.
	machine.update(mtest_data);
	machine.update(mtest_data);
	machine.update(mtest_data);
	EXPECT_EQ(mtest_data.num_onenterfrom_calls, 2u);
	EXPECT_EQ(mtest_data.num_onenter_calls, 2u);
	EXPECT_EQ(mtest_data.num_onupdate_calls, 2u);
	EXPECT_EQ(mtest_data.num_onexit_calls, 1u);
	EXPECT_EQ(mtest_data.num_onexitto_calls, 1u);

	// And back to walk.
	machine.trigger<transition::do_walk>(mtest_data);
	EXPECT_EQ(mtest_data.num_onenterfrom_calls, 2u);
	EXPECT_EQ(mtest_data.num_onenter_calls, 3u);
	EXPECT_EQ(mtest_data.num_onupdate_calls, 2u);
	EXPECT_EQ(mtest_data.num_onexit_calls, 1u);
	EXPECT_EQ(mtest_data.num_onexitto_calls, 2u);
}

TEST(fsm, basics) {
	enum class state {
		walk,
		run,
		jump,
		count,
	};

	enum class transition {
		do_walk,
		do_run,
		do_jump,
		ignore_me,
		count,
	};

	size_t on_enters = 0;
	size_t on_updates = 0;
	size_t on_exits = 0;
	bool inpute = false;

	fea::fsm<transition, state, void(bool&)> machine;

	fea::fsm_state<transition, state, void(bool&)> walk_state;
	walk_state.add_event<fea::fsm_event::on_enter>([&](bool& b, auto&) {
		b = true;
		++on_enters;
	});
	walk_state.add_event<fea::fsm_event::on_update>([&](bool& b, auto&) {
		b = true;
		++on_updates;
		machine.trigger<transition::do_run>(b);
	});
	walk_state.add_event<fea::fsm_event::on_exit>([&](bool& b, auto&) {
		b = true;
		++on_exits;
	});
	walk_state.add_transition<transition::do_run, state::run>();
	walk_state.ignore_transition<transition::ignore_me>();
	machine.add_state<state::walk>(std::move(walk_state));

	fea::fsm_state<transition, state, void(bool&)> run_state;
	run_state.add_event<fea::fsm_event::on_enter>([&](bool& b, auto&) {
		b = true;
		++on_enters;
	});
	run_state.add_event<fea::fsm_event::on_update>([&](bool& b, auto&) {
		b = true;
		++on_updates;
		machine.trigger<transition::do_jump>(b);
	});
	run_state.add_event<fea::fsm_event::on_exit>([&](bool& b, auto&) {
		b = true;
		++on_exits;
	});
	run_state.add_transition<transition::do_jump, state::run>();
	run_state.ignore_transition2(transition::ignore_me);
	machine.add_state<state::run>(std::move(run_state));

	fea::fsm_state<transition, state, void(bool&)> jump_state;
	jump_state.add_event<fea::fsm_event::on_enter>([&](bool& b, auto&) {
		b = true;
		++on_enters;
	});
	jump_state.add_event<fea::fsm_event::on_update>([&](bool& b, auto&) {
		b = true;
		++on_updates;
		machine.trigger<transition::do_walk>(b);
	});
	jump_state.add_event<fea::fsm_event::on_exit>([&](bool& b, auto&) {
		b = true;
		++on_exits;
	});
	jump_state.add_transition<transition::do_walk, state::walk>();
	jump_state.ignore_transition<transition::ignore_me>();
	machine.add_state<state::jump>(std::move(jump_state));

	machine.update(inpute);
	machine.update(inpute);
	machine.update(inpute);

	EXPECT_TRUE(inpute);
	EXPECT_EQ(on_enters, 4u);
	EXPECT_EQ(on_updates, 3u);
	EXPECT_EQ(on_exits, 3u);

	// Check we don't throw.
	{
		bool executed = false;
		machine.reset();

		// On the first call, we execute the initialization.
		machine.trigger<transition::ignore_me>(executed);
		EXPECT_TRUE(executed);

		// Try again now.
		executed = false;
		machine.trigger<transition::ignore_me>(executed);
		EXPECT_FALSE(executed);

		machine.trigger<transition::do_run>(executed);
		EXPECT_TRUE(executed);

		executed = false;
		machine.trigger<transition::ignore_me>(executed);
		EXPECT_FALSE(executed);

		machine.trigger<transition::do_jump>(executed);
		EXPECT_TRUE(executed);

		executed = false;
		machine.trigger<transition::ignore_me>(executed);
		EXPECT_FALSE(executed);
	}
}

TEST(fsm, event_triggering) {
	struct test_data {
		size_t num_onenterfrom_calls = 0;
		size_t num_onenter_calls = 0;
		size_t num_onupdate_calls = 0;
		size_t num_onexit_calls = 0;
		size_t num_onexitto_calls = 0;
	};
	test_data mtest_data;

	// Create your enums. They MUST end with 'count'.
	enum class state { walk, run, jump, count };
	enum class transition { do_walk, do_run, do_jump, count };

	// Used for callbacks
	using machine_t = fea::fsm<transition, state, void(test_data&)>;

	// Create your state machine.
	fea::fsm<transition, state, void(test_data&)> machine;

	// Create your states.
	// Walk
	{
		fea::fsm_state<transition, state, void(test_data&)> walk_state;

		// Add allowed transitions.
		walk_state.add_transition<transition::do_run, state::run>();
		walk_state.add_transition<transition::do_jump, state::jump>();

		// Add state events.
		walk_state.add_event<fea::fsm_event::on_enter>(
				[](test_data& t, machine_t& machine) {
					++t.num_onenter_calls;
					machine.trigger<transition::do_run>(t);
				});
		walk_state.add_event<fea::fsm_event::on_enter_from, state::run>(
				[](test_data& t, machine_t&) {
					++t.num_onenterfrom_calls;
					// Should finish here.
					// machine.trigger<transition::do_jump>(t);
				});
		walk_state.add_event<fea::fsm_event::on_exit_to, state::run>(
				[](test_data& t, machine_t& machine) {
					++t.num_onexitto_calls;
					machine.trigger<transition::do_jump>(t);
				});
		machine.add_state<state::walk>(std::move(walk_state));
	}

	// Run
	{
		fea::fsm_state<transition, state, void(test_data&)> run_state;
		run_state.add_transition<transition::do_walk, state::walk>();
		run_state.add_transition<transition::do_jump, state::jump>();
		run_state.add_event<fea::fsm_event::on_enter_from, state::jump>(
				[](test_data& t, machine_t& machine) {
					++t.num_onenterfrom_calls;
					machine.trigger<transition::do_jump>(t);
				});
		run_state.add_event<fea::fsm_event::on_exit_to, state::jump>(
				[](test_data& t, machine_t& machine) {
					++t.num_onexitto_calls;
					machine.trigger<transition::do_walk>(t);
				});
		machine.add_state<state::run>(std::move(run_state));
	}

	// Jump
	{
		fea::fsm_state<transition, state, void(test_data&)> jump_state;
		jump_state.add_transition<transition::do_walk, state::walk>();
		jump_state.add_transition<transition::do_run, state::run>();

		jump_state.add_event<fea::fsm_event::on_enter_from, state::walk>(
				[](test_data& t, machine_t& m) {
					++t.num_onenterfrom_calls;
					m.trigger<transition::do_run>(t);
				});

		jump_state.add_event<fea::fsm_event::on_exit_to, state::run>(
				[](test_data& t, machine_t& m) {
					++t.num_onexitto_calls;
					m.trigger<transition::do_run>(t);
				});

		machine.add_state<state::jump>(std::move(jump_state));
	}

	machine.update(mtest_data);
	EXPECT_EQ(mtest_data.num_onenterfrom_calls, 3u);
	EXPECT_EQ(mtest_data.num_onenter_calls, 1u);
	EXPECT_EQ(mtest_data.num_onupdate_calls, 0u);
	EXPECT_EQ(mtest_data.num_onexit_calls, 0u);
	EXPECT_EQ(mtest_data.num_onexitto_calls, 3u);
}

// The same tests, but using the non-templated overloads we need to baby sit
// MSVC.
TEST(fsm, basics_non_tmp) {
	enum class state {
		walk,
		run,
		jump,
		s1,
		s2,
		s3,
		s4,
		s5,
		s6,
		s7,
		s8,
		count,
	};

	enum class transition {
		do_walk,
		do_run,
		do_jump,
		t1,
		t2,
		t3,
		t4,
		t5,
		t6,
		t7,
		t8,
		count,
	};

	size_t on_enters = 0;
	size_t on_updates = 0;
	size_t on_exits = 0;
	bool inpute = false;

	fea::fsm<transition, state, void(bool&)> machine;

	fea::fsm_state<transition, state, void(bool&)> walk_state;
	walk_state.add_event2(fea::fsm_event::on_enter, [&](bool& b, auto&) {
		b = true;
		++on_enters;
	});
	walk_state.add_event2(fea::fsm_event::on_update, [&](bool& b, auto&) {
		b = true;
		++on_updates;
		machine.trigger<transition::do_run>(b);
	});
	walk_state.add_event2(fea::fsm_event::on_exit, [&](bool& b, auto&) {
		b = true;
		++on_exits;
	});
	walk_state.add_transition2(transition::do_run, state::run);
	machine.add_state<state::walk>(std::move(walk_state));

	fea::fsm_state<transition, state, void(bool&)> run_state;
	run_state.add_event2(fea::fsm_event::on_enter, [&](bool& b, auto&) {
		b = true;
		++on_enters;
	});
	run_state.add_event2(fea::fsm_event::on_update, [&](bool& b, auto&) {
		b = true;
		++on_updates;
		machine.trigger<transition::do_jump>(b);
	});
	run_state.add_event2(fea::fsm_event::on_exit, [&](bool& b, auto&) {
		b = true;
		++on_exits;
	});
	run_state.add_transition2(transition::do_jump, state::run);
	machine.add_state<state::run>(std::move(run_state));

	fea::fsm_state<transition, state, void(bool&)> jump_state;
	jump_state.add_event2(fea::fsm_event::on_enter, [&](bool& b, auto&) {
		b = true;
		++on_enters;
	});
	jump_state.add_event2(fea::fsm_event::on_update, [&](bool& b, auto&) {
		b = true;
		++on_updates;
		machine.trigger<transition::do_walk>(b);
	});
	jump_state.add_event2(fea::fsm_event::on_exit, [&](bool& b, auto&) {
		b = true;
		++on_exits;
	});
	jump_state.add_transition2(transition::do_walk, state::walk);
	machine.add_state<state::jump>(std::move(jump_state));

	machine.update(inpute);
	machine.update(inpute);
	machine.update(inpute);

	EXPECT_TRUE(inpute);
	EXPECT_EQ(on_enters, 4u);
	EXPECT_EQ(on_updates, 3u);
	EXPECT_EQ(on_exits, 3u);
}

TEST(fsm, event_triggering_non_tmp) {
	struct test_data {
		size_t num_onenterfrom_calls = 0;
		size_t num_onenter_calls = 0;
		size_t num_onupdate_calls = 0;
		size_t num_onexit_calls = 0;
		size_t num_onexitto_calls = 0;
	};
	test_data mtest_data;

	// Create your enums. They MUST end with 'count'.
	enum class state {
		walk,
		run,
		jump,
		count,
	};
	enum class transition {
		do_walk,
		do_run,
		do_jump,
		count,
	};

	// Used for callbacks
	using machine_t = fea::fsm<transition, state, void(test_data&)>;

	// Create your state machine.
	fea::fsm<transition, state, void(test_data&)> machine;

	// Create your states.
	// Walk
	{
		fea::fsm_state<transition, state, void(test_data&)> walk_state;

		// Add allowed transitions.
		walk_state.add_transition2(transition::do_run, state::run);
		walk_state.add_transition2(transition::do_jump, state::jump);

		// Add state events.
		walk_state.add_event2(
				fea::fsm_event::on_enter, [](test_data& t, machine_t& machine) {
					++t.num_onenter_calls;
					machine.trigger<transition::do_run>(t);
				});
		walk_state.add_event2(fea::fsm_event::on_enter_from, state::run,
				[](test_data& t, machine_t&) {
					++t.num_onenterfrom_calls;
					// Should finish here.
					// machine.trigger<transition::do_jump>(t);
				});
		walk_state.add_event2(fea::fsm_event::on_exit_to, state::run,
				[](test_data& t, machine_t& machine) {
					++t.num_onexitto_calls;
					machine.trigger<transition::do_jump>(t);
				});
		machine.add_state<state::walk>(std::move(walk_state));
	}

	// Run
	{
		fea::fsm_state<transition, state, void(test_data&)> run_state;
		run_state.add_transition2(transition::do_walk, state::walk);
		run_state.add_transition2(transition::do_jump, state::jump);
		run_state.add_event2(fea::fsm_event::on_enter_from, state::jump,
				[](test_data& t, machine_t& machine) {
					++t.num_onenterfrom_calls;
					machine.trigger<transition::do_jump>(t);
				});
		run_state.add_event2(fea::fsm_event::on_exit_to, state::jump,
				[](test_data& t, machine_t& machine) {
					++t.num_onexitto_calls;
					machine.trigger<transition::do_walk>(t);
				});
		machine.add_state<state::run>(std::move(run_state));
	}

	// Jump
	{
		fea::fsm_state<transition, state, void(test_data&)> jump_state;
		jump_state.add_transition2(transition::do_walk, state::walk);
		jump_state.add_transition2(transition::do_run, state::run);

		jump_state.add_event2(fea::fsm_event::on_enter_from, state::walk,
				[](test_data& t, machine_t& m) {
					++t.num_onenterfrom_calls;
					m.trigger<transition::do_run>(t);
				});

		jump_state.add_event2(fea::fsm_event::on_exit_to, state::run,
				[](test_data& t, machine_t& m) {
					++t.num_onexitto_calls;
					m.trigger<transition::do_run>(t);
				});

		machine.add_state<state::jump>(std::move(jump_state));
	}

	machine.update(mtest_data);
	EXPECT_EQ(mtest_data.num_onenterfrom_calls, 3u);
	EXPECT_EQ(mtest_data.num_onenter_calls, 1u);
	EXPECT_EQ(mtest_data.num_onupdate_calls, 0u);
	EXPECT_EQ(mtest_data.num_onexit_calls, 0u);
	EXPECT_EQ(mtest_data.num_onexitto_calls, 3u);
}

TEST(fsm, options) {
	enum class state {
		s1,
		s2,
		s3,
		s4,
		s5,
		s6,
		s7,
		s8,
		count,
	};

	enum class transition {
		t1,
		t2,
		t3,
		t4,
		t5,
		t6,
		t7,
		t8,
		count,
	};

	// no fsm arg
	{
		fea::fsm<transition, state, void(bool&), fea::fsm_option::no_fsm_arg> m;

		{
			fea::fsm_state<transition, state, void(bool&),
					fea::fsm_option::no_fsm_arg>
					s1;

			static_assert(decltype(s1)::no_fsm_arg, FAIL_MSG);
			static_assert(!decltype(s1)::void_fsm_arg, FAIL_MSG);

			// Add allowed transitions.
			s1.add_transition<transition::t1, state::s1>();
			s1.add_transition2(transition::t2, state::s2);

			// Add state events.
			s1.add_event<fea::fsm_event::on_enter>([](bool& b) { b = true; });
			s1.add_event<fea::fsm_event::on_enter_from, state::s2>(
					[](bool& b) { b = true; });
			s1.add_event<fea::fsm_event::on_update>([](bool& b) { b = true; });
			s1.add_event<fea::fsm_event::on_exit>([](bool& b) { b = true; });
			s1.add_event<fea::fsm_event::on_exit_to, state::s2>(
					[](bool& b) { b = true; });
			m.add_state<state::s1>(std::move(s1));
		}

		{
			fea::fsm_state<transition, state, void(bool&),
					fea::fsm_option::no_fsm_arg>
					s2;

			static_assert(decltype(s2)::no_fsm_arg, FAIL_MSG);
			static_assert(!decltype(s2)::void_fsm_arg, FAIL_MSG);

			// Add allowed transitions.
			s2.add_transition<transition::t1, state::s1>();
			s2.add_transition2(transition::t2, state::s2);

			// Add state events.
			s2.add_event<fea::fsm_event::on_enter>([](bool& b) { b = true; });
			s2.add_event<fea::fsm_event::on_enter_from, state::s1>(
					[](bool& b) { b = true; });
			s2.add_event<fea::fsm_event::on_update>([](bool& b) { b = true; });
			s2.add_event<fea::fsm_event::on_exit>([](bool& b) { b = true; });
			s2.add_event<fea::fsm_event::on_exit_to, state::s1>(
					[](bool& b) { b = true; });
			m.add_state<state::s2>(std::move(s2));
		}

		bool evaled = false;
		m.trigger<transition::t1>(evaled);
		EXPECT_TRUE(evaled);

		evaled = false;
		m.update(evaled);
		EXPECT_TRUE(evaled);

		evaled = false;
		m.trigger<transition::t2>(evaled);
		EXPECT_TRUE(evaled);

		evaled = false;
		m.update(evaled);
		EXPECT_TRUE(evaled);

		evaled = false;
		m.trigger<transition::t1>(evaled);
		EXPECT_TRUE(evaled);
	}

	// void* arg
	{
		fea::fsm<transition, state, void(bool&), fea::fsm_option::void_fsm_arg>
				m;

		{

			fea::fsm_state<transition, state, void(bool&),
					fea::fsm_option::void_fsm_arg>
					s1;

			static_assert(decltype(s1)::void_fsm_arg, FAIL_MSG);
			static_assert(!decltype(s1)::no_fsm_arg, FAIL_MSG);

			// Add allowed transitions.
			s1.add_transition<transition::t1, state::s1>();
			s1.add_transition2(transition::t2, state::s2);

			// Add state events.
			s1.add_event<fea::fsm_event::on_enter>([&](bool& b, void* ptr) {
				EXPECT_EQ(&m, ptr);
				b = true;
			});
			s1.add_event<fea::fsm_event::on_enter_from, state::s2>(
					[&](bool& b, void* ptr) {
						EXPECT_EQ(&m, ptr);
						b = true;
					});
			s1.add_event<fea::fsm_event::on_update>([&](bool& b, void* ptr) {
				EXPECT_EQ(&m, ptr);
				b = true;
			});
			s1.add_event<fea::fsm_event::on_exit>([&](bool& b, void* ptr) {
				EXPECT_EQ(&m, ptr);
				b = true;
			});
			s1.add_event<fea::fsm_event::on_exit_to, state::s2>(
					[&](bool& b, void* ptr) {
						EXPECT_EQ(&m, ptr);
						b = true;
					});
			m.add_state<state::s1>(std::move(s1));
		}

		{
			fea::fsm_state<transition, state, void(bool&),
					fea::fsm_option::void_fsm_arg>
					s2;

			static_assert(decltype(s2)::void_fsm_arg, FAIL_MSG);
			static_assert(!decltype(s2)::no_fsm_arg, FAIL_MSG);

			// Add allowed transitions.
			s2.add_transition<transition::t1, state::s1>();
			s2.add_transition2(transition::t2, state::s2);

			// Add state events.
			s2.add_event<fea::fsm_event::on_enter>([&](bool& b, void* ptr) {
				EXPECT_EQ(&m, ptr);
				b = true;
			});
			s2.add_event<fea::fsm_event::on_enter_from, state::s1>(
					[&](bool& b, void* ptr) {
						EXPECT_EQ(&m, ptr);
						b = true;
					});
			s2.add_event<fea::fsm_event::on_update>([&](bool& b, void* ptr) {
				EXPECT_EQ(&m, ptr);
				b = true;
			});
			s2.add_event<fea::fsm_event::on_exit>([&](bool& b, void* ptr) {
				EXPECT_EQ(&m, ptr);
				b = true;
			});
			s2.add_event<fea::fsm_event::on_exit_to, state::s1>(
					[&](bool& b, void* ptr) {
						EXPECT_EQ(&m, ptr);
						b = true;
					});
			m.add_state<state::s2>(std::move(s2));
		}

		bool evaled = false;
		m.trigger<transition::t1>(evaled);
		EXPECT_TRUE(evaled);

		evaled = false;
		m.update(evaled);
		EXPECT_TRUE(evaled);

		evaled = false;
		m.trigger<transition::t2>(evaled);
		EXPECT_TRUE(evaled);

		evaled = false;
		m.update(evaled);
		EXPECT_TRUE(evaled);

		evaled = false;
		m.trigger<transition::t1>(evaled);
		EXPECT_TRUE(evaled);
	}
}
} // namespace

#include <algorithm>
#include <cstdint>
#include <fea/graphs/lazy_graph.hpp>
#include <fea/maps/unsigned_map.hpp>
#include <fea/utils/platform.hpp>
#include <gtest/gtest.h>
#include <mutex>
#include <vector>

#if FEA_WITH_TBB

extern bool contains(const std::vector<unsigned>& vec, unsigned i);
extern bool contains(
		fea::span<const fea::parent_status<unsigned>> vec, unsigned i);

extern size_t get_index(fea::span<const unsigned> vec, unsigned i);
extern size_t num_dirty(fea::span<const fea::parent_status<unsigned>> vec);

namespace {

size_t num_dirty(fea::span<const fea::parent_status<unsigned>> vec) {
	return std::count_if(vec.begin(), vec.end(),
			[](const fea::parent_status<unsigned>& p) { return p.was_dirty; });
}


/**
 * Makes the following graphs :
 *
 *     0        12   13      17
 *    / \       |    / \     | \
 *   1   2      |   14  15   18 \
 *  /\   /\     |    \  /    / \ |
 * 3 4   5 6    |     16    19  20
 *  \_\ /_/     |
 *     |       /
 *     7      /
 *    /|\    /
 *   8 9 10 /
 *   \_|_/ /
 *     \  /
 *      11
 */
constexpr unsigned num_nodes = 21;

template <class Graph>
void reset_graph(Graph& graph) {
	// first graph
	graph.add_dependency(1, 0);
	graph.add_dependency(2, 0);
	graph.add_dependency(3, 1);
	graph.add_dependency(4, 1);
	graph.add_dependency(5, 2);
	graph.add_dependency(6, 2);
	graph.add_dependency(7, 3);
	graph.add_dependency(7, 4);
	graph.add_dependency(7, 5);
	graph.add_dependency(7, 6);
	graph.add_dependency(8, 7);
	graph.add_dependency(9, 7);
	graph.add_dependency(10, 7);
	graph.add_dependency(11, 8);
	graph.add_dependency(11, 9);
	graph.add_dependency(11, 10);
	graph.add_dependency(11, 12);

	// second
	graph.add_dependency(14, 13);
	graph.add_dependency(15, 13);
	graph.add_dependency(16, 14);
	graph.add_dependency(16, 15);

	// third
	graph.add_dependency(18, 17);
	graph.add_dependency(19, 18);
	graph.add_dependency(20, 18);
	graph.add_dependency(20, 17);
}

void test_parents(
		unsigned id, fea::span<const fea::parent_status<unsigned>> parents) {
	if (id == 1) {
		EXPECT_EQ(parents.size(), 1u);
		EXPECT_TRUE(contains(parents, 0u));
	} else if (id == 2) {
		EXPECT_EQ(parents.size(), 1u);
		EXPECT_TRUE(contains(parents, 0u));
	} else if (id == 3) {
		EXPECT_EQ(parents.size(), 1u);
		EXPECT_TRUE(contains(parents, 1u));
	} else if (id == 4) {
		EXPECT_EQ(parents.size(), 1u);
		EXPECT_TRUE(contains(parents, 1u));
	} else if (id == 5) {
		EXPECT_EQ(parents.size(), 1u);
		EXPECT_TRUE(contains(parents, 2u));
	} else if (id == 6) {
		EXPECT_EQ(parents.size(), 1u);
		EXPECT_TRUE(contains(parents, 2u));
	} else if (id == 7) {
		EXPECT_EQ(parents.size(), 4u);
		EXPECT_TRUE(contains(parents, 3u));
		EXPECT_TRUE(contains(parents, 4u));
		EXPECT_TRUE(contains(parents, 5u));
		EXPECT_TRUE(contains(parents, 6u));
	} else if (id == 8) {
		EXPECT_EQ(parents.size(), 1u);
		EXPECT_TRUE(contains(parents, 7u));
	} else if (id == 9) {
		EXPECT_EQ(parents.size(), 1u);
		EXPECT_TRUE(contains(parents, 7u));
	} else if (id == 10) {
		EXPECT_EQ(parents.size(), 1u);
		EXPECT_TRUE(contains(parents, 7u));
	} else if (id == 11) {
		EXPECT_EQ(parents.size(), 4u);
		EXPECT_TRUE(contains(parents, 8u));
		EXPECT_TRUE(contains(parents, 9u));
		EXPECT_TRUE(contains(parents, 10u));
		EXPECT_TRUE(contains(parents, 12u));

	} else if (id == 14) {
		EXPECT_EQ(parents.size(), 1u);
		EXPECT_TRUE(contains(parents, 13u));
	} else if (id == 15) {
		EXPECT_EQ(parents.size(), 1u);
		EXPECT_TRUE(contains(parents, 13u));
	} else if (id == 16) {
		EXPECT_EQ(parents.size(), 2u);
		EXPECT_TRUE(contains(parents, 14u));
		EXPECT_TRUE(contains(parents, 15u));

	} else if (id == 18) {
		EXPECT_EQ(parents.size(), 1u);
		EXPECT_TRUE(contains(parents, 17u));
	} else if (id == 19) {
		EXPECT_EQ(parents.size(), 1u);
		EXPECT_TRUE(contains(parents, 18u));
	} else if (id == 20) {
		EXPECT_EQ(parents.size(), 2u);
		EXPECT_TRUE(contains(parents, 18u));
		EXPECT_TRUE(contains(parents, 17u));
	}
}

TEST(fea_lazy_graph, threading) {
	// These tests are run with an unsigned_map.
	fea::lazy_graph<unsigned, char, uint8_t, fea::unsigned_map> graph;
	using my_callback_data = fea::callback_data<unsigned>;

	reset_graph(graph);

	// Dirty everything.
	graph.make_dirty(0);
	graph.make_dirty_if_not(0);
	graph.make_dirty(12);
	graph.make_dirty(13);
	graph.make_dirty_if_not(17);

	// Roots are never dirty.
	EXPECT_FALSE(graph.is_dirty(0));
	EXPECT_FALSE(graph.is_dirty(12));
	EXPECT_FALSE(graph.is_dirty(13));
	EXPECT_FALSE(graph.is_dirty(17));

	EXPECT_TRUE(graph.is_dirty(1));
	EXPECT_TRUE(graph.is_dirty(2));
	EXPECT_TRUE(graph.is_dirty(3));
	EXPECT_TRUE(graph.is_dirty(4));
	EXPECT_TRUE(graph.is_dirty(5));
	EXPECT_TRUE(graph.is_dirty(6));
	EXPECT_TRUE(graph.is_dirty(7));
	EXPECT_TRUE(graph.is_dirty(8));
	EXPECT_TRUE(graph.is_dirty(9));
	EXPECT_TRUE(graph.is_dirty(10));
	EXPECT_TRUE(graph.is_dirty(11));
	EXPECT_TRUE(graph.is_dirty(14));
	EXPECT_TRUE(graph.is_dirty(15));
	EXPECT_TRUE(graph.is_dirty(16));
	EXPECT_TRUE(graph.is_dirty(18));
	EXPECT_TRUE(graph.is_dirty(19));
	EXPECT_TRUE(graph.is_dirty(20));

	std::vector<unsigned> to_clean{ 11, 16, 20 };
	auto ind_data = graph.are_eval_graphs_independent(to_clean);
	EXPECT_EQ(ind_data.independent_graphs.size(), 3u);
	EXPECT_EQ(ind_data.dependent_graphs.size(), 0u);

	to_clean = { 9, 14, 20, 19 };
	ind_data = graph.are_eval_graphs_independent(to_clean);
	EXPECT_EQ(ind_data.independent_graphs.size(), 2u);
	EXPECT_EQ(ind_data.dependent_graphs.size(), 2u);

	to_clean = { 11, 12 };
	ind_data = graph.are_eval_graphs_independent(to_clean);
	EXPECT_EQ(ind_data.independent_graphs.size(), 0u);
	EXPECT_EQ(ind_data.dependent_graphs.size(), 2u);

	// Trigger clean on the whole graph.
	to_clean = { 11, 16, 19, 20 };
	ind_data = graph.are_eval_graphs_independent(to_clean);
	EXPECT_EQ(ind_data.independent_graphs.size(), 2u);
	EXPECT_EQ(ind_data.dependent_graphs.size(), 2u);

	std::vector<unsigned> cleaned_ids;
	std::mutex m;
	graph.clean_mt(to_clean, fea::make_callback([&](const my_callback_data& d) {
		std::lock_guard<std::mutex> g{ m };
		test_parents(d.id, d.parents);
		EXPECT_EQ(num_dirty(d.parents), d.parents.size());
		cleaned_ids.push_back(d.id);
	}));

	for (unsigned i = 0; i < num_nodes; ++i) {
		// Roots
		if (graph.is_root(i)) {
			EXPECT_FALSE(contains(cleaned_ids, i));
		} else {
			EXPECT_TRUE(contains(cleaned_ids, i));
		}
	}

	// Test loose ordering.
	// No root
	// EXPECT_GT(get_index(cleaned_ids, 1), get_index(cleaned_ids, 0));
	// EXPECT_GT(get_index(cleaned_ids, 2), get_index(cleaned_ids, 0));
	EXPECT_GT(get_index(cleaned_ids, 3), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 4), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 5), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 6), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 7), get_index(cleaned_ids, 3));
	EXPECT_GT(get_index(cleaned_ids, 7), get_index(cleaned_ids, 4));
	EXPECT_GT(get_index(cleaned_ids, 7), get_index(cleaned_ids, 5));
	EXPECT_GT(get_index(cleaned_ids, 7), get_index(cleaned_ids, 6));
	EXPECT_GT(get_index(cleaned_ids, 8), get_index(cleaned_ids, 7));
	EXPECT_GT(get_index(cleaned_ids, 9), get_index(cleaned_ids, 7));
	EXPECT_GT(get_index(cleaned_ids, 10), get_index(cleaned_ids, 7));
	EXPECT_GT(get_index(cleaned_ids, 11), get_index(cleaned_ids, 8));
	EXPECT_GT(get_index(cleaned_ids, 11), get_index(cleaned_ids, 9));
	EXPECT_GT(get_index(cleaned_ids, 11), get_index(cleaned_ids, 10));

	// No root
	// EXPECT_GT(get_index(cleaned_ids, 14), get_index(cleaned_ids, 13));
	// EXPECT_GT(get_index(cleaned_ids, 15), get_index(cleaned_ids, 13));
	EXPECT_GT(get_index(cleaned_ids, 16), get_index(cleaned_ids, 14));
	EXPECT_GT(get_index(cleaned_ids, 16), get_index(cleaned_ids, 15));

	// No root
	// EXPECT_GT(get_index(cleaned_ids, 18), get_index(cleaned_ids, 17));
	// EXPECT_GT(get_index(cleaned_ids, 20), get_index(cleaned_ids, 17));
	EXPECT_GT(get_index(cleaned_ids, 19), get_index(cleaned_ids, 18));
	EXPECT_GT(get_index(cleaned_ids, 20), get_index(cleaned_ids, 18));


	// Test dirty_parents
	{
		graph.make_dirty(3);
		to_clean = { 7 };
		graph.clean_mt(
				to_clean, fea::make_callback([&](const my_callback_data& d) {
					std::lock_guard<std::mutex> g{ m };
					test_parents(d.id, d.parents);
					if (d.id == 7) {
						EXPECT_EQ(num_dirty(d.parents), 1u);
						EXPECT_TRUE(contains(d.parents, 3u));
					} else {
						EXPECT_EQ(num_dirty(d.parents), d.parents.size());
					}
					cleaned_ids.push_back(d.id);
				}));
	}
}

TEST(fea_lazy_graph, threading_max_parents) {
	// These tests are run with an unsigned_map.
	fea::lazy_graph<unsigned, char, uint8_t, fea::unsigned_map, 32> graph;
	using my_callback_data = fea::callback_data<unsigned>;

	reset_graph(graph);

	// Dirty everything.
	graph.make_dirty(0);
	graph.make_dirty(12);
	graph.make_dirty(13);
	graph.make_dirty(17);

	// Roots are never dirty.
	EXPECT_FALSE(graph.is_dirty(0));
	EXPECT_FALSE(graph.is_dirty(12));
	EXPECT_FALSE(graph.is_dirty(13));
	EXPECT_FALSE(graph.is_dirty(17));

	EXPECT_TRUE(graph.is_dirty(1));
	EXPECT_TRUE(graph.is_dirty(2));
	EXPECT_TRUE(graph.is_dirty(3));
	EXPECT_TRUE(graph.is_dirty(4));
	EXPECT_TRUE(graph.is_dirty(5));
	EXPECT_TRUE(graph.is_dirty(6));
	EXPECT_TRUE(graph.is_dirty(7));
	EXPECT_TRUE(graph.is_dirty(8));
	EXPECT_TRUE(graph.is_dirty(9));
	EXPECT_TRUE(graph.is_dirty(10));
	EXPECT_TRUE(graph.is_dirty(11));
	EXPECT_TRUE(graph.is_dirty(14));
	EXPECT_TRUE(graph.is_dirty(15));
	EXPECT_TRUE(graph.is_dirty(16));
	EXPECT_TRUE(graph.is_dirty(18));
	EXPECT_TRUE(graph.is_dirty(19));
	EXPECT_TRUE(graph.is_dirty(20));

	std::vector<unsigned> to_clean{ 11, 16, 20 };
	auto ind_data = graph.are_eval_graphs_independent(to_clean);
	EXPECT_EQ(ind_data.independent_graphs.size(), 3u);
	EXPECT_EQ(ind_data.dependent_graphs.size(), 0u);

	to_clean = { 9, 14, 20, 19 };
	ind_data = graph.are_eval_graphs_independent(to_clean);
	EXPECT_EQ(ind_data.independent_graphs.size(), 2u);
	EXPECT_EQ(ind_data.dependent_graphs.size(), 2u);

	to_clean = { 11, 12 };
	ind_data = graph.are_eval_graphs_independent(to_clean);
	EXPECT_EQ(ind_data.independent_graphs.size(), 0u);
	EXPECT_EQ(ind_data.dependent_graphs.size(), 2u);

	// Trigger clean on the whole graph.
	to_clean = { 11, 16, 19, 20 };
	ind_data = graph.are_eval_graphs_independent(to_clean);
	EXPECT_EQ(ind_data.independent_graphs.size(), 2u);
	EXPECT_EQ(ind_data.dependent_graphs.size(), 2u);

	std::vector<unsigned> cleaned_ids;
	std::mutex m;
	graph.clean_mt(to_clean, fea::make_callback([&](const my_callback_data& d) {
		std::lock_guard<std::mutex> g{ m };
		test_parents(d.id, d.parents);
		EXPECT_EQ(num_dirty(d.parents), d.parents.size());
		cleaned_ids.push_back(d.id);
	}));

	for (unsigned i = 0; i < num_nodes; ++i) {
		// Roots
		if (graph.is_root(i)) {
			EXPECT_FALSE(contains(cleaned_ids, i));
		} else {
			EXPECT_TRUE(contains(cleaned_ids, i));
		}
	}

	// Test loose ordering.
	// No root
	// EXPECT_GT(get_index(cleaned_ids, 1), get_index(cleaned_ids, 0));
	// EXPECT_GT(get_index(cleaned_ids, 2), get_index(cleaned_ids, 0));
	EXPECT_GT(get_index(cleaned_ids, 3), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 4), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 5), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 6), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 7), get_index(cleaned_ids, 3));
	EXPECT_GT(get_index(cleaned_ids, 7), get_index(cleaned_ids, 4));
	EXPECT_GT(get_index(cleaned_ids, 7), get_index(cleaned_ids, 5));
	EXPECT_GT(get_index(cleaned_ids, 7), get_index(cleaned_ids, 6));
	EXPECT_GT(get_index(cleaned_ids, 8), get_index(cleaned_ids, 7));
	EXPECT_GT(get_index(cleaned_ids, 9), get_index(cleaned_ids, 7));
	EXPECT_GT(get_index(cleaned_ids, 10), get_index(cleaned_ids, 7));
	EXPECT_GT(get_index(cleaned_ids, 11), get_index(cleaned_ids, 8));
	EXPECT_GT(get_index(cleaned_ids, 11), get_index(cleaned_ids, 9));
	EXPECT_GT(get_index(cleaned_ids, 11), get_index(cleaned_ids, 10));

	// No root
	// EXPECT_GT(get_index(cleaned_ids, 14), get_index(cleaned_ids, 13));
	// EXPECT_GT(get_index(cleaned_ids, 15), get_index(cleaned_ids, 13));
	EXPECT_GT(get_index(cleaned_ids, 16), get_index(cleaned_ids, 14));
	EXPECT_GT(get_index(cleaned_ids, 16), get_index(cleaned_ids, 15));

	// No root
	// EXPECT_GT(get_index(cleaned_ids, 18), get_index(cleaned_ids, 17));
	// EXPECT_GT(get_index(cleaned_ids, 20), get_index(cleaned_ids, 17));
	EXPECT_GT(get_index(cleaned_ids, 19), get_index(cleaned_ids, 18));
	EXPECT_GT(get_index(cleaned_ids, 20), get_index(cleaned_ids, 18));


	// Test dirty_parents
	{
		graph.make_dirty(3);
		to_clean = { 7 };
		graph.clean_mt(
				to_clean, fea::make_callback([&](const my_callback_data& d) {
					std::lock_guard<std::mutex> g{ m };
					test_parents(d.id, d.parents);
					if (d.id == 7) {
						EXPECT_EQ(num_dirty(d.parents), 1u);
						EXPECT_TRUE(contains(d.parents, 3u));
					} else {
						EXPECT_EQ(num_dirty(d.parents), d.parents.size());
					}
					cleaned_ids.push_back(d.id);
				}));
	}
}
} // namespace
#endif

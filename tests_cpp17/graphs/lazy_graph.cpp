﻿#include <algorithm>
#include <cstdint>
#include <fea/containers/id_slotmap.hpp>
#include <fea/graphs/lazy_graph.hpp>
#include <fea/utils/platform.hpp>
#include <fea/utils/unused.hpp>
#include <gtest/gtest.h>
#include <mutex>
#include <vector>

bool contains(const std::vector<unsigned>& vec, unsigned i) {
	return std::find(vec.begin(), vec.end(), i) != vec.end();
}
bool contains(fea::span<const fea::parent_status<unsigned>> vec, unsigned i) {
	return std::find_if(vec.begin(), vec.end(),
				   [&](const fea::parent_status<unsigned>& s) {
					   return s.parent_id == i;
				   })
		!= vec.end();
}

size_t get_index(fea::span<const unsigned> vec, unsigned i) {
	auto it = std::find(vec.begin(), vec.end(), i);
	return std::distance(vec.begin(), it);
}
// size_t get_index(
//		const std::vector<std::pair<unsigned, bool>>& vec, unsigned i) {
//	auto it = std::find_if(vec.begin(), vec.end(),
//			[i](const auto& p) { return p.first == i; });
//	return std::distance(vec.begin(), it);
//}

size_t num_dirty(fea::span<const fea::parent_status<unsigned>> vec) {
	return std::count_if(vec.begin(), vec.end(),
			[](const fea::parent_status<unsigned>& p) { return p.was_dirty; });
}

namespace {
TEST(lazy_graph, example) {
	// You choose your id. It can be an unsigned integer, or a pointer.
	// The id must be usable as a key in an unordered_map, unique and stable.
	using my_id_t = unsigned;

	// Create a graph with your id type.
	// You can also pass in a node data type, the version unsigned int to use
	// and an unordered_map container of your choice.
	fea::lazy_graph<my_id_t> graph;
	using my_callback_data = typename decltype(graph)::callback_data_t;

	/**
	 * Graph construction / relationship functions
	 */

	// Add a node with no parents and no children.
	graph.add_node(0);
	graph.add_node(1);

	// Get information about the graph.
	graph.is_root(0); // true
	graph.has_children(0); // false
	graph.has_parents(0); // false

	// All function signatures consistently accept (child_id, parent_id) when
	// using both.
	graph.has_child(1, 0); // false
	graph.has_parent(1, 0); // false

	// Check if adding a child would cause a loop or other issues.
	graph.is_invalid_child(1, 0); // false

	// Remove a node.
	graph.remove_node(0);

	// Remove a node and its children, only if its children would be orphaned.
	graph.remove_subgraph(1);

	// Add dependencies between child -> parent.
	// If the nodes don't exist, they will be created.
	// Returns whether the dependency was created or not.
	graph.add_dependency(1, 0); // true
	graph.add_dependency(2, 1); // true

	// Remove a dependency.
	graph.remove_dependency(1, 0);

	// Does the graph contain a node?
	graph.contains(0); // true

	// Is the graph empty?
	graph.empty(); // false

	// Get a nodes children. Readonly.
	/*const std::vector<my_id_t>& children = */ graph.children(0);

	// Get a nodes parents. Readonly.
	// It is a map so we don't uselessly create a vector. You can ignore the
	// 'second' variable in the pair.
	/*const std::unordered_map<my_id_t, uint16_t>& parents = */ graph.parents(
			1);

	// Remove all nodes and reset the graph.
	// graph.clear();


	/**
	 * Graph dirtyness functions.
	 */

	// Mark a node as dirtied.
	graph.make_dirty(1);

	// Check if a node is dirty.
	graph.is_dirty(2); // true

	// Clean a node.
	// Calls your callback from top to bottom in the dirtied graph.
	// Provides id to clean.
	graph.clean(2, fea::make_callback([](const my_callback_data&) {
		// do fancy things.
	}));

	// Clean multiple nodes at a time.
	std::vector<my_id_t> my_nodes_to_clean{ 0, 1, 2 };
	graph.clean(
			my_nodes_to_clean, fea::make_callback([](const my_callback_data&) {
				// do fancy things.
			}));
}

TEST(lazy_graph, advanced_example) {
	// These examples show multi-threading apis and advanced calls with graph
	// information.
	// These are optional.

	// IMPORTANT : When using threaded calls, you must respect 2 rules to keep
	// your evaluation thread safe.
	// RULE 1 : Only read from your parents.
	// RULE 2 : Only write to yourself.

	using my_id_t = unsigned;
	// You can add some extra data that all nodes will have.
	// You can customize the unsigned integer we use to track versions.
	// You can also customize the unordered_map container, as long as your
	// container fulfills std::unordered_map APIs.

	// fea::lazy_graph<id_type, node_data, versioning_type, unordered_map>
	fea::lazy_graph<my_id_t, char, uint8_t, fea::id_slotmap> graph;
	using my_callback_data = typename decltype(graph)::callback_data_t;

	graph.add_dependency(1, 0);
	graph.add_dependency(2, 1);

	// Clean Multithreaded.
	// This cleans a node (walks its eval graph top to bottom) but schedules
	// your function in a threaded breadth manner.
	// It will lock between stages that aren't independent.
#if FEA_WITH_TBB
	graph.clean_mt(2, fea::make_callback([](const my_callback_data&) {}));
#else
	graph.clean(2, fea::make_callback([](const my_callback_data&) {}));
#endif

	// Clean multiple nodes in a multithreaded eval.
	// This is the BEST call to make for maximum threading.
	// It will launch independent eval graphs in seperate threads, plus thread
	// the graphs' breadths as it can.
	std::vector<my_id_t> my_nodes_to_clean{ 0, 1, 2 };
#if FEA_WITH_TBB
	graph.clean_mt(my_nodes_to_clean,
			fea::make_callback([](const my_callback_data&) {}));
#else
	graph.clean(my_nodes_to_clean,
			fea::make_callback([](const my_callback_data&) {}));
#endif


	// Even more advanced calls.
	// These will allow you to schedule evaluation yourself if you so desire.

	// If you need information on the evaluation graph of a node, you can call
	// evaluation_graph on it.
	// The result is a left to right breadth first ordered vector.
	// Note this isn't const, as the eval graph computation is also lazy.
	// Lazy is good.
	fea::span<const my_id_t> my_node_eval_graph = graph.evaluation_graph(2);
	fea::unused(my_node_eval_graph);

	// Graph independance.
	// You can easily clean a node using optimal threading with clean_mt.
	// However, if you need to evaluate multiple nodes at a time, you would also
	// want to thread at a higher level if possible. This call will split your
	// node ids into those that can be launched in threads, and those that
	// can't.
	my_nodes_to_clean = { 0, 1, 2 };

	auto ind_data = graph.are_eval_graphs_independent(my_nodes_to_clean);

	// 'ind_data.independent_graphs' can be cleaned in parallel.
#if FEA_WITH_TBB
	tbb::task_group g;
	for (my_id_t id : ind_data.independent_graphs) {
		g.run([&, id]() {
			graph.clean_mt(
					id, fea::make_callback([](const my_callback_data&) {}));
		});
	}

	// 'ind_data.dependent_graphs' cannot be cleaned in parallel.
	// But they can still execute in parallel with the other nodes.
	g.run_and_wait([&]() {
		for (my_id_t id : ind_data.independent_graphs) {
			// Clean one at a time. But you can still call clean_mt at least.
			graph.clean_mt(
					id, fea::make_callback([](const my_callback_data&) {}));
		}
	});
#else
	for (my_id_t id : ind_data.independent_graphs) {
		graph.clean(id, fea::make_callback([](const my_callback_data&) {}));
	}
	for (my_id_t id : ind_data.independent_graphs) {
		graph.clean(id, fea::make_callback([](const my_callback_data&) {}));
	}
#endif
}

/**
 * Makes the following graph :
 *
 *   0
 *   |\
 *   1 \
 *  / \|
 * |   2
 * |   |
 * |   3
 * |  /|\
 * | 4 5 6
 * \  \|/
 *  \ /
 *   7
 */
template <class Graph>
void reset_graph(Graph& graph) {
	graph.add_dependency(1, 0);
	graph.add_dependency(2, 0);
	graph.add_dependency(2, 1);
	graph.add_dependency(3, 2);
	graph.add_dependency(4, 3);
	graph.add_dependency(5, 3);
	graph.add_dependency(6, 3);
	graph.add_dependency(7, 4);
	graph.add_dependency(7, 5);
	graph.add_dependency(7, 6);
	graph.add_dependency(7, 1);
}

void test_parents(
		unsigned id, fea::span<const fea::parent_status<unsigned>> parents) {
	if (id == 1) {
		EXPECT_EQ(parents.size(), 1u);
		EXPECT_TRUE(contains(parents, 0u));
	} else if (id == 2) {
		EXPECT_EQ(parents.size(), 2u);
		EXPECT_TRUE(contains(parents, 0u));
		EXPECT_TRUE(contains(parents, 1u));
	} else if (id == 3) {
		EXPECT_EQ(parents.size(), 1u);
		EXPECT_TRUE(contains(parents, 2u));
	} else if (id == 4) {
		EXPECT_EQ(parents.size(), 1u);
		EXPECT_TRUE(contains(parents, 3u));
	} else if (id == 5) {
		EXPECT_EQ(parents.size(), 1u);
		EXPECT_TRUE(contains(parents, 3u));
	} else if (id == 6) {
		EXPECT_EQ(parents.size(), 1u);
		EXPECT_TRUE(contains(parents, 3u));
	} else if (id == 7) {
		EXPECT_EQ(parents.size(), 4u);
		EXPECT_TRUE(contains(parents, 1u));
		EXPECT_TRUE(contains(parents, 4u));
		EXPECT_TRUE(contains(parents, 5u));
		EXPECT_TRUE(contains(parents, 6u));
	}
}

TEST(lazy_graph, basics) {
	// We use a smaller version tracking int so we can test the edge case when
	// version has to wrap around.
	// fea::lazy_graph<unsigned, uint8_t> graph;

	// Can use with different map, as long as it fulfills std::unordered_map
	// apis.
	fea::lazy_graph<unsigned, char, uint8_t> graph;

	// Basics
	{
		EXPECT_TRUE(graph.empty());
		EXPECT_FALSE(graph.contains(0));

		graph.add_node(0);
		EXPECT_FALSE(graph.empty());
		EXPECT_TRUE(graph.contains(0));
		EXPECT_TRUE(graph.is_root(0));
		EXPECT_FALSE(graph.has_children(0));

		graph.remove_node(0);
		EXPECT_TRUE(graph.empty());
		EXPECT_FALSE(graph.contains(0));

		graph.add_node(0);
		EXPECT_FALSE(graph.empty());
		EXPECT_TRUE(graph.contains(0));
		EXPECT_TRUE(graph.is_root(0));
		EXPECT_FALSE(graph.has_children(0));

		EXPECT_TRUE(graph.add_dependency(1, 0));
		EXPECT_TRUE(graph.contains(1));
		EXPECT_TRUE(graph.is_root(0));
		EXPECT_FALSE(graph.is_root(1));
		EXPECT_TRUE(graph.has_children(0));
		EXPECT_FALSE(graph.has_children(1));

		EXPECT_TRUE(graph.is_invalid_child(1, 0));
		EXPECT_TRUE(graph.is_invalid_child(0, 1));

		EXPECT_FALSE(graph.is_invalid_child(2, 0));
		EXPECT_FALSE(graph.is_invalid_child(2, 1));

		EXPECT_TRUE(graph.add_dependency(2, 0));
		EXPECT_TRUE(graph.is_invalid_child(2, 0));
		EXPECT_TRUE(graph.is_invalid_child(0, 2));
	}


	// Test loop prevention.
	{
		EXPECT_TRUE(graph.add_dependency(2, 1));
		EXPECT_TRUE(graph.add_dependency(3, 2));
		EXPECT_TRUE(graph.add_dependency(4, 3));

		EXPECT_TRUE(graph.is_invalid_child(0, 2));
		EXPECT_TRUE(graph.is_invalid_child(0, 3));
		EXPECT_TRUE(graph.is_invalid_child(0, 4));
		EXPECT_TRUE(graph.is_invalid_child(1, 2));
		EXPECT_TRUE(graph.is_invalid_child(1, 3));
		EXPECT_TRUE(graph.is_invalid_child(1, 4));
		EXPECT_TRUE(graph.is_invalid_child(2, 2));
		EXPECT_TRUE(graph.is_invalid_child(2, 3));
		EXPECT_TRUE(graph.is_invalid_child(2, 4));
		EXPECT_TRUE(graph.is_invalid_child(3, 3));
		EXPECT_TRUE(graph.is_invalid_child(3, 4));
		EXPECT_TRUE(graph.is_invalid_child(4, 4));

		EXPECT_FALSE(graph.add_dependency(0, 2));
		EXPECT_FALSE(graph.add_dependency(0, 3));
		EXPECT_FALSE(graph.add_dependency(0, 4));
		EXPECT_FALSE(graph.add_dependency(1, 2));
		EXPECT_FALSE(graph.add_dependency(1, 3));
		EXPECT_FALSE(graph.add_dependency(1, 4));
		EXPECT_FALSE(graph.add_dependency(2, 2));
		EXPECT_FALSE(graph.add_dependency(2, 3));
		EXPECT_FALSE(graph.add_dependency(2, 4));
		EXPECT_FALSE(graph.add_dependency(3, 3));
		EXPECT_FALSE(graph.add_dependency(3, 4));
		EXPECT_FALSE(graph.add_dependency(4, 4));

		graph.remove_node(1);
		EXPECT_FALSE(graph.contains(1));
		EXPECT_TRUE(graph.has_children(0)); // has 2
		EXPECT_FALSE(graph.has_child(1, 0));
		EXPECT_TRUE(graph.has_parents(2)); // has 0
		EXPECT_FALSE(graph.has_parent(2, 1));
	}
}

TEST(lazy_graph, removing) {
	fea::lazy_graph<unsigned, char, uint8_t, std::unordered_map, 4, 4> graph;
	reset_graph(graph);

	// Test removing
	graph.remove_node(2);
	graph.remove_node(3);
	EXPECT_FALSE(graph.contains(2));
	EXPECT_FALSE(graph.contains(3));
	EXPECT_TRUE(graph.has_children(0)); // has 1
	EXPECT_FALSE(graph.has_child(2, 0));
	EXPECT_FALSE(graph.has_child(3, 0)); // never had it
	EXPECT_FALSE(graph.has_child(2, 1));
	EXPECT_FALSE(graph.has_child(3, 1));
	EXPECT_FALSE(graph.has_parents(4)); // has 0
	EXPECT_FALSE(graph.has_parent(4, 3));


	// Remove subgraph
	reset_graph(graph);

	// Should remove 1 only, since 2 also depends on 0, 3 depends on 2
	graph.remove_subgraph(1);
	EXPECT_TRUE(graph.contains(0));
	EXPECT_TRUE(graph.contains(2));
	EXPECT_FALSE(graph.contains(1));
	EXPECT_TRUE(graph.contains(3));
	EXPECT_TRUE(graph.contains(4));
	EXPECT_TRUE(graph.contains(5));
	EXPECT_TRUE(graph.contains(6));
	EXPECT_TRUE(graph.contains(7));

	reset_graph(graph);

	graph.remove_dependency(3, 2);
	EXPECT_TRUE(graph.add_dependency(3, 1));

	// Should remove 1, 3, 4, 5, 6, 7
	graph.remove_subgraph(1);
	EXPECT_TRUE(graph.contains(0));
	EXPECT_TRUE(graph.contains(2));
	EXPECT_FALSE(graph.contains(1));
	EXPECT_FALSE(graph.contains(3));
	EXPECT_FALSE(graph.contains(4));
	EXPECT_FALSE(graph.contains(5));
	EXPECT_FALSE(graph.contains(6));
	EXPECT_FALSE(graph.contains(7));
}

TEST(lazy_graph, advanced) {
	fea::lazy_graph<unsigned, char, uint8_t> graph;
	reset_graph(graph);

	// Evaluation graphs
	fea::span<const unsigned> subgraph = graph.evaluation_graph(7);
	size_t idx_0 = get_index(subgraph, 0u);
	size_t idx_1 = get_index(subgraph, 1u);
	size_t idx_2 = get_index(subgraph, 2u);
	size_t idx_3 = get_index(subgraph, 3u);
	size_t idx_4 = get_index(subgraph, 4u);
	size_t idx_5 = get_index(subgraph, 5u);
	size_t idx_6 = get_index(subgraph, 6u);
	size_t idx_7 = get_index(subgraph, 7u);
	EXPECT_EQ(idx_0, 0u); // should be at front
	EXPECT_EQ(idx_1, 1u); // should be second
	EXPECT_EQ(idx_2, 2u); // should be third
	EXPECT_EQ(idx_3, 3u); // should be fourth
	EXPECT_EQ(idx_7, 7u); // should be at end

	EXPECT_GT(idx_4, idx_3);
	EXPECT_GT(idx_5, idx_3);
	EXPECT_GT(idx_6, idx_3);

	EXPECT_GT(idx_7, idx_0);
	EXPECT_GT(idx_7, idx_1);
	EXPECT_GT(idx_7, idx_2);
	EXPECT_GT(idx_7, idx_3);
	EXPECT_GT(idx_7, idx_4);
	EXPECT_GT(idx_7, idx_5);
	EXPECT_GT(idx_7, idx_6);

	std::vector<unsigned> to_eval{ 0, 1, 2, 3, 4, 5, 6, 7 };
	auto ind_data = graph.are_eval_graphs_independent(to_eval);
	EXPECT_EQ(ind_data.independent_graphs.size(), 0u);
	EXPECT_EQ(ind_data.dependent_graphs.size(), 8u);
}

TEST(lazy_graph, dirtyness) {
	fea::lazy_graph<unsigned, char, uint8_t> graph;
	using my_callback_data = fea::callback_data<unsigned>;
	reset_graph(graph);

	// Dirtyness
	// Everything starts dirty, except root.
	EXPECT_FALSE(graph.is_dirty(0));
	EXPECT_TRUE(graph.is_dirty(1));
	EXPECT_TRUE(graph.is_dirty(2));
	EXPECT_TRUE(graph.is_dirty(3));
	EXPECT_TRUE(graph.is_dirty(4));
	EXPECT_TRUE(graph.is_dirty(5));
	EXPECT_TRUE(graph.is_dirty(6));
	EXPECT_TRUE(graph.is_dirty(7));

	// Clean it.
	std::vector<unsigned> evaled_ids;
	std::vector<unsigned> cleaned_ids;
	graph.evaluate_dirty(4, fea::make_callback([&](const my_callback_data& d) {
		evaled_ids.push_back(d.id);
	}));

	graph.clean(4, fea::make_callback([&](const my_callback_data& d) {
		test_parents(d.id, d.parents);
		EXPECT_EQ(num_dirty(d.parents), d.parents.size());

		cleaned_ids.push_back(d.id);
	}));

	EXPECT_EQ(evaled_ids, cleaned_ids);

	// Test the order of evaluation.
	// Root is not cleaned, since it doesn't depend on anything.
	EXPECT_GT(get_index(cleaned_ids, 2), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 3), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 3), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 4), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 4), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 4), get_index(cleaned_ids, 3));

	// Tests that the lambda was only called once per node (aka no duplicate
	// messages).
	std::sort(cleaned_ids.begin(), cleaned_ids.end());
	auto it = std::unique(cleaned_ids.begin(), cleaned_ids.end());
	EXPECT_EQ(it, cleaned_ids.end());

	// Graph should be clean now.
	EXPECT_FALSE(graph.is_dirty(0));
	EXPECT_FALSE(graph.is_dirty(1));
	EXPECT_FALSE(graph.is_dirty(2));
	EXPECT_FALSE(graph.is_dirty(3));
	EXPECT_FALSE(graph.is_dirty(4));
	EXPECT_TRUE(graph.is_dirty(5));
	EXPECT_TRUE(graph.is_dirty(6));
	EXPECT_TRUE(graph.is_dirty(7));

	// We should have visited everything except the root.
	EXPECT_FALSE(contains(cleaned_ids, 0u));
	EXPECT_TRUE(contains(cleaned_ids, 1u));
	EXPECT_TRUE(contains(cleaned_ids, 2u));
	EXPECT_TRUE(contains(cleaned_ids, 3u));
	EXPECT_TRUE(contains(cleaned_ids, 4u));
	EXPECT_FALSE(contains(cleaned_ids, 5u));
	EXPECT_FALSE(contains(cleaned_ids, 6u));
	EXPECT_FALSE(contains(cleaned_ids, 7u));

	// Invalidate whole graph again.
	uint8_t ver_before = graph.version(0);
	graph.make_dirty(0);
	EXPECT_FALSE(graph.is_dirty(0));
	EXPECT_TRUE(graph.is_dirty(1));
	EXPECT_TRUE(graph.is_dirty(2));
	EXPECT_TRUE(graph.is_dirty(3));
	EXPECT_TRUE(graph.is_dirty(4));
	EXPECT_TRUE(graph.is_dirty(5));
	EXPECT_TRUE(graph.is_dirty(6));
	EXPECT_TRUE(graph.is_dirty(7));

	// Clean it again.
	cleaned_ids.clear();
	graph.clean(7, fea::make_callback([&](const my_callback_data& d) {
		test_parents(d.id, d.parents);
		EXPECT_EQ(num_dirty(d.parents), d.parents.size());
		cleaned_ids.push_back(d.id);
	}));

	EXPECT_GT(graph.version(0), ver_before);

	// Test the order of evaluation.
	EXPECT_GT(get_index(cleaned_ids, 2), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 3), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 3), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 4), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 4), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 4), get_index(cleaned_ids, 3));
	EXPECT_GT(get_index(cleaned_ids, 5), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 5), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 5), get_index(cleaned_ids, 3));
	EXPECT_GT(get_index(cleaned_ids, 6), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 6), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 6), get_index(cleaned_ids, 3));
	EXPECT_GT(get_index(cleaned_ids, 7), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 7), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 7), get_index(cleaned_ids, 3));
	EXPECT_GT(get_index(cleaned_ids, 7), get_index(cleaned_ids, 4));
	EXPECT_GT(get_index(cleaned_ids, 7), get_index(cleaned_ids, 5));
	EXPECT_GT(get_index(cleaned_ids, 7), get_index(cleaned_ids, 6));

	// Test no duplicate messages
	std::sort(cleaned_ids.begin(), cleaned_ids.end());
	it = std::unique(cleaned_ids.begin(), cleaned_ids.end());
	EXPECT_EQ(it, cleaned_ids.end());

	// Graph should be clean now.
	EXPECT_FALSE(graph.is_dirty(0));
	EXPECT_FALSE(graph.is_dirty(1));
	EXPECT_FALSE(graph.is_dirty(2));
	EXPECT_FALSE(graph.is_dirty(3));
	EXPECT_FALSE(graph.is_dirty(4));
	EXPECT_FALSE(graph.is_dirty(5));
	EXPECT_FALSE(graph.is_dirty(6));
	EXPECT_FALSE(graph.is_dirty(7));

	// We should have visited everything except the root.
	EXPECT_FALSE(contains(cleaned_ids, 0u));
	EXPECT_TRUE(contains(cleaned_ids, 1u));
	EXPECT_TRUE(contains(cleaned_ids, 2u));
	EXPECT_TRUE(contains(cleaned_ids, 3u));
	EXPECT_TRUE(contains(cleaned_ids, 4u));
	EXPECT_TRUE(contains(cleaned_ids, 5u));
	EXPECT_TRUE(contains(cleaned_ids, 6u));
	EXPECT_TRUE(contains(cleaned_ids, 7u));

	// Test wrapping around version values.
	for (size_t i = 0; i < 256; ++i) {
		graph.make_dirty(0); // Should invalidate whole graph.

		EXPECT_FALSE(graph.is_dirty(0));
		EXPECT_TRUE(graph.is_dirty(1));
		EXPECT_TRUE(graph.is_dirty(2));
		EXPECT_TRUE(graph.is_dirty(3));
		EXPECT_TRUE(graph.is_dirty(4));
		EXPECT_TRUE(graph.is_dirty(5));
		EXPECT_TRUE(graph.is_dirty(6));
		EXPECT_TRUE(graph.is_dirty(7));
	}

	evaled_ids.clear();
	cleaned_ids.clear();

	graph.evaluate_dirty(2, fea::make_callback([&](const my_callback_data& d) {
		evaled_ids.push_back(d.id);
	}));
	graph.clean(2, fea::make_callback([&](const my_callback_data& d) {
		test_parents(d.id, d.parents);
		EXPECT_EQ(num_dirty(d.parents), d.parents.size());
		cleaned_ids.push_back(d.id);
	}));

	EXPECT_EQ(evaled_ids, cleaned_ids);

	// Test the order of evaluation.
	EXPECT_GT(get_index(cleaned_ids, 2), get_index(cleaned_ids, 1));

	// Test no duplicate messages.
	std::sort(cleaned_ids.begin(), cleaned_ids.end());
	it = std::unique(cleaned_ids.begin(), cleaned_ids.end());
	EXPECT_EQ(it, cleaned_ids.end());

	EXPECT_FALSE(graph.is_dirty(0));
	EXPECT_FALSE(graph.is_dirty(1));
	EXPECT_FALSE(graph.is_dirty(2));
	EXPECT_TRUE(graph.is_dirty(3));
	EXPECT_TRUE(graph.is_dirty(4));
	EXPECT_TRUE(graph.is_dirty(5));
	EXPECT_TRUE(graph.is_dirty(6));
	EXPECT_TRUE(graph.is_dirty(7));

	EXPECT_FALSE(contains(cleaned_ids, 0u));
	EXPECT_TRUE(contains(cleaned_ids, 1u));
	EXPECT_TRUE(contains(cleaned_ids, 2u));
	EXPECT_FALSE(contains(cleaned_ids, 3u));
	EXPECT_FALSE(contains(cleaned_ids, 4u));
	EXPECT_FALSE(contains(cleaned_ids, 5u));
	EXPECT_FALSE(contains(cleaned_ids, 6u));
	EXPECT_FALSE(contains(cleaned_ids, 7u));

	graph.make_dirty(0);
	cleaned_ids.clear();
	graph.clean(5, fea::make_callback([&](const my_callback_data& d) {
		test_parents(d.id, d.parents);
		EXPECT_EQ(num_dirty(d.parents), d.parents.size());
		cleaned_ids.push_back(d.id);
	}));

	// Test the order of evaluation.
	EXPECT_GT(get_index(cleaned_ids, 2), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 3), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 3), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 5), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 5), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 5), get_index(cleaned_ids, 3));

	// Test no duplicate messages
	std::sort(cleaned_ids.begin(), cleaned_ids.end());
	it = std::unique(cleaned_ids.begin(), cleaned_ids.end());
	EXPECT_EQ(it, cleaned_ids.end());

	EXPECT_FALSE(graph.is_dirty(0));
	EXPECT_FALSE(graph.is_dirty(1));
	EXPECT_FALSE(graph.is_dirty(2));
	EXPECT_FALSE(graph.is_dirty(3));
	EXPECT_TRUE(graph.is_dirty(4));
	EXPECT_FALSE(graph.is_dirty(5));
	EXPECT_TRUE(graph.is_dirty(6));
	EXPECT_TRUE(graph.is_dirty(7));

	EXPECT_FALSE(contains(cleaned_ids, 0u));
	EXPECT_TRUE(contains(cleaned_ids, 1u));
	EXPECT_TRUE(contains(cleaned_ids, 2u));
	EXPECT_TRUE(contains(cleaned_ids, 3u));
	EXPECT_FALSE(contains(cleaned_ids, 4u));
	EXPECT_TRUE(contains(cleaned_ids, 5u));
	EXPECT_FALSE(contains(cleaned_ids, 6u));
	EXPECT_FALSE(contains(cleaned_ids, 7u));

	// Clean everything
	graph.clean(7, fea::make_callback([](const my_callback_data&) {}));
	EXPECT_FALSE(graph.is_dirty(0));
	EXPECT_FALSE(graph.is_dirty(1));
	EXPECT_FALSE(graph.is_dirty(2));
	EXPECT_FALSE(graph.is_dirty(3));
	EXPECT_FALSE(graph.is_dirty(4));
	EXPECT_FALSE(graph.is_dirty(5));
	EXPECT_FALSE(graph.is_dirty(6));
	EXPECT_FALSE(graph.is_dirty(7));

	graph.make_dirty(3);
	EXPECT_FALSE(graph.is_dirty(0));
	EXPECT_FALSE(graph.is_dirty(1));
	EXPECT_FALSE(graph.is_dirty(2));
	// Writing to a node doesn't mean it needs to recompute using its
	// parents.
	EXPECT_FALSE(graph.is_dirty(3));
	EXPECT_TRUE(graph.is_dirty(4));
	EXPECT_TRUE(graph.is_dirty(5));
	EXPECT_TRUE(graph.is_dirty(6));
	EXPECT_TRUE(graph.is_dirty(7));

	evaled_ids.clear();
	cleaned_ids.clear();

	graph.evaluate_dirty(6, fea::make_callback([&](const my_callback_data& d) {
		evaled_ids.push_back(d.id);
	}));
	graph.clean(6, fea::make_callback([&](const my_callback_data& d) {
		test_parents(d.id, d.parents);
		EXPECT_EQ(num_dirty(d.parents), d.parents.size());
		cleaned_ids.push_back(d.id);
	}));

	EXPECT_EQ(evaled_ids, cleaned_ids);

	// Only should clean 6.
	EXPECT_EQ(cleaned_ids.size(), 1u);

	// Test no duplicate messages
	std::sort(cleaned_ids.begin(), cleaned_ids.end());
	it = std::unique(cleaned_ids.begin(), cleaned_ids.end());
	EXPECT_EQ(it, cleaned_ids.end());

	EXPECT_FALSE(graph.is_dirty(0));
	EXPECT_FALSE(graph.is_dirty(1));
	EXPECT_FALSE(graph.is_dirty(2));
	EXPECT_FALSE(graph.is_dirty(3));
	EXPECT_TRUE(graph.is_dirty(4));
	EXPECT_TRUE(graph.is_dirty(5));
	EXPECT_FALSE(graph.is_dirty(6));
	EXPECT_TRUE(graph.is_dirty(7));

	EXPECT_FALSE(contains(cleaned_ids, 0u));
	EXPECT_FALSE(contains(cleaned_ids, 1u));
	EXPECT_FALSE(contains(cleaned_ids, 2u));
	EXPECT_FALSE(contains(cleaned_ids, 3u));
	EXPECT_FALSE(contains(cleaned_ids, 4u));
	EXPECT_FALSE(contains(cleaned_ids, 5u));
	EXPECT_TRUE(contains(cleaned_ids, 6u));
	EXPECT_FALSE(contains(cleaned_ids, 7u));
}

TEST(lazy_graph, dirtyness_mt) {
	fea::lazy_graph<unsigned, char, uint8_t> graph;
	using my_callback_data = fea::callback_data<unsigned>;
	reset_graph(graph);

	// Dirtyness multi-threaded
	graph.make_dirty(0);

	// Everything starts dirty, except root.
	EXPECT_FALSE(graph.is_dirty(0));
	EXPECT_TRUE(graph.is_dirty(1));
	EXPECT_TRUE(graph.is_dirty(2));
	EXPECT_TRUE(graph.is_dirty(3));
	EXPECT_TRUE(graph.is_dirty(4));
	EXPECT_TRUE(graph.is_dirty(5));
	EXPECT_TRUE(graph.is_dirty(6));
	EXPECT_TRUE(graph.is_dirty(7));

	// Clean it.
	std::vector<unsigned> evaled_ids;
	std::vector<unsigned> cleaned_ids;
	std::mutex m;

#if FEA_WITH_TBB
	graph.evaluate_dirty_mt(
			4, fea::make_callback([&](const my_callback_data& d) {
				std::lock_guard<std::mutex> g{ m };
				evaled_ids.push_back(d.id);
			}));
	graph.clean_mt(4, fea::make_callback([&](const my_callback_data& d) {
		std::lock_guard<std::mutex> g{ m };
		test_parents(d.id, d.parents);
		EXPECT_EQ(num_dirty(d.parents), d.parents.size());
		cleaned_ids.push_back(d.id);
	}));
#else
	graph.evaluate_dirty(4, fea::make_callback([&](const my_callback_data& d) {
		std::lock_guard<std::mutex> g{ m };
		evaled_ids.push_back(d.id);
	}));
	graph.clean(4, fea::make_callback([&](const my_callback_data& d) {
		std::lock_guard<std::mutex> g{ m };
		test_parents(d.id, d.parents);
		EXPECT_EQ(num_dirty(d.parents), d.parents.size());
		cleaned_ids.push_back(d.id);
	}));
#endif


	// Test the order of evaluation.
	// Root is not cleaned, since it doesn't depend on anything.
	EXPECT_GT(get_index(cleaned_ids, 2), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 3), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 3), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 4), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 4), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 4), get_index(cleaned_ids, 3));

	// Tests that the lambda was only called once per node (aka no duplicate
	// messages).
	std::sort(evaled_ids.begin(), evaled_ids.end());
	std::sort(cleaned_ids.begin(), cleaned_ids.end());
	EXPECT_EQ(evaled_ids, cleaned_ids);

	auto it = std::unique(cleaned_ids.begin(), cleaned_ids.end());
	EXPECT_EQ(it, cleaned_ids.end());

	// Graph should be clean now.
	EXPECT_FALSE(graph.is_dirty(0));
	EXPECT_FALSE(graph.is_dirty(1));
	EXPECT_FALSE(graph.is_dirty(2));
	EXPECT_FALSE(graph.is_dirty(3));
	EXPECT_FALSE(graph.is_dirty(4));
	EXPECT_TRUE(graph.is_dirty(5));
	EXPECT_TRUE(graph.is_dirty(6));
	EXPECT_TRUE(graph.is_dirty(7));

	// We should have visited everything except the root.
	EXPECT_FALSE(contains(cleaned_ids, 0u));
	EXPECT_TRUE(contains(cleaned_ids, 1u));
	EXPECT_TRUE(contains(cleaned_ids, 2u));
	EXPECT_TRUE(contains(cleaned_ids, 3u));
	EXPECT_TRUE(contains(cleaned_ids, 4u));
	EXPECT_FALSE(contains(cleaned_ids, 5u));
	EXPECT_FALSE(contains(cleaned_ids, 6u));
	EXPECT_FALSE(contains(cleaned_ids, 7u));

	// Invalidate whole graph again.
	graph.make_dirty(0);
	EXPECT_FALSE(graph.is_dirty(0));
	EXPECT_TRUE(graph.is_dirty(1));
	EXPECT_TRUE(graph.is_dirty(2));
	EXPECT_TRUE(graph.is_dirty(3));
	EXPECT_TRUE(graph.is_dirty(4));
	EXPECT_TRUE(graph.is_dirty(5));
	EXPECT_TRUE(graph.is_dirty(6));
	EXPECT_TRUE(graph.is_dirty(7));

	// Clean it again.
	cleaned_ids.clear();
#if FEA_WITH_TBB
	graph.clean_mt(7, fea::make_callback([&](const my_callback_data& d) {
		std::lock_guard<std::mutex> g{ m };
		test_parents(d.id, d.parents);
		EXPECT_EQ(num_dirty(d.parents), d.parents.size());
		cleaned_ids.push_back(d.id);
	}));
#else
	graph.clean(7, fea::make_callback([&](const my_callback_data& d) {
		std::lock_guard<std::mutex> g{ m };
		test_parents(d.id, d.parents);
		EXPECT_EQ(num_dirty(d.parents), d.parents.size());
		cleaned_ids.push_back(d.id);
	}));
#endif

	// Test the order of evaluation.
	EXPECT_GT(get_index(cleaned_ids, 2), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 3), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 3), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 4), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 4), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 4), get_index(cleaned_ids, 3));
	EXPECT_GT(get_index(cleaned_ids, 5), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 5), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 5), get_index(cleaned_ids, 3));
	EXPECT_GT(get_index(cleaned_ids, 6), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 6), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 6), get_index(cleaned_ids, 3));
	EXPECT_GT(get_index(cleaned_ids, 7), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 7), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 7), get_index(cleaned_ids, 3));
	EXPECT_GT(get_index(cleaned_ids, 7), get_index(cleaned_ids, 4));
	EXPECT_GT(get_index(cleaned_ids, 7), get_index(cleaned_ids, 5));
	EXPECT_GT(get_index(cleaned_ids, 7), get_index(cleaned_ids, 6));

	// Test no duplicate messages
	std::sort(cleaned_ids.begin(), cleaned_ids.end());
	it = std::unique(cleaned_ids.begin(), cleaned_ids.end());
	EXPECT_EQ(it, cleaned_ids.end());

	// Graph should be clean now.
	EXPECT_FALSE(graph.is_dirty(0));
	EXPECT_FALSE(graph.is_dirty(1));
	EXPECT_FALSE(graph.is_dirty(2));
	EXPECT_FALSE(graph.is_dirty(3));
	EXPECT_FALSE(graph.is_dirty(4));
	EXPECT_FALSE(graph.is_dirty(5));
	EXPECT_FALSE(graph.is_dirty(6));
	EXPECT_FALSE(graph.is_dirty(7));

	// We should have visited everything except the root.
	EXPECT_FALSE(contains(cleaned_ids, 0u));
	EXPECT_TRUE(contains(cleaned_ids, 1u));
	EXPECT_TRUE(contains(cleaned_ids, 2u));
	EXPECT_TRUE(contains(cleaned_ids, 3u));
	EXPECT_TRUE(contains(cleaned_ids, 4u));
	EXPECT_TRUE(contains(cleaned_ids, 5u));
	EXPECT_TRUE(contains(cleaned_ids, 6u));
	EXPECT_TRUE(contains(cleaned_ids, 7u));

	// Test wrapping around version values.
	for (size_t i = 0; i < 256; ++i) {
		graph.make_dirty(0); // Should invalidate whole graph.

		EXPECT_FALSE(graph.is_dirty(0));
		EXPECT_TRUE(graph.is_dirty(1));
		EXPECT_TRUE(graph.is_dirty(2));
		EXPECT_TRUE(graph.is_dirty(3));
		EXPECT_TRUE(graph.is_dirty(4));
		EXPECT_TRUE(graph.is_dirty(5));
		EXPECT_TRUE(graph.is_dirty(6));
		EXPECT_TRUE(graph.is_dirty(7));
	}

	evaled_ids.clear();
	cleaned_ids.clear();
#if FEA_WITH_TBB
	graph.evaluate_dirty_mt(
			2, fea::make_callback([&](const my_callback_data& d) {
				std::lock_guard<std::mutex> g{ m };
				evaled_ids.push_back(d.id);
			}));
	graph.clean_mt(2, fea::make_callback([&](const my_callback_data& d) {
		std::lock_guard<std::mutex> g{ m };
		test_parents(d.id, d.parents);
		EXPECT_EQ(num_dirty(d.parents), d.parents.size());
		cleaned_ids.push_back(d.id);
	}));
#else
	graph.evaluate_dirty(2, fea::make_callback([&](const my_callback_data& d) {
		std::lock_guard<std::mutex> g{ m };
		evaled_ids.push_back(d.id);
	}));
	graph.clean(2, fea::make_callback([&](const my_callback_data& d) {
		std::lock_guard<std::mutex> g{ m };
		test_parents(d.id, d.parents);
		EXPECT_EQ(num_dirty(d.parents), d.parents.size());
		cleaned_ids.push_back(d.id);
	}));
#endif


	// Test the order of evaluation.
	EXPECT_GT(get_index(cleaned_ids, 2), get_index(cleaned_ids, 1));

	// Test no duplicate messages.
	std::sort(evaled_ids.begin(), evaled_ids.end());
	std::sort(cleaned_ids.begin(), cleaned_ids.end());
	EXPECT_EQ(evaled_ids, cleaned_ids);

	it = std::unique(cleaned_ids.begin(), cleaned_ids.end());
	EXPECT_EQ(it, cleaned_ids.end());

	EXPECT_FALSE(graph.is_dirty(0));
	EXPECT_FALSE(graph.is_dirty(1));
	EXPECT_FALSE(graph.is_dirty(2));
	EXPECT_TRUE(graph.is_dirty(3));
	EXPECT_TRUE(graph.is_dirty(4));
	EXPECT_TRUE(graph.is_dirty(5));
	EXPECT_TRUE(graph.is_dirty(6));
	EXPECT_TRUE(graph.is_dirty(7));

	EXPECT_FALSE(contains(cleaned_ids, 0u));
	EXPECT_TRUE(contains(cleaned_ids, 1u));
	EXPECT_TRUE(contains(cleaned_ids, 2u));
	EXPECT_FALSE(contains(cleaned_ids, 3u));
	EXPECT_FALSE(contains(cleaned_ids, 4u));
	EXPECT_FALSE(contains(cleaned_ids, 5u));
	EXPECT_FALSE(contains(cleaned_ids, 6u));
	EXPECT_FALSE(contains(cleaned_ids, 7u));

	graph.make_dirty(0);
	evaled_ids.clear();
	cleaned_ids.clear();

#if FEA_WITH_TBB
	graph.evaluate_dirty_mt(
			5, fea::make_callback([&](const my_callback_data& d) {
				std::lock_guard<std::mutex> g{ m };
				evaled_ids.push_back(d.id);
			}));
	graph.clean_mt(5, fea::make_callback([&](const my_callback_data& d) {
		std::lock_guard<std::mutex> g{ m };
		test_parents(d.id, d.parents);
		EXPECT_EQ(num_dirty(d.parents), d.parents.size());
		cleaned_ids.push_back(d.id);
	}));
#else
	graph.evaluate_dirty(5, fea::make_callback([&](const my_callback_data& d) {
		std::lock_guard<std::mutex> g{ m };
		evaled_ids.push_back(d.id);
	}));
	graph.clean(5, fea::make_callback([&](const my_callback_data& d) {
		std::lock_guard<std::mutex> g{ m };
		test_parents(d.id, d.parents);
		EXPECT_EQ(num_dirty(d.parents), d.parents.size());
		cleaned_ids.push_back(d.id);
	}));
#endif


	// Test the order of evaluation.
	EXPECT_GT(get_index(cleaned_ids, 2), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 3), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 3), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 5), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 5), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 5), get_index(cleaned_ids, 3));

	// Test no duplicate messages
	std::sort(evaled_ids.begin(), evaled_ids.end());
	std::sort(cleaned_ids.begin(), cleaned_ids.end());
	EXPECT_EQ(evaled_ids, cleaned_ids);

	it = std::unique(cleaned_ids.begin(), cleaned_ids.end());
	EXPECT_EQ(it, cleaned_ids.end());

	EXPECT_FALSE(graph.is_dirty(0));
	EXPECT_FALSE(graph.is_dirty(1));
	EXPECT_FALSE(graph.is_dirty(2));
	EXPECT_FALSE(graph.is_dirty(3));
	EXPECT_TRUE(graph.is_dirty(4));
	EXPECT_FALSE(graph.is_dirty(5));
	EXPECT_TRUE(graph.is_dirty(6));
	EXPECT_TRUE(graph.is_dirty(7));

	EXPECT_FALSE(contains(cleaned_ids, 0u));
	EXPECT_TRUE(contains(cleaned_ids, 1u));
	EXPECT_TRUE(contains(cleaned_ids, 2u));
	EXPECT_TRUE(contains(cleaned_ids, 3u));
	EXPECT_FALSE(contains(cleaned_ids, 4u));
	EXPECT_TRUE(contains(cleaned_ids, 5u));
	EXPECT_FALSE(contains(cleaned_ids, 6u));
	EXPECT_FALSE(contains(cleaned_ids, 7u));

	// Clean everything
#if FEA_WITH_TBB
	graph.clean_mt(7, fea::make_callback([](const my_callback_data&) {}));
#else
	graph.clean(7, fea::make_callback([](const my_callback_data&) {}));
#endif
	EXPECT_FALSE(graph.is_dirty(0));
	EXPECT_FALSE(graph.is_dirty(1));
	EXPECT_FALSE(graph.is_dirty(2));
	EXPECT_FALSE(graph.is_dirty(3));
	EXPECT_FALSE(graph.is_dirty(4));
	EXPECT_FALSE(graph.is_dirty(5));
	EXPECT_FALSE(graph.is_dirty(6));
	EXPECT_FALSE(graph.is_dirty(7));

	graph.make_dirty(3);
	EXPECT_FALSE(graph.is_dirty(0));
	EXPECT_FALSE(graph.is_dirty(1));
	EXPECT_FALSE(graph.is_dirty(2));
	// Writing to a node doesn't mean it needs to recompute using its
	// parents.
	EXPECT_FALSE(graph.is_dirty(3));
	EXPECT_TRUE(graph.is_dirty(4));
	EXPECT_TRUE(graph.is_dirty(5));
	EXPECT_TRUE(graph.is_dirty(6));
	EXPECT_TRUE(graph.is_dirty(7));

	cleaned_ids.clear();
#if FEA_WITH_TBB
	graph.clean_mt(6, fea::make_callback([&](const my_callback_data& d) {
		std::lock_guard<std::mutex> g{ m };
		test_parents(d.id, d.parents);
		EXPECT_EQ(num_dirty(d.parents), d.parents.size());
		cleaned_ids.push_back(d.id);
	}));
#else
	graph.clean(6, fea::make_callback([&](const my_callback_data& d) {
		std::lock_guard<std::mutex> g{ m };
		test_parents(d.id, d.parents);
		EXPECT_EQ(num_dirty(d.parents), d.parents.size());
		cleaned_ids.push_back(d.id);
	}));
#endif


	// Only should clean 6.
	EXPECT_EQ(cleaned_ids.size(), 1u);

	// Test no duplicate messages
	std::sort(cleaned_ids.begin(), cleaned_ids.end());
	it = std::unique(cleaned_ids.begin(), cleaned_ids.end());
	EXPECT_EQ(it, cleaned_ids.end());

	EXPECT_FALSE(graph.is_dirty(0));
	EXPECT_FALSE(graph.is_dirty(1));
	EXPECT_FALSE(graph.is_dirty(2));
	EXPECT_FALSE(graph.is_dirty(3));
	EXPECT_TRUE(graph.is_dirty(4));
	EXPECT_TRUE(graph.is_dirty(5));
	EXPECT_FALSE(graph.is_dirty(6));
	EXPECT_TRUE(graph.is_dirty(7));

	EXPECT_FALSE(contains(cleaned_ids, 0u));
	EXPECT_FALSE(contains(cleaned_ids, 1u));
	EXPECT_FALSE(contains(cleaned_ids, 2u));
	EXPECT_FALSE(contains(cleaned_ids, 3u));
	EXPECT_FALSE(contains(cleaned_ids, 4u));
	EXPECT_FALSE(contains(cleaned_ids, 5u));
	EXPECT_TRUE(contains(cleaned_ids, 6u));
	EXPECT_FALSE(contains(cleaned_ids, 7u));
}

TEST(lazy_graph, basics_max_parents) {
	// We use a smaller version tracking int so we can test the edge case when
	// version has to wrap around.
	// fea::lazy_graph<unsigned, uint8_t> graph;

	// Can use with different map, as long as it fulfills std::unordered_map
	// apis.
	fea::lazy_graph<unsigned, char, uint8_t, std::unordered_map, 32> graph;

	// Basics
	{
		EXPECT_TRUE(graph.empty());
		EXPECT_FALSE(graph.contains(0));

		graph.add_node(0);
		EXPECT_FALSE(graph.empty());
		EXPECT_TRUE(graph.contains(0));
		EXPECT_TRUE(graph.is_root(0));
		EXPECT_FALSE(graph.has_children(0));

		graph.remove_node(0);
		EXPECT_TRUE(graph.empty());
		EXPECT_FALSE(graph.contains(0));

		graph.add_node(0);
		EXPECT_FALSE(graph.empty());
		EXPECT_TRUE(graph.contains(0));
		EXPECT_TRUE(graph.is_root(0));
		EXPECT_FALSE(graph.has_children(0));

		EXPECT_TRUE(graph.add_dependency(1, 0));
		EXPECT_TRUE(graph.contains(1));
		EXPECT_TRUE(graph.is_root(0));
		EXPECT_FALSE(graph.is_root(1));
		EXPECT_TRUE(graph.has_children(0));
		EXPECT_FALSE(graph.has_children(1));

		EXPECT_TRUE(graph.is_invalid_child(1, 0));
		EXPECT_TRUE(graph.is_invalid_child(0, 1));

		EXPECT_FALSE(graph.is_invalid_child(2, 0));
		EXPECT_FALSE(graph.is_invalid_child(2, 1));

		EXPECT_TRUE(graph.add_dependency(2, 0));
		EXPECT_TRUE(graph.is_invalid_child(2, 0));
		EXPECT_TRUE(graph.is_invalid_child(0, 2));
	}


	// Test loop prevention.
	{
		EXPECT_TRUE(graph.add_dependency(2, 1));
		EXPECT_TRUE(graph.add_dependency(3, 2));
		EXPECT_TRUE(graph.add_dependency(4, 3));

		EXPECT_TRUE(graph.is_invalid_child(0, 2));
		EXPECT_TRUE(graph.is_invalid_child(0, 3));
		EXPECT_TRUE(graph.is_invalid_child(0, 4));
		EXPECT_TRUE(graph.is_invalid_child(1, 2));
		EXPECT_TRUE(graph.is_invalid_child(1, 3));
		EXPECT_TRUE(graph.is_invalid_child(1, 4));
		EXPECT_TRUE(graph.is_invalid_child(2, 2));
		EXPECT_TRUE(graph.is_invalid_child(2, 3));
		EXPECT_TRUE(graph.is_invalid_child(2, 4));
		EXPECT_TRUE(graph.is_invalid_child(3, 3));
		EXPECT_TRUE(graph.is_invalid_child(3, 4));
		EXPECT_TRUE(graph.is_invalid_child(4, 4));

		EXPECT_FALSE(graph.add_dependency(0, 2));
		EXPECT_FALSE(graph.add_dependency(0, 3));
		EXPECT_FALSE(graph.add_dependency(0, 4));
		EXPECT_FALSE(graph.add_dependency(1, 2));
		EXPECT_FALSE(graph.add_dependency(1, 3));
		EXPECT_FALSE(graph.add_dependency(1, 4));
		EXPECT_FALSE(graph.add_dependency(2, 2));
		EXPECT_FALSE(graph.add_dependency(2, 3));
		EXPECT_FALSE(graph.add_dependency(2, 4));
		EXPECT_FALSE(graph.add_dependency(3, 3));
		EXPECT_FALSE(graph.add_dependency(3, 4));
		EXPECT_FALSE(graph.add_dependency(4, 4));

		graph.remove_node(1);
		EXPECT_FALSE(graph.contains(1));
		EXPECT_TRUE(graph.has_children(0)); // has 2
		EXPECT_FALSE(graph.has_child(1, 0));
		EXPECT_TRUE(graph.has_parents(2)); // has 0
		EXPECT_FALSE(graph.has_parent(2, 1));
	}
}

TEST(lazy_graph, removing_max_parents) {
	fea::lazy_graph<unsigned, char, uint8_t, std::unordered_map, 32> graph;
	reset_graph(graph);

	// Test removing
	graph.remove_node(2);
	graph.remove_node(3);
	EXPECT_FALSE(graph.contains(2));
	EXPECT_FALSE(graph.contains(3));
	EXPECT_TRUE(graph.has_children(0)); // has 1
	EXPECT_FALSE(graph.has_child(2, 0));
	EXPECT_FALSE(graph.has_child(3, 0)); // never had it
	EXPECT_FALSE(graph.has_child(2, 1));
	EXPECT_FALSE(graph.has_child(3, 1));
	EXPECT_FALSE(graph.has_parents(4)); // has 0
	EXPECT_FALSE(graph.has_parent(4, 3));


	// Remove subgraph
	reset_graph(graph);

	// Should remove 1 only, since 2 also depends on 0, 3 depends on 2
	graph.remove_subgraph(1);
	EXPECT_TRUE(graph.contains(0));
	EXPECT_TRUE(graph.contains(2));
	EXPECT_FALSE(graph.contains(1));
	EXPECT_TRUE(graph.contains(3));
	EXPECT_TRUE(graph.contains(4));
	EXPECT_TRUE(graph.contains(5));
	EXPECT_TRUE(graph.contains(6));
	EXPECT_TRUE(graph.contains(7));

	reset_graph(graph);

	graph.remove_dependency(3, 2);
	EXPECT_TRUE(graph.add_dependency(3, 1));

	// Should remove 1, 3, 4, 5, 6, 7
	graph.remove_subgraph(1);
	EXPECT_TRUE(graph.contains(0));
	EXPECT_TRUE(graph.contains(2));
	EXPECT_FALSE(graph.contains(1));
	EXPECT_FALSE(graph.contains(3));
	EXPECT_FALSE(graph.contains(4));
	EXPECT_FALSE(graph.contains(5));
	EXPECT_FALSE(graph.contains(6));
	EXPECT_FALSE(graph.contains(7));
}

TEST(lazy_graph, advanced_max_parents) {
	fea::lazy_graph<unsigned, char, uint8_t, std::unordered_map, 32> graph;
	reset_graph(graph);

	// Evaluation graphs
	fea::span<const unsigned> subgraph = graph.evaluation_graph(7);
	size_t idx_0 = get_index(subgraph, 0u);
	size_t idx_1 = get_index(subgraph, 1u);
	size_t idx_2 = get_index(subgraph, 2u);
	size_t idx_3 = get_index(subgraph, 3u);
	size_t idx_4 = get_index(subgraph, 4u);
	size_t idx_5 = get_index(subgraph, 5u);
	size_t idx_6 = get_index(subgraph, 6u);
	size_t idx_7 = get_index(subgraph, 7u);
	EXPECT_EQ(idx_0, 0u); // should be at front
	EXPECT_EQ(idx_1, 1u); // should be second
	EXPECT_EQ(idx_2, 2u); // should be third
	EXPECT_EQ(idx_3, 3u); // should be fourth
	EXPECT_EQ(idx_7, 7u); // should be at end

	EXPECT_GT(idx_4, idx_3);
	EXPECT_GT(idx_5, idx_3);
	EXPECT_GT(idx_6, idx_3);

	EXPECT_GT(idx_7, idx_0);
	EXPECT_GT(idx_7, idx_1);
	EXPECT_GT(idx_7, idx_2);
	EXPECT_GT(idx_7, idx_3);
	EXPECT_GT(idx_7, idx_4);
	EXPECT_GT(idx_7, idx_5);
	EXPECT_GT(idx_7, idx_6);

	std::vector<unsigned> to_eval{ 0, 1, 2, 3, 4, 5, 6, 7 };
	auto ind_data = graph.are_eval_graphs_independent(to_eval);
	EXPECT_EQ(ind_data.independent_graphs.size(), 0u);
	EXPECT_EQ(ind_data.dependent_graphs.size(), 8u);
}

TEST(lazy_graph, dirtyness_max_parents) {
	fea::lazy_graph<unsigned, char, uint8_t, std::unordered_map, 32> graph;
	using my_callback_data = fea::callback_data<unsigned>;
	reset_graph(graph);

	// Dirtyness
	// Everything starts dirty, except root.
	EXPECT_FALSE(graph.is_dirty(0));
	EXPECT_TRUE(graph.is_dirty(1));
	EXPECT_TRUE(graph.is_dirty(2));
	EXPECT_TRUE(graph.is_dirty(3));
	EXPECT_TRUE(graph.is_dirty(4));
	EXPECT_TRUE(graph.is_dirty(5));
	EXPECT_TRUE(graph.is_dirty(6));
	EXPECT_TRUE(graph.is_dirty(7));

	// Clean it.
	std::vector<unsigned> evaled_ids;
	std::vector<unsigned> cleaned_ids;

	graph.evaluate_dirty(4, fea::make_callback([&](const my_callback_data& d) {
		evaled_ids.push_back(d.id);
	}));
	graph.clean(4, fea::make_callback([&](const my_callback_data& d) {
		test_parents(d.id, d.parents);
		EXPECT_EQ(num_dirty(d.parents), d.parents.size());

		cleaned_ids.push_back(d.id);
	}));

	EXPECT_EQ(evaled_ids, cleaned_ids);

	// Test the order of evaluation.
	// Root is not cleaned, since it doesn't depend on anything.
	EXPECT_GT(get_index(cleaned_ids, 2), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 3), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 3), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 4), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 4), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 4), get_index(cleaned_ids, 3));

	// Tests that the lambda was only called once per node (aka no duplicate
	// messages).
	std::sort(cleaned_ids.begin(), cleaned_ids.end());
	auto it = std::unique(cleaned_ids.begin(), cleaned_ids.end());
	EXPECT_EQ(it, cleaned_ids.end());

	// Graph should be clean now.
	EXPECT_FALSE(graph.is_dirty(0));
	EXPECT_FALSE(graph.is_dirty(1));
	EXPECT_FALSE(graph.is_dirty(2));
	EXPECT_FALSE(graph.is_dirty(3));
	EXPECT_FALSE(graph.is_dirty(4));
	EXPECT_TRUE(graph.is_dirty(5));
	EXPECT_TRUE(graph.is_dirty(6));
	EXPECT_TRUE(graph.is_dirty(7));

	// We should have visited everything except the root.
	EXPECT_FALSE(contains(cleaned_ids, 0u));
	EXPECT_TRUE(contains(cleaned_ids, 1u));
	EXPECT_TRUE(contains(cleaned_ids, 2u));
	EXPECT_TRUE(contains(cleaned_ids, 3u));
	EXPECT_TRUE(contains(cleaned_ids, 4u));
	EXPECT_FALSE(contains(cleaned_ids, 5u));
	EXPECT_FALSE(contains(cleaned_ids, 6u));
	EXPECT_FALSE(contains(cleaned_ids, 7u));

	// Invalidate whole graph again.
	uint8_t ver_before = graph.version(0);
	graph.make_dirty(0);
	EXPECT_FALSE(graph.is_dirty(0));
	EXPECT_TRUE(graph.is_dirty(1));
	EXPECT_TRUE(graph.is_dirty(2));
	EXPECT_TRUE(graph.is_dirty(3));
	EXPECT_TRUE(graph.is_dirty(4));
	EXPECT_TRUE(graph.is_dirty(5));
	EXPECT_TRUE(graph.is_dirty(6));
	EXPECT_TRUE(graph.is_dirty(7));

	// Clean it again.
	evaled_ids.clear();
	cleaned_ids.clear();

	graph.evaluate_dirty(7, fea::make_callback([&](const my_callback_data& d) {
		evaled_ids.push_back(d.id);
	}));
	graph.clean(7, fea::make_callback([&](const my_callback_data& d) {
		test_parents(d.id, d.parents);
		EXPECT_EQ(num_dirty(d.parents), d.parents.size());
		cleaned_ids.push_back(d.id);
	}));

	EXPECT_EQ(evaled_ids, cleaned_ids);

	EXPECT_GT(graph.version(0), ver_before);

	// Test the order of evaluation.
	EXPECT_GT(get_index(cleaned_ids, 2), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 3), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 3), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 4), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 4), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 4), get_index(cleaned_ids, 3));
	EXPECT_GT(get_index(cleaned_ids, 5), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 5), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 5), get_index(cleaned_ids, 3));
	EXPECT_GT(get_index(cleaned_ids, 6), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 6), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 6), get_index(cleaned_ids, 3));
	EXPECT_GT(get_index(cleaned_ids, 7), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 7), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 7), get_index(cleaned_ids, 3));
	EXPECT_GT(get_index(cleaned_ids, 7), get_index(cleaned_ids, 4));
	EXPECT_GT(get_index(cleaned_ids, 7), get_index(cleaned_ids, 5));
	EXPECT_GT(get_index(cleaned_ids, 7), get_index(cleaned_ids, 6));

	// Test no duplicate messages
	std::sort(cleaned_ids.begin(), cleaned_ids.end());
	it = std::unique(cleaned_ids.begin(), cleaned_ids.end());
	EXPECT_EQ(it, cleaned_ids.end());

	// Graph should be clean now.
	EXPECT_FALSE(graph.is_dirty(0));
	EXPECT_FALSE(graph.is_dirty(1));
	EXPECT_FALSE(graph.is_dirty(2));
	EXPECT_FALSE(graph.is_dirty(3));
	EXPECT_FALSE(graph.is_dirty(4));
	EXPECT_FALSE(graph.is_dirty(5));
	EXPECT_FALSE(graph.is_dirty(6));
	EXPECT_FALSE(graph.is_dirty(7));

	// We should have visited everything except the root.
	EXPECT_FALSE(contains(cleaned_ids, 0u));
	EXPECT_TRUE(contains(cleaned_ids, 1u));
	EXPECT_TRUE(contains(cleaned_ids, 2u));
	EXPECT_TRUE(contains(cleaned_ids, 3u));
	EXPECT_TRUE(contains(cleaned_ids, 4u));
	EXPECT_TRUE(contains(cleaned_ids, 5u));
	EXPECT_TRUE(contains(cleaned_ids, 6u));
	EXPECT_TRUE(contains(cleaned_ids, 7u));

	// Test wrapping around version values.
	for (size_t i = 0; i < 256; ++i) {
		graph.make_dirty(0); // Should invalidate whole graph.

		EXPECT_FALSE(graph.is_dirty(0));
		EXPECT_TRUE(graph.is_dirty(1));
		EXPECT_TRUE(graph.is_dirty(2));
		EXPECT_TRUE(graph.is_dirty(3));
		EXPECT_TRUE(graph.is_dirty(4));
		EXPECT_TRUE(graph.is_dirty(5));
		EXPECT_TRUE(graph.is_dirty(6));
		EXPECT_TRUE(graph.is_dirty(7));
	}

	cleaned_ids.clear();
	graph.clean(2, fea::make_callback([&](const my_callback_data& d) {
		test_parents(d.id, d.parents);
		EXPECT_EQ(num_dirty(d.parents), d.parents.size());
		cleaned_ids.push_back(d.id);
	}));


	// Test the order of evaluation.
	EXPECT_GT(get_index(cleaned_ids, 2), get_index(cleaned_ids, 1));

	// Test no duplicate messages.
	std::sort(cleaned_ids.begin(), cleaned_ids.end());
	it = std::unique(cleaned_ids.begin(), cleaned_ids.end());
	EXPECT_EQ(it, cleaned_ids.end());

	EXPECT_FALSE(graph.is_dirty(0));
	EXPECT_FALSE(graph.is_dirty(1));
	EXPECT_FALSE(graph.is_dirty(2));
	EXPECT_TRUE(graph.is_dirty(3));
	EXPECT_TRUE(graph.is_dirty(4));
	EXPECT_TRUE(graph.is_dirty(5));
	EXPECT_TRUE(graph.is_dirty(6));
	EXPECT_TRUE(graph.is_dirty(7));

	EXPECT_FALSE(contains(cleaned_ids, 0u));
	EXPECT_TRUE(contains(cleaned_ids, 1u));
	EXPECT_TRUE(contains(cleaned_ids, 2u));
	EXPECT_FALSE(contains(cleaned_ids, 3u));
	EXPECT_FALSE(contains(cleaned_ids, 4u));
	EXPECT_FALSE(contains(cleaned_ids, 5u));
	EXPECT_FALSE(contains(cleaned_ids, 6u));
	EXPECT_FALSE(contains(cleaned_ids, 7u));

	graph.make_dirty(0);
	evaled_ids.clear();
	cleaned_ids.clear();
	graph.evaluate_dirty(5, fea::make_callback([&](const my_callback_data& d) {
		evaled_ids.push_back(d.id);
	}));
	graph.clean(5, fea::make_callback([&](const my_callback_data& d) {
		test_parents(d.id, d.parents);
		EXPECT_EQ(num_dirty(d.parents), d.parents.size());
		cleaned_ids.push_back(d.id);
	}));
	EXPECT_EQ(evaled_ids, cleaned_ids);

	// Test the order of evaluation.
	EXPECT_GT(get_index(cleaned_ids, 2), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 3), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 3), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 5), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 5), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 5), get_index(cleaned_ids, 3));

	// Test no duplicate messages
	std::sort(cleaned_ids.begin(), cleaned_ids.end());
	it = std::unique(cleaned_ids.begin(), cleaned_ids.end());
	EXPECT_EQ(it, cleaned_ids.end());

	EXPECT_FALSE(graph.is_dirty(0));
	EXPECT_FALSE(graph.is_dirty(1));
	EXPECT_FALSE(graph.is_dirty(2));
	EXPECT_FALSE(graph.is_dirty(3));
	EXPECT_TRUE(graph.is_dirty(4));
	EXPECT_FALSE(graph.is_dirty(5));
	EXPECT_TRUE(graph.is_dirty(6));
	EXPECT_TRUE(graph.is_dirty(7));

	EXPECT_FALSE(contains(cleaned_ids, 0u));
	EXPECT_TRUE(contains(cleaned_ids, 1u));
	EXPECT_TRUE(contains(cleaned_ids, 2u));
	EXPECT_TRUE(contains(cleaned_ids, 3u));
	EXPECT_FALSE(contains(cleaned_ids, 4u));
	EXPECT_TRUE(contains(cleaned_ids, 5u));
	EXPECT_FALSE(contains(cleaned_ids, 6u));
	EXPECT_FALSE(contains(cleaned_ids, 7u));

	// Clean everything
	graph.clean(7, fea::make_callback([](const my_callback_data&) {}));
	EXPECT_FALSE(graph.is_dirty(0));
	EXPECT_FALSE(graph.is_dirty(1));
	EXPECT_FALSE(graph.is_dirty(2));
	EXPECT_FALSE(graph.is_dirty(3));
	EXPECT_FALSE(graph.is_dirty(4));
	EXPECT_FALSE(graph.is_dirty(5));
	EXPECT_FALSE(graph.is_dirty(6));
	EXPECT_FALSE(graph.is_dirty(7));

	graph.make_dirty(3);
	EXPECT_FALSE(graph.is_dirty(0));
	EXPECT_FALSE(graph.is_dirty(1));
	EXPECT_FALSE(graph.is_dirty(2));
	// Writing to a node doesn't mean it needs to recompute using its
	// parents.
	EXPECT_FALSE(graph.is_dirty(3));
	EXPECT_TRUE(graph.is_dirty(4));
	EXPECT_TRUE(graph.is_dirty(5));
	EXPECT_TRUE(graph.is_dirty(6));
	EXPECT_TRUE(graph.is_dirty(7));

	evaled_ids.clear();
	cleaned_ids.clear();

	graph.evaluate_dirty(6, fea::make_callback([&](const my_callback_data& d) {
		evaled_ids.push_back(d.id);
	}));
	graph.clean(6, fea::make_callback([&](const my_callback_data& d) {
		test_parents(d.id, d.parents);
		EXPECT_EQ(num_dirty(d.parents), d.parents.size());
		cleaned_ids.push_back(d.id);
	}));

	EXPECT_EQ(evaled_ids, cleaned_ids);

	// Only should clean 6.
	EXPECT_EQ(cleaned_ids.size(), 1u);

	// Test no duplicate messages
	std::sort(cleaned_ids.begin(), cleaned_ids.end());
	it = std::unique(cleaned_ids.begin(), cleaned_ids.end());
	EXPECT_EQ(it, cleaned_ids.end());

	EXPECT_FALSE(graph.is_dirty(0));
	EXPECT_FALSE(graph.is_dirty(1));
	EXPECT_FALSE(graph.is_dirty(2));
	EXPECT_FALSE(graph.is_dirty(3));
	EXPECT_TRUE(graph.is_dirty(4));
	EXPECT_TRUE(graph.is_dirty(5));
	EXPECT_FALSE(graph.is_dirty(6));
	EXPECT_TRUE(graph.is_dirty(7));

	EXPECT_FALSE(contains(cleaned_ids, 0u));
	EXPECT_FALSE(contains(cleaned_ids, 1u));
	EXPECT_FALSE(contains(cleaned_ids, 2u));
	EXPECT_FALSE(contains(cleaned_ids, 3u));
	EXPECT_FALSE(contains(cleaned_ids, 4u));
	EXPECT_FALSE(contains(cleaned_ids, 5u));
	EXPECT_TRUE(contains(cleaned_ids, 6u));
	EXPECT_FALSE(contains(cleaned_ids, 7u));
}

TEST(lazy_graph, dirtyness_mt_max_parents) {
	fea::lazy_graph<unsigned, char, uint8_t, std::unordered_map, 32> graph;
	using my_callback_data = fea::callback_data<unsigned>;
	reset_graph(graph);

	// Dirtyness multi-threaded
	graph.make_dirty(0);

	// Everything starts dirty, except root.
	EXPECT_FALSE(graph.is_dirty(0));
	EXPECT_TRUE(graph.is_dirty(1));
	EXPECT_TRUE(graph.is_dirty(2));
	EXPECT_TRUE(graph.is_dirty(3));
	EXPECT_TRUE(graph.is_dirty(4));
	EXPECT_TRUE(graph.is_dirty(5));
	EXPECT_TRUE(graph.is_dirty(6));
	EXPECT_TRUE(graph.is_dirty(7));

	// Clean it.
	std::vector<unsigned> evaled_ids;
	std::vector<unsigned> cleaned_ids;
	std::mutex m;
#if FEA_WITH_TBB
	graph.evaluate_dirty_mt(
			4, fea::make_callback([&](const my_callback_data& d) {
				std::lock_guard<std::mutex> g{ m };
				evaled_ids.push_back(d.id);
			}));
	graph.clean_mt(4, fea::make_callback([&](const my_callback_data& d) {
		std::lock_guard<std::mutex> g{ m };
		test_parents(d.id, d.parents);
		EXPECT_EQ(num_dirty(d.parents), d.parents.size());
		cleaned_ids.push_back(d.id);
	}));
#else
	graph.evaluate_dirty(4, fea::make_callback([&](const my_callback_data& d) {
		std::lock_guard<std::mutex> g{ m };
		evaled_ids.push_back(d.id);
	}));
	graph.clean(4, fea::make_callback([&](const my_callback_data& d) {
		std::lock_guard<std::mutex> g{ m };
		test_parents(d.id, d.parents);
		EXPECT_EQ(num_dirty(d.parents), d.parents.size());
		cleaned_ids.push_back(d.id);
	}));
#endif


	// Test the order of evaluation.
	// Root is not cleaned, since it doesn't depend on anything.
	EXPECT_GT(get_index(cleaned_ids, 2), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 3), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 3), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 4), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 4), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 4), get_index(cleaned_ids, 3));

	// Tests that the lambda was only called once per node (aka no duplicate
	// messages).
	std::sort(evaled_ids.begin(), evaled_ids.end());
	std::sort(cleaned_ids.begin(), cleaned_ids.end());
	EXPECT_EQ(evaled_ids, cleaned_ids);

	auto it = std::unique(cleaned_ids.begin(), cleaned_ids.end());
	EXPECT_EQ(it, cleaned_ids.end());

	// Graph should be clean now.
	EXPECT_FALSE(graph.is_dirty(0));
	EXPECT_FALSE(graph.is_dirty(1));
	EXPECT_FALSE(graph.is_dirty(2));
	EXPECT_FALSE(graph.is_dirty(3));
	EXPECT_FALSE(graph.is_dirty(4));
	EXPECT_TRUE(graph.is_dirty(5));
	EXPECT_TRUE(graph.is_dirty(6));
	EXPECT_TRUE(graph.is_dirty(7));

	// We should have visited everything except the root.
	EXPECT_FALSE(contains(cleaned_ids, 0u));
	EXPECT_TRUE(contains(cleaned_ids, 1u));
	EXPECT_TRUE(contains(cleaned_ids, 2u));
	EXPECT_TRUE(contains(cleaned_ids, 3u));
	EXPECT_TRUE(contains(cleaned_ids, 4u));
	EXPECT_FALSE(contains(cleaned_ids, 5u));
	EXPECT_FALSE(contains(cleaned_ids, 6u));
	EXPECT_FALSE(contains(cleaned_ids, 7u));

	// Invalidate whole graph again.
	graph.make_dirty(0);
	EXPECT_FALSE(graph.is_dirty(0));
	EXPECT_TRUE(graph.is_dirty(1));
	EXPECT_TRUE(graph.is_dirty(2));
	EXPECT_TRUE(graph.is_dirty(3));
	EXPECT_TRUE(graph.is_dirty(4));
	EXPECT_TRUE(graph.is_dirty(5));
	EXPECT_TRUE(graph.is_dirty(6));
	EXPECT_TRUE(graph.is_dirty(7));

	// Clean it again.
	cleaned_ids.clear();
#if FEA_WITH_TBB
	graph.clean_mt(7, fea::make_callback([&](const my_callback_data& d) {
		std::lock_guard<std::mutex> g{ m };
		test_parents(d.id, d.parents);
		EXPECT_EQ(num_dirty(d.parents), d.parents.size());
		cleaned_ids.push_back(d.id);
	}));
#else
	graph.clean(7, fea::make_callback([&](const my_callback_data& d) {
		std::lock_guard<std::mutex> g{ m };
		test_parents(d.id, d.parents);
		EXPECT_EQ(num_dirty(d.parents), d.parents.size());
		cleaned_ids.push_back(d.id);
	}));
#endif

	// Test the order of evaluation.
	EXPECT_GT(get_index(cleaned_ids, 2), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 3), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 3), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 4), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 4), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 4), get_index(cleaned_ids, 3));
	EXPECT_GT(get_index(cleaned_ids, 5), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 5), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 5), get_index(cleaned_ids, 3));
	EXPECT_GT(get_index(cleaned_ids, 6), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 6), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 6), get_index(cleaned_ids, 3));
	EXPECT_GT(get_index(cleaned_ids, 7), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 7), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 7), get_index(cleaned_ids, 3));
	EXPECT_GT(get_index(cleaned_ids, 7), get_index(cleaned_ids, 4));
	EXPECT_GT(get_index(cleaned_ids, 7), get_index(cleaned_ids, 5));
	EXPECT_GT(get_index(cleaned_ids, 7), get_index(cleaned_ids, 6));

	// Test no duplicate messages
	std::sort(cleaned_ids.begin(), cleaned_ids.end());
	it = std::unique(cleaned_ids.begin(), cleaned_ids.end());
	EXPECT_EQ(it, cleaned_ids.end());

	// Graph should be clean now.
	EXPECT_FALSE(graph.is_dirty(0));
	EXPECT_FALSE(graph.is_dirty(1));
	EXPECT_FALSE(graph.is_dirty(2));
	EXPECT_FALSE(graph.is_dirty(3));
	EXPECT_FALSE(graph.is_dirty(4));
	EXPECT_FALSE(graph.is_dirty(5));
	EXPECT_FALSE(graph.is_dirty(6));
	EXPECT_FALSE(graph.is_dirty(7));

	// We should have visited everything except the root.
	EXPECT_FALSE(contains(cleaned_ids, 0u));
	EXPECT_TRUE(contains(cleaned_ids, 1u));
	EXPECT_TRUE(contains(cleaned_ids, 2u));
	EXPECT_TRUE(contains(cleaned_ids, 3u));
	EXPECT_TRUE(contains(cleaned_ids, 4u));
	EXPECT_TRUE(contains(cleaned_ids, 5u));
	EXPECT_TRUE(contains(cleaned_ids, 6u));
	EXPECT_TRUE(contains(cleaned_ids, 7u));

	// Test wrapping around version values.
	for (size_t i = 0; i < 256; ++i) {
		graph.make_dirty(0); // Should invalidate whole graph.

		EXPECT_FALSE(graph.is_dirty(0));
		EXPECT_TRUE(graph.is_dirty(1));
		EXPECT_TRUE(graph.is_dirty(2));
		EXPECT_TRUE(graph.is_dirty(3));
		EXPECT_TRUE(graph.is_dirty(4));
		EXPECT_TRUE(graph.is_dirty(5));
		EXPECT_TRUE(graph.is_dirty(6));
		EXPECT_TRUE(graph.is_dirty(7));
	}

	evaled_ids.clear();
	cleaned_ids.clear();

#if FEA_WITH_TBB
	graph.evaluate_dirty_mt(
			2, fea::make_callback([&](const my_callback_data& d) {
				std::lock_guard<std::mutex> g{ m };
				evaled_ids.push_back(d.id);
			}));
	graph.clean_mt(2, fea::make_callback([&](const my_callback_data& d) {
		std::lock_guard<std::mutex> g{ m };
		test_parents(d.id, d.parents);
		EXPECT_EQ(num_dirty(d.parents), d.parents.size());
		cleaned_ids.push_back(d.id);
	}));
#else
	graph.evaluate_dirty(2, fea::make_callback([&](const my_callback_data& d) {
		std::lock_guard<std::mutex> g{ m };
		evaled_ids.push_back(d.id);
	}));
	graph.clean(2, fea::make_callback([&](const my_callback_data& d) {
		std::lock_guard<std::mutex> g{ m };
		test_parents(d.id, d.parents);
		EXPECT_EQ(num_dirty(d.parents), d.parents.size());
		cleaned_ids.push_back(d.id);
	}));
#endif


	// Test the order of evaluation.
	EXPECT_GT(get_index(cleaned_ids, 2), get_index(cleaned_ids, 1));

	// Test no duplicate messages.
	std::sort(evaled_ids.begin(), evaled_ids.end());
	std::sort(cleaned_ids.begin(), cleaned_ids.end());
	EXPECT_EQ(evaled_ids, cleaned_ids);

	it = std::unique(cleaned_ids.begin(), cleaned_ids.end());
	EXPECT_EQ(it, cleaned_ids.end());

	EXPECT_FALSE(graph.is_dirty(0));
	EXPECT_FALSE(graph.is_dirty(1));
	EXPECT_FALSE(graph.is_dirty(2));
	EXPECT_TRUE(graph.is_dirty(3));
	EXPECT_TRUE(graph.is_dirty(4));
	EXPECT_TRUE(graph.is_dirty(5));
	EXPECT_TRUE(graph.is_dirty(6));
	EXPECT_TRUE(graph.is_dirty(7));

	EXPECT_FALSE(contains(cleaned_ids, 0u));
	EXPECT_TRUE(contains(cleaned_ids, 1u));
	EXPECT_TRUE(contains(cleaned_ids, 2u));
	EXPECT_FALSE(contains(cleaned_ids, 3u));
	EXPECT_FALSE(contains(cleaned_ids, 4u));
	EXPECT_FALSE(contains(cleaned_ids, 5u));
	EXPECT_FALSE(contains(cleaned_ids, 6u));
	EXPECT_FALSE(contains(cleaned_ids, 7u));

	graph.make_dirty(0);
	cleaned_ids.clear();
#if FEA_WITH_TBB
	graph.clean_mt(5, fea::make_callback([&](const my_callback_data& d) {
		std::lock_guard<std::mutex> g{ m };
		test_parents(d.id, d.parents);
		EXPECT_EQ(num_dirty(d.parents), d.parents.size());
		cleaned_ids.push_back(d.id);
	}));
#else
	graph.clean(5, fea::make_callback([&](const my_callback_data& d) {
		std::lock_guard<std::mutex> g{ m };
		test_parents(d.id, d.parents);
		EXPECT_EQ(num_dirty(d.parents), d.parents.size());
		cleaned_ids.push_back(d.id);
	}));
#endif


	// Test the order of evaluation.
	EXPECT_GT(get_index(cleaned_ids, 2), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 3), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 3), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 5), get_index(cleaned_ids, 1));
	EXPECT_GT(get_index(cleaned_ids, 5), get_index(cleaned_ids, 2));
	EXPECT_GT(get_index(cleaned_ids, 5), get_index(cleaned_ids, 3));

	// Test no duplicate messages
	std::sort(cleaned_ids.begin(), cleaned_ids.end());
	it = std::unique(cleaned_ids.begin(), cleaned_ids.end());
	EXPECT_EQ(it, cleaned_ids.end());

	EXPECT_FALSE(graph.is_dirty(0));
	EXPECT_FALSE(graph.is_dirty(1));
	EXPECT_FALSE(graph.is_dirty(2));
	EXPECT_FALSE(graph.is_dirty(3));
	EXPECT_TRUE(graph.is_dirty(4));
	EXPECT_FALSE(graph.is_dirty(5));
	EXPECT_TRUE(graph.is_dirty(6));
	EXPECT_TRUE(graph.is_dirty(7));

	EXPECT_FALSE(contains(cleaned_ids, 0u));
	EXPECT_TRUE(contains(cleaned_ids, 1u));
	EXPECT_TRUE(contains(cleaned_ids, 2u));
	EXPECT_TRUE(contains(cleaned_ids, 3u));
	EXPECT_FALSE(contains(cleaned_ids, 4u));
	EXPECT_TRUE(contains(cleaned_ids, 5u));
	EXPECT_FALSE(contains(cleaned_ids, 6u));
	EXPECT_FALSE(contains(cleaned_ids, 7u));

	// Clean everything
#if FEA_WITH_TBB
	graph.clean_mt(7, fea::make_callback([](const my_callback_data&) {}));
#else
	graph.clean(7, fea::make_callback([](const my_callback_data&) {}));
#endif
	EXPECT_FALSE(graph.is_dirty(0));
	EXPECT_FALSE(graph.is_dirty(1));
	EXPECT_FALSE(graph.is_dirty(2));
	EXPECT_FALSE(graph.is_dirty(3));
	EXPECT_FALSE(graph.is_dirty(4));
	EXPECT_FALSE(graph.is_dirty(5));
	EXPECT_FALSE(graph.is_dirty(6));
	EXPECT_FALSE(graph.is_dirty(7));

	graph.make_dirty(3);
	EXPECT_FALSE(graph.is_dirty(0));
	EXPECT_FALSE(graph.is_dirty(1));
	EXPECT_FALSE(graph.is_dirty(2));
	// Writing to a node doesn't mean it needs to recompute using its
	// parents.
	EXPECT_FALSE(graph.is_dirty(3));
	EXPECT_TRUE(graph.is_dirty(4));
	EXPECT_TRUE(graph.is_dirty(5));
	EXPECT_TRUE(graph.is_dirty(6));
	EXPECT_TRUE(graph.is_dirty(7));

	evaled_ids.clear();
	cleaned_ids.clear();

#if FEA_WITH_TBB
	graph.evaluate_dirty_mt(
			6, fea::make_callback([&](const my_callback_data& d) {
				std::lock_guard<std::mutex> g{ m };
				evaled_ids.push_back(d.id);
			}));
	graph.clean_mt(6, fea::make_callback([&](const my_callback_data& d) {
		std::lock_guard<std::mutex> g{ m };
		test_parents(d.id, d.parents);
		EXPECT_EQ(num_dirty(d.parents), d.parents.size());
		cleaned_ids.push_back(d.id);
	}));
#else
	graph.evaluate_dirty(6, fea::make_callback([&](const my_callback_data& d) {
		std::lock_guard<std::mutex> g{ m };
		evaled_ids.push_back(d.id);
	}));
	graph.clean(6, fea::make_callback([&](const my_callback_data& d) {
		std::lock_guard<std::mutex> g{ m };
		test_parents(d.id, d.parents);
		EXPECT_EQ(num_dirty(d.parents), d.parents.size());
		cleaned_ids.push_back(d.id);
	}));
#endif


	// Only should clean 6.
	EXPECT_EQ(cleaned_ids.size(), 1u);

	// Test no duplicate messages
	std::sort(evaled_ids.begin(), evaled_ids.end());
	std::sort(cleaned_ids.begin(), cleaned_ids.end());
	EXPECT_EQ(evaled_ids, cleaned_ids);

	it = std::unique(cleaned_ids.begin(), cleaned_ids.end());
	EXPECT_EQ(it, cleaned_ids.end());

	EXPECT_FALSE(graph.is_dirty(0));
	EXPECT_FALSE(graph.is_dirty(1));
	EXPECT_FALSE(graph.is_dirty(2));
	EXPECT_FALSE(graph.is_dirty(3));
	EXPECT_TRUE(graph.is_dirty(4));
	EXPECT_TRUE(graph.is_dirty(5));
	EXPECT_FALSE(graph.is_dirty(6));
	EXPECT_TRUE(graph.is_dirty(7));

	EXPECT_FALSE(contains(cleaned_ids, 0u));
	EXPECT_FALSE(contains(cleaned_ids, 1u));
	EXPECT_FALSE(contains(cleaned_ids, 2u));
	EXPECT_FALSE(contains(cleaned_ids, 3u));
	EXPECT_FALSE(contains(cleaned_ids, 4u));
	EXPECT_FALSE(contains(cleaned_ids, 5u));
	EXPECT_TRUE(contains(cleaned_ids, 6u));
	EXPECT_FALSE(contains(cleaned_ids, 7u));
}

TEST(lazy_graph, fixed_size) {
	{
		fea::lazy_graph<unsigned, char, uint8_t, std::unordered_map, 2, 2>
				graph;
#if FEA_DEBUG || FEA_NOTHROW
		EXPECT_DEATH(reset_graph(graph), "");
#else
		EXPECT_THROW(reset_graph(graph), std::runtime_error);
#endif
	}

	{
		fea::lazy_graph<unsigned, char, uint8_t, std::unordered_map, 2, 2>
				graph;
		graph.add_dependency(1, 0);
		graph.add_dependency(2, 0);

#if FEA_DEBUG || FEA_NOTHROW
		EXPECT_DEATH(graph.add_dependency(3, 0), "");
#else
		EXPECT_THROW(graph.add_dependency(3, 0), std::runtime_error);
#endif
	}

	{
		fea::lazy_graph<unsigned, char, uint8_t, std::unordered_map, 2, 2>
				graph;
		graph.add_dependency(0, 1);
		graph.add_dependency(0, 2);

#if FEA_DEBUG || FEA_NOTHROW
		EXPECT_DEATH(graph.add_dependency(0, 3), "");
#else
		EXPECT_THROW(graph.add_dependency(0, 3), std::runtime_error);
#endif
	}
}
} // namespace

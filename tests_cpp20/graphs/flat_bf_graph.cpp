#include "flat_bf_graph.hpp"

#include <algorithm>
#include <compare>
#include <fea/numerics/random.hpp>
#include <fea/utils/platform.hpp>
#include <gtest/gtest.h>
#include <numeric>
#include <ranges>
#include <vector>

namespace {
TEST(flat_bf_graph, builder_basics) {
	using id_t = uint32_t;
	static constexpr id_t sentinel = (std::numeric_limits<id_t>::max)();
	id_t next_id = 0u;
	struct node {
		id_t id = sentinel;
	};

	fea::flat_bf_graph_builder<id_t, node> builder;
	using builder_node_t =
			typename fea::flat_bf_graph_builder<id_t, node>::node_type;

	EXPECT_TRUE(builder.empty());
	EXPECT_EQ(builder.size(), 0u);
	EXPECT_EQ(builder.capacity(), 0u);
	builder.reserve(10u);
	EXPECT_EQ(builder.capacity(), 10u);
	builder.shrink_to_fit();
	EXPECT_EQ(builder.capacity(), 0u);

	{
		// Add root node.
		id_t root_id = ++next_id;
		builder.push_back(root_id, node{});
		EXPECT_TRUE(builder.is_root(root_id));

		EXPECT_FALSE(builder.empty());
		EXPECT_EQ(builder.size(), 1u);
		builder.shrink_to_fit();
		EXPECT_EQ(builder.capacity(), 1u);
		builder.reserve(10u);
		EXPECT_EQ(builder.capacity(), 10u);
		builder.shrink_to_fit();
		EXPECT_EQ(builder.capacity(), 1u);

		{
			node& parent_n = builder.at(root_id);
			EXPECT_EQ(parent_n.id, sentinel);
			parent_n.id = root_id;

			const node& parent_n2 = builder.at(root_id);
			EXPECT_EQ(parent_n2.id, root_id);
		}

		// Add child to root.
		id_t child_id = ++next_id;
		builder.push_back(root_id, child_id, node{});
		EXPECT_EQ(child_id, 2u);
		EXPECT_FALSE(builder.is_root(child_id));
		{
			node& child_n = builder.at(child_id);
			EXPECT_EQ(child_n.id, sentinel);
			child_n.id = child_id;

			const node& child_n2 = builder.at(child_id);
			EXPECT_EQ(child_n2.id, child_id);
		}


		// Add new child.
		child_id = ++next_id;
		builder.push_back(root_id, child_id, node{ child_id });
		EXPECT_EQ(child_id, 3u);
		EXPECT_FALSE(builder.is_root(child_id));
		{
			node& child_n = builder.at(child_id);
			EXPECT_EQ(child_n.id, child_id);
		}

		// Iterators.
		{
			auto b = builder.key_begin();
			auto e = builder.key_end();
			EXPECT_EQ(3, std::distance(b, e));
			EXPECT_EQ(*b, 1u);
			EXPECT_EQ(*(++b), 2u);
			EXPECT_EQ(*(++b), 3u);
		}

		// Children
		{
			std::span<const id_t> children = builder.children(root_id);
			EXPECT_EQ(children.size(), 2);
			EXPECT_EQ(children.front(), 2u);
			EXPECT_EQ(children.back(), 3u);

			auto b = builder.key_begin(root_id);
			auto e = builder.key_end(root_id);
			EXPECT_EQ(2, std::distance(b, e));
			EXPECT_EQ(*b, 2u);
			EXPECT_EQ(*(++b), 3u);
		}

		// Capacity
		{
			EXPECT_FALSE(builder.empty());
			EXPECT_EQ(builder.size(), 3u);
			builder.shrink_to_fit();
			EXPECT_EQ(builder.capacity(), 3u);
			builder.reserve(10u);
			EXPECT_EQ(builder.capacity(), 10u);
			builder.shrink_to_fit();
			EXPECT_EQ(builder.capacity(), 3u);
		}

		// Node iterators.
		{
			id_t i = 1u;

			for (const builder_node_t& n : builder) {
				EXPECT_TRUE(n.key() == i);
				EXPECT_TRUE(n.value().id == i);
				++i;
			}
		}

		// Clear
		{
			auto builder2 = builder;
			EXPECT_FALSE(builder2.empty());
			EXPECT_EQ(builder2.size(), 3u);
			EXPECT_EQ(builder2.capacity(), 3u);
			builder2.reserve(10u);
			EXPECT_EQ(builder2.capacity(), 10u);
			builder2.shrink_to_fit();
			EXPECT_EQ(builder2.capacity(), 3u);

			builder2.clear();
			EXPECT_TRUE(builder2.empty());
			EXPECT_EQ(builder2.size(), 0u);
			EXPECT_EQ(builder2.capacity(), 3u);
			builder2.shrink_to_fit();
			EXPECT_EQ(builder2.capacity(), 0u);
		}
	}

	// Stress builder conversion by adding unordered nodes.
	{
		constexpr unsigned count = 5;
		// Add roots.
		for (unsigned i = 0; i < count; ++i) {
			++next_id;
			builder.push_back(next_id, node{ next_id });
		}

		// Add children.
		for (unsigned i = 0; i < count; ++i) {
			++next_id;
			builder.push_back(next_id - count, next_id, node{ next_id });
		}
		for (unsigned i = 0; i < count; ++i) {
			++next_id;
			builder.push_back(next_id - count, next_id, node{ next_id });
		}
		for (unsigned i = 0; i < count; ++i) {
			++next_id;
			builder.push_back(next_id - count, next_id, node{ next_id });
		}
		for (unsigned i = 0; i < count; ++i) {
			++next_id;
			builder.push_back(next_id - count, next_id, node{ next_id });
		}
		for (unsigned i = 0; i < count; ++i) {
			++next_id;
			builder.push_back(next_id - count * 3u, next_id, node{ next_id });
		}
		for (unsigned i = 0; i < count; ++i) {
			++next_id;
			builder.push_back(next_id - count * 4u, next_id, node{ next_id });
		}
		for (unsigned i = 0; i < count; ++i) {
			++next_id;
			builder.push_back(next_id - count * 2u, next_id, node{ next_id });
		}
		for (unsigned i = 0; i < count; ++i) {
			++next_id;
			builder.push_back(next_id, node{ next_id });
		}
	}

	// Now create a flat constant graph using builder.
	// After this point, you may not retopologize the graph,
	// unless you recreate it with a new builder.
	size_t builder_size = builder.size();
	fea::flat_bf_graph graph{ std::move(builder) };
	EXPECT_FALSE(graph.empty());
	EXPECT_EQ(graph.size(), builder_size);
	EXPECT_EQ(graph.breadth_size(), 5u);
	EXPECT_GE(graph.key_capacity(), graph.capacity());

	// Check topology and expected values.
	{
		std::span<const std::span<const id_t>> children = graph.children();
		std::span<const id_t> keys = graph.keys();
		size_t midx = 0;
		for (id_t k : keys) {
			// Check basics.
			EXPECT_TRUE(graph.contains(k));
			EXPECT_EQ(graph.at(k).id, k);
			EXPECT_EQ(graph.at(k).id, graph.at_unchecked(k).id);
			EXPECT_EQ(graph[midx].id, graph.at(k).id);
			++midx;

			id_t parent = graph.parent(k);
			if (parent == graph.root_key()) {
				continue;
			}

			size_t parent_idx = graph.index(parent);

			// Check children spans are well formed.
			std::span<const id_t> parent_children = graph.children(parent);
			EXPECT_NE(std::find(parent_children.begin(), parent_children.end(),
							  k),
					parent_children.end());

			EXPECT_EQ(parent_children.data(), children[parent_idx].data());
		}

		// Breadths
		std::span<const std::span<const id_t>> breadths = graph.breadths();
		for (size_t i = 0; i < graph.breadth_size(); ++i) {
			std::span<const id_t> breadth = graph.breadth(i);
			EXPECT_EQ(breadth.data(), breadths[i].data());

			for (id_t k : breadth) {
				EXPECT_TRUE(graph.contains(k));

				if (i == 0) {
					EXPECT_EQ(graph.parent(k), graph.root_key());
					EXPECT_TRUE(graph.is_root(k));
				} else {
					EXPECT_NE(graph.parent(k), graph.root_key());
					EXPECT_FALSE(graph.is_root(k));
				}
			}
		}

		// Iterators.
		std::span<const node> values = graph.values();
		midx = 0;
		for (const node& n : graph) {
			EXPECT_EQ(n.id, values[midx].id);
			++midx;
		}

		keys = graph.keys();
		midx = 0;
		for (auto it = graph.key_begin(); it != graph.key_end(); ++it) {
			EXPECT_EQ(*it, keys[midx]);
			++midx;
		}
	}
}

} // namespace

#include "global.hpp"

#include <algorithm>
#include <array>
#include <gtest/gtest.h>
#include <numeric>
#include <vector>

namespace {
struct octree_node {
	size_t parent_id = std::numeric_limits<size_t>::max();
	size_t id = std::numeric_limits<size_t>::max();

	std::array<size_t, 8> children{
		std::numeric_limits<size_t>::max(),
		std::numeric_limits<size_t>::max(),
		std::numeric_limits<size_t>::max(),
		std::numeric_limits<size_t>::max(),
		std::numeric_limits<size_t>::max(),
		std::numeric_limits<size_t>::max(),
		std::numeric_limits<size_t>::max(),
		std::numeric_limits<size_t>::max(),
	};

	using citer = typename std::array<size_t, 8>::const_iterator;
	using iter = typename std::array<size_t, 8>::iterator;
};
} // namespace

namespace fea {
template <>
std::pair<octree_node::iter, octree_node::iter> children_range(
		octree_node::iter parent, std::vector<octree_node>* tree) {

	size_t parent_idx = *parent;
	if (parent_idx == std::numeric_limits<size_t>::max()) {
		return { {}, {} };
	}

	octree_node& n = (*tree)[parent_idx];
	return { n.children.begin(), n.children.end() };
}

template <>
std::pair<octree_node::citer, octree_node::citer> children_range(
		octree_node::citer parent, const std::vector<octree_node>* tree) {

	size_t parent_idx = *parent;
	if (parent_idx == std::numeric_limits<size_t>::max()) {
		return { {}, {} };
	}

	const octree_node& n = (*tree)[parent_idx];
	return { n.children.begin(), n.children.end() };
}
} // namespace fea

namespace {
// This scenario tests nodes which are pure represantations of a graph, without
// containing data.
// The graph isn't stored inside the nodes, but as a seperate container.
// AKA, nodes are "dumb".
TEST(flat_recurse, octree) {
	std::vector<int> data(1000);
	std::iota(data.begin(), data.end(), 0);

	std::vector<size_t> available_ids(data.size());
	std::iota(available_ids.rbegin(), available_ids.rend(), 0);

	// Create an "external" graph that refers to data.
	std::vector<octree_node> tree;

	octree_node root;
	root.id = available_ids.back();
	available_ids.pop_back();
	tree.push_back(root);

	for (size_t i = 0; i < data.size(); ++i) {
		if (available_ids.empty()) {
			break;
		}

		for (size_t j = 0; j < 8; ++j) {
			if (available_ids.empty()) {
				break;
			}

			octree_node& parent = tree[i];
			octree_node child;
			child.parent_id = parent.id;
			child.id = available_ids.back();
			available_ids.pop_back();

			parent.children[j] = child.id;
			tree.push_back(child);
		}
	}

	const std::vector<octree_node>* const_tree_ptr = &tree;

	std::array<size_t, 8> root_arr; // match iterator type
	root_arr[0] = 0;
	octree_node::iter root_it = root_arr.begin();
	octree_node::citer const_root_it = root_arr.begin();

	SCOPED_TRACE("octree test breadth");
	test_breadth(root_it, &tree);
	test_breadth(const_root_it, const_tree_ptr);

	SCOPED_TRACE("octree test depth");
	test_depth(root_it, &tree);
	test_depth(const_root_it, const_tree_ptr);

	// cull invalid
	{
		auto cull_pred = [&](auto idx_it) {
			size_t idx = *idx_it;
			return idx == std::numeric_limits<size_t>::max();
		};

		auto parent_cull_pred = [&](auto idx_it) {
			size_t idx = *idx_it;
			if (idx == std::numeric_limits<size_t>::max()) {
				return true;
			}

			auto& n = tree[idx];
			size_t parent_idx = n.parent_id;
			if (parent_idx == std::numeric_limits<size_t>::max()) {
				return cull_pred(idx_it);
			}

			return false;
		};

		SCOPED_TRACE("octree test cull disabled");
		test_culling(root_it, cull_pred, parent_cull_pred, &tree);
		test_culling(
				const_root_it, cull_pred, parent_cull_pred, const_tree_ptr);
	}

	// cull % 2 and invalid
	{
		auto cull_pred = [&](auto idx_it) {
			size_t idx = *idx_it;
			if (idx == std::numeric_limits<size_t>::max()) {
				return true;
			}

			return (idx % 2) == 0;
		};

		auto parent_cull_pred = [&](auto idx_it) {
			size_t idx = *idx_it;
			if (idx == std::numeric_limits<size_t>::max()) {
				return true;
			}

			auto& n = tree[idx];
			size_t parent_idx = n.parent_id;
			if (parent_idx == std::numeric_limits<size_t>::max()) {
				return cull_pred(idx_it);
			}

			return (parent_idx % 2) == 0;
		};

		SCOPED_TRACE("octree test cull disabled");
		test_culling(root_it, cull_pred, parent_cull_pred, &tree);
		test_culling(
				const_root_it, cull_pred, parent_cull_pred, const_tree_ptr);
	}
}
} // namespace

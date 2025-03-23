#include "global.hpp"

#include <fea/utils/platform.hpp>
#include <gtest/gtest.h>
#include <list>
#include <unordered_map>

namespace {
size_t disable_counter = 0;

struct list_node {
	using iter = typename std::list<list_node>::iterator;
	using citer = typename std::list<list_node>::const_iterator;

	list_node(list_node* p)
			: parent(p) {
	}

	void create_graph(const size_t max_depth, const size_t num_children,
			const size_t depth = 0) {
		if (depth == max_depth - 1)
			return;

		++disable_counter;
		disabled = disable_counter % 6 == 0;

		children.resize(num_children, { this });

		for (size_t i = 0; i < num_children; ++i) {
			children.push_back({ this });
			children.back().create_graph(max_depth, num_children, depth + 1);
		}
	}

	citer begin() const {
		return children.begin();
	}
	iter begin() {
		return children.begin();
	}

	citer end() const {
		return children.end();
	}
	iter end() {
		return children.end();
	}

	bool operator==(const list_node& other) const {
		return this == &other;
	}

	std::list<list_node> children;
	list_node* parent = nullptr;
	bool disabled = false;
};
} // namespace

// gcc unordered_map implementation doesn't work here.
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53339
#if !FEA_GCC
namespace {
size_t id_counter = 0;
struct umap_node {
	using iter = typename std::unordered_map<size_t, umap_node>::iterator;
	using citer =
			typename std::unordered_map<size_t, umap_node>::const_iterator;

	umap_node() = default;
	umap_node(umap_node* p)
			: parent(p)
			, id(id_counter++) {
	}

	void create_graph(const size_t max_depth, const size_t num_children,
			const size_t depth = 0) {
		if (depth == max_depth - 1)
			return;

		++disable_counter;
		disabled = disable_counter % 6 == 0;

		for (size_t i = 0; i < num_children; ++i) {
			umap_node child{ this };
			children[child.id] = child;
			children[child.id].create_graph(max_depth, num_children, depth + 1);
		}
	}

	citer begin() const {
		return children.begin();
	}
	iter begin() {
		return children.begin();
	}

	citer end() const {
		return children.end();
	}
	iter end() {
		return children.end();
	}

	bool operator==(const umap_node& other) const {
		return this == &other;
	}

	std::unordered_map<size_t, umap_node> children;
	umap_node* parent = nullptr;
	size_t id = 0;
	bool disabled = false;
};
} // namespace

namespace fea {
template <>
inline std::pair<umap_node::iter, umap_node::iter> children_range(
		umap_node::iter parent, const void*) {
	return { parent->second.children.begin(), parent->second.children.end() };
}
} // namespace fea
#endif

namespace {
TEST(flat_recurse, list_iters) {
	std::list<list_node> root_vec;
	root_vec.push_back({ nullptr });
	root_vec.back().create_graph(6, 8);

	auto root_it = root_vec.begin();

	SCOPED_TRACE("small_obj bidir_it test breadth");
	test_breadth(root_it);

	SCOPED_TRACE("small_obj bidir_it test depth");
	test_depth(root_it);

	// cull disabled
	{
		auto cull_pred = [](auto node) { return node->disabled; };
		auto parent_cull_pred = [=](auto node) {
			if (node->parent == nullptr) {
				return cull_pred(node);
			}
			return cull_pred(node->parent);
		};

		root_vec.back().disabled = false;
		SCOPED_TRACE("small_obj test cull disabled");
		test_culling(root_it, cull_pred, parent_cull_pred);
	}

	// cull enabled
	{
		auto cull_pred = [](auto node) { return node->disabled == false; };
		auto parent_cull_pred = [=](auto node) {
			if (node->parent == nullptr) {
				return cull_pred(node);
			}
			return cull_pred(node->parent);
		};

		root_vec.back().disabled = true;
		SCOPED_TRACE("small_obj test cull enabled");
		test_culling(root_it, cull_pred, parent_cull_pred);
	}
}

#if !FEA_GCC
TEST(flat_recurse, umap_iters) {
	umap_node n{ nullptr };
	std::unordered_map<size_t, umap_node> root_vec;
	root_vec[n.id] = n;
	root_vec[n.id].create_graph(6, 8);

	auto root_it = root_vec.begin();

	SCOPED_TRACE("small_obj bidir_it test breadth");
	test_breadth(root_it);

	SCOPED_TRACE("small_obj bidir_it test depth");
	test_depth(root_it);

	// cull disabled
	{
		auto cull_pred
				= [](umap_node::iter node) { return node->second.disabled; };
		auto parent_cull_pred = [=](auto node) {
			if (node->second.parent == nullptr) {
				return cull_pred(node);
			}
			return node->second.parent->disabled;
		};

		root_vec[n.id].disabled = false;
		SCOPED_TRACE("small_obj test cull disabled");
		test_culling(root_it, cull_pred, parent_cull_pred);
	}

	// cull enabled
	{
		auto cull_pred
				= [](auto node) { return node->second.disabled == false; };
		auto parent_cull_pred = [=](auto node) {
			if (node->second.parent == nullptr) {
				return cull_pred(node);
			}
			return node->second.parent->disabled == false;
		};

		root_vec[n.id].disabled = true;
		SCOPED_TRACE("small_obj test cull enabled");
		test_culling(root_it, cull_pred, parent_cull_pred);
	}
}
#endif
} // namespace

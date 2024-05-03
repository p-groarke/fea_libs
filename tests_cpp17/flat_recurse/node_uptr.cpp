#include "node_uptr.hpp"
#include "global.hpp"

#include <fea/flat_recurse/flat_recurse.hpp>
#include <gtest/gtest.h>

namespace fea {
template <>
std::pair<std::unique_ptr<node_uptr>*, std::unique_ptr<node_uptr>*>
children_range(std::unique_ptr<node_uptr>* parent, const void*) {

	if (parent->get()->children().empty()) {
		return { nullptr, nullptr };
	}

	std::unique_ptr<node_uptr>* beg = &parent->get()->children().front();
	return { beg, beg + parent->get()->children().size() };
}
template <>
std::pair<const std::unique_ptr<node_uptr>*, const std::unique_ptr<node_uptr>*>
children_range(const std::unique_ptr<node_uptr>* parent, const void*) {

	if (parent->get()->children().empty()) {
		return { nullptr, nullptr };
	}

	const std::unique_ptr<node_uptr>* beg = &parent->get()->children().front();
	return { beg, beg + parent->get()->children().size() };
}
} // namespace fea


namespace {
size_t id_counter = 0;
size_t disable_counter = 0;
} // namespace

node_uptr::node_uptr(node_uptr* parent)
		: _id(id_counter++)
		, _parent(parent) {
	// data.fill(0);
}

void node_uptr::create_graph(const size_t max_depth, const size_t num_children,
		const size_t depth /*= 0*/) {
	if (depth == max_depth - 1)
		return;

	++disable_counter;
	_disabled = disable_counter % 6 == 0;

	for (size_t i = 0; i < num_children; ++i) {
		_children.push_back(std::make_unique<node_uptr>(this));

		_children.back()->create_graph(max_depth, num_children, depth + 1);
	}
}

const std::vector<std::unique_ptr<node_uptr>>& node_uptr::children() const {
	return _children;
}

std::vector<std::unique_ptr<node_uptr>>& node_uptr::children() {
	return _children;
}

bool node_uptr::disabled() const {
	return _disabled;
}

void node_uptr::disabled(bool disabled) {
	_disabled = disabled;
}

const node_uptr* node_uptr::parent() const {
	return _parent;
}

node_uptr* node_uptr::parent() {
	return _parent;
}

bool node_uptr::operator==(const node_uptr& other) const {
	return _id == other._id;
}


namespace {
TEST(flat_recurse, node_uptr_deeper) {
	std::unique_ptr<node_uptr> root = std::make_unique<node_uptr>(nullptr);
	root->create_graph(8, 6);

	SCOPED_TRACE("node_uptr test breadth");
	test_breadth(&root);

	SCOPED_TRACE("node_uptr test depth");
	test_depth(&root);

	// cull disabled
	{
		auto cull_pred = [](auto node) { return node->get()->disabled(); };
		auto parent_cull_pred = [=](auto node) {
			if (node->get()->parent() == nullptr) {
				return cull_pred(node);
			}
			return node->get()->parent()->disabled();
		};

		root->disabled(false);
		SCOPED_TRACE("node_uptr test cull disabled");
		test_culling(&root, cull_pred, parent_cull_pred);
	}

	// cull enabled
	{
		auto cull_pred
				= [](auto node) { return node->get()->disabled() == false; };
		auto parent_cull_pred = [=](auto node) {
			if (node->get()->parent() == nullptr) {
				return cull_pred(node);
			}
			return node->get()->parent()->disabled() == false;
		};

		root->disabled(true);
		SCOPED_TRACE("node_uptr test cull enabled");
		test_culling(&root, cull_pred, parent_cull_pred);
	}
}

TEST(flat_recurse, node_uptr_wider) {
	std::unique_ptr<node_uptr> root = std::make_unique<node_uptr>(nullptr);
	root->create_graph(2, 50);

	SCOPED_TRACE("node_uptr test breadth");
	test_breadth(&root);

	SCOPED_TRACE("node_uptr test depth");
	test_depth(&root);

	// cull disabled
	{
		auto cull_pred = [](auto node) { return node->get()->disabled(); };
		auto parent_cull_pred = [=](auto node) {
			if (node->get()->parent() == nullptr) {
				return cull_pred(node);
			}
			return node->get()->parent()->disabled();
		};

		root->disabled(false);
		SCOPED_TRACE("node_uptr test cull disabled");
		test_culling(&root, cull_pred, parent_cull_pred);
	}

	// cull enabled
	{
		auto cull_pred
				= [](auto node) { return node->get()->disabled() == false; };
		auto parent_cull_pred = [=](auto node) {
			if (node->get()->parent() == nullptr) {
				return cull_pred(node);
			}
			return node->get()->parent()->disabled() == false;
		};

		root->disabled(true);
		SCOPED_TRACE("node_uptr test cull enabled");
		test_culling(&root, cull_pred, parent_cull_pred);
	}
}

} // namespace

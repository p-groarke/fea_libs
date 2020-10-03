/*
BSD 3-Clause License

Copyright (c) 2020, Philippe Groarke
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#pragma once
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <tbb/task_group.h>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace fea {
// TODO : callback should be 1 vector of pair<parent_id, bool_was_dirty>

template <class Id, class NodeData, class DirtyVersion>
struct node {
	/**
	 * Graph Functions
	 */

	bool is_root() const {
		return _parents.empty();
	}

	bool has_children() const {
		return !_children.empty();
	}

	bool has_child(Id child_id) const {
		auto it = std::find(_children.begin(), _children.end(), child_id);
		return it != _children.end();
	}

	void add_child(Id child_id) {
		_children.push_back(child_id);
	}

	void remove_child(Id child_id) {
		auto it = std::find(_children.begin(), _children.end(), child_id);
		if (it == _children.end()) {
			return;
		}

		if (it != std::prev(_children.end())) {
			*it = _children.back();
		}

		_children.pop_back();
	}

	bool has_parent(Id parent_id) const {
		return _parents.count(parent_id) != 0;
	}

	void add_parent(Id parent_id) {
		_parents.insert({ parent_id, dirty_sentinel() });
		_dirty_evaluation_graph = true;
	}

	void remove_parent(Id parent_id) {
		_parents.erase(parent_id);
		_dirty_evaluation_graph = true;
	}

	const std::vector<Id>& children() const {
		return _children;
	}

	const std::unordered_map<Id, DirtyVersion>& parents() const {
		return _parents;
	}
	// Only use this to change dirty version, not add or remove parents.
	std::unordered_map<Id, DirtyVersion>& parents() {
		return _parents;
	}

	bool is_evaluation_graph_dirty() const {
		return _dirty_evaluation_graph;
	}
	void clean_evaluation_graph() {
		_dirty_evaluation_graph = false;
	}

	// A left to right graph of parents needed to update this node.
	const std::vector<Id>& evaluation_graph() const {
		if (_dirty_evaluation_graph) {
			throw std::runtime_error{
				__FUNCTION__ " : reading dirty evaluation graph"
			};
		}
		return _evaluation_graph;
	}
	std::vector<Id>& evaluation_graph() {
		return _evaluation_graph;
	}

	const NodeData& node_data() const {
		return _node_data;
	}
	NodeData& node_data() {
		return _node_data;
	}

	/**
	 * Dirtyness Functions
	 */

	DirtyVersion version() const {
		return _dirty_version;
	}
	DirtyVersion& version() {
		return _dirty_version;
	}

	DirtyVersion parent_version(Id parent_id) const {
		return _parents.at(parent_id);
	}
	DirtyVersion& parent_version(Id parent_id) {
		return _parents.at(parent_id);
	}


	// The graph uses a dirty versioning system. This means you can check if you
	// are dirty in O(1) by comparing your last updated parent version to the
	// current parent version.
	// Some values are set in stone to guarantee certain states. For ex, when
	// adding a child you are guarenteed dirty compared to your parent, aka
	// dirty sentinel.
	// When a version reaches the DirtyVersion max value, we reset the version
	// to init_sentinel and recurse through children to set their version to
	// dirty_sentinel.
	static constexpr DirtyVersion dirty_sentinel() {
		return 0;
	}
	static constexpr DirtyVersion clean_sentinel() {
		return 1;
	}
	static constexpr DirtyVersion init_sentinel() {
		return 2;
	}

private:
	// Your parents.
	// Stored in unordered_map because we do random lookups very often vs.
	// adding or looping.
	// The value is the version of your parent when you were last updated.
	// Used to check if you are dirty (my version != current parent version).

	// TODO : investigate storing the versions inside the parent, to minimize
	// map lookups.
	std::unordered_map<Id, DirtyVersion> _parents;

	// Your children.
	std::vector<Id> _children;

	// This is an optimization, we tradeoff memory and insert time for faster
	// clean times.
	// This is a sorted parent to child graph to evaluate this node (clean
	// it).
	// It should never contain duplicates and iterating left to right should
	// evaluate the parent graph correctly.
	std::vector<Id> _evaluation_graph;

	// My version. ++ when updating this node.
	DirtyVersion _dirty_version = init_sentinel();

	// This is used to lazily compute the eval graph.
	// We need this because removing a node or a subtree causes insane recursion
	// if we don't do this.
	bool _dirty_evaluation_graph = true;

	NodeData _node_data;
};


// Id is user provided and must be insertable in an unordered_map.
// It should be as small as possible.
// NodeData is optional. It is extra data you want each node to have.
// DirtyVersion is optional. It is the unsigned used to compare dirtyness (1 per
// node).
// UnorderedContainer is optional. You can provide your own std::unordered_map
// compliant container.
template <class Id, class NodeData = char, class DirtyVersion = uint64_t,
		template <class...> class UnorderedContainer = std::unordered_map>
struct lazy_graph {
	static_assert(std::is_unsigned<DirtyVersion>::value,
			"fea::lazy_graph : DirtyVersion must be an unsigned integral");

	static_assert(std::is_default_constructible<NodeData>::value,
			"fea::lazy_graph : NodeData must be default constructible.");

	// The storage container will take care of not compiling if the data is not
	// copyable/moveable.

	/**
	 * Graph Functions
	 */

	// Get your user data stored inside a node.
	const NodeData& node_data(Id id) const {
		return _nodes.at(id).node_data();
	}
	NodeData& node_data(Id id) {
		return _nodes.at(id).node_data();
	}

	// Is the node a root (has no parents)?
	bool is_root(Id id) const {
		return _nodes.at(id).is_root();
	}

	// Does the node have this child?
	bool has_child(Id child_id, Id parent_id) const {
		return _nodes.at(parent_id).has_child(child_id);
	}
	// Does the node have children?
	bool has_children(Id id) const {
		return _nodes.at(id).has_children();
	}

	// Does the node have this parent?
	bool has_parent(Id child_id, Id parent_id) const {
		// TODO : deal with children that don't exist or continue throwing?
		return _nodes.at(child_id).has_parent(parent_id);
	}
	// Does the node have children?
	bool has_parents(Id id) const {
		return !is_root(id);
	}


	// This is called for you in add_dependency, but you can still check
	// yourself if you prefer.
	// Checks if nodes are the same, if child is already set, if the child would
	// create a loop.
	bool is_invalid_child(Id child_id, Id parent_id) const {
		if (parent_id == child_id) {
			return true;
		}

		// If the nodes don't exist, then there is no loop or problem. So it is
		// valid.
		// AKA, if parent doesn't exist, it will be a root with no child.
		// If child doesn't exist, parent doesn't already have child and there
		// is no possibility for a loop.
		auto parent_it = _nodes.find(parent_id);
		auto child_it = _nodes.find(child_id);
		if (parent_it == _nodes.end() || child_it == _nodes.end()) {
			return false;
		}

		// Already has dependency?
		if (parent_it->second.has_child(child_id)) {
			assert(child_it->second.has_parent(parent_id));
			return true;
		}

		// TODO : Benchmark threaded recursion.
		return recurse_up(parent_id,
				[&](Id, const node_t& n) { return n.has_parent(child_id); });
	}

	// Adds a root node with no dependency.
	// noop if it already exists.
	void add_node(Id id) {
		_nodes.insert({ id, {} });
	}

	// Removes a given node from the graph.
	// Its children are orphaned.
	void remove_node(Id id) {
		auto it = _nodes.find(id);
		if (it == _nodes.end()) {
			return;
		}

		node_t& n = it->second;

		const std::unordered_map<Id, DirtyVersion>& parents = n.parents();
		for (const std::pair<const Id, DirtyVersion>& parent : parents) {
			_nodes.at(parent.first).remove_child(id);
		}

		const std::vector<Id>& children = n.children();
		for (Id child_id : children) {
			_nodes.at(child_id).remove_parent(id);
		}

		_nodes.erase(it);
	}

	// Removes a node and its subgraph.
	// The children are removed if and ONLY IF, the children would be
	// orphaned.
	// In other words, removes a node and its children hierarchy if those
	// children don't have other parents.
	void remove_subgraph(Id node_id) {
		if (_nodes.count(node_id) == 0) {
			return;
		}

		// Collect candidate nodes to remove.
		// The end result is a breadth ordered vector, front to back.
		std::vector<Id> subgraph;

		recurse_breadth_down(node_id, [&](Id id, const node_t&) {
			subgraph.push_back(id);
			return false; // recurse whole graph
		});

		// We must force remove the top node manually. This is because it could
		// have other parents, but we don't care about that.
		remove_node(subgraph.front());
		subgraph.erase(subgraph.begin());

		// Now, go through the subgraph and remove nodes that would become
		// leaves. We keep the duplicates, as a node could have multiple parents
		// that end up being removed.
		for (Id parent_id : subgraph) {
			if (_nodes.count(parent_id) == 0) {
				continue;
			}

			if (!has_parents(parent_id)) {
				remove_node(parent_id);
			}
		}
	}

	// Creates a dependency between 2 nodes.
	// If any node doesn't exist, creates it.
	// Returns true if the dependency was created.
	// Causes for failure :
	// - Adding the dependency would create a loop.
	// - Dependency already exists.
	bool add_dependency(Id child_id, Id parent_id) {
		// Would this dependency create a loop?
		// Call this before creating the nodes, as it is a shortcut.
		if (is_invalid_child(child_id, parent_id)) {
			return false;
		}

		// Inserts if not already in.
		// Invalidates iterators.
		{
			node_t& child = _nodes.insert({ child_id, {} }).first->second;
			child.add_parent(parent_id);
		}
		{
			node_t& parent = _nodes.insert({ parent_id, {} }).first->second;
			parent.add_child(child_id);
		}

		return true;
	}

	// Removes a dependency relationship.
	// Leaves nodes in place.
	void remove_dependency(Id child_id, Id parent_id) {
		node_t& child = _nodes.at(child_id);
		if (!child.has_parent(parent_id)) {
			assert(!_nodes.at(parent_id).has_child(child_id));
			return;
		}

		node_t& parent = _nodes.at(parent_id);

		child.remove_parent(parent_id);
		parent.remove_child(child_id);
	}

	// Does the graph contain this node?
	bool contains(Id id) const {
		return _nodes.count(id) != 0;
	}

	// Is the graph empty?
	bool empty() const {
		return _nodes.empty();
	}

	void clear() {
		_nodes.clear();
	}

	// Get a nodes children
	const std::vector<Id>& children(Id id) const {
		return _nodes.at(id).children();
	}

	// Get a nodes parent.
	// Note this is in a map with dirtyness.
	const std::unordered_map<Id, DirtyVersion>& parents(Id id) const {
		return _nodes.at(id).parents();
	}


	/**
	 * Dirtyness Functions
	 */

	// Mark a node as written to. AKA deal with the dirtyness.
	void make_dirty(Id id) {
		node_t& n = _nodes.at(id);

		// We've reached the end of the version space.
		// Reset version to init_sentinel and set all children to
		// dirty_sentinel. Happens rarely.
		if (n.version() == (std::numeric_limits<DirtyVersion>::max)()) {
			n.version() = node_t::init_sentinel();

			const std::vector<Id>& children = n.children();
			for (Id child_id : children) {
				_nodes.at(child_id).parent_version(id)
						= node_t::dirty_sentinel();
			}

			return;
		}

		++n.version();
	}

	// Can you read a node? Does a node need an update?
	bool is_dirty(Id id) const {
		return recurse_up(id, [&, this](Id, const node_t& n) {
			const std::unordered_map<Id, DirtyVersion>& parents = n.parents();
			for (const std::pair<Id, DirtyVersion>& parent : parents) {
				if (parent.second != _nodes.at(parent.first).version()) {
					return true;
				}
			}
			return false;
		});
	}

	// Update a node.
	// Your function should clean the provided node.
	// Your lambda will be called recursively from parent to child.
	// A node that depends on nothing is always valid. Your function will not be
	// called on valid nodes.
	// This call is heavy, so the overhead of std::function is minimized.
	void clean(Id id,
			const std::function<void(Id /* node_id */,
					const std::vector<Id>& /* node_parents */,
					const std::vector<Id>& /* dirty_parents */)>& func) {
		if (_nodes.at(id).is_root()) {
			return;
		}

		// Get back to front node subgraph.
		const std::vector<Id>& graph = evaluation_graph(id);

		// Stored here to reuse memory.
		std::vector<Id> callback_parents;
		std::vector<Id> callback_dirty_parents;

		// Now that we have the correct evaluation graph, evaluate it.
		// Call the user funcion with the current node id and provide it's
		// parent ids.
		// Since we evaluate from top to bottom here, we can check if
		// things are clean in a faster way (still relatively slow though).
		// All we need to check is our parents version.
		for (size_t i = 0; i < graph.size(); ++i) {
			Id nid = graph[i];
			node_t& n = _nodes.at(nid);

			callback_parents.clear();
			callback_dirty_parents.clear();

			// Nodes that don't depend on anything never need to be updated.
			if (n.is_root()) {
				continue;
			}

			// Check all my parents to see if I am really dirty.
			// Also reset my version while we are looping.
			bool dirty = false;
			std::unordered_map<Id, DirtyVersion>& parents = n.parents();

			// eh, not great. better way to pass on parents to user funcion?
			for (std::pair<const Id, DirtyVersion>& parent : parents) {
				callback_parents.push_back(parent.first);

				DirtyVersion parent_version = _nodes.at(parent.first).version();
				if (parent.second != parent_version) {
					callback_dirty_parents.push_back(parent.first);
					parent.second = parent_version;
					dirty = true;

					// Can't break here, all children must be cleaned.
				}
			}

			if (!dirty) {
				continue;
			}

			// And finally, call the user lambda. Provide it with its id and
			// parent ids.
			// todo : pass parent ids
			func(nid, callback_parents, callback_dirty_parents);
			make_dirty(nid);
		}
	}

	// Update multiple nodes.
	// Your function should clean the provided node.
	// Your lambda will be called recursively from parent to child.
	// A node that depends on nothing is always valid. Your function will not be
	// called on valid nodes.
	// This call is heavy, so the overhead of std::function is minimized.
	void clean(const std::vector<Id>& ids,
			const std::function<void(Id /* node_id */,
					const std::vector<Id>& /* node_parents */,
					const std::vector<Id>& /* dirty_parents */)>& func) {

		for (Id id : ids) {
			clean(id, func);
		}
	}

	// Update a node, thread breadths that are threadable.
	// Your function should clean the provided node.
	// Your lambda will be called recursively from parent to child an seperate
	// threads.
	// A node that depends on nothing is always valid. Your function
	// will not be called on valid nodes.
	// It is important to only read your parents, and only write to yourself
	// during this evaluation!
	void clean_mt(Id id,
			const std::function<void(Id /* node_id */,
					const std::vector<Id>& /* node_parents */,
					const std::vector<Id>& /* dirty_parents */)>& func) {

		if (_nodes.at(id).is_root()) {
			return;
		}

		// Get back to front node subgraph.
		const std::vector<Id>& graph = evaluation_graph(id);


		// We will keep a vector of currently evaluating nodes.
		// If a child node depends on something that is evaluating, we lock
		// since we've reached a new breadth of the graph.
		std::vector<Id> evaluating;
		tbb::task_group g;

		// Now that we have the correct evaluation graph, evaluate it.
		// Call the user funcion with the current node id.
		// Since we evaluate from top to bottom here, we can check if
		// things are clean in a faster way (still relatively slow though).
		// All we need to check is our parents version.
		for (size_t i = 0; i < graph.size(); ++i) {
			Id nid = graph[i];
			node_t& n = _nodes.at(nid);

			// Nodes that don't depend on anything never need to be updated.
			if (n.is_root()) {
				continue;
			}

			// Here, we lock if any of our parent is being evaluated.
			// This would be both problematic for data access and dirty
			// checking.
			for (size_t j = 0; j < evaluating.size(); ++j) {
				if (n.has_parent(evaluating[j])) {
					g.wait();
					evaluating.clear();
					break;
				}
			}

			// We can check the dirtyness with parents on the main thread, since
			// we guarantee you never execute at the same time as your parents.

			// Check all my parents to see if I am really dirty.
			// Also reset my version while we are looping.
			bool dirty = false;
			std::unordered_map<Id, DirtyVersion>& parents = n.parents();

			std::vector<Id> callback_parents;
			callback_parents.reserve(parents.size());
			std::vector<Id> callback_dirty_parents;

			// eh, not great. better way to pass on parents to user funcion?
			for (std::pair<const Id, DirtyVersion>& parent : parents) {
				callback_parents.push_back(parent.first);

				DirtyVersion parent_version = _nodes.at(parent.first).version();
				if (parent.second != parent_version) {
					callback_dirty_parents.push_back(parent.first);
					parent.second = parent_version;
					dirty = true;

					// Can't break here, all versions must be cleaned.
				}
			}

			if (!dirty) {
				continue;
			}

			evaluating.push_back(nid);
			g.run([p = std::move(callback_parents),
						  dp = std::move(callback_dirty_parents), nid, func,
						  this]() {
				// And finally, call the user lambda. Provide it with its id.
				func(nid, p, dp);
				make_dirty(nid);
			});
		}

		g.wait();
	}

	// Update multiple node, thread as much as possible.
	// Thread independent evaluation graphs and each's breadths (when
	// applicable).
	// Your function should clean the provided node.
	// Your lambda will be called recursively from parent to child an seperate
	// threads.
	// A node that depends on nothing is always valid. Your function
	// will not be called on valid nodes.
	// It is important to only read your parents, and only write to yourself
	// during this evaluation!
	void clean_mt(const std::vector<Id>& ids,
			const std::function<void(Id /* node_id */,
					const std::vector<Id>& /* node_parents */,
					const std::vector<Id>& /* dirty_parents */)>& func) {
		// Figure out which graphs can run completely in parallel and which
		// can't.
		auto ind_data = are_eval_graphs_independent(ids);

		// 'ind_data.independent_graphs' can be updated in parallel.
		tbb::task_group g;
		for (Id id : ind_data.independent_graphs) {
			g.run([&, id, this]() { clean_mt(id, func); });
		}

		// 'ind_data.dependent_graphs' cannot be cleaned in parallel.
		// But they are still independent from 'ind_data.independent_graphs'.
		g.run([&, this]() {
			for (Id id : ind_data.dependent_graphs) {
				// Clean one at a time. But you can still call clean_mt at
				// least.
				clean_mt(id, func);
			}
		});

		g.wait();
	}


	// Data representing independance information for evaluation graphs.
	struct independance_data {
		// These node graphs are independent from one-another.
		std::vector<Id> independent_graphs;
		// These node graphs are NOT independent.
		std::vector<Id> dependent_graphs;
	};

	// Use this call to figure out if multiple evaluation graphs are
	// independent.
	// An eval graph is independent if none if its children share a
	// parent with the other graph, and none of its parents share a child with
	// the other graph. This is usefull when threading cleaning, as you can
	// launch multiple threads to clean different subgraphs at the same time
	// without locks.
	// This function is not const as it will compute the evaluation graphs if
	// needed.
	independance_data are_eval_graphs_independent(
			const std::vector<Id>& nodes) {
		if (nodes.size() < 2) {
			return { nodes, {} };
		}

		independance_data ret;
		std::vector<const std::vector<Id>*> eval_graphs(nodes.size());

		// TODO : thread this?
		for (size_t i = 0; i < eval_graphs.size(); ++i) {
			eval_graphs[i] = &evaluation_graph(nodes[i]);
		}

		// Check which channels are independent.
		// That is, if all channels of a graph aren't found in other graphs,
		// it is independent and that whole graph can be run in parrallel to
		// others.
		// This works because each eval graph only has an id once (no
		// duplicates). So if the node count is more than 1, another graph
		// refers to that node. aka, not independent.

		std::unordered_map<Id, size_t> node_counter;

		for (size_t i = 0; i < eval_graphs.size(); ++i) {
			for (Id id : *eval_graphs[i]) {
				++node_counter[id];
			}
		}

		for (size_t i = 0; i < eval_graphs.size(); ++i) {
			bool found = false;

			for (Id id : *eval_graphs[i]) {
				assert(node_counter[id] != 0);

				if (node_counter[id] != 1) {
					ret.dependent_graphs.push_back(nodes[i]);
					found = true;
					break;
				}
			}

			if (found) {
				continue;
			}

			ret.independent_graphs.push_back(nodes[i]);
		}

		return ret;
	}

	// This returns the flattened graph required to clean a node.
	// To visit the graph, loop left to right.
	// This function isn't const because it will recompute the eval graph if
	// needed.
	// You shouldn't need to call this yourself, but it is exposed for debugging
	// and testing purposes.
	const std::vector<Id>& evaluation_graph(Id node_id) {
		node_t& n = _nodes.at(node_id);
		if (!n.is_evaluation_graph_dirty()) {
			return n.evaluation_graph();
		}

		// Clear the eval graph, we will recompute it all.
		std::vector<Id>& eval_graph = n.evaluation_graph();
		eval_graph.clear();

		// Keep track of visited nodes in O(1).
		// Also stores thier index in the graph. It is necessary for quick
		// rotations.
		std::unordered_map<Id, size_t> visited;

		// Now go through parents and parents of parents.
		// If a parent was previously visited, it must be moved earlier in
		// the evaluation. It means that parent has a higher up dependency.
		// During the recursion, the graph is sorted back to front. We
		// reverse it at the end because it just makes things easier to
		// reason about.
		recurse_breadth_up(node_id, [&, this](Id id, const node_t&) {
			if (visited.count(id) == 0) {
				// Not previously visited, simply push back.
				visited.insert({ id, eval_graph.size() });
				eval_graph.push_back(id);
			} else {
				// A previous node also had this parent. So we must evaluate
				// it before the current node. Which means move the parent
				// to the end of the evaluation graph (remember back to
				// front).

				// will be at end
				auto beg_it = eval_graph.begin() + visited[id];
				// chunk to move before duplicate
				auto pivot_it = beg_it + 1;

				// update positions
				visited[id] = eval_graph.size() - 1; // now at end

				for (auto it = pivot_it; it != eval_graph.end(); ++it) {
					// channels are moved 1 position to the left.
					--visited[*it];
				}

				// move shared parent to the end, move everything else 1 pos
				// to the left
				std::rotate(beg_it, pivot_it, eval_graph.end());
			}

			return false; // go through whole graph
		});

		std::reverse(eval_graph.begin(), eval_graph.end());

#if !defined(NDEBUG)
		// Make sure there are no duplicate nodes (aka, duplicate messages).
		std::vector<Id> dup = eval_graph;
		std::sort(dup.begin(), dup.end());
		auto it = std::unique(dup.begin(), dup.end());
		assert(it == dup.end());
#endif

		n.clean_evaluation_graph();
		return n.evaluation_graph();
	}


private:
	using node_t = node<Id, NodeData, DirtyVersion>;


	// Recurse downward.
	// Your function should accept both an id and a node reference.
	// We pass the node on to minimize map lookups.
	// Your function should return true to stop recursion.
	template <class Func>
	bool recurse_down(Id id, Func func) const {
		const node_t& n = _nodes.at(id);
		if (func(id, n)) {
			return true;
		}

		const std::vector<Id>& children = n.children();
		for (Id child_id : children) {
			if (recurse_down(child_id, func)) {
				return true;
			}
		}

		return false;
	}
	template <class Func>
	bool recurse_down(Id id, Func func) {
		node_t& n = _nodes.at(id);
		if (func(id, n)) {
			return true;
		}

		const std::vector<Id>& children = n.children();
		for (Id child_id : children) {
			if (recurse_down(child_id, func)) {
				return true;
			}
		}

		return false;
	}

	template <class Func>
	bool recurse_breadth_down(Id id, Func func) const {
		// We must gather the children in a container graph.
		// We will iterate through this as long as there are new children,
		// or the user function returns true.
		std::vector<Id> graph;

		// Prime it.
		graph.push_back(id);

		for (size_t i = 0; i < graph.size(); ++i) {
			Id mid = graph[i];
			const node_t& n = _nodes.at(mid);

			if (func(mid, n)) {
				return true;
			}

			const std::vector<Id>& children = n.children();
			graph.insert(graph.end(), children.begin(), children.end());
		}

		return false;
	}

	// Recurse upward.
	// Your function should accept both an id and a node reference.
	// We pass the node on to minimize map lookups.
	// Your function should return true to stop recursion.
	template <class Func>
	bool recurse_up(Id id, Func func) const {
		const node_t& n = _nodes.at(id);
		if (func(id, n)) {
			return true;
		}

		const std::unordered_map<Id, DirtyVersion>& parents = n.parents();
		for (const std::pair<Id, DirtyVersion>& parent_pair : parents) {
			if (recurse_up(parent_pair.first, func)) {
				return true;
			}
		}

		return false;
	}

	template <class Func>
	bool recurse_breadth_up(Id id, Func func) const {
		// We need to gather a graph to recurse upwards.
		// This graph *will* contain duplicates, it is up to the user
		// function to cull them if desired.
		std::vector<Id> graph;

		// Prime it.
		graph.push_back(id);

		// As long as there are parents, push them back in the vector and
		// continue recursing upwards.
		for (size_t i = 0; i < graph.size(); ++i) {
			Id mid = graph[i];
			const node_t& n = _nodes.at(mid);

			if (func(mid, n)) {
				return true;
			}

			const std::unordered_map<Id, DirtyVersion>& parents = n.parents();
			for (const std::pair<Id, DirtyVersion>& parent_pair : parents) {
				graph.push_back(parent_pair.first);
			}
		}

		return false;
	}

	UnorderedContainer<Id, node_t> _nodes;
}; // namespace fea
} // namespace fea

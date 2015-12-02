#include "StdAfx.h"
#include "VertexVertexBoundaryPath.h"

VertexVertexBoundaryPath::VertexVertexBoundaryPath(VolumeMesh *_mesh, OvmVeH _sv, OvmVeH _ev, 
	std::unordered_set<OvmEgH> *_allowed_ehs, std::unordered_set<OvmEgH> *_forbidden_ehs)
	: mesh (_mesh), start_vh (_sv), end_vh (_ev), allowed_ehs (_allowed_ehs), forbidden_ehs (_forbidden_ehs)
{
	end_pt = mesh->vertex (end_vh);
	min_depth = 1;
}


VertexVertexBoundaryPath::~VertexVertexBoundaryPath(void)
{

}

void VertexVertexBoundaryPath::shortest_path (std::deque<OvmHaEgH> &path)
{
	auto n = create_first_node ();
	open_set.insert (n, start_vh);

	while (!open_set.empty ())
	{
		auto smallest_node = open_set.top ();
		open_set.pop (smallest_node->vh);
		close_set.insert (smallest_node, smallest_node->vh);

		//此处检查是否到达终点
		if (smallest_node->vh == end_vh){
			trace_back (path);
			return;
		}

		std::vector<VertexVertexBoundaryPathNode*> next_node_candidates;
		get_next_node_candidates (smallest_node, next_node_candidates);

		foreach (VertexVertexBoundaryPathNode *test_node, next_node_candidates){
			//检查to_vh是否已经在close set中, 如果已经在close set中，则不做任何处理
			if (close_set.exists (test_node->vh)){
				delete test_node;
				continue;
			}
			calc_F_value (smallest_node, test_node);
			//该节点还没有被放入open set，则将其放入
			if (!open_set.exists (test_node->vh)){
				open_set.insert (test_node, test_node->vh);
			}else{
				auto old_node = open_set.retrieve (test_node->vh);
				if (old_node->g > test_node->g){
					open_set.update (test_node, test_node->vh);
				}
			}
		}
	}//end while (!open_set.empty ())...
}

VertexVertexBoundaryPathNode *VertexVertexBoundaryPath::create_first_node ()
{
	auto n = new VertexVertexBoundaryPathNode (start_vh);
	calc_F_value (NULL, n);
	return n;
}

void VertexVertexBoundaryPath::shortest_path (std::vector<OvmHaEgH> &path)
{
	std::deque<OvmHaEgH> deque_path;
	shortest_path (deque_path);
	foreach (OvmHaEgH heh, deque_path)
		path.push_back (heh);
}

void VertexVertexBoundaryPath::get_next_node_candidates (VertexVertexBoundaryPathNode *current_node, 
	std::vector<VertexVertexBoundaryPathNode*> &next_node_candidates)
{
	for (auto voh_it = mesh->voh_iter (current_node->vh); voh_it; ++voh_it)
	{
		auto eh = mesh->edge_handle (*voh_it);
		if (allowed_ehs && !JC::contains (*allowed_ehs, eh)) continue;
		if (forbidden_ehs && JC::contains (*forbidden_ehs, eh)) continue;

		if (!mesh->is_boundary (eh)) continue;

		auto to_vh = mesh->halfedge (*voh_it).to_vertex ();
		auto new_node = new VertexVertexBoundaryPathNode (to_vh, current_node->vh);
		next_node_candidates.push_back (new_node);
	}
}

double VertexVertexBoundaryPath::calc_F_value (VertexVertexBoundaryPathNode *parent_node, VertexVertexBoundaryPathNode *current_node)
{
	double this_G = 1.0f;
	OvmVec3d cur_pt = mesh->vertex (current_node->vh);
	//该节点的G值为父节点的G值加上1
	if (parent_node){
		this_G += parent_node->g;
	}
	double penalty = 0.0f;
	if (parent_node && parent_node->parent_vh != mesh->InvalidVertexHandle)
	{
		auto heh1 = mesh->halfedge (parent_node->parent_vh, parent_node->vh),
			heh2 = mesh->halfedge (parent_node->vh, current_node->vh);
		penalty = calc_penalty (mesh->edge_handle (heh1), parent_node->vh, mesh->edge_handle (heh2));
	}
	this_G += penalty;
	double this_H = calc_H_value (current_node->vh, end_vh);
	double this_F = this_G + this_H;
	current_node->g = this_G; current_node->h = this_H; current_node->f = this_F;
	return this_F;
}

int VertexVertexBoundaryPath::calc_H_value (OvmVeH vh1, OvmVeH vh2)
{
	std::unordered_set<OvmVeH> visited_vhs;
	int dist = 0;
	visited_vhs.insert (vh1);
	std::unordered_set<OvmVeH> front_set;
	front_set.insert (vh1);
	forever{
		dist++;
		std::unordered_set<OvmVeH> new_front_set;
		foreach (auto &vh, front_set){
			for (auto voh_it = mesh->voh_iter (vh); voh_it; ++voh_it){
				auto eh = mesh->edge_handle (*voh_it);
				if (!mesh->is_boundary (eh)) continue;

				auto to_vh = mesh->halfedge (*voh_it).to_vertex ();
				if (JC::contains (visited_vhs, to_vh)) continue;
				new_front_set.insert (to_vh);
				visited_vhs.insert (to_vh);
			}
		}//end foreach (auto &vh, front_set){...
		if (JC::contains (new_front_set, vh2)) break;
		if (new_front_set.empty ()) {
			dist = -1;
			break;
		}
		front_set = new_front_set;
	}
	return dist;
}

double VertexVertexBoundaryPath::calc_penalty (OvmEgH eh1, OvmVeH vh, OvmEgH eh2)
{
	double penalty = 0.0f;
	std::unordered_set<OvmFaH> boundary_adj_fhs;
	JC::get_adj_boundary_faces_around_vertex (mesh, vh, boundary_adj_fhs);
	std::queue<OvmFaH> spread_set;
	spread_set.push (*(boundary_adj_fhs.begin ()));
	std::unordered_set<OvmFaH> one_side_adj_fhs;
	one_side_adj_fhs.insert (*(boundary_adj_fhs.begin ()));
	while (!spread_set.empty ()){
		OvmFaH front_fh = spread_set.front ();
		spread_set.pop ();

		std::unordered_set<OvmFaH> tmp_bound_adj_fhs;
		JC::get_adj_boundary_faces_around_face (mesh, front_fh, tmp_bound_adj_fhs);
		foreach (auto &adj_fh, tmp_bound_adj_fhs){
			if (!JC::contains (boundary_adj_fhs, adj_fh)) continue;
			if (JC::contains (one_side_adj_fhs, adj_fh)) continue;

			auto comm_eh = JC::get_common_edge_handle (mesh, front_fh, adj_fh);
			if (comm_eh == eh1 || comm_eh == eh2) continue;
			spread_set.push (adj_fh);
			one_side_adj_fhs.insert (adj_fh);
		}
	}

	assert (one_side_adj_fhs.size () != boundary_adj_fhs.size ());

	auto V_NODE_LEVEL = mesh->request_vertex_property<int> ("node level");

	auto div1 = one_side_adj_fhs.size () + 2 - 4;
	auto div2 = (boundary_adj_fhs.size () - one_side_adj_fhs.size ()) + 2 - 4;
	assert (V_NODE_LEVEL[vh] > 0);
	int level_penalty = 1;
	if (V_NODE_LEVEL[vh] < min_depth){
		level_penalty += min_depth - V_NODE_LEVEL[vh];
	}
	penalty = (div1 * div1 + div2 * div2) * (level_penalty * level_penalty * level_penalty);

	return penalty;
}

void VertexVertexBoundaryPath::trace_back (std::deque<OvmHaEgH> &path)
{
	OvmVeH trace_back_vh = end_vh;
	while (trace_back_vh != start_vh)
	{
		VertexVertexBoundaryPathNode * n = close_set.retrieve (trace_back_vh);
		assert (n);
		auto heh = mesh->halfedge (n->parent_vh, trace_back_vh);
		path.push_front (heh);
		trace_back_vh = n->parent_vh;
	}
}
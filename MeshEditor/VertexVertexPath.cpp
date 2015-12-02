#include "StdAfx.h"
#include "VertexVertexPath.h"


VertexVertexPath::VertexVertexPath(VolumeMesh *_mesh, OvmVeH _sv, OvmVeH _ev)
	: mesh (_mesh), start_vh (_sv), end_vh (_ev)
{
	end_pt = mesh->vertex (end_vh);
}


VertexVertexPath::~VertexVertexPath(void)
{
}

double VertexVertexPath::shortest_path (std::deque<OvmHaEgH> &path, int min_val /* = 0 */)
{
	min_valence = min_val;

	auto n = new VertexVertexPathNode (start_vh);
	calc_F_value (NULL, n);
	open_set.insert (n, start_vh);

	while (!open_set.empty ())
	{
		auto smallest_node = open_set.top ();
		open_set.pop (smallest_node->vh);
		close_set.insert (smallest_node, smallest_node->vh);

		//此处检查是否到达终点
		if (smallest_node->vh == end_vh)
			return trace_back (path);

		std::vector<VertexVertexPathNode*> next_node_candidates;
		get_next_node_candidates (smallest_node, next_node_candidates);

		foreach (VertexVertexPathNode *test_node, next_node_candidates){
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
	return -1;
}

double VertexVertexPath::shortest_path (std::vector<OvmHaEgH> &path, int min_val /* = 0 */)
{
	std::deque<OvmHaEgH> deque_path;
	double result = shortest_path (deque_path, min_val);
	foreach (OvmHaEgH heh, deque_path)
		path.push_back (heh);
	return result;
}

void VertexVertexPath::get_next_node_candidates (VertexVertexPathNode *current_node, 
	std::vector<VertexVertexPathNode*> &next_node_candidates)
{
	OvmVec3d cur_pt = mesh->vertex (current_node->vh);
	//当前节点的G值
	double cur_G = current_node->g;

	for (auto voh_it = mesh->voh_iter (current_node->vh); voh_it; ++voh_it)
	{
		auto eh = mesh->edge_handle (*voh_it);
		int edge_valence = mesh->valence (eh);
		//根据特殊要求，在这里对边进行筛选
		if (min_valence > 0 && edge_valence < min_valence)
			continue;
		auto eg = mesh->edge (eh);
		auto to_vh = eg.to_vertex () == current_node->vh? eg.from_vertex () : eg.to_vertex ();
		if (mesh->is_boundary (to_vh) && to_vh != end_vh)
			continue;
		auto new_node = new VertexVertexPathNode (to_vh, current_node->vh);
		next_node_candidates.push_back (new_node);
	}
}

double VertexVertexPath::calc_F_value (VertexVertexPathNode *parent_node, VertexVertexPathNode *current_node)
{
	auto V_LEVEL = mesh->request_vertex_property<int> ("node level");
	auto V_MAX_LEVEL = mesh->request_mesh_property<int> ("max node level");

	double this_G = 0.0f;
	OvmVec3d cur_pt = mesh->vertex (current_node->vh);
	//该节点的G值为父节点的G值加上父节点和该节点之间的网格边的长度的平方值
	if (parent_node){
		//OvmVec3d par_pt = mesh->vertex (parent_node->vh);
		//this_G += parent_node->g + std::sqrt (square_distance (cur_pt, par_pt));
		this_G = parent_node->g + 1;
	}
	//下面对拐弯的角度做一些修正，即尽可能让找到的路径平滑一点
	double penalty = 0.0f;
	if (parent_node && parent_node->parent_vh != mesh->InvalidVertexHandle)
	{
		auto parent_pt = mesh->vertex (parent_node->vh),
			parent_parent_pt = mesh->vertex (parent_node->parent_vh);
		OvmVec3d vec1 = cur_pt - parent_pt,
			vec2 = parent_pt - parent_parent_pt;
		penalty = JC::calc_interior_angle (vec1, vec2);
		penalty *= 1;
	}
	this_G += penalty + (V_MAX_LEVEL[0] - V_LEVEL[current_node->vh]);
	//double this_H = std::sqrt (square_distance (cur_pt, end_pt));
	double this_H = calc_H_value (current_node->vh, end_vh);

	double this_F = this_G + this_H;
	current_node->g = this_G; current_node->h = this_H; current_node->f = this_F;
	return this_F;
}

int VertexVertexPath::calc_H_value (OvmVeH vh1, OvmVeH vh2)
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
				if (mesh->is_boundary (eh)) continue;

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

double VertexVertexPath::trace_back (std::deque<OvmHaEgH> &path)
{
	OvmVeH trace_back_vh = end_vh;
	double total_distance = 0.0f;
	while (trace_back_vh != start_vh)
	{
		VertexVertexPathNode * n = close_set.retrieve (trace_back_vh);
		assert (n);
		total_distance += std::sqrt (square_distance (n->parent_vh, trace_back_vh));
		auto heh = mesh->halfedge (n->parent_vh, trace_back_vh);
		path.push_front (heh);
		trace_back_vh = n->parent_vh;
	}
	return total_distance;
}

double VertexVertexPath::square_distance (OvmVeH vh1, OvmVeH vh2)
{
	auto pt1 = mesh->vertex (vh1), pt2 = mesh->vertex (vh2);
	return square_distance (pt1, pt2);
}

double VertexVertexPath::square_distance (OvmVec3d &pt1, OvmVec3d &pt2)
{
	return ((pt1[0] - pt2[0]) * (pt1[0] - pt2[0]) +
		(pt1[1] - pt2[1]) * (pt1[1] - pt2[1]) +
		(pt1[2] - pt2[2]) * (pt1[2] - pt2[2]));
}
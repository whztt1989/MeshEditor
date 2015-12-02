#include "stdafx.h"
#include "FaceFacePath.h"
#include <limits>
#include <cmath>

FaceFacePath::FaceFacePath (VolumeMesh *_mesh, OvmFaH _sf, OvmFaH _ef, std::unordered_set<OvmFaH> * _allowed_fhs)
	: mesh (_mesh), start_fh (_sf), end_fh (_ef), allowed_fhs (_allowed_fhs)
{

}

FaceFacePath::~FaceFacePath(void)
{
}

double FaceFacePath::shortest_path (std::deque<OvmFaH> &path)
{
	auto n = new FaceFacePathNode (start_fh);
	calc_F_value (NULL, n);
	open_set.insert (n, start_fh);

	while (!open_set.empty ())
	{
		auto smallest_node = open_set.top ();
		open_set.pop (smallest_node->fh);
		close_set.insert (smallest_node, smallest_node->fh);

		//此处检查是否到达终点
		if (smallest_node->fh == end_fh)
			return trace_back (path);

		std::vector<FaceFacePathNode*> next_node_candidates;
		get_next_node_candidates (smallest_node, next_node_candidates);

		foreach (auto test_node, next_node_candidates){
			//检查to_vh是否已经在close set中, 如果已经在close set中，则不做任何处理
			if (close_set.exists (test_node->fh)){
				delete test_node;
				continue;
			}
			calc_F_value (smallest_node, test_node);
			//该节点还没有被放入open set，则将其放入
			if (!open_set.exists (test_node->fh)){
				open_set.insert (test_node, test_node->fh);
			}else{
				auto old_node = open_set.retrieve (test_node->fh);
				if (old_node->g > test_node->g){
					open_set.update (test_node, test_node->fh);
				}
			}
		}
	}//end while (!open_set.empty ())...
	return -1;
}

void FaceFacePath::get_next_node_candidates (FaceFacePathNode *current_node, 
	std::vector<FaceFacePathNode*> &next_node_candidates)
{
	OvmVec3d cur_pt = mesh->barycenter (current_node->fh);
	//当前节点的G值
	double cur_G = current_node->g;
	std::unordered_set<OvmFaH> adj_fhs;
	auto heh_vec = mesh->face (current_node->fh).halfedges ();
	foreach (auto &heh, heh_vec){
		std::unordered_set<OvmFaH> tmp_adj_fhs;
		JC::get_adj_faces_around_edge (mesh, heh, tmp_adj_fhs);
		foreach (auto &test_fh, tmp_adj_fhs){
			if (test_fh == current_node->fh || !JC::contains (*allowed_fhs, test_fh)) continue;
			adj_fhs.insert (test_fh);
		}
	}
	foreach (auto &fh, adj_fhs){
		auto new_node = new FaceFacePathNode (fh, current_node->fh);
		next_node_candidates.push_back (new_node);
	}
}

double FaceFacePath::calc_F_value (FaceFacePathNode *parent_node, FaceFacePathNode *current_node)
{
	double this_G = 0.0f;
	OvmVec3d cur_pt = mesh->barycenter (current_node->fh);
	//该节点的G值为父节点的G值加上父节点和该节点之间的网格边的长度的平方值
	if (parent_node){
		OvmVec3d par_pt = mesh->barycenter (parent_node->fh);
		this_G += parent_node->g + std::sqrt (square_distance (cur_pt, par_pt));
	}
	//下面对拐弯的角度做一些修正，即尽可能让找到的路径平滑一点
	double penalty = 0.0f;
	if (parent_node && parent_node->parent_fh != mesh->InvalidFaceHandle)
	{
		auto parent_pt = mesh->barycenter (parent_node->fh),
			parent_parent_pt = mesh->barycenter (parent_node->parent_fh);
		OvmVec3d vec1 = cur_pt - parent_pt,
			vec2 = parent_pt - parent_parent_pt;
		penalty = JC::calc_interior_angle (vec1, vec2);
		penalty *= 10;
	}
	this_G += penalty;
	double this_H = std::sqrt (square_distance (cur_pt, end_pt));
	double this_F = this_G + this_H;
	current_node->g = this_G; current_node->h = this_H; current_node->f = this_F;
	return this_F;
}

double FaceFacePath::trace_back (std::deque<OvmFaH> &path)
{
	OvmFaH trace_back_fh = end_fh;
	double total_distance = 0.0f;
	while (trace_back_fh != start_fh)
	{
		FaceFacePathNode * n = close_set.retrieve (trace_back_fh);
		assert (n);
		total_distance += std::sqrt (square_distance (n->parent_fh, trace_back_fh));
		path.push_front (trace_back_fh);
		trace_back_fh = n->parent_fh;
	}
	path.push_front (start_fh);
	return total_distance;
}

double FaceFacePath::square_distance (OvmFaH fh1, OvmFaH fh2)
{
	auto pt1 = mesh->barycenter (fh1), pt2 = mesh->barycenter (fh2);
	return square_distance (pt1, pt2);
}

double FaceFacePath::square_distance (OvmVec3d &pt1, OvmVec3d &pt2)
{
	return ((pt1[0] - pt2[0]) * (pt1[0] - pt2[0]) +
		(pt1[1] - pt2[1]) * (pt1[1] - pt2[1]) +
		(pt1[2] - pt2[2]) * (pt1[2] - pt2[2]));
}
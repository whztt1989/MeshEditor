#include "StdAfx.h"
#include "MeshDijkstra.h"


MeshDijkstra::MeshDijkstra(VolumeMesh *_mesh, OvmVeH _start_vh, std::unordered_set<OvmVeH> *_end_vhs)
	: mesh (_mesh), start_vh (_start_vh), end_vhs (_end_vhs)
{
	search_boundary = false;
	search_inner = true;
	min_valence = 4;
	consider_topology_only = false;
}


MeshDijkstra::~MeshDijkstra(void)
{
}

double MeshDijkstra::shortest_path (std::vector<OvmEgH> &best_path, OvmVeH &closest_vh)
{
	OvmVeH best_vh = mesh->InvalidVertexHandle;
	DijkstraVVPathNode *current_node = new DijkstraVVPathNode (start_vh);
	current_node->dist = 0;
	S_set.insert (current_node, start_vh);
	std::vector<DijkstraVVPathNode*> new_adj_nodes;
	get_adj_nodes (current_node, new_adj_nodes);
	foreach (DijkstraVVPathNode *n, new_adj_nodes){
		U_set.insert (n, n->vh);
	}

	while (true){
		auto closest_node = U_set.top ();
		U_set.pop (closest_node->vh);

		S_set.insert (closest_node, closest_node->vh);

		if (reach_the_end (closest_node)){
			std::deque<OvmEgH> path;
			trace_back (path, closest_node->vh);
			foreach (OvmEgH eh, path)
				best_path.push_back (eh);
			closest_vh = closest_node->vh;
			return closest_node->dist;
		}

		

		get_adj_nodes (closest_node, new_adj_nodes);

		foreach (DijkstraVVPathNode *test_node, new_adj_nodes){
			auto old_node = U_set.retrieve (test_node->vh);
			if (!old_node)
				U_set.insert (test_node, test_node->vh);
			else if (need_update (old_node, test_node))
				U_set.update (test_node, test_node->vh);
		}
	}

	return best_vh;
}

bool MeshDijkstra::reach_the_end (DijkstraVVPathNode *node)
{
	return JC::contains (*end_vhs, node->vh);
}

bool MeshDijkstra::need_update (DijkstraVVPathNode *old_node, DijkstraVVPathNode *test_node)
{
	return test_node->dist < old_node->dist;
}

void MeshDijkstra::get_adj_nodes (DijkstraVVPathNode *node, std::vector<DijkstraVVPathNode*> &adj_nodes)
{
	adj_nodes.clear ();
	for (auto voh_it = mesh->voh_iter (node->vh); voh_it; ++voh_it){
		OvmEgH test_eh = mesh->edge_handle (*voh_it);
		if (min_valence > 0 && mesh->valence (test_eh) < min_valence) continue;
		//如果searchable_ehs不为空，则需要判断一下test_eh是否包含在searchable_ehs中
		if (!searchable_ehs.empty () && !JC::contains (searchable_ehs, test_eh)) continue;
		//如果forbidden_ehs不为空，则需要判断一下test_eh是否包含在forbidden_ehs中
		if (!forbidden_ehs.empty () && JC::contains (forbidden_ehs, test_eh)) continue;
		OvmVeH test_vh = mesh->halfedge (*voh_it).to_vertex ();
		if (S_set.exists (test_vh)) continue;

		if (!search_boundary && mesh->is_boundary (test_vh) && !JC::contains (*end_vhs, test_vh)) continue;
		if (!search_inner && !mesh->is_boundary (test_vh)) continue;

		double marks = 0.0f, penalty = 0.0f;
		//如果consider_topology_only为true，表示本次路径搜索只考虑拓扑信息，即边或者点的度数
		if (consider_topology_only){
			//marks = 1;
			marks = 0;
			if (node->parent_vh != mesh->InvalidVertexHandle){
				auto vh_val = JC::vertex_valence_change (mesh, node->vh, node->parent_vh,test_vh);
				penalty = vh_val;
			}
			marks = node->dist + penalty;
		}else{
			double dist = std::sqrt (square_distance (test_vh, node->vh));
			if (node->parent_vh != mesh->InvalidVertexHandle){
				OvmVec3d parent_pt = mesh->vertex (node->parent_vh),
					current_pt = mesh->vertex (node->vh),
					test_pt = mesh->vertex (test_vh);
				OvmVec3d vec1 = current_pt - parent_pt,
					vec2 = test_pt - current_pt;
				penalty = calc_interior_angle (vec1, vec2);
				penalty *= 100;
			}
			marks = node->dist + dist + penalty;
		}
		
		DijkstraVVPathNode *new_adj_node = new DijkstraVVPathNode (test_vh, node->vh);
		new_adj_node->dist = marks;
		adj_nodes.push_back (new_adj_node);
	}
}

double MeshDijkstra::calc_interior_angle (const OvmVec3d &vec1, const OvmVec3d &vec2)
{
	double angle_cos = (vec1 | vec2) / (vec1.length () * vec2.length ());
	return acos (angle_cos);
}

double MeshDijkstra::trace_back (std::deque<OvmEgH> &path, OvmVeH end_vh)
{
	OvmVeH trace_back_vh = end_vh;
	double total_distance = 0.0f;
	while (trace_back_vh != start_vh)
	{
		DijkstraVVPathNode * n = S_set.retrieve (trace_back_vh);
		assert (n);
		total_distance += std::sqrt (square_distance (n->parent_vh, trace_back_vh));
		auto heh = mesh->halfedge (n->parent_vh, trace_back_vh);
		path.push_front (mesh->edge_handle (heh));
		trace_back_vh = n->parent_vh;
	}
	return total_distance;
}

double MeshDijkstra::square_distance (OvmVeH vh1, OvmVeH vh2)
{
	auto pt1 = mesh->vertex (vh1), pt2 = mesh->vertex (vh2);
	return square_distance (pt1, pt2);
}

double MeshDijkstra::square_distance (OvmVec3d &pt1, OvmVec3d &pt2)
{
	return ((pt1[0] - pt2[0]) * (pt1[0] - pt2[0]) +
		(pt1[1] - pt2[1]) * (pt1[1] - pt2[1]) +
		(pt1[2] - pt2[2]) * (pt1[2] - pt2[2]));
}
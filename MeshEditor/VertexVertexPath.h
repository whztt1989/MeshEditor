#pragma once

#include "MeshDefs.h"
#include "DualDefs.h"
#include "FuncDefs.h"

#include <set>
#include <map>
#include <deque>

#include "PrioritySetManager.h"

struct VertexVertexPathNode
{
	VertexVertexPathNode (OvmVeH _vh, OvmVeH _p = OvmVeH (-1))
		: vh(_vh), parent_vh (_p)
	{
		g = h = f = 0.0f;
	}
	VertexVertexPathNode ()
	{
		vh = OvmVeH ();
		parent_vh = OvmVeH ();
		g = h = f = 0.0f;
	}
	~VertexVertexPathNode(){}
	OvmVeH vh;
	OvmVeH parent_vh;
	double g, h, f;
};

struct VertexVertexPathNodeComp{
	bool operator () (VertexVertexPathNode *n1, VertexVertexPathNode *n2){
		return n1->f < n2->f;
	}
};


class VertexVertexPath
{
public:
	VertexVertexPath(VolumeMesh *_mesh, OvmVeH _sv, OvmVeH _ev);
	~VertexVertexPath(void);
public:
	double shortest_path (std::deque<OvmHaEgH> &path, int min_val = 0);
	double shortest_path (std::vector<OvmHaEgH> &path, int min_val = 0);
private:
	double calc_F_value (VertexVertexPathNode *parent_node, VertexVertexPathNode *current_node);
	int VertexVertexPath::calc_H_value (OvmVeH vh1, OvmVeH vh2);
	double trace_back (std::deque<OvmHaEgH> &path);
	void get_next_node_candidates (VertexVertexPathNode *current_node, 
		std::vector<VertexVertexPathNode*> &next_node_candidates);
	double square_distance (OvmVeH vh1, OvmVeH vh2);
	double square_distance (OvmVec3d &pt1, OvmVec3d &pt2);
private:
	VolumeMesh *mesh;
	OvmVeH start_vh, end_vh;
	NormalSetManager<VertexVertexPathNode, OvmVeH> close_set;
	PrioritySetManager<VertexVertexPathNode, VertexVertexPathNodeComp, OvmVeH> open_set;
	OvmVec3d end_pt;
	std::hash_map<OvmVeH, VertexVertexPathNode*> mapping;
	int min_valence;
};


#pragma once

#include "MeshDefs.h"
#include "DualDefs.h"
#include "FuncDefs.h"

#include <set>
#include <map>
#include <deque>

#include "PrioritySetManager.h"

struct VertexVertexBoundaryPathNode
{
	VertexVertexBoundaryPathNode (OvmVeH _vh, OvmVeH _p = OvmVeH (-1))
		: vh(_vh), parent_vh (_p)
	{
		g = h = f = 0.0f;
	}
	VertexVertexBoundaryPathNode ()
	{
		vh = OvmVeH ();
		parent_vh = OvmVeH ();
		g = h = f = 0.0f;
	}
	~VertexVertexBoundaryPathNode(){}
	OvmVeH vh;
	OvmVeH parent_vh;
	double g, h, f;
};

struct VertexVertexBoundaryPathNodeComp{
	bool operator () (VertexVertexBoundaryPathNode *n1, VertexVertexBoundaryPathNode *n2){
		return n1->f < n2->f;
	}
};

class VertexVertexBoundaryPath
{
public:
	VertexVertexBoundaryPath(VolumeMesh *_mesh, OvmVeH _sv, OvmVeH _ev, 
		std::unordered_set<OvmEgH> *_allowed_ehs, std::unordered_set<OvmEgH> *_forbidden_ehs);
	~VertexVertexBoundaryPath(void);
public:
	void set_min_depth (int _min_depth) {min_depth = _min_depth;}
	void shortest_path (std::deque<OvmHaEgH> &path);
	void shortest_path (std::vector<OvmHaEgH> &path);
private:
	VertexVertexBoundaryPathNode *create_first_node ();
	double calc_F_value (VertexVertexBoundaryPathNode *parent_node, VertexVertexBoundaryPathNode *current_node);
	void trace_back (std::deque<OvmHaEgH> &path);
	void get_next_node_candidates (VertexVertexBoundaryPathNode *current_node, 
		std::vector<VertexVertexBoundaryPathNode*> &next_node_candidates);
	int calc_H_value (OvmVeH vh1, OvmVeH vh2);
	double calc_penalty (OvmEgH eh1, OvmVeH vh, OvmEgH eh2);
private:
	VolumeMesh *mesh;
	OvmVeH start_vh, end_vh;
	std::unordered_set<OvmEgH> *forbidden_ehs;
	std::unordered_set<OvmEgH> *allowed_ehs;
	NormalSetManager<VertexVertexBoundaryPathNode, OvmVeH> close_set;
	PrioritySetManager<VertexVertexBoundaryPathNode, VertexVertexBoundaryPathNodeComp, OvmVeH> open_set;
	OvmVec3d end_pt;
	std::hash_map<OvmVeH, VertexVertexBoundaryPathNode*> mapping;
	int min_depth;
};


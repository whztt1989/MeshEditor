#pragma once

#include "MeshDefs.h"
#include "DualDefs.h"
#include "FuncDefs.h"

#include <set>
#include <map>
#include <deque>
#include "PrioritySetManager.h"

struct FaceFacePathNode
{
	FaceFacePathNode (OvmFaH _fh, OvmFaH _p = OvmFaH (-1))
		: fh(_fh), parent_fh (_p)
	{
		g = h = f = 0.0f;
	}
	FaceFacePathNode ()
	{
		fh = OvmFaH ();
		parent_fh = OvmFaH ();
		g = h = f = 0.0f;
	}
	~FaceFacePathNode(){}
	OvmFaH fh;
	OvmFaH parent_fh;
	double g, h, f;
};

struct FaceFacePathNodeComp{
	bool operator () (FaceFacePathNode *n1, FaceFacePathNode *n2){
		return n1->f < n2->f;
	}
};

class FaceFacePath
{
public:
	FaceFacePath(VolumeMesh *_mesh, OvmFaH _sf, OvmFaH _ef, std::unordered_set<OvmFaH> * _allowed_fhs);
	~FaceFacePath(void);
	double shortest_path (std::deque<OvmFaH> &faces);
private:
private:
	double calc_F_value (FaceFacePathNode *parent_node, FaceFacePathNode *current_node);
	double trace_back (std::deque<OvmFaH> &path);
	void get_next_node_candidates (FaceFacePathNode *current_node, 
		std::vector<FaceFacePathNode*> &next_node_candidates);
	double square_distance (OvmFaH fh1, OvmFaH fh2);
	double square_distance (OvmVec3d &pt1, OvmVec3d &pt2);
private:
	VolumeMesh *mesh;
	OvmFaH start_fh, end_fh;
	std::unordered_set<OvmFaH> * allowed_fhs;
	NormalSetManager<FaceFacePathNode, OvmFaH> close_set;
	PrioritySetManager<FaceFacePathNode, FaceFacePathNodeComp, OvmFaH> open_set;
	OvmVec3d end_pt;
	std::hash_map<OvmFaH, FaceFacePathNode*> mapping;

};


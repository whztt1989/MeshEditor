#pragma once

#include "MeshDefs.h"
#include "DualDefs.h"
#include "FuncDefs.h"

#include <set>
#include <map>
#include <deque>

#include "PrioritySetManager.h"

struct LoopCompletionNode
{
	LoopCompletionNode (OvmVeH _vh, OvmVeH _p = OvmVeH (-1))
		: vh(_vh), parent_vh (_p)
	{
		g = h = f = 0.0f;
	}
	LoopCompletionNode ()
	{
		vh = OvmVeH ();
		parent_vh = OvmVeH ();
		g = h = f = 0.0f;
	}
	~LoopCompletionNode(){}
	OvmVeH vh;
	OvmVeH parent_vh;
	double g, h, f;
};

struct LoopCompletionNodeComp{
	bool operator () (LoopCompletionNode *n1, LoopCompletionNode *n2){
		return n1->f < n2->f;
	}
};

class LoopCompletion
{
public:
	LoopCompletion(VolumeMesh *_mesh, OvmVeH _sv, OvmVeH _ev, 
		std::unordered_set<OvmEgH> *_allowed_ehs, std::unordered_set<OvmEgH> *_forbidden_ehs);
	~LoopCompletion(void);
public:
	void set_min_depth (int _min_depth) {min_depth = _min_depth;}
	void shortest_path (std::deque<OvmHaEgH> &path);
	void shortest_path (std::vector<OvmHaEgH> &path);
private:
	LoopCompletionNode *create_first_node ();
	double calc_F_value (LoopCompletionNode *parent_node, LoopCompletionNode *current_node);
	void trace_back (std::deque<OvmHaEgH> &path);
	void get_next_node_candidates (LoopCompletionNode *current_node, 
		std::vector<LoopCompletionNode*> &next_node_candidates);
	int calc_H_value (OvmVeH vh1, OvmVeH vh2);
	double calc_penalty (OvmEgH eh1, OvmVeH vh, OvmEgH eh2);
private:
	VolumeMesh *mesh;
	OvmVeH start_vh, end_vh;
	std::unordered_set<OvmEgH> *forbidden_ehs;
	std::unordered_set<OvmEgH> *allowed_ehs;
	NormalSetManager<LoopCompletionNode, OvmVeH> close_set;
	PrioritySetManager<LoopCompletionNode, LoopCompletionNodeComp, OvmVeH> open_set;
	OvmVec3d end_pt;
	std::hash_map<OvmVeH, LoopCompletionNode*> mapping;
	int min_depth;
};


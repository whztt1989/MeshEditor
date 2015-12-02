#pragma once

#include "MeshDefs.h"
#include "DualDefs.h"
#include "FuncDefs.h"

#include <set>
#include <map>
#include <deque>
#include <queue>
#include <stack>

#include "PrioritySetManager.h"
#include "VertexVertexPath.h"
#include "CrossCrossPathStep.h"

class CrossCrossPath
{
public:
	CrossCrossPath(VolumeMesh *_mesh,
		std::vector<std::pair<std::unordered_set<OvmFaH>, std::unordered_set<OvmFaH> > > &_fsps,
		OvmVeH _sv, OvmVeH _ev);
	CrossCrossPath (VolumeMesh *_mesh, std::vector<OvmEgH> &_start_ehs, 
		std::vector<std::unordered_set<OvmFaH> > &_start_cross_face_set, OvmVeH _start_vh, OvmVeH _end_vh);
	~CrossCrossPath(void);
public:
	int shortest_path (std::vector<OvmEgH> &path, CCPSHConf &cross_hexas);
	bool unconstrained_shortest_path (std::vector<OvmEgH> &path, CCPSHConf &cross_hexas, std::vector<std::pair<OvmEgH, OvmEgH> > &edge_pairs);
private:
	void handle_end_vh (CCPSHConf &hexa_conf, std::vector<std::pair<OvmEgH, OvmEgH> > &edge_pairs);
	void trace_back (CCPSHConf &hexa_conf, std::stack<CCPStep*> &ccpsteps_stack);
private:
	VolumeMesh *mesh;
	//std::vector<std::pair<OvmEgH, OvmEgH> > cross_edge_pairs;
	std::vector<std::pair<std::unordered_set<OvmFaH>, std::unordered_set<OvmFaH> > > cross_face_set_pairs;
	OvmVeH start_vh, end_vh;
	std::vector<OvmEgH> start_ehs;
	std::vector<std::unordered_set<OvmFaH> > start_cross_face_set;
};


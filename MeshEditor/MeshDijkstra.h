#pragma once
#include "MeshDefs.h"
#include "DualDefs.h"
#include "FuncDefs.h"

#include <set>
#include <map>
#include <deque>
#include "PrioritySetManager.h"

struct DijkstraVVPathNode
{
	DijkstraVVPathNode (OvmVeH _vh, OvmVeH _p = OvmVeH (-1))
		: vh(_vh), parent_vh (_p)
	{
		dist = std::numeric_limits<double>::max ();
	}
	DijkstraVVPathNode ()
	{
		vh = OvmVeH ();
		parent_vh = OvmVeH ();
		dist = std::numeric_limits<double>::max ();
	}
	~DijkstraVVPathNode(){}
	OvmVeH vh;
	OvmVeH parent_vh;
	double dist;
};

struct DijkstraVVPathNodeComp{
	bool operator () (DijkstraVVPathNode *n1, DijkstraVVPathNode *n2){
		return n1->dist < n2->dist;
	}
};

class MeshDijkstra
{
public:
	MeshDijkstra(VolumeMesh *_mesh, OvmVeH _start_vh, std::unordered_set<OvmVeH> *_end_vhs);
	virtual ~MeshDijkstra(void);
public:
	double shortest_path (std::vector<OvmEgH> &best_path, OvmVeH &closest_vh);
	bool search_boundary, search_inner;
	bool consider_topology_only;
	int min_valence;
	std::unordered_set<OvmEgH> searchable_ehs, forbidden_ehs;
protected:
	virtual void get_adj_nodes (DijkstraVVPathNode *node, std::vector<DijkstraVVPathNode*> &adj_nodes);
	virtual bool reach_the_end (DijkstraVVPathNode *node);
	double trace_back (std::deque<OvmEgH> &path, OvmVeH end_vh);
	double square_distance (OvmVeH vh1, OvmVeH vh2);
	double square_distance (OvmVec3d &pt1, OvmVec3d &pt2);
	double calc_interior_angle (const OvmVec3d &vec1, const OvmVec3d &vec2);
	virtual bool need_update (DijkstraVVPathNode *old_node, DijkstraVVPathNode *test_node);
protected:
	VolumeMesh *mesh;
	OvmVeH start_vh;
	std::unordered_set<OvmVeH> *end_vhs;
	NormalSetManager<DijkstraVVPathNode, OvmVeH> S_set;
	PrioritySetManager<DijkstraVVPathNode, DijkstraVVPathNodeComp, OvmVeH> U_set;
};


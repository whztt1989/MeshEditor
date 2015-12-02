#pragma once

#include "MeshDefs.h"
#include "DualDefs.h"
#include "FuncDefs.h"

#include <set>
#include <map>
#include <deque>
#include <queue>

typedef std::unordered_set<OvmCeH> CCPSHSet;
typedef std::vector<CCPSHSet> CCPSHConf;

struct CCPStepNode{
	CCPStepNode () {penalty = 0.0f;}
	CCPSHConf hexa_conf;
	double penalty;
};

bool valid_hexa_config (VolumeMesh *mesh, CCPSHConf &hexa_conf);

class CCPStep{
public:
	CCPStep (VolumeMesh *_mesh, CCPSHConf &_prev_conf, 
		OvmVeH _curr_vh, OvmEgH _next_eh);
	~CCPStep(){}
public:
	bool has_next_conf ();
	CCPSHConf get_next_conf ();
	CCPSHConf get_saved_conf ();
private:
	void get_all_candidate ();
	bool valid_config (CCPSHConf &hexa_conf);
	bool eligible_candidate (CCPSHConf &hexa_conf);
	double calc_penalty (CCPSHConf &hexa_conf);
	CCPSHSet convert_conf_to_hexa_set (CCPSHConf &hexa_conf);
private:
	VolumeMesh *mesh;
	CCPSHConf prev_conf;
	OvmEgH next_eh;
	OvmVeH curr_vh;
	CCPSHSet hexas_around_vh, hexas_around_next_eh;
	std::unordered_set<OvmEgH> edges_around_vh;
	std::vector<CCPStepNode> step_node_queue;
	CCPSHConf saved_conf;
};
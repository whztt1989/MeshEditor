#pragma once
#include "acis_headers.h"
#include "MeshDefs.h"
#include "DualDefs.h"
#include "FuncDefs.h"

class DepthCutHandler
{
public:
	DepthCutHandler(VolumeMesh *_mesh, BODY *_body);
	~DepthCutHandler(void);
	void get_start_fhs ();
	void get_hexas_within_depth ();
	void create_cutting_face ();
	void depth_cutting ();
	void save_data ();
public:
	FACE *start_face;
	std::unordered_set<OvmFaH> start_fhs;
	VolumeMesh *mesh;
	BODY *body;
	int depth_num;
	std::unordered_set<OvmCeH> hexas_within_depth;
	std::unordered_set<OvmFaH> joint_fhs;
	FACE *cut_face;
	VolumeMesh *new_mesh;
};


#pragma once

#include "MeshDefs.h"
#include "DualDefs.h"
#include "FuncDefs.h"

#include <set>
#include <map>
#include <deque>

std::unordered_set<OvmFaH> get_volume_mesh_min_cut (VolumeMesh *mesh, 
	std::unordered_set<OvmCeH> &s_chs, std::unordered_set<OvmCeH> &t_chs);

std::unordered_set<OvmEgH> get_quad_mesh_min_cut (VolumeMesh *mesh, 
	std::unordered_set<OvmFaH> &s_fhs, std::unordered_set<OvmFaH> &t_fhs,
	std::vector<std::unordered_set<OvmFaH> > &obstacles);

std::unordered_set<OvmFaH> get_local_mesh_min_cut (VolumeMesh *mesh, 
	std::unordered_set<OvmCeH> &all_chs, std::unordered_set<OvmCeH> &s_chs, 
	std::unordered_set<OvmCeH> &t_chs);
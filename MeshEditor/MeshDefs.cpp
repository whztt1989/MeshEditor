#include "stdafx.h"
#include "MeshDefs.h"
#include "FuncDefs.h"

void VolumeMeshElementGroup::add_edges_on_quad_set ()
{
	foreach (auto &fh, fhs){
		std::unordered_set<OvmEgH> adj_ehs;
		JC::get_adj_edges_around_face (mesh, fh, adj_ehs);
		foreach (auto &eh, adj_ehs)
			ehs.insert (eh);
	}
}
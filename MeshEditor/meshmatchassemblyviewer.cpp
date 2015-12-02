#include "StdAfx.h"
#include "meshmatchassemblyviewer.h"
#include "hoopsview.h"
#include "manualmeshmatchwidget.h"

MeshMatchAssemblyViewer::MeshMatchAssemblyViewer(QWidget *parent)
	: QWidget(parent)
{
	mesh1 = mesh2 = NULL;
	inter_face = NULL;
	myresabs = SPAresabs * 1000;
	mm_handler = NULL;
	ui.setupUi(this);
	hoopsview = ui.hoopsview;
	mm_widget = NULL;
	connect (ui.pushButton_Refresh_Matching, SIGNAL (clicked ()), SLOT (on_refresh_matching ()));
}

MeshMatchAssemblyViewer::~MeshMatchAssemblyViewer()
{

}

void MeshMatchAssemblyViewer::on_refresh_matching ()
{
	if (mesh1 == NULL || mesh2 == NULL || inter_face == NULL)
		return;
	//auto fGetInterfaceEhs = [&] (VolumeMesh *mesh, std::unordered_set<OvmFaH> &fhs, std::unordered_set<OvmEgH> &ehs){
	//	
	//	SPAposition closest_pt;
	//	double dist = 0.0f;
	//	int n = mesh->n_faces ();
	//	for (auto f_it = mesh->faces_begin (); f_it != mesh->faces_end (); ++f_it){
	//		if (!mesh->is_boundary (*f_it)) continue;
	//		auto centre_pos = mesh->barycenter (*f_it);
	//		api_entity_point_distance (inter_face, POS2SPA(centre_pos), closest_pt, dist);
	//		if (dist < myresabs)
	//			fhs.insert (*f_it);
	//	}
	//	foreach (auto &fh, fhs){
	//		auto heh_vec = mesh->face (fh).halfedges ();
	//		foreach (auto &heh, heh_vec)
	//			ehs.insert (mesh->edge_handle (heh));
	//	}
	//};
	//
	//interface_fhs1.clear (); interface_fhs2.clear ();
	//std::unordered_set<OvmEgH> inter_ehs1, inter_ehs2;
	//fGetInterfaceEhs (mesh1, interface_fhs1, inter_ehs1);
	//fGetInterfaceEhs (mesh2, interface_fhs2, inter_ehs2);

	auto fGetInterfaceEhs = [&] (VolumeMesh *mesh, std::unordered_set<OvmFaH> &fhs, std::unordered_set<OvmEgH> &ehs){
		
		SPAposition closest_pt;
		double dist = 0.0f;
		int n = mesh->n_faces ();
		for (auto f_it = mesh->faces_begin (); f_it != mesh->faces_end (); ++f_it){
			if (!mesh->is_boundary (*f_it)) continue;
			auto centre_pos = mesh->barycenter (*f_it);
			api_entity_point_distance (inter_face, POS2SPA(centre_pos), closest_pt, dist);
			if (dist < myresabs)
				fhs.insert (*f_it);
		}
		foreach (auto &fh, fhs){
			auto heh_vec = mesh->face (fh).halfedges ();
			foreach (auto &heh, heh_vec){
				auto eh = mesh->edge_handle (heh);
				if (JC::contains (ehs, eh))
					ehs.erase (eh);
				else
					ehs.insert (eh);
			}
		}
	};
	
	interface_fhs1.clear (); interface_fhs2.clear ();
	std::unordered_set<OvmEgH> inter_ehs1, inter_ehs2;
	fGetInterfaceEhs (mesh1, interface_fhs1, inter_ehs1);

	//render
	hoopsview->derender_all_mesh_groups ();

	auto group = new VolumeMeshElementGroup (mesh1, "mm", "interface ehs 1");
	group->ehs = inter_ehs1;
	MeshRenderOptions render_options;
	render_options.edge_color = "black";
	render_options.edge_width = 6;
	hoopsview->render_mesh_group (group, render_options);
	hoopsview->derender_all_chords ();
	foreach (auto &p, matched_chord_pairs){
		hoopsview->render_chord (p.first, "blue", 8);
		hoopsview->render_chord (p.second, "green", 8);
	}
	//group = new VolumeMeshElementGroup (mesh2, "mm", "interface ehs 2");
	//group->ehs = inter_ehs2;
	//render_options.edge_color = "pink";
	//render_options.edge_width = 3;
	//hoopsview->render_mesh_group (group, render_options);
	//hoopsview->OnZoomToExtents ();
}
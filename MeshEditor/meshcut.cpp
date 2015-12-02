#include "StdAfx.h"
#include "meshcut.h"
#include <QFileDialog>

MeshCut::MeshCut(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	connect (ui.pb_Load_Mesh_File_For_Cut, SIGNAL (clicked ()), SLOT (onLoadMeshFile ()));
	connect (ui.pb_Load_Cut_CAD_Model, SIGNAL (clicked ()), SLOT (onLoadCutCADModel ()));
	connect (ui.pb_Cut_Mesh_By_CAD_Model, SIGNAL (clicked ()), SLOT (onCutMesh ()));
	connect (ui.pb_Revert_Cut_Mesh_By_CAD_Model, SIGNAL (clicked ()), SLOT (onRevertCutMesh ()));
	connect (ui.pb_Attach_And_Smooth_Cut_Mesh, SIGNAL (clicked ()), SLOT (onAttachAndSmooth ()));
	connect (ui.pb_Save_Cut_Mesh, SIGNAL (clicked ()), SLOT (onSave ()));

	hoopsview1 = ui.hoopsview1;
	hoopsview2 = ui.hoopsview2;
	hoopsview3 = ui.hoopsview3;
	mesh = NULL;
	match_mesh = NULL;
	left_mesh = NULL;
	body = NULL;
}

MeshCut::~MeshCut()
{

}

void MeshCut::onLoadMeshFile ()
{
	QString mesh_path = QFileDialog::getOpenFileName (this, "Select Mesh File", QString(), "OVM Files (*.ovm)");
	if (mesh_path != ""){
		mesh = JC::load_volume_mesh (mesh_path);
		hoopsview1->render_VolumeMesh (mesh);

		hoopsview1->show_boundary (true);
		hoopsview1->show_boundary_vertices (false);
		hoopsview1->show_boundary_edges (true);
		hoopsview1->show_boundary_faces (true);
		hoopsview1->show_boundary_cells (false);
		hoopsview1->show_boundary_vertices_indices (false);
		hoopsview1->show_boundary_edges_indices (false);
		hoopsview1->show_boundary_faces_indices (false);
		hoopsview1->show_boundary_cells_indices (false);

		hoopsview1->show_inner (false);
		hoopsview1->show_inner_vertices (false);
		hoopsview1->show_inner_edges (true);
		hoopsview1->show_inner_faces (false);
		hoopsview1->show_inner_cells (false);
		hoopsview1->show_inner_vertices_indices (false);
		hoopsview1->show_inner_edges_indices (false);
		hoopsview1->show_inner_faces_indices (false);
		hoopsview1->show_inner_cells_indices (false);
		hoopsview1->set_edges_selectable (true);
	}
}

void MeshCut::onLoadCutCADModel ()
{
	QString model_path = QFileDialog::getOpenFileName (this, "Select ACIS File", QString(), "ACIS Files (*.sat)");
	if (model_path != ""){
		body = JC::load_acis_model (model_path);
		//JC::init_volume_mesh (mesh, body, SPAresabs * 1000);

		HC_Open_Segment_By_Key (hoopsview1->GetHoopsView ()->GetModelKey ());{
			HC_Open_Segment ("body");{
				HC_Set_Visibility ("markers=off,vertices=off,lines=on,faces=off");
				HC_Set_Line_Weight (6);
				HC_Set_Color ("lines=red,edges=red");
				HA_Render_Entity (body, ".");

			}HC_Close_Segment ();
		}HC_Close_Segment ();
	}
}

void MeshCut::onCutMesh ()
{
	auto fGetLeftChs = [] (VolumeMesh *whole_mesh, BODY *match_body, 
		std::unordered_set<OvmCeH> &match_chs, std::unordered_set<OvmCeH> &left_chs){
		for (auto c_it = whole_mesh->cells_begin (); c_it != whole_mesh->cells_end (); ++c_it){
			auto centre_pt = whole_mesh->barycenter (*c_it);
			SPAposition acis_pos = POS2SPA(centre_pt);
			point_containment pc;
			api_point_in_body (acis_pos, match_body, pc);
			if (pc == point_inside)
				match_chs.insert (*c_it);
			else
				left_chs.insert (*c_it);
		}
	};

	std::unordered_set<OvmCeH> match_chs, left_chs;
	fGetLeftChs (mesh, body, match_chs, left_chs);

	match_mesh = JC::form_new_mesh (mesh, match_chs);

	hoopsview2->render_VolumeMesh (match_mesh);
			 
	hoopsview2->show_boundary (true);
	hoopsview2->show_boundary_vertices (false);
	hoopsview2->show_boundary_edges (true);
	hoopsview2->show_boundary_faces (true);
	hoopsview2->show_boundary_cells (false);
	hoopsview2->show_boundary_vertices_indices (false);
	hoopsview2->show_boundary_edges_indices (false);
	hoopsview2->show_boundary_faces_indices (false);
	hoopsview2->show_boundary_cells_indices (false);
			 
	hoopsview2->show_inner (false);
	hoopsview2->show_inner_vertices (false);
	hoopsview2->show_inner_edges (true);
	hoopsview2->show_inner_faces (false);
	hoopsview2->show_inner_cells (false);
	hoopsview2->show_inner_vertices_indices (false);
	hoopsview2->show_inner_edges_indices (false);
	hoopsview2->show_inner_faces_indices (false);
	hoopsview2->show_inner_cells_indices (false);
	hoopsview2->set_edges_selectable (true);

	left_mesh = JC::form_new_mesh (mesh, left_chs);

	hoopsview3->render_VolumeMesh (left_mesh);
			 
	hoopsview3->show_boundary (true);
	hoopsview3->show_boundary_vertices (false);
	hoopsview3->show_boundary_edges (true);
	hoopsview3->show_boundary_faces (true);
	hoopsview3->show_boundary_cells (false);
	hoopsview3->show_boundary_vertices_indices (false);
	hoopsview3->show_boundary_edges_indices (false);
	hoopsview3->show_boundary_faces_indices (false);
	hoopsview3->show_boundary_cells_indices (false);

	hoopsview3->show_inner (false);
	hoopsview3->show_inner_vertices (false);
	hoopsview3->show_inner_edges (true);
	hoopsview3->show_inner_faces (false);
	hoopsview3->show_inner_cells (false);
	hoopsview3->show_inner_vertices_indices (false);
	hoopsview3->show_inner_edges_indices (false);
	hoopsview3->show_inner_faces_indices (false);
	hoopsview3->show_inner_cells_indices (false);
	hoopsview3->set_edges_selectable (true);
}

void MeshCut::onRevertCutMesh ()
{
	auto tmp = match_mesh;
	match_mesh = left_mesh;
	left_mesh = tmp;

	hoopsview2->rerender_VolumeMesh (match_mesh);
	hoopsview3->rerender_VolumeMesh (left_mesh);
}

void MeshCut::onAttachAndSmooth ()
{
	std::unordered_set<OvmVeH> left_vhs;
	JC::init_volume_mesh (match_mesh, body, SPAresabs * 1000, &left_vhs);

	

	if (!left_vhs.empty () ){
		auto group = new VolumeMeshElementGroup (match_mesh, "aa", "left vh set");
		group->vhs = left_vhs;
		MeshRenderOptions render_options;
		render_options.vertex_color = "red";
		hoopsview2->render_mesh_group (group, render_options);
	}
	//JC::smooth_volume_mesh (match_mesh, body, 5, hoopsview2);
}

void MeshCut::onSave ()
{
	QString file_path = QFileDialog::getSaveFileName (this, "Save Mesh File", QString (), "OVM Files (*.ovm)");
	if (file_path != ""){
		if (!JC::save_volume_mesh (match_mesh, file_path)){
			QMessageBox::warning (this, "ERROR", "Save match mesh failed!");
			return;
		}
	}

	file_path = QFileDialog::getSaveFileName (this, "Save ACIS File", QString (), "ACIS Files (*.sat)");
	if (file_path != ""){
		JC::save_acis_entity (body, file_path.toAscii ().data ());
	}
}
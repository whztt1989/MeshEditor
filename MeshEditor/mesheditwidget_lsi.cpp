#include "StdAfx.h"
#include "mesheditwidget.h"

void MeshEditWidget::on_select_edges_ok_for_auto_lsi ()
{
	if (ossi_handler) delete ossi_handler;
	ossi_handler = new OneSimpleSheetInflationHandler (mesh, body, hoopsview);
	ossi_handler->init ();

	
	selected_ehs_set.clear ();

	if (completed_loop_ehs.empty ()){
		std::vector<OvmEgH> selected_ehs;
		if (!hoopsview->get_selected_elements (std::vector<OvmVeH>(), selected_ehs, std::vector<OvmFaH> ())){
			QMessageBox::warning (this, tr("警告"), tr("未选中任何网格边！"), QMessageBox::Ok);
			return;
		}
		JC::vector_to_unordered_set (selected_ehs, selected_ehs_set);
	}else{
		selected_ehs_set = completed_loop_ehs;
	}
	
	//selected_ehs = selected_ehs_set;

	if (!ossi_handler->analyze_input_edges (selected_ehs_set)){
		QMessageBox::critical (this, tr("错误"), ossi_handler->get_last_err (), QMessageBox::Ok);
		return;
	}

	if (!ossi_handler->is_complete ()){
		ui.pushButton_Select_Constant_Faces_OK_For_Auto_LSI->setEnabled (true);
		ui.pushButton_Set_Depth_Control_For_Auto_LSI->setEnabled (true);

		FACE *inter_face = NULL;
		auto V_ENTITY_PTR = mesh->request_vertex_property<unsigned int> ("entityptr");
		foreach (OvmEgH eh, selected_ehs_set){
			OvmVeH vh1 = mesh->edge (eh).from_vertex (),
				vh2 = mesh->edge (eh).to_vertex ();
			ENTITY *entity1 = (ENTITY*)(V_ENTITY_PTR[vh1]),
				*entity2 = (ENTITY*)(V_ENTITY_PTR[vh2]);
			if (is_FACE (entity1)){
				inter_face = (FACE*)entity1;
				break;
			}else if (is_FACE (entity2)){
				inter_face = (FACE*)entity2;
				break;
			}
		}
		if (inter_face == NULL){
			QMessageBox::critical (this, tr("错误！"), tr ("找不到输入网格边所在的几何面！无法推导！"));
			return;
		}

		ossi_handler->set_interface (inter_face);
	}else{
		ui.pushButton_Get_Hexa_Set->setEnabled (true);
		
	}
	ui.pushButton_Select_Edges_OK_For_Auto_LSI->setEnabled (false);

	hoopsview->clear_selection ();
	hoopsview->derender_all_mesh_groups ();
	//render
	auto group = new VolumeMeshElementGroup (mesh, "lsi", "select edges");
	group->ehs = selected_ehs_set;
	MeshRenderOptions render_options;
	render_options.edge_width = 3;
	hoopsview->render_mesh_group (group, render_options);
}


void get_diagonal_quad_patches (VolumeMesh *mesh,
	std::vector<std::unordered_set<OvmFaH> > &separated_fhs_patches,
	std::unordered_set<OvmFaH> &mid_patch, 
	std::unordered_set<OvmFaH> &diagonal_patch1,
	std::unordered_set<OvmFaH> &diagonal_patch2)
{
	std::vector<std::set<OvmVeH> > vhs_on_patches;
	vhs_on_patches.resize (separated_fhs_patches.size ());
	for (int i = 0; i != separated_fhs_patches.size (); ++i){
		auto &patch = separated_fhs_patches[i];
		std::set<OvmVeH> vhs_on_patch;
		foreach (auto &fh, patch){
			auto heh_vec = mesh->face (fh).halfedges ();
			foreach (auto &heh, heh_vec)
				vhs_on_patches[i].insert (mesh->halfedge (heh).to_vertex ());
		}
	}

	for (int i = 0; i != separated_fhs_patches.size (); ++i){
		auto &patch1 = separated_fhs_patches[i];
		auto &vhs_on_patch1 = vhs_on_patches[i];
		std::vector<int> diagonal_adj_patch_indices;
		std::vector<OvmVeH> comm_vhs_vec;
		for (int j = 0; j != separated_fhs_patches.size (); ++j){
			if (j == i) continue;

			auto &patch2 = separated_fhs_patches[j];
			auto &vhs_on_patch2 = vhs_on_patches[j];

			std::vector<OvmVeH> comm_vhs;
			std::set_intersection (vhs_on_patch1.begin (), vhs_on_patch1.end (), 
				vhs_on_patch2.begin (), vhs_on_patch2.end (), std::back_inserter (comm_vhs));

			if (comm_vhs.size () == 1){
				diagonal_adj_patch_indices.push_back (j);
				comm_vhs_vec.push_back (comm_vhs.front ());
			}
		}
		if (diagonal_adj_patch_indices.size () == 2){
			mid_patch = patch1;
			diagonal_patch1 = separated_fhs_patches[diagonal_adj_patch_indices.front ()];
			diagonal_patch2 = separated_fhs_patches[diagonal_adj_patch_indices.back ()];
			return;
		}
	}
}

void MeshEditWidget::on_get_surf_patches_for_auto_lsi ()
{
	std::unordered_set<OvmFaH> bound_fhs;
	for (auto bf_it = mesh->bf_iter (); bf_it; ++bf_it)
		bound_fhs.insert (*bf_it);
	std::vector<std::unordered_set<OvmFaH> > sep_fhs_patches;
	JC::get_separate_fhs_patches (mesh, bound_fhs, selected_ehs_set, sep_fhs_patches);
	hoopsview->derender_mesh_groups ("lsi", "surf patches");
	char *colors[] = {"green", "yellow", "pink"};
	MeshRenderOptions render_options;
	render_options.edge_width = 8;
	render_options.edge_color = "red";
	auto group = new VolumeMeshElementGroup (mesh, "lsi", "selected ehs");
	group->ehs = selected_ehs_set;
	hoopsview->render_mesh_group (group, render_options);
	render_options.edge_width = 5;
	render_options.edge_color = "black";

	if (sep_fhs_patches.size () == 2){
		auto smaller_one = sep_fhs_patches.front ().size () < sep_fhs_patches.back ().size () ?
			sep_fhs_patches.front () : sep_fhs_patches.back ();
		auto group = new VolumeMeshElementGroup (mesh, "lsi", "surf patches");
		group->fhs =smaller_one;
		foreach (auto &fh, group->fhs){
			auto heh_vec = mesh->face (fh).halfedges ();
			foreach (auto &heh, heh_vec){
				auto eh = mesh->edge_handle (heh);
				if (JC::contains (selected_ehs_set, eh)) continue;
				group->ehs.insert (eh);
			}
		}
		
		render_options.face_color = colors[0];
		hoopsview->render_mesh_group (group, render_options);

	}else{
		std::unordered_set<OvmFaH> diagonal_patches[3];
		get_diagonal_quad_patches (mesh, sep_fhs_patches, diagonal_patches[0], diagonal_patches[1], diagonal_patches[2]);
		//render
		
		for (int i = 0; i != 3; ++i){
			auto &patch = diagonal_patches[i];
			auto group = new VolumeMeshElementGroup (mesh, "lsi", "surf patches");
			group->fhs = diagonal_patches[i];
			foreach (auto &fh, group->fhs){
				auto heh_vec = mesh->face (fh).halfedges ();
				foreach (auto &heh, heh_vec){
					auto eh = mesh->edge_handle (heh);
					if (JC::contains (selected_ehs_set, eh)) continue;
					group->ehs.insert (eh);
				}
			}
			render_options.face_color = colors[i];
			hoopsview->render_mesh_group (group, render_options);
		}
	}
	mesh_render_controller->render_wireframe ();
}

void MeshEditWidget::on_set_depth_control_for_auto_lsi ()
{
	bool ok = false;
	int min_depth = QInputDialog::getInt (this, tr("深度控制"), tr("请输入最小深度"), 4, 1, 1000, 1, &ok);
	if (!ok) return;
	ossi_handler->set_minimum_depth (min_depth);
}

void MeshEditWidget::on_select_constant_faces_ok_for_auto_lsi ()
{
	std::vector<OvmFaH> selected_fhs;
	if (!hoopsview->get_selected_elements (std::vector<OvmVeH>(), std::vector<OvmEgH>(), selected_fhs)){
		QMessageBox::warning (this, tr("警告"), tr("未选中任何网格面！"), QMessageBox::Ok);
		return;
	}
	FACE *constant_face = NULL;
	auto V_ENTITY_PTR = mesh->request_vertex_property<unsigned int>("entityptr");

	foreach (auto &fh, selected_fhs){
		auto adj_vhs = JC::get_adj_vertices_around_face (mesh, fh);
		foreach (auto &vh, adj_vhs){
			ENTITY *entity = (ENTITY*)(V_ENTITY_PTR[vh]);
			if (is_FACE (entity))
				constant_face = (FACE *)entity;
		}
	}
	
	std::unordered_set<OvmFaH> constant_fhs;
	JC::get_fhs_on_acis_face (mesh, constant_face, constant_fhs);
	ossi_handler->set_constant_fhs (constant_fhs);

	ui.pushButton_Select_Constant_Faces_OK_For_Auto_LSI->setEnabled (false);
	ui.pushButton_Get_Hexa_Set->setEnabled (true);
	ui.pushButton_Set_Depth_Control_For_Auto_LSI->setEnabled (false);

	auto group = new VolumeMeshElementGroup (mesh, "lsi", "constant faces");
	group->fhs = constant_fhs;
	MeshRenderOptions render_options;
	render_options.face_color = "(diffuse=pink,transmission = (r=0.2 g=0.2 b=0.2))";
	hoopsview->render_mesh_group (group, render_options);

}

void MeshEditWidget::on_get_hexa_set_for_auto_lsi ()
{
	auto hexa_set = ossi_handler->get_hexa_set ();
	if (hexa_set.empty ()){
		QMessageBox::critical (this, tr("错误"), ossi_handler->get_last_err (), QMessageBox::Ok);
		return;
	}

	hoopsview->derender_mesh_groups ("lsi", "optimized diagonal hexa sets", true);
	auto group = new VolumeMeshElementGroup (mesh, "lsi", "optimized diagonal hexa sets");
	group->chs = hexa_set;
	MeshRenderOptions render_options;
	render_options.cell_color = "(diffuse=red,transmission = (r=0.2 g=0.2 b=0.2))";
	hoopsview->render_mesh_group (group, render_options);
	mesh_render_controller->render_wireframe ();
	ui.pushButton_Get_Quad_Set_For_Auto_LSI->setEnabled (true);
	ui.pushButton_Get_Hexa_Set->setEnabled (false);
}

void MeshEditWidget::on_get_quad_set_for_auto_lsi ()
{
	auto fhs_for_inflation = ossi_handler->get_inflation_fhs ();
	if (fhs_for_inflation.empty ()){
		QMessageBox::critical (this, tr("错误"), ossi_handler->get_last_err (), QMessageBox::Ok);
		return;
	}

	hoopsview->derender_mesh_groups ("lsi", "optimized diagonal hexa sets", true);
	auto group = new VolumeMeshElementGroup (mesh, "lsi", "quad set for inflation");
	group->fhs = fhs_for_inflation;
	foreach (auto &fh, group->fhs){
		auto heh_vec = mesh->face (fh).halfedges ();
		foreach (auto &heh, heh_vec){
			auto eh = mesh->edge_handle (heh);
			group->ehs.insert (eh);
		}
	}
	MeshRenderOptions render_options;
	//render_options.face_color = "(diffuse=red,transmission = (r=0.2 g=0.2 b=0.2))";
	render_options.face_color = "blue";
	hoopsview->render_mesh_group (group, render_options);
	mesh_render_controller->render_wireframe ();
	ui.pushButton_Sheet_Inflate_For_Auto_LSI->setEnabled (true);
	ui.pushButton_Get_Quad_Set_For_Auto_LSI->setEnabled (false);
}

void MeshEditWidget::on_sheet_inflate_for_auto_lsi ()
{
	auto new_sheet = ossi_handler->sheet_inflation ();
	if (!new_sheet){
		QMessageBox::critical (this, tr("错误"), ossi_handler->get_last_err (), QMessageBox::Ok);
		return;
	}

	JC::retrieve_sheets (mesh, sheet_set);

	hoopsview->derender_all_mesh_groups ();
	for (int i = 0; i != 10; ++i)
		JC::smooth_volume_mesh (mesh, body, 1);
	hoopsview->rerender_VolumeMesh (mesh);

	MeshRenderOptions render_options;
	render_options.cell_color = "red";
	hoopsview->render_mesh_group (new_sheet, render_options);

	foreach (auto sheet, sheet_set){
		QString str = QString ("sheet%1").arg (sheet->name);
		auto group = new VolumeMeshElementGroup (mesh, "sheet", str.toAscii ().data ());
		group->chs = sheet->chs;
		hoopsview->render_mesh_group (group, render_options);
		hoopsview->show_mesh_group (group, false);
	}
	ui.pushButton_Sheet_Inflate_For_Auto_LSI->setEnabled (false);
	ui.pushButton_Reprocess_For_Auto_LSI->setEnabled (true);
}

void MeshEditWidget::on_reprocess_for_auto_lsi ()
{
	ossi_handler->reprocess ();

	hoopsview->derender_all_mesh_groups ();
	mesh_render_controller->render_shaded ();
	ui.pushButton_Select_Edges_OK_For_Auto_LSI->setEnabled (true);
	ui.pushButton_Set_Depth_Control_For_Auto_LSI->setEnabled (false);
	ui.pushButton_Select_Constant_Faces_OK_For_Auto_LSI->setEnabled (false);
	ui.pushButton_Get_Quad_Set_For_Auto_LSI->setEnabled (false);
	ui.pushButton_Sheet_Inflate_For_Auto_LSI->setEnabled (false);
	ui.pushButton_Reprocess_For_Auto_LSI->setEnabled (false);
}
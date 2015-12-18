#include "StdAfx.h"
#include "mesheditcontroller.h"
#include "hoopsview.h"
#include <QMessageBox>
#include <QInputDialog>

void MeshEditController::on_open_for_pd ()
{
	//selected_fhs.clear ();
	//if (!hoopsview->get_selected_elements (NULL, NULL, &selected_fhs)){
	//	QMessageBox::warning (this, tr("警告"), tr("未选中任何网格面！"), QMessageBox::Ok);
	//	return;
	//}
	//hoopsview->clear_selection ();
	//hoopsview->begin_camera_manipulate ();

	//std::unordered_set<OvmCeH> all_chs_mesh;
	//foreach (auto &fh, selected_fhs){
	//	JC::get_adj_hexas_incident_face (mesh, fh, all_chs_mesh);
	//}

	//std::vector<std::unordered_set<OvmCeH> > sep_chs_sets;
	//std::unordered_set<OvmFaH> tmp_fhs;
	//JC::vector_to_unordered_set (selected_fhs, tmp_fhs);
	//JC::get_separate_chs_sets (mesh, &all_chs_mesh, &tmp_fhs, &sep_chs_sets);
	//auto group = new VolumeMeshElementGroup (mesh, "si", "quad set for inflation");
	//group->chs = sep_chs_sets.front ();
	//MeshRenderOptions render_options;
	//render_options.cell_color = "blue";
	//hoopsview->render_mesh_group (group, render_options);

	//group = new VolumeMeshElementGroup (mesh, "si", "quad set for inflation");
	//group->chs = sep_chs_sets.back ();
	//render_options.cell_color = "pink";
	//hoopsview->render_mesh_group (group, render_options);

	hoopsview->begin_select_by_click ();
	hoopsview->set_faces_selectable (true);
	hoopsview->show_mesh_faces (true);
}

std::unordered_set<OvmCeH> get_chs_within_depth3 (VolumeMesh *mesh, int max_depth, std::unordered_set<OvmFaH> &interface_fhs,
	std::unordered_set<OvmFaH> &adjoint_fhs, std::unordered_set<OvmFaH> &side_fhs)
{
	//对深度进行控制
	std::unordered_set<OvmCeH> chs_less_than_min_depth;
	int depth = 0;
	auto curr_front_fhs = interface_fhs;
	while (depth++ < max_depth){
		//获得curr_front_fhs上的点
		std::unordered_set<OvmVeH> curr_front_vhs;
		foreach (auto &fh, curr_front_fhs){
			auto heh_vec = mesh->face (fh).halfedges ();
			foreach (auto &heh, heh_vec)
				curr_front_vhs.insert (mesh->halfedge (heh).to_vertex ());
		}
		//由这些点得到所有的相邻六面体
		foreach (auto &vh, curr_front_vhs){
			JC::get_adj_hexas_around_vertex (mesh, vh, chs_less_than_min_depth);
		}
		//得到表面四边形面集
		curr_front_fhs.clear ();
		std::unordered_set<OvmFaH> bound_fhs;
		JC::collect_boundary_element (mesh, chs_less_than_min_depth, NULL, NULL, &bound_fhs);
		//剔除掉bound_fhs中在interface_fhs上以及在侧面表面上的四边形(其实就是在几何表面的四边形)
		foreach (auto &fh, bound_fhs){
			if (!mesh->is_boundary (fh))
				curr_front_fhs.insert (fh);
			else{
				if (!JC::contains (interface_fhs, fh))
					side_fhs.insert (fh);
			}
		}
	}
	adjoint_fhs = curr_front_fhs; 
	return chs_less_than_min_depth;
}

void MeshEditController::on_show_interface_patches_for_pd ()
{
	selected_fhs.clear ();
	if (!hoopsview->get_selected_elements (NULL, NULL, &selected_fhs)){
		QMessageBox::warning (this, tr("警告"), tr("未选中任何网格边！"), QMessageBox::Ok);
		return;
	}
	hoopsview->clear_selection ();
	hoopsview->begin_camera_manipulate ();
	std::unordered_set<OvmFaH> interface_fhs;
	JC::vector_to_unordered_set (selected_fhs, interface_fhs);
	std::unordered_set<OvmFaH> all_fhs;
	for (int i = 1; i != 12; ++i){
		std::unordered_set<OvmFaH> adjoint_fhs, side_fhs;
		auto chs = get_chs_within_depth3 (mesh, i, interface_fhs, adjoint_fhs, side_fhs);
		foreach (auto &fh, adjoint_fhs)
			all_fhs.insert (fh);
	}
	auto group = new VolumeMeshElementGroup (mesh, "si", "quad set for inflation");
	group->fhs = all_fhs;
	MeshRenderOptions render_options;
	render_options.face_color = "red";
	foreach (auto &fh, all_fhs)
		JC::get_adj_edges_around_face (mesh, fh, group->ehs);
	render_options.edge_color = "black";
	render_options.edge_width = 2;
	hoopsview->render_mesh_group (group, render_options);
	hoopsview->show_mesh_faces (false);

	//std::unordered_set<OvmFaH> interface_fhs;
	//auto V_ENTITY_PTR = mesh->request_vertex_property<unsigned int> ("entityptr");

	//for (auto bf_it = mesh->bf_iter (); bf_it; ++bf_it){
	//	auto vhs_on_fh = JC::get_adj_vertices_around_face (mesh, *bf_it);
	//	FACE *f = JC::get_associated_geometry_face_of_boundary_fh (mesh, *bf_it);
	//	assert (f);
	//	if (f == inter_face) interface_fhs.insert (*bf_it);
	//}
	//std::unordered_set<OvmEgH> sep_ehs;
	//JC::vector_to_unordered_set (selected_ehs, sep_ehs);
	//std::vector<std::unordered_set<OvmFaH> > fhs_patches;
	//JC::get_separate_fhs_patches (mesh, interface_fhs, sep_ehs, fhs_patches);


}

void MeshEditController::on_reach_depth_for_pd ()
{

}

void MeshEditController::on_select_vertices_for_pd ()
{
	hoopsview->begin_select_by_click ();

	hoopsview->show_mesh_faces (false);
	hoopsview->show_mesh_edges (true);
	hoopsview->show_mesh_vertices (true);

}

void MeshEditController::on_shortest_path_for_pd ()
{
	if (selected_ehs.empty ()){
		QMessageBox::critical (this, tr("错误"), "selected ehs 为空！", QMessageBox::Ok);
		return;
	}
	std::unordered_set<OvmEgH> selected_ehs_set;
	JC::vector_to_unordered_set (selected_ehs, selected_ehs_set);

	if (ossi_handler) delete ossi_handler;
	ossi_handler = new OneSimpleSheetInflationHandler (mesh, body, hoopsview);
	ossi_handler->init ();

	if (!ossi_handler->analyze_input_edges (selected_ehs_set)){
		QMessageBox::critical (this, tr("错误"), ossi_handler->get_last_err (), QMessageBox::Ok);
		return;
	}

	if (!ossi_handler->is_complete ()){
		QMessageBox::critical (this, tr("错误"), "当前只能处理完整的loop！", QMessageBox::Ok);
		return;
	}

	auto group = new VolumeMeshElementGroup (mesh, "tmp", "hexa set");
	foreach (auto &eh, selected_ehs)
		group->ehs.insert (eh);
	MeshRenderOptions render_options1;
	render_options1.edge_color = "red";
	render_options1.edge_width = 8;
	hoopsview->render_mesh_group (group, render_options1);

	shrink_set = ossi_handler->get_hexa_set ();
	hoopsview->show_mesh_faces (false);
}

void MeshEditController::on_save_selected_for_pd ()
{
	auto group = new VolumeMeshElementGroup (mesh);

	
	QString name_str = QInputDialog::getText (this, "输入名字", "保存名字");
	if (name_str != ""){
		if (name_str == "int-loop"){
			JC::vector_to_unordered_set (selected_vhs, group->vhs);
			JC::vector_to_unordered_set (selected_ehs, group->ehs);
			JC::vector_to_unordered_set (selected_fhs, group->fhs);
			JC::save_mesh_element_group (group, name_str.toAscii ().data ());
		}
		else if (name_str == "diag-hex"){
			group->chs = selected_chs;
			QString tmp = name_str;
			JC::save_mesh_element_group (group, tmp.toAscii ().data ());
		}
	}
}

void MeshEditController::on_load_selected_for_pd ()
{
	//QString name_str = QInputDialog::getText (this, "输入名字", "读取名字");
	//if (name_str == "int-loop"){
	//	auto group = JC::load_mesh_element_group (name_str.toAscii ().data ());
	//	group->mesh = mesh;
	//	MeshRenderOptions render_options;
	//	render_options.edge_color = "red";
	//	render_options.edge_width = 8;
	//	hoopsview->render_mesh_group (group, render_options);

	//	selected_vhs.clear (); selected_ehs.clear (); selected_fhs.clear ();
	//	foreach (auto &vh, group->vhs)
	//		selected_vhs.push_back (vh);
	//	foreach (auto &eh, group->ehs)
	//		selected_ehs.push_back (eh);
	//	foreach (auto &fh, group->fhs)
	//		selected_fhs.push_back (fh);
	//}
	//else if (name_str == "diag-hex"){
	//	QString tmp = name_str + "1";
	//	tmp_group_for_pd1 = JC::load_mesh_element_group (tmp.toAscii ().data ());
	//	tmp = name_str + "2";
	//	tmp_group_for_pd2 = JC::load_mesh_element_group (tmp.toAscii ().data ());
	//}

	selected_ehs.clear ();
	if (!hoopsview->get_selected_elements (std::vector<OvmVeH>(), selected_ehs, std::vector<OvmFaH> ())){
		QMessageBox::warning (this, tr("警告"), tr("未选中任何网格边！"), QMessageBox::Ok);
		return;
	}
	hoopsview->clear_selection ();
	hoopsview->begin_camera_manipulate ();

	//render
	auto group = new VolumeMeshElementGroup (mesh, "lsi", "loop edges");
	JC::vector_to_unordered_set (selected_ehs, group->ehs);
	MeshRenderOptions render_options;
	render_options.edge_color = "blue";
	render_options.edge_width = 6;
	hoopsview->render_mesh_group (group, render_options);

	hoopsview->show_mesh_faces (false);
}

void MeshEditController::on_adjust_diag_hexa_for_pd ()
{
	//auto diag_group1 = *(hoopsview->retrieve_mesh_groups ("tmp", "hexa set 1").begin ());
	//auto diag_group2 = *(hoopsview->retrieve_mesh_groups ("tmp", "hexa set 2").begin ());

	//hoopsview->show_mesh_faces (false);
	//

	selected_ehs.clear ();
	if (!hoopsview->get_selected_elements (std::vector<OvmVeH>(), selected_ehs, std::vector<OvmFaH> ())){
		QMessageBox::warning (this, tr("警告"), tr("未选中任何网格边！"), QMessageBox::Ok);
		return;
	}
	hoopsview->clear_selection ();
	hoopsview->begin_camera_manipulate ();

	//render
	auto group = new VolumeMeshElementGroup (mesh, "lsi", "loop edges");
	JC::vector_to_unordered_set (selected_ehs, group->ehs);
	MeshRenderOptions render_options;
	render_options.edge_color = "red";
	render_options.edge_width = 8;
	hoopsview->render_mesh_group (group, render_options);
}

void MeshEditController::on_select_faces_for_pd ()
{
	hoopsview->begin_select_by_click ();
}

void MeshEditController::on_select_faces_ok_for_pd ()
{
	selected_fhs.clear ();
	if (!hoopsview->get_selected_elements (NULL, NULL, &selected_fhs)){
		QMessageBox::warning (this, tr("警告"), tr("未选中任何网格面！"), QMessageBox::Ok);
		return;
	}
	hoopsview->clear_selection ();
	hoopsview->begin_camera_manipulate ();

	std::unordered_set<OvmCeH> selected_chs_for_render;
	std::unordered_set<OvmFaH> selected_fhs_for_render, inter_fhs;
	foreach (auto &fh, selected_fhs){
		std::unordered_set<OvmCeH> adj_chs;
		JC::get_adj_hexas_incident_face (mesh, fh, adj_chs);
		bool has_adj_ch = false;
		foreach (auto &ch, adj_chs){
			if (JC::contains (chs_for_rendering, ch)){
				selected_chs_for_render.insert (ch); 
				has_adj_ch = true;
				break;
			}
		}
		if (!has_adj_ch) selected_fhs_for_render.insert (fh);
	}

	std::vector<OvmCeH> chs_vec;
	JC::unordered_set_to_vector (selected_chs_for_render, chs_vec);
	if (!chs_vec.empty ()){
		for (int i = 0; i != chs_vec.size () - 1; ++i){
			std::unordered_set<OvmFaH> adj_fhs1;
			JC::get_adj_faces_around_hexa (mesh, chs_vec[i], adj_fhs1);
			for (int j = i + 1; j != chs_vec.size (); ++j){
				std::unordered_set<OvmFaH> adj_fhs2;
				JC::get_adj_faces_around_hexa (mesh, chs_vec[j], adj_fhs2);
				foreach (auto &fh1, adj_fhs1){
					foreach (auto &fh2, adj_fhs2){
						if (fh1 == fh2) inter_fhs.insert (fh1);
					}
				}
			}
		}
	}


	std::unordered_set<OvmEgH> one_ehs, other_ehs;
	foreach (auto &fh, inter_fhs){
		JC::get_adj_edges_around_face (mesh, fh, one_ehs);
	}

	foreach (auto &ch, selected_chs_for_render){
		std::unordered_set<OvmEgH> ehs_tmp;
		JC::get_adj_edges_around_cell (mesh, ch, ehs_tmp);
		foreach (auto &eh, ehs_tmp){
			if (!JC::contains (one_ehs, eh)) other_ehs.insert (eh);
		}
	}
	foreach (auto &fh, selected_fhs_for_render){
		std::unordered_set<OvmEgH> ehs_tmp;
		JC::get_adj_edges_around_face (mesh, fh, ehs_tmp);
		foreach (auto &eh, ehs_tmp){
			if (!JC::contains (one_ehs, eh)) other_ehs.insert (eh);
		}
	}
	hoopsview->derender_all_mesh_groups ();
	auto group = new VolumeMeshElementGroup (mesh, "pd", "pd1");
	MeshRenderOptions render_options;
	render_options.edge_width = 6;
	render_options.edge_color = "blue green";
	group->ehs = one_ehs;
	hoopsview->render_mesh_group (group, render_options);

	group = new VolumeMeshElementGroup (mesh, "pd", "pd1");
	render_options.edge_color = "blue green";
	group->ehs = other_ehs;
	hoopsview->render_mesh_group (group, render_options);
	hoopsview->show_mesh_edges (false);
	hoopsview->show_mesh_faces (false);
}
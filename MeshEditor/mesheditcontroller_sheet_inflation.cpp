#include "StdAfx.h"
#include "mesheditcontroller.h"
#include "hoopsview.h"
#include <QMessageBox>
#include <QInputDialog>
#include "mesh_min_cut.h"

void MeshEditController::on_select_edges_for_si ()
{
	hoopsview->show_mesh_faces (true);

	if (!mouse_controller){
		mouse_controller = new MouseControlWidget (hoopsview, this);
		mouse_controller->setWindowFlags (mouse_controller->windowFlags () | Qt::Tool);
	}
	connect (mouse_controller, SIGNAL (opt_ok()), SLOT (on_select_edges_ok_for_si ()));
	mouse_controller->start_selecting_edges ();
	mouse_controller->show ();
}

void MeshEditController::on_select_edges_ok_for_si ()
{
	selected_ehs.clear ();
	if (!hoopsview->get_selected_elements (std::vector<OvmVeH>(), selected_ehs, std::vector<OvmFaH> ())){
		QMessageBox::warning (this, tr("警告"), tr("未选中任何网格边！"), QMessageBox::Ok);
		mouse_controller->start_selecting_edges ();
		mouse_controller->show ();
		return;
	}

	disconnect (mouse_controller, SIGNAL (opt_ok()), this, SLOT (on_select_edges_ok_for_si ()));
	hoopsview->clear_selection ();
	mouse_controller->start_camera_manipulate ();

	
	if (!loops_ehs.empty ()){
		QMessageBox msgBox;
		msgBox.setText (tr("添加还是全新选择？"));
		msgBox.setInformativeText (tr("当前loop列表不为空，添加所选吗？若全新选择请选择取消。"));
		msgBox.setStandardButtons (QMessageBox::Ok | QMessageBox::Cancel);
		auto sel = msgBox.exec ();
		if (sel == QMessageBox::Cancel)
			loops_ehs.clear ();		
	}
	foreach (auto eh, selected_ehs)
		loops_ehs.insert (eh);
	
	hoopsview->derender_mesh_groups ("sheet inflation", "loop edges");
	//render
	auto group = new VolumeMeshElementGroup (mesh, "sheet inflation", "loop edges");
	group->ehs = loops_ehs;
	MeshRenderOptions render_options;
	render_options.edge_color = "green";
	render_options.edge_width = 3;
	hoopsview->render_mesh_group (group, render_options);
}

void MeshEditController::on_read_loop_ehs_for_si ()
{
	QString file_path = QFileDialog::getOpenFileName (this, tr("选择loop文件"), QString(), "Loop Files(*.loop)");
	if (file_path != ""){
		loops_ehs.clear ();
		QFile file (file_path);
		file.open (QIODevice::ReadOnly);
		QDataStream in (&file);
		QVector<int> loop_ehs_vec;
		in>>loop_ehs_vec;
		foreach (auto eh_idx, loop_ehs_vec)
			loops_ehs.insert (OvmEgH (eh_idx));
		file.close ();

		hoopsview->derender_mesh_groups ("sheet inflation", "loop edges");
		//render
		auto group = new VolumeMeshElementGroup (mesh, "sheet inflation", "loop edges");
		group->ehs = loops_ehs;
		MeshRenderOptions render_options;
		render_options.edge_color = "green";
		render_options.edge_width = 3;
		hoopsview->render_mesh_group (group, render_options);
	}
}

void MeshEditController::on_save_loop_ehs_for_si ()
{
	QString file_path = QFileDialog::getSaveFileName (this, tr("选择保存位置"), QString(), "Loop Files(*.loop)");
	if (file_path != ""){
		QFile file (file_path);
		file.open (QIODevice::WriteOnly);
		QDataStream out (&file);
		QVector<int> loop_ehs_vec;
		foreach (auto eh, loops_ehs)
			loop_ehs_vec.push_back (eh.idx ());
		out<<loop_ehs_vec;
		file.close ();
	}
}

void MeshEditController::on_get_quad_set_by_sweep_for_si ()
{
	if (loops_ehs.empty ()){
		QMessageBox::warning (this, tr("警告"), tr("未选择任何表面网格边！"), QMessageBox::Ok);
		return;
	}

	std::unordered_set<OvmFaH> inf_fhs;
	auto first_eh = *(loops_ehs.begin ());
	auto adj_fhs = JC::get_adj_faces_around_edge (mesh, first_eh);
	foreach (auto &fh, adj_fhs){
		if (!mesh->is_boundary (fh))
			inf_fhs.insert (fh);
	}
	
	sweeping_quad_sets.clear ();
	foreach (auto dire_fh, inf_fhs){
		std::unordered_set<OvmFaH> sweeping_quad_set;
		if (JC::get_sweeping_quad_set (mesh, loops_ehs, dire_fh, sweeping_quad_set))
			sweeping_quad_sets.push_back (sweeping_quad_set);
	}
	if (sweeping_quad_sets.empty ()){
		QMessageBox::warning (this, tr("警告"), tr("这组网格边不能够扫略得到四边形面集！"), QMessageBox::Ok);
		return;
	}else if (sweeping_quad_sets.size () == 1){
		on_get_quad_set_by_sweep_direction_changed_for_si (0);
	}else{
		if (!choices_controller){
			choices_controller = new ChoicesSelectionWidget (this);
			choices_controller->setWindowFlags (choices_controller->windowFlags () | Qt::Tool);
		}

		choices_controller->setChoicesNum (1);
		QStringList choices_labels;
		choices_labels<<tr("扫层方向:");
		choices_controller->setChoicesLabels (choices_labels);
		QStringList choice_txt;
		for (int i = 0; i != inf_fhs.size (); ++i){
			choice_txt<<QString ("%1").arg (i);
		}
		choices_controller->setChoices (0, choice_txt);

		connect (choices_controller, SIGNAL (choice1_changed (int)), 
			SLOT (on_get_quad_set_by_sweep_direction_changed_for_si (int)));
		connect (choices_controller, SIGNAL (choosing_ok ()), SLOT (on_get_quad_set_by_sweep_direction_changed_ok_for_si ()));

		choices_controller->show ();
		hoopsview->show_mesh_faces (false);

		on_get_quad_set_by_sweep_direction_changed_for_si (0);
	}
}

void MeshEditController::on_get_quad_set_by_sweep_direction_changed_for_si (int idx)
{
	auto sweep_fhs = sweeping_quad_sets[idx];

	hoopsview->derender_mesh_groups ("sheet inflation", "quad set for inflation", true);
	auto group = new VolumeMeshElementGroup (mesh, "sheet inflation", "quad set for inflation");
	group->fhs = sweep_fhs;
	group->add_edges_on_quad_set ();

	MeshRenderOptions render_options;
	render_options.face_color = "blue";
	render_options.edge_color = "red";
	render_options.edge_width = 6;
	hoopsview->render_mesh_group (group, render_options);
	hoopsview->show_mesh_faces (false);

	inflation_quad_set = sweep_fhs;

}

void MeshEditController::on_get_quad_set_by_sweep_direction_changed_ok_for_si ()
{
	disconnect (choices_controller, SIGNAL (choosing_ok ()), this, 
		SLOT (on_get_quad_set_by_sweep_direction_changed_ok_for_si ()));
}

std::unordered_set<OvmCeH> get_chs_within_depth (VolumeMesh *mesh, int max_depth, 
	std::unordered_set<OvmFaH> &fhs, std::unordered_set<OvmFaH> &rest_fhs,
	std::unordered_set<OvmFaH> &adjoint_fhs)
{
	int depth = 0;
	auto curr_front_fhs = fhs, curr_rest_fhs = rest_fhs;
	std::unordered_set<OvmCeH> wanted_chs, rest_chs;
	while (depth++ < max_depth){
		JC::get_direct_adjacent_hexas (mesh, curr_front_fhs, wanted_chs);
		
		//获得curr_front_fhs上的点
		std::unordered_set<OvmVeH> curr_rest_vhs;
		foreach (auto &fh, curr_rest_fhs){
			auto heh_vec = mesh->face (fh).halfedges ();
			foreach (auto &heh, heh_vec)
				curr_rest_vhs.insert (mesh->halfedge (heh).to_vertex ());
		}
		//由这些点得到所有的相邻六面体
		foreach (auto &vh, curr_rest_vhs){
			JC::get_adj_hexas_around_vertex (mesh, vh, rest_chs);
		}

		foreach (auto &ch, wanted_chs){
			rest_chs.erase (ch);
		}

		//得到表面四边形面集
		curr_front_fhs.clear (); curr_rest_fhs.clear ();
		std::unordered_set<OvmFaH> bound_front_fhs, bound_rest_fhs;
		JC::collect_boundary_element (mesh, wanted_chs, NULL, NULL, &bound_front_fhs);
		JC::collect_boundary_element (mesh, rest_chs, NULL, NULL, &bound_rest_fhs);
		foreach (auto &fh, bound_front_fhs){
			if (!mesh->is_boundary (fh) && !JC::contains (bound_rest_fhs, fh))
				curr_front_fhs.insert (fh);
		}
		foreach (auto &fh, bound_rest_fhs){
			if (!mesh->is_boundary (fh) && !JC::contains (bound_front_fhs, fh))
				curr_rest_fhs.insert (fh);
		}
	}
	std::unordered_set<OvmFaH> bound_fhs;
	JC::collect_boundary_element (mesh, wanted_chs, NULL, NULL, &bound_fhs);
	foreach (auto &fh, bound_fhs){
		if (!mesh->is_boundary (fh))
			adjoint_fhs.insert (fh);
	}
	
	return wanted_chs;
}

void MeshEditController::on_get_quad_set_directly_for_si ()
{
	std::unordered_set<OvmFaH> bound_fhs;
	for (auto bf_it = mesh->bf_iter(); bf_it; ++bf_it)
	{
		bound_fhs.insert (*bf_it);
	}

	std::vector<std::unordered_set<OvmFaH> > fhs_patches;
	JC::get_separate_fhs_patches (mesh, bound_fhs, loops_ehs, fhs_patches);

	std::vector<std::unordered_set<OvmCeH> > hex_sets;
	foreach (auto &fhs_patch, fhs_patches){
		std::unordered_set<OvmCeH> hex_set;
		JC::get_direct_adjacent_hexas (mesh, fhs_patch, hex_set);
		hex_sets.push_back (hex_set);
	}

	inflation_quad_set.clear ();
	for (int i = 0; i != hex_sets.size () - 1; ++i){
		std::unordered_set<OvmCeH> other_chs;
		for (int j = 0; j != hex_sets.size (); ++j){
			if (j == i) continue;
			foreach (auto &ch, hex_sets[j]) other_chs.insert (ch);
		}
		auto sep_fhs = get_volume_mesh_min_cut (mesh, hex_sets[i], other_chs);
		foreach (auto fh, sep_fhs)
			inflation_quad_set.insert (fh);
	}

	hoopsview->derender_mesh_groups ("sheet inflation", "quad set for inflation", true);
	auto group = new VolumeMeshElementGroup (mesh, "sheet inflation", "quad set for inflation");
	group->fhs = inflation_quad_set;
	group->add_edges_on_quad_set ();

	MeshRenderOptions render_options;
	render_options.face_color = "blue";
	render_options.edge_color = "red";
	render_options.edge_width = 6;
	hoopsview->render_mesh_group (group, render_options);
	hoopsview->show_mesh_faces (false);
}

void MeshEditController::on_pcs_depth_get_quad_set_for_si ()
{
	if (loops_ehs.empty ()){
		QMessageBox::warning (this, tr("警告"), tr("未选择任何表面网格边！"), QMessageBox::Ok);
		return;
	}

	if (int_quads.empty ()){
		get_interface_quads ();
	}

	JC::get_separate_fhs_patches (mesh, int_quads, loops_ehs, quad_sets_on_interfaces);
	if (!choices_controller){
		choices_controller = new ChoicesSelectionWidget (this);
		choices_controller->setWindowFlags (choices_controller->windowFlags () | Qt::Tool);
	}

	choices_controller->setChoicesNum (2);
	QStringList choices_labels;
	choices_labels<<tr("Quad Set:")<<tr("深度：");
	choices_controller->setChoicesLabels (choices_labels);
	QStringList choice_txt;
	for (int i = 0; i != quad_sets_on_interfaces.size (); ++i){
		choice_txt<<QString ("%1").arg (i);
	}
	choices_controller->setChoices (0, choice_txt);
	choice_txt.clear ();
	for (int i = 1; i != 10; ++i){
		choice_txt<<QString ("%1").arg (i);
	}
	choices_controller->setChoices (1, choice_txt);

	connect (choices_controller, SIGNAL (choice1_changed (int)), SLOT (on_pcs_quad_set_changed_for_si (int)));
	connect (choices_controller, SIGNAL (choice2_changed (int)), SLOT (on_pcs_depth_changed_for_si (int)));
	connect (choices_controller, SIGNAL (choosing_ok ()), SLOT (on_pcs_choosing_ok_for_si ()));

	choices_controller->show ();
	hoopsview->show_mesh_faces (false);

	on_pcs_quad_set_changed_for_si (0);
	selected_quad_set_on_interface_idx = 0;
}

void MeshEditController::on_pcs_quad_set_changed_for_si (int idx)
{
	hoopsview->derender_mesh_groups ("sheet inflation", "pcs quad set", true);
	auto group = new VolumeMeshElementGroup (mesh, "sheet inflation", "pcs quad set");
	group->fhs = quad_sets_on_interfaces[idx];
	group->add_edges_on_quad_set ();

	MeshRenderOptions render_options;
	render_options.face_color = "blue";
	render_options.edge_color = "red";
	render_options.edge_width = 6;
	hoopsview->render_mesh_group (group, render_options);
	hoopsview->show_mesh_faces (false);
	
	selected_quad_set_on_interface_idx = idx;
}

void MeshEditController::on_pcs_depth_changed_for_si (int depth)
{
	std::unordered_set<OvmFaH> front_fhs, rest_fhs;
	for (int i = 0; i != quad_sets_on_interfaces.size (); ++i){
		if (i == selected_quad_set_on_interface_idx) continue;
		foreach (auto fh, quad_sets_on_interfaces[i])
			rest_fhs.insert (fh);
	}
	auto chs_within_depth = get_chs_within_depth (mesh, depth, quad_sets_on_interfaces[selected_quad_set_on_interface_idx], 
		rest_fhs, front_fhs);

	inflation_quad_set = front_fhs;
	shrink_set = chs_within_depth;

	hoopsview->derender_mesh_groups ("sheet inflation", "quad set for inflation", true);
	auto group = new VolumeMeshElementGroup (mesh, "sheet inflation", "quad set for inflation");
	group->fhs = inflation_quad_set;
	group->add_edges_on_quad_set ();

	MeshRenderOptions render_options;
	render_options.face_color = "blue";
	render_options.edge_color = "red";
	render_options.edge_width = 6;
	hoopsview->render_mesh_group (group, render_options);
	hoopsview->show_mesh_faces (false);
}

void MeshEditController::on_pcs_choosing_ok_for_si ()
{
	disconnect (choices_controller, SIGNAL (choice1_changed (int)), this, SLOT (on_pcs_quad_set_changed_for_si (int)));
	disconnect (choices_controller, SIGNAL (choice2_changed (int)), this, SLOT (on_pcs_depth_changed_for_si (int)));
	disconnect (choices_controller, SIGNAL (choosing_ok ()), this, SLOT (on_pcs_choosing_ok_for_si ()));
}

void MeshEditController::on_modify_quad_set_for_si ()
{
	hoopsview->derender_mesh_groups ("sheet inflation", "quad set for inflation", true);
	hoopsview->derender_mesh_groups ("sheet inflation", "shrink set", true);

	auto group = new VolumeMeshElementGroup (mesh, "sheet inflation", "shrink set");
	group->chs = shrink_set;
	MeshRenderOptions render_options;
	render_options.cell_color = "(diffuse=red,transmission = (r=0.2 g=0.2 b=0.2))";
	render_options.cell_size = 0.7;
	hoopsview->render_mesh_group (group, render_options);
	hoopsview->show_mesh_faces (false);

	mouse_controller->start_selecting_faces ();
	mouse_controller->show ();
}

void MeshEditController::on_add_quad_for_si ()
{
	selected_fhs.clear ();
	if (!hoopsview->get_selected_elements (NULL, NULL, &selected_fhs)){
		QMessageBox::warning (this, tr("警告"), tr("未选中任何网格面！"), QMessageBox::Ok);
		return;
	}

	foreach (auto fh, selected_fhs){
		std::unordered_set<OvmCeH> adj_chs;
		JC::get_adj_hexas_incident_face (mesh, fh, adj_chs);
		foreach (auto ch, adj_chs)
			shrink_set.insert (ch);
	}

	hoopsview->derender_mesh_groups ("sheet inflation", "shrink set", true);

	auto group = new VolumeMeshElementGroup (mesh, "sheet inflation", "shrink set");
	group->chs = shrink_set;
	MeshRenderOptions render_options;
	render_options.cell_color = "(diffuse=red,transmission = (r=0.2 g=0.2 b=0.2))";
	render_options.cell_size = 0.7;
	hoopsview->render_mesh_group (group, render_options);
}

void MeshEditController::on_remove_quad_for_si ()
{
	selected_fhs.clear ();
	if (!hoopsview->get_selected_elements (NULL, NULL, &selected_fhs)){
		QMessageBox::warning (this, tr("警告"), tr("未选中任何网格面！"), QMessageBox::Ok);
		return;
	}

	foreach (auto fh, selected_fhs){
		std::unordered_set<OvmCeH> adj_chs;
		JC::get_adj_hexas_incident_face (mesh, fh, adj_chs);
		foreach (auto ch, adj_chs)
			shrink_set.erase (ch);
	}

	hoopsview->derender_mesh_groups ("sheet inflation", "shrink set", true);

	auto group = new VolumeMeshElementGroup (mesh, "sheet inflation", "shrink set");
	group->chs = shrink_set;
	MeshRenderOptions render_options;
	render_options.cell_color = "(diffuse=red,transmission = (r=0.2 g=0.2 b=0.2))";
	render_options.cell_size = 0.7;
	hoopsview->render_mesh_group (group, render_options);
}

void MeshEditController::on_modify_quad_set_ok_for_si ()
{
	hoopsview->derender_mesh_groups ("sheet inflation", "shrink set", true);
	std::unordered_set<OvmFaH> tmp_bnd_fhs;
	JC::collect_boundary_element (mesh, shrink_set, NULL, NULL, &tmp_bnd_fhs);

	inflation_quad_set.clear ();
	foreach (auto fh, tmp_bnd_fhs){
		if (!mesh->is_boundary (fh))
			inflation_quad_set.insert (fh);
	}

	auto group = new VolumeMeshElementGroup (mesh, "sheet inflation", "quad set for inflation");
	group->fhs = inflation_quad_set;
	group->add_edges_on_quad_set ();

	MeshRenderOptions render_options;
	render_options.face_color = "blue";
	render_options.edge_color = "red";
	render_options.edge_width = 6;
	hoopsview->render_mesh_group (group, render_options);

	mouse_controller->start_camera_manipulate ();
	mouse_controller->hide ();
}

void MeshEditController::on_determine_shrink_set_for_si ()
{
	if (inflation_quad_set.empty ()){
		QMessageBox::warning (this, tr("警告"), tr("当前inflation quad set为空！"), QMessageBox::Ok);
		return;
	}

	hex_sets_sep_by_inf_fhs.clear ();
	JC::get_separate_chs_sets (mesh, &inflation_quad_set, &hex_sets_sep_by_inf_fhs);

	choices_controller->setChoicesNum (1);
	QStringList choices_labels;
	choices_labels<<tr("Shrink Set:");
	choices_controller->setChoicesLabels (choices_labels);
	QStringList choice_txt;
	for (int i = 0; i != hex_sets_sep_by_inf_fhs.size (); ++i){
		choice_txt<<QString ("%1").arg (i);
	}
	choices_controller->setChoices (0, choice_txt);

	connect (choices_controller, SIGNAL (choice1_changed (int)), SLOT (on_shrink_set_changed_for_si (int)));

	choices_controller->show ();
	hoopsview->show_mesh_faces (false);
	on_shrink_set_changed_for_si (0);
}

void MeshEditController::on_shrink_set_changed_for_si (int idx)
{
	shrink_set = hex_sets_sep_by_inf_fhs[idx];

	hoopsview->derender_mesh_groups ("sheet inflation", "shrink set", true);
	auto group = new VolumeMeshElementGroup (mesh, "sheet inflation", "shrink set");
	group->chs = shrink_set;

	MeshRenderOptions render_options;
	render_options.cell_color = "pink";
	render_options.cell_size = 0.8;
	hoopsview->render_mesh_group (group, render_options);
	hoopsview->show_mesh_faces (false);
}

void MeshEditController::on_sheet_inflation_for_si ()
{
	new_sheets.clear ();
	std::unordered_set<OvmEgH> int_ehs;
	std::map<OvmEgH, int> ehs_count;
	foreach (auto &fh, inflation_quad_set){
		auto heh_vec = mesh->face (fh).halfedges ();
		foreach (auto &heh, heh_vec){
			auto eh = mesh->edge_handle (heh);
			ehs_count[eh]++;
		}
	}
	foreach (auto &p, ehs_count){
		if (p.second == 4)
			int_ehs.insert (p.first);
	}

	old_new_vhs_mapping.clear ();
	new_sheets = JC::one_simple_sheet_inflation (mesh, inflation_quad_set, shrink_set, int_ehs, old_new_vhs_mapping);

	hoopsview->derender_all_mesh_groups ();
	//JC::smooth_volume_mesh (mesh, body, 1, hoopsview);
	update_mesh ();

	
	MeshRenderOptions render_options;
	render_options.cell_color = "red";
	foreach (auto &sheet, new_sheets)
		hoopsview->render_mesh_group (sheet, render_options);
}

void MeshEditController::on_postprocess_for_si ()
{
	loops_ehs.clear ();
	hoopsview->derender_all_mesh_groups ();
	hoopsview->show_mesh_faces (true);
	hoopsview->show_mesh_edges (true);
	shrink_set.clear ();
	inflation_quad_set.clear ();
}

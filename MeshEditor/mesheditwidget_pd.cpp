#include "StdAfx.h"
#include "mesheditwidget.h"
#include <QColorDialog>
#include <QInputDialog>
#include "mesh_min_cut.h"

void MeshEditWidget::on_set_color_for_pd ()
{
	auto tmp_color = QColorDialog::getColor (pd_group_color, this);
	if (tmp_color.isValid ()){
		pd_group_color = tmp_color;
		ui.label_group_color->setStyleSheet (QString("background-color: rgb(%1, %2, %3);")
			.arg (pd_group_color.red ()).arg (pd_group_color.green ()).arg (pd_group_color.blue ()));

		update_temp_element_group ();
	}
}

void MeshEditWidget::on_set_line_weight_for_pd ()
{
	pd_line_weight = ui.doubleSpinBox_Line_Weight->value ();
	update_temp_element_group ();
}

void MeshEditWidget::on_set_cell_ratio_for_pd ()
{
	pd_cell_ratio = ui.doubleSpinBox_Cell_Ratio->value ();
	update_temp_element_group ();
}

void MeshEditWidget::on_add_cells_for_pd ()
{
	if (pd_temp_group == NULL){
		pd_temp_group = new VolumeMeshElementGroup (mesh, "tmp", "tmp");
	}

	selected_fhs.clear ();
	if (!hoopsview->get_selected_elements (NULL, NULL, &selected_fhs)){
		QMessageBox::warning (this, tr("警告"), tr("未选中任何网格面！"), QMessageBox::Ok);
		return;
	}
	foreach (auto &fh, selected_fhs){
		JC::get_adj_hexas_incident_face (mesh, fh, pd_temp_group->chs);
	}

	update_temp_element_group ();
}

void MeshEditWidget::on_remove_cells_for_pd ()
{
	if (pd_temp_group == NULL){
		pd_temp_group = new VolumeMeshElementGroup (mesh, "tmp", "tmp");
	}

	selected_fhs.clear ();
	if (!hoopsview->get_selected_elements (NULL, NULL, &selected_fhs)){
		QMessageBox::warning (this, tr("警告"), tr("未选中任何网格面！"), QMessageBox::Ok);
		return;
	}

	foreach (auto &fh, selected_fhs){
		std::unordered_set<OvmCeH> adj_chs;
		JC::get_adj_hexas_incident_face (mesh, fh, adj_chs);
		foreach (auto &ch, adj_chs)
			pd_temp_group->chs.erase (ch);
	}

	update_temp_element_group ();
}

void MeshEditWidget::on_add_element_for_pd ()
{
	if (pd_temp_group == NULL){
		pd_temp_group = new VolumeMeshElementGroup (mesh, "tmp", "tmp");
	}

	selected_vhs.clear ();
	selected_ehs.clear ();
	selected_fhs.clear ();
	if (!hoopsview->get_selected_elements (&selected_vhs, &selected_ehs, &selected_fhs)){
		QMessageBox::warning (this, tr("警告"), tr("未选中任何网格单元！"), QMessageBox::Ok);
		return;
	}

	foreach (auto &vh, selected_vhs)
		pd_temp_group->vhs.insert (vh);
	foreach (auto &eh, selected_ehs)
		pd_temp_group->ehs.insert (eh);
	foreach (auto &fh, selected_fhs)
		pd_temp_group->fhs.insert (fh);

	if (!selected_ehs.empty ()){
		//qs_loop.clear ();
		JC::vector_to_unordered_set (selected_ehs, qs_loop);
	}
	update_temp_element_group ();
}

void MeshEditWidget::on_remove_element_for_pd ()
{
	if (pd_temp_group == NULL){
		pd_temp_group = new VolumeMeshElementGroup (mesh, "tmp", "tmp");
	}

	selected_vhs.clear ();
	selected_ehs.clear ();
	selected_fhs.clear ();
	if (!hoopsview->get_selected_elements (&selected_vhs, &selected_ehs, &selected_fhs)){
		QMessageBox::warning (this, tr("警告"), tr("未选中任何网格单元！"), QMessageBox::Ok);
		return;
	}

	foreach (auto &vh, selected_vhs)
		pd_temp_group->vhs.erase (vh);
	foreach (auto &eh, selected_ehs)
		pd_temp_group->ehs.erase (eh);
	foreach (auto &fh, selected_fhs)
		pd_temp_group->fhs.erase (fh);

	if (!selected_ehs.empty ()){
		//qs_loop.clear ();
		foreach (auto &eh, selected_ehs)
			qs_loop.erase (eh);
	}
	update_temp_element_group ();
}

void MeshEditWidget::on_add_element_group_for_pd ()
{
	auto new_group = pd_temp_group;
	new_group->type = QInputDialog::getText (this, "Group Type", "Input group type:", QLineEdit::Normal, "normal");
	new_group->name = QInputDialog::getText (this, "Group Name", "Input group name:", QLineEdit::Normal, "group");
	pd_temp_group = new VolumeMeshElementGroup (mesh, "tmp", "tmp");
}

void MeshEditWidget::update_temp_element_group ()
{
	if (pd_temp_group){
		hoopsview->derender_one_mesh_group (pd_temp_group);
		MeshRenderOptions render_options;
		render_options.edge_width = pd_line_weight;
		render_options.cell_size = pd_cell_ratio;
		char color_buf[1000]={0};
		sprintf (color_buf, "(r=%f g=%f b=%f)", 
			pd_group_color.redF (), pd_group_color.greenF (), pd_group_color.blueF ());
		render_options.vertex_color = color_buf;
		render_options.edge_color = color_buf;
		render_options.face_color = color_buf;
		render_options.cell_color = color_buf;
		hoopsview->render_mesh_group (pd_temp_group, render_options);
		hoopsview->set_mesh_group_selectability (pd_temp_group, true, true, true);
	}
}

void MeshEditWidget::on_recursive_spreading ()
{
	if (qs_hexa_set.empty ()){
		foreach (auto &eh, qs_loop){
			OvmVeH vh1 = mesh->edge (eh).to_vertex (),
				vh2 = mesh->edge (eh).from_vertex ();
			JC::get_adj_hexas_around_vertex (mesh, vh1, qs_hexa_set);
			JC::get_adj_hexas_around_vertex (mesh, vh2, qs_hexa_set);
		}
	}else{
		auto tmp = qs_hexa_set;
		foreach (auto &ch, tmp){
			std::unordered_set<OvmVeH> adj_vhs;
			JC::get_adj_vertices_around_cell (mesh, ch, adj_vhs);
			foreach (auto &vh, adj_vhs)
				JC::get_adj_hexas_around_vertex (mesh, vh, qs_hexa_set);
		}
	}

	std::unordered_set<OvmFaH> inner_bound_fhs;
	std::unordered_set<OvmFaH> bound_fhs;
	JC::collect_boundary_element (mesh, qs_hexa_set, NULL, NULL, &bound_fhs);
	foreach (auto &fh, bound_fhs){
		if (!mesh->is_boundary (fh)) inner_bound_fhs.insert (fh);
	}

	hoopsview->derender_mesh_groups ("pd", "spread");
	hoopsview->derender_mesh_groups ("pd", "inner bound");
	MeshRenderOptions render_options;
	render_options.edge_width = pd_line_weight;
	render_options.cell_size = pd_cell_ratio;
	char color_buf[1000]={0};
	sprintf (color_buf, "(r=%f g=%f b=%f)", 
		pd_group_color.redF (), pd_group_color.greenF (), pd_group_color.blueF ());
	render_options.cell_color = color_buf;
	auto tmp_group = new VolumeMeshElementGroup (mesh, "pd", "spread");
	tmp_group->chs = qs_hexa_set;
	hoopsview->render_mesh_group (tmp_group, render_options);

	tmp_group = new VolumeMeshElementGroup (mesh, "pd", "inner bound");
	tmp_group->fhs = inner_bound_fhs;
	foreach (auto &fh, inner_bound_fhs){
		JC::get_adj_edges_around_face (mesh, fh, tmp_group->ehs);
	}
	render_options.edge_color = "black";
	hoopsview->render_mesh_group (tmp_group, render_options);
}

void MeshEditWidget::on_loop_sep ()
{
	std::unordered_set<OvmFaH> bound_fhs;
	JC::collect_boundary_element (mesh, qs_hexa_set, NULL, NULL, &bound_fhs);

	std::vector<std::unordered_set<OvmFaH> > fhs_patches;
	JC::get_separate_fhs_patches (mesh, bound_fhs, qs_loop, fhs_patches);
	pd_fhs_patches = fhs_patches;

	hoopsview->derender_mesh_groups ("pd", "sepfhs");

	char *colors[] = {"(r=0 g=0.67 b=0)", "(r=0 g=0.67 b=1)", "(r=1 g=0.33 b=0)", "(r=0.67 g=0 b=1)", "(r=1 g=0.67 b=0)", "(r=0.67 g=0 b=0.50)"};

	MeshRenderOptions render_options;
	for (int i = 0; i != pd_fhs_patches.size (); ++i){
		auto tmp_group = new VolumeMeshElementGroup (mesh, "pd", "sepfhs");
		tmp_group->fhs = pd_fhs_patches[i];
		render_options.face_color = colors[i % 6];
		hoopsview->render_mesh_group (tmp_group, render_options);
	}
}

void MeshEditWidget::on_get_st_node ()
{
	std::unordered_set<OvmFaH> all_adj_fhs;
	foreach (auto &eh, qs_loop){
		OvmVeH vh1 = mesh->edge (eh).to_vertex (),
			vh2 = mesh->edge (eh).from_vertex ();
		JC::get_adj_boundary_faces_around_vertex (mesh, vh1, all_adj_fhs);
		JC::get_adj_boundary_faces_around_vertex (mesh, vh2, all_adj_fhs);


	}

	std::unordered_set<OvmFaH> seed_fhs;
	JC::get_adj_boundary_faces_around_edge (mesh, *(qs_loop.begin ()), seed_fhs);

	auto tmp_begin = seed_fhs.begin ();
	OvmFaH seed_fh1 = *(tmp_begin),
		seed_fh2 = *(++tmp_begin);

	auto fGetOneSide = [&] (OvmFaH seed_fh)->std::unordered_set<OvmFaH>{
		std::queue<OvmFaH> spread_set;
		std::unordered_set<OvmFaH> local_adj_fhs;
		std::unordered_set<OvmFaH> one_side;
		one_side.insert (seed_fh);
		JC::get_adj_boundary_faces_around_face (mesh, seed_fh, local_adj_fhs);
		foreach (auto &fh, local_adj_fhs){
			if (!JC::contains (all_adj_fhs, fh)) continue;
			if (JC::contains (qs_loop, JC::get_common_edge_handle (mesh, seed_fh, fh)))
				continue;
			spread_set.push (fh);
			one_side.insert (fh);
		}
		
		while (!spread_set.empty ()){
			auto cur_fh = spread_set.front ();
			spread_set.pop ();

			local_adj_fhs.clear ();
			JC::get_adj_boundary_faces_around_face (mesh, cur_fh, local_adj_fhs);
			foreach (auto &fh, local_adj_fhs){
				if (!JC::contains (all_adj_fhs, fh)) continue;
				if (JC::contains (qs_loop, JC::get_common_edge_handle (mesh, cur_fh, fh)))
					continue;
				if (JC::contains (one_side, fh)) continue;
				spread_set.push (fh);
				one_side.insert (fh);
			}
		}
		return one_side;
	};
	

	MeshRenderOptions render_options;
	render_options.face_color = "(r=0 g=0.67 b=1)";
	auto tmp_group = new VolumeMeshElementGroup (mesh, "pd", "sidefhs1");
	tmp_group->fhs = fGetOneSide (seed_fh1);
	hoopsview->render_mesh_group (tmp_group, render_options);

	render_options.face_color = "(r=1 g=0.33 b=0)";
	tmp_group = new VolumeMeshElementGroup (mesh, "pd", "sidefhs2");
	tmp_group->fhs = fGetOneSide (seed_fh2);
	hoopsview->render_mesh_group (tmp_group, render_options);
}

void MeshEditWidget::on_finally_sep ()
{
	selected_ehs.clear ();
	if (!hoopsview->get_selected_elements (NULL, &selected_ehs, NULL)){
		QMessageBox::warning (this, tr("警告"), tr("未选中任何网格边！"), QMessageBox::Ok);
		return;
	}

	foreach (auto &eh, selected_ehs)
		qs_loop.insert (eh);

	MeshRenderOptions render_options;
	render_options.edge_width = 6;
	render_options.edge_color = "(r=0 g=0.67 b=0)";
	auto tmp_group = new VolumeMeshElementGroup (mesh, "pd", "added loops");
	JC::vector_to_unordered_set (selected_ehs, tmp_group->ehs);
	hoopsview->render_mesh_group (tmp_group, render_options);


}

void MeshEditWidget::on_sep_hex_seeds ()
{
	if (pd_fhs_patches.empty ()) return;

	pd_hex_sets.clear ();

	foreach (auto &fhs_patch, pd_fhs_patches){
		std::unordered_set<OvmCeH> tmp, one_hex_set;
		JC::get_direct_adjacent_hexas (mesh, fhs_patch, tmp);
		foreach (auto &ch, tmp){
			if (JC::contains (qs_hexa_set, ch))
				one_hex_set.insert (ch);
		}
		pd_hex_sets.push_back (one_hex_set);
	}

	char *colors[] = {"(r=0 g=0.67 b=1)", "(r=1 g=0.33 b=0)", "(r=0.67 g=0 b=1)", "(r=1 g=0.67 b=0)", "(r=0.67 g=0 b=0.50)"};

	MeshRenderOptions render_options;
	hoopsview->derender_mesh_groups ("pd", "seed hex sets");
	for (int i = 0; i != pd_hex_sets.size (); ++i){
		auto tmp_group = new VolumeMeshElementGroup (mesh, "pd", "seed hex sets");
		tmp_group->chs = pd_hex_sets[i];
		render_options.cell_color = colors[i % 5];
		hoopsview->render_mesh_group (tmp_group, render_options);
	}
	//render_options.face_color = "(r=0 g=0.67 b=1)";
	//auto tmp_group = new VolumeMeshElementGroup (mesh, "pd", "sepfhs1");
	//tmp_group->fhs = fhs_patches.front ();
	//hoopsview->render_mesh_group (tmp_group, render_options);

	//render_options.face_color = "(r=1 g=0.33 b=0)";
	//tmp_group = new VolumeMeshElementGroup (mesh, "pd", "sepfhs2");
	//tmp_group->fhs = fhs_patches.back ();
	//hoopsview->render_mesh_group (tmp_group, render_options);
}

void MeshEditWidget::on_get_init_quad_set_for_pd ()
{
	pd_init_fhs_sets.clear ();

	std::sort (pd_hex_sets.begin (), pd_hex_sets.end (), [&](const std::unordered_set<OvmCeH> &hs1,
		const std::unordered_set<OvmCeH> &hs2)->bool{
			return hs1.size () <= hs2.size ();
	});

	std::unordered_set<OvmFaH> all_init_fhs;
	std::unordered_set<OvmCeH> all_init_chs;

	for (int i = 0; i != pd_hex_sets.size () - 1; ++i){
		std::unordered_set<OvmCeH> other_chs;
		for (int j = 0; j != pd_hex_sets.size (); ++j){
			if (j == i) continue;
			foreach (auto &ch, pd_hex_sets[j]) other_chs.insert (ch);
		}

		auto sep_fhs = get_local_mesh_min_cut (mesh, qs_hexa_set, pd_hex_sets[i], other_chs);
		pd_init_fhs_sets.push_back (sep_fhs);
		foreach (auto &fh, sep_fhs) all_init_fhs.insert (fh);
		foreach (auto &ch, pd_hex_sets[i]) all_init_chs.insert (ch);

		std::vector<std::unordered_set<OvmCeH> > sep_chs;
		JC::get_separate_chs_sets (mesh, &qs_hexa_set, &sep_fhs, &sep_chs);
		foreach (auto &chs, sep_chs){
			if (JC::intersects (pd_hex_sets[i], chs)){
				pd_hex_sets[i] = chs;
				break;
			}
		}
	}

	char *colors[] = {"(r=0 g=0.67 b=1)", "(r=1 g=0.33 b=0)", "(r=0.67 g=0 b=1)", "(r=1 g=0.67 b=0)", "(r=0.67 g=0 b=0.50)"};

	MeshRenderOptions render_options;
	render_options.edge_color = "black";
	render_options.edge_width = 3;
	hoopsview->derender_mesh_groups ("pd", "init sep fhs sets");
	for (int i = 0; i != pd_init_fhs_sets.size (); ++i){
		auto tmp_group = new VolumeMeshElementGroup (mesh, "pd", "init sep fhs sets");
		tmp_group->fhs = pd_init_fhs_sets[i];
		//render_options.face_color = colors[i % 5];
		render_options.face_color = "(r=0 g=0.67 b=1)";
		foreach (auto &fh, tmp_group->fhs){
			JC::get_adj_edges_around_face (mesh, fh, tmp_group->ehs);
		}

		hoopsview->render_mesh_group (tmp_group, render_options);
	}

	qs_inf_fhs = all_init_fhs;
	qs_hexa_set = all_init_chs;
	foreach (auto &one_int_path, quad_set_data.int_paths){
		foreach (auto &eh_idx, one_int_path){
			qs_int_path_set.insert (OvmEgH (eh_idx));
		}
	}

	foreach (auto &fh, qs_inf_fhs)
		quad_set_data.init_quad_set.insert (fh.idx ());

	foreach (auto &ch, qs_hexa_set)
		quad_set_data.init_hex_set.insert (ch.idx ());
}

void MeshEditWidget::on_int_sep_hex_seeds ()
{
	if (pd_fhs_patches.empty ()) return;

	std::vector<std::unordered_set<OvmCeH> > all_hex_sets;
	foreach (auto &cros, quad_set_data.int_hex_sets){
		
		foreach (auto os, cros){
			std::unordered_set<OvmCeH> one_hex_set;
			foreach (auto i, os)
				one_hex_set.insert (OvmCeH (i));
			all_hex_sets.push_back (one_hex_set);
		}
		
	}

	std::vector<std::unordered_set<OvmCeH> > bound_hex_sets;
	foreach (auto &fhs_patch, pd_fhs_patches){
		std::unordered_set<OvmCeH> tmp, one_hex_set;
		JC::get_direct_adjacent_hexas (mesh, fhs_patch, tmp);
		foreach (auto &ch, tmp){
			if (JC::contains (qs_hexa_set, ch))
				one_hex_set.insert (ch);
		}
		all_hex_sets.push_back (one_hex_set);
		bound_hex_sets.push_back (one_hex_set);
	}

	std::vector<std::pair<OvmCeH, std::unordered_set<OvmCeH> > > all_int_chs;
	while (true){
		bool is_ok = true;
		for (int i = 0; i != all_hex_sets.size () - 1; ++i){
			auto &test_hex_set = all_hex_sets[i];
			for (int j = i + 1; j != all_hex_sets.size (); ++j){
				if (JC::intersects (test_hex_set, all_hex_sets[j])){
					foreach (auto &ch_1, test_hex_set){
						if (JC::contains (all_hex_sets[j], ch_1)){
							auto tttmp = test_hex_set;
							foreach (auto &ch_2, all_hex_sets[j]) tttmp.insert (ch_2);
							all_int_chs.push_back (std::make_pair (ch_1, tttmp));break;
						}
					}
					auto locate = all_hex_sets.begin () + j;
					foreach (auto ch, all_hex_sets[j]) test_hex_set.insert (ch);
					all_hex_sets.erase (locate);
					is_ok = false; break;
				}
			}
			if (is_ok == false) break;
		}

		if (is_ok) break;
	}

	pd_hex_sets = all_hex_sets;
	char *colors[] = {"(r=0 g=0.67 b=0)", "(r=0 g=0.67 b=1)", "(r=1 g=0.33 b=0)", "(r=0.67 g=0 b=1)", "(r=1 g=0.67 b=0)", "(r=0.67 g=0 b=0.50)"};

	//char *colors[] = {"(r=0 g=0.67 b=1)", "(r=1 g=0.33 b=0)", "(r=0.67 g=0 b=1)", "(r=1 g=0.67 b=0)", "(r=0.67 g=0 b=0.50)"};
	MeshRenderOptions render_options;
	hoopsview->derender_mesh_groups ("pd", "bound hex sets");
	for (int i = 0; i != bound_hex_sets.size (); ++i){
		auto tmp_group = new VolumeMeshElementGroup (mesh, "pd", "bound hex sets");
		tmp_group->chs = bound_hex_sets[i];
		render_options.cell_color = colors[i % 6];
		hoopsview->render_mesh_group (tmp_group, render_options);
	}

	hoopsview->derender_mesh_groups ("pd", "int seed hex sets");
	for (int i = 0; i != pd_hex_sets.size (); ++i){
		auto tmp_group = new VolumeMeshElementGroup (mesh, "pd", "int seed hex sets");
		tmp_group->chs = pd_hex_sets[i];
		render_options.cell_color = colors[i % 5];
		hoopsview->render_mesh_group (tmp_group, render_options);
	}

	for (int i = 0; i != 100; ++i){
		hoopsview->derender_mesh_groups ("pd", QString("iiint sets %1").arg (i).toAscii ().data ());
	}
	for (int i = 0; i != all_int_chs.size (); ++i){
		auto tmp_group = new VolumeMeshElementGroup (mesh, "pd", QString("iiint sets %1").arg (i));
		tmp_group->chs.insert (all_int_chs[i].first);
		render_options.cell_color = "green";
		hoopsview->render_mesh_group (tmp_group, render_options);

		tmp_group = new VolumeMeshElementGroup (mesh, "pd", QString("iiint sets %1").arg (i));
		tmp_group->chs = all_int_chs[i].second;
		render_options.cell_color = "blue";
		hoopsview->render_mesh_group (tmp_group, render_options);
	}

}

int irregular_degree (VolumeMesh *mesh, const std::set<OvmCeH> &chs, OvmEgH eh){
	int irre_degree = 0;
	std::unordered_set<OvmCeH> adj_chs;
	JC::get_adj_hexas_around_edge (mesh, eh, adj_chs);
	int in_count = 0, out_count = 0;
	foreach (auto &ch, adj_chs){
		if (JC::contains (chs, ch)) in_count++;
	}
	out_count = adj_chs.size () - in_count;
	if (in_count == 0 || in_count == adj_chs.size ()) return 0;

	if (mesh->is_boundary (eh)){
		irre_degree += std::abs(in_count - 1) + std::abs (out_count - 1) - std::abs ((int)(adj_chs.size () - 2));
	}else{
		irre_degree += std::abs(in_count - 2) + std::abs (out_count - 2) - std::abs ((int)(adj_chs.size () - 4));
	}
	return irre_degree;
}

int whole_irregular_degree (VolumeMesh *mesh, const std::set<OvmCeH> &chs, std::unordered_set<OvmFaH> &bound_fhs){
	std::unordered_set<OvmEgH> ehs_on_bound_fhs;
	foreach (auto &fh, bound_fhs) JC::get_adj_edges_around_face (mesh, fh, ehs_on_bound_fhs);

	int irre_degree = 0;
	foreach (auto &eh, ehs_on_bound_fhs) 
		irre_degree += irregular_degree (mesh, chs, eh);

	return irre_degree;
}

int irregular_degree_variation (VolumeMesh *mesh, std::set<OvmCeH> &old_chs, OvmCeH modified_ch){
	std::unordered_set<OvmEgH> adj_ehs;
	JC::get_adj_edges_around_cell (mesh, modified_ch, adj_ehs);
	int local_old_irre_degree = 0, local_new_irre_degree = 0;
	foreach (auto &eh, adj_ehs){
		local_old_irre_degree += irregular_degree (mesh, old_chs, eh);
	}
	//if (JC::contains (old_chs, modified_ch)){ //删除
	//	old_chs.erase (modified_ch);
	//}else{ //添加
		old_chs.insert (modified_ch);
	//}

	foreach (auto &eh, adj_ehs){
		local_new_irre_degree += irregular_degree (mesh, old_chs, eh);
	}
	old_chs.erase (modified_ch);
	return (local_new_irre_degree - local_old_irre_degree);
}

void recursive_calc_best_irregular_degree (VolumeMesh *mesh, std::set<OvmCeH> &chs, const std::unordered_set<OvmCeH> &bound_chs,
	std::unordered_set<OvmFaH>& bound_fhs, int this_irre_variant, int &smallest_irre_variant, const int max_irre_variant, 
	std::map<std::set<OvmCeH>, int>& irre_mapping){

	foreach (auto &fh, bound_fhs){
		auto hfh = mesh->halfface_handle (fh, 0);
		auto adj_ch = mesh->incident_cell (hfh);
		if (JC::contains (chs, adj_ch)){
			hfh = mesh->opposite_halfface_handle (hfh);
			adj_ch = mesh->incident_cell (hfh);
		}
		if (JC::contains (bound_chs, adj_ch)) continue;

		int local_old_irre_degree = 0, local_new_irre_degree = 0;
		std::unordered_set<OvmEgH> adj_ehs;
		JC::get_adj_edges_around_cell (mesh, adj_ch, adj_ehs);
		
		foreach (auto &eh, adj_ehs){
			local_old_irre_degree += irregular_degree (mesh, chs, eh);
		}

		//将adj_ch放入chs，生成新的集合
		chs.insert (adj_ch);

		int rec_irre_variant = 0;
		auto locate = irre_mapping.find (chs);
		//若irre_mapping中有该chs，表示该chs已经在recursive了，不需要再往下进行了
		if (locate != irre_mapping.end ()){
			chs.erase (adj_ch);
			continue;
		}
		foreach (auto &eh, adj_ehs){
			local_new_irre_degree += irregular_degree (mesh, chs, eh);
		}

		rec_irre_variant = (local_new_irre_degree - local_old_irre_degree) + this_irre_variant;
		irre_mapping.insert (std::make_pair (chs, rec_irre_variant));

		//如果当前非规则度超过限定的最大不规则度，则不再该分支上继续递归
		if (rec_irre_variant > max_irre_variant) {
			chs.erase (adj_ch);
			continue;
		}
		//找到了更小的非规则度差，则更新最小非规则度差
		if (rec_irre_variant < smallest_irre_variant){
			smallest_irre_variant = rec_irre_variant;
		}

		//更新边界面集bound_fhs
		auto hfh_vec = mesh->cell (adj_ch).halffaces ();
		foreach (auto &hfh, hfh_vec){
			auto adj_fh = mesh->face_handle (hfh);
			if (mesh->is_boundary (adj_fh)) continue;
			if (JC::contains (bound_fhs, adj_fh)) bound_fhs.erase (adj_fh);
			else bound_fhs.insert (adj_fh);
		}
		
		//对该分支继续往下递归
		recursive_calc_best_irregular_degree (mesh, chs, bound_chs, bound_fhs, 
			rec_irre_variant, smallest_irre_variant, max_irre_variant, irre_mapping);

		//恢复chs原样
		chs.erase (adj_ch);

		//再次更新边界面集bound_fhs，恢复原样
		foreach (auto &hfh, hfh_vec){
			auto adj_fh = mesh->face_handle (hfh);
			if (mesh->is_boundary (adj_fh)) continue;
			if (JC::contains (bound_fhs, adj_fh)) bound_fhs.erase (adj_fh);
			else bound_fhs.insert (adj_fh);
		}
	}//end foreach (auto &fh, bound_fhs){...
}

void SimulatedAnnealing (VolumeMesh *mesh, std::unordered_set<OvmCeH> &chs, const std::unordered_set<OvmCeH> &bound_chs,
	std::unordered_set<OvmCeH> &bound_fhs){
	double T = 100, T_min = 10;
	double delta = 0.98;

	//保存原始六面体集合，这部分六面体是不能删的
	const auto ori_chs = chs;

	while (T > T_min){


		T *= delta;
	}
}

void MeshEditWidget::on_algo_test_for_pd ()
{
	std::set<OvmCeH> chs_set;
	foreach (auto &ch, pd_temp_group->chs) chs_set.insert (ch);
	if (chs_set.empty ()) return;

	std::unordered_set<OvmFaH> tmp_bound_fhs, bound_fhs;
	JC::collect_boundary_element (mesh, pd_temp_group->chs, NULL, NULL, &tmp_bound_fhs);
	foreach (auto &fh, tmp_bound_fhs){
		if (!mesh->is_boundary (fh)) bound_fhs.insert (fh);
	}

	int irre_degree = whole_irregular_degree (mesh, chs_set, bound_fhs);
	std::map<std::set<OvmCeH>, int> chs_irre_degree_mapping;
	chs_irre_degree_mapping.insert (std::make_pair(chs_set, 0));

	//QMessageBox::information (this, "Irregular degree", QString("is: %1").arg (irre_degree));
	std::unordered_set<OvmCeH> bound_chs;
	for (auto c_it = mesh->cells_begin (); c_it != mesh->cells_end (); ++c_it){
		std::unordered_set<OvmEgH> adj_ehs;
		JC::get_adj_edges_around_cell (mesh, *c_it, adj_ehs);

		bool is_adj_ch_on_boundary = false;
		foreach (auto &eh, adj_ehs){
			if (mesh->is_boundary (eh)){
				is_adj_ch_on_boundary = true;
				break;
			}
		}
		if (is_adj_ch_on_boundary) bound_chs.insert (*c_it);
	}

	int max_irre_variant = QInputDialog::getInt (this, "Input tolarence", "Tolarence:", 10, 0);
	int smallest_variant = 0;
	recursive_calc_best_irregular_degree (mesh, chs_set, bound_chs, bound_fhs, 0, smallest_variant, 
		max_irre_variant, chs_irre_degree_mapping);

	QMessageBox::information (this, "smallest variant", QString("is: %1").arg (smallest_variant));

	std::unordered_set<OvmCeH> best_chs;
	foreach (auto &p, chs_irre_degree_mapping){
		if (p.second == smallest_variant){
			foreach (auto ch, p.first){
				best_chs.insert (ch);
				
			}
			break;
		}
	}

	hoopsview->derender_mesh_groups ("pd", "best hex sets");
	auto tmp_group = new VolumeMeshElementGroup (mesh, "pd", "best hex sets");
	tmp_group->chs = best_chs;
	MeshRenderOptions render_options;
	render_options.cell_color = "green";
	hoopsview->render_mesh_group (tmp_group, render_options);
}

void MeshEditWidget::on_show_sheet_edges_and_faces_for_pd ()
{
	if (!osse_handler) return;
	auto sheet = osse_handler->get_sheet_to_extract ();

	hoopsview->derender_mesh_groups ("pd", "sheet edges");
	hoopsview->derender_mesh_groups ("pd", "sheet faces");

	char color_buf[1000]={0};
	sprintf (color_buf, "(r=%f g=%f b=%f)", 
		pd_group_color.redF (), pd_group_color.greenF (), pd_group_color.blueF ());

	std::unordered_set<OvmFaH> all_fhs_on_sheet;
	std::unordered_set<OvmEgH> all_ehs_on_sheet;
	foreach (auto &eh, sheet->ehs){
		if (!mesh->is_boundary (eh) || JC::contains (qs_loop, eh)) all_ehs_on_sheet.insert (eh);

		std::unordered_set<OvmFaH> adj_fhs;
		JC::get_adj_faces_around_edge (mesh, eh, adj_fhs);
		foreach (auto &fh, adj_fhs){
			if (!mesh->is_boundary (fh)) all_fhs_on_sheet.insert (fh);
		}
	}
	
	
	auto tmp_group = new VolumeMeshElementGroup (mesh, "pd", "sheet edges");
	tmp_group->ehs = all_ehs_on_sheet;
	MeshRenderOptions render_options;
	render_options.edge_color = "green";
	render_options.edge_width = 6;
	hoopsview->render_mesh_group (tmp_group, render_options);

	tmp_group = new VolumeMeshElementGroup (mesh, "pd", "sheet faces");
	
	tmp_group->fhs = all_fhs_on_sheet;
	foreach (auto &fh, tmp_group->fhs){
		JC::get_adj_edges_around_face (mesh, fh, tmp_group->ehs);
	}
	render_options.edge_color = "black";
	render_options.edge_width = 3;
	render_options.face_color = color_buf;
	hoopsview->render_mesh_group (tmp_group, render_options);
}

void MeshEditWidget::on_append_edges_on_faces_for_pd ()
{
	std::unordered_set<OvmEgH> ehs;
	if (!pd_temp_group || pd_temp_group->fhs.empty ()) return;
	foreach (auto &fh, pd_temp_group->fhs){
		JC::get_adj_edges_around_face (mesh, fh, ehs);
	}
	auto tmp_group = new VolumeMeshElementGroup (mesh, "pd", "edges");
	tmp_group->ehs = ehs;
	MeshRenderOptions render_options;
	render_options.edge_color = "black";
	render_options.edge_width = 3;
	hoopsview->render_mesh_group (tmp_group, render_options);
}

void MeshEditWidget::on_turn_to_inflation_for_pd ()
{
	qs_hexa_set = pd_temp_group->chs;
	if (qs_hexa_set.empty ()) return;
	std::unordered_set<OvmFaH> tmp_bound_fhs;
	JC::collect_boundary_element (mesh, qs_hexa_set, NULL, NULL, &tmp_bound_fhs);
	qs_inf_fhs.clear ();
	foreach (auto &fh, tmp_bound_fhs){
		if (!mesh->is_boundary (fh)) qs_inf_fhs.insert (fh);
	}

	std::hash_map<OvmEgH, int> ehs_int_counting;
	foreach (auto &fh, qs_inf_fhs){
		std::unordered_set<OvmEgH> adj_ehs;
		JC::get_adj_edges_around_face (mesh, fh, adj_ehs);
		foreach (auto &eh, adj_ehs)
			ehs_int_counting[eh]++;
	}

	qs_int_path_set.clear ();
	foreach (auto &p, ehs_int_counting){
		if (p.second == 4) qs_int_path_set.insert (p.first);
	}
}
#include "StdAfx.h"
#include "mesheditwidget.h"
#include "sheetintdiagramwidget.h"
#include "sheetintdiagramsdashboard.h"

void MeshEditWidget::on_select_sheet_for_lse ()
{
	if (osse_handler) delete osse_handler;
	osse_handler = new OneSimpleSheetExtractionHandler (mesh, body, hoopsview);
//	osse_handler->init ();

	std::vector<OvmEgH> selected_ehs;
	if (!hoopsview->get_selected_elements (std::vector<OvmVeH>(), selected_ehs, std::vector<OvmFaH> ())){
		QMessageBox::warning (this, tr("警告"), tr("未选中任何网格边！"), QMessageBox::Ok);
		return;
	}
	JC::retrieve_sheets (mesh, sheet_set);
	OvmEgH eh = selected_ehs.front ();
	auto E_SHEET_PTR = mesh->request_edge_property<unsigned long> ("sheetptr");
	DualSheet *sheet = (DualSheet*)(E_SHEET_PTR[eh]);
	osse_handler->set_sheet_to_extract (sheet);
	MeshRenderOptions render_options;
	render_options.cell_color = "red";
	hoopsview->render_mesh_group (sheet, render_options);
}

void MeshEditWidget::on_select_interface_faces_for_lse ()
{
	std::vector<OvmFaH> selected_fhs;
	if (!hoopsview->get_selected_elements (std::vector<OvmVeH>(), std::vector<OvmEgH>(), selected_fhs)){
		QMessageBox::warning (this, tr("警告"), tr("未选中任何网格面！"), QMessageBox::Ok);
		return;
	}
	FACE *interface_face = NULL;
	auto V_ENTITY_PTR = mesh->request_vertex_property<unsigned long>("entityptr");

	foreach (auto &fh, selected_fhs){
		auto adj_vhs = JC::get_adj_vertices_around_face (mesh, fh);
		foreach (auto &vh, adj_vhs){
			ENTITY *entity = (ENTITY*)(V_ENTITY_PTR[vh]);
			if (is_FACE (entity))
				interface_face = (FACE *)entity;
		}
	}

	osse_handler->set_interface (interface_face);
	osse_handler->update_interface_fhs ();

	auto group = new VolumeMeshElementGroup (mesh, "lse", "interface faces");
	group->fhs = osse_handler->get_interface_fhs ();
	MeshRenderOptions render_options;
	render_options.face_color = "(diffuse=pink,transmission = (r=0.2 g=0.2 b=0.2))";
	hoopsview->render_mesh_group (group, render_options);
}

void MeshEditWidget::on_select_constant_faces_for_lse ()
{
	std::vector<OvmFaH> selected_fhs;
	if (!hoopsview->get_selected_elements (std::vector<OvmVeH>(), std::vector<OvmEgH>(), selected_fhs)){
		QMessageBox::warning (this, tr("警告"), tr("未选中任何网格面！"), QMessageBox::Ok);
		return;
	}
	FACE *constant_face = NULL;
	auto V_ENTITY_PTR = mesh->request_vertex_property<unsigned long>("entityptr");

	foreach (auto &fh, selected_fhs){
		auto adj_vhs = JC::get_adj_vertices_around_face (mesh, fh);
		foreach (auto &vh, adj_vhs){
			ENTITY *entity = (ENTITY*)(V_ENTITY_PTR[vh]);
			if (is_FACE (entity))
				constant_face = (FACE *)entity;
		}
	}
	osse_handler->set_const_face (constant_face);
	osse_handler->update_constant_fhs ();
	

	auto group = new VolumeMeshElementGroup (mesh, "lse", "constant faces");
	group->fhs = osse_handler->get_constant_fhs ();
	MeshRenderOptions render_options;
	render_options.face_color = "(diffuse=green,transmission = (r=0.2 g=0.2 b=0.2))";
	hoopsview->render_mesh_group (group, render_options);
}

void MeshEditWidget::on_extract_all_for_lse ()
{
	std::unordered_set<OvmFaH> result_fhs;
	JC::one_simple_sheet_extraction (mesh, osse_handler->get_sheet_to_extract (), result_fhs);
	osse_handler->update_constant_fhs ();
	osse_handler->update_interface_fhs ();
	std::unordered_set<OvmEgH> result_ehs, ehs_on_constant_fhs, ehs_on_result_fhs;
	foreach (auto &fh, osse_handler->get_constant_fhs ()){
		auto heh_vec = mesh->face (fh).halfedges ();
		foreach (auto &heh, heh_vec){
			auto eh = mesh->edge_handle (heh);
			ehs_on_constant_fhs.insert (eh);
		}
	}
	foreach (auto &fh, result_fhs){
		auto heh_vec = mesh->face (fh).halfedges ();
		foreach (auto &heh, heh_vec){
			auto eh = mesh->edge_handle (heh);
			ehs_on_result_fhs.insert (eh);
		}
	}
	foreach (auto &eh, ehs_on_result_fhs){
		//if (JC::contains (ehs_on_constant_fhs, eh))
			result_ehs.insert (eh);
	}
	osse_handler->set_extract_result_ehs (result_ehs);
	std::unordered_set<OvmEgH> bound_result_ehs;
	JC::collect_boundary_element (mesh, result_fhs, NULL, &bound_result_ehs);

	hoopsview->derender_all_mesh_groups ();
	JC::smooth_volume_mesh (mesh, body, 10);
	hoopsview->rerender_VolumeMesh (mesh);

	auto group = new VolumeMeshElementGroup (mesh, "lse", "result faces");
	group->fhs = result_fhs;
	MeshRenderOptions render_options;
	render_options.face_color = "red";
	hoopsview->render_mesh_group (group, render_options);

	group = new VolumeMeshElementGroup (mesh, "lse", "interface faces");
	group->fhs = osse_handler->get_interface_fhs ();
	render_options.face_color = "(diffuse=pink,transmission = (r=0.2 g=0.2 b=0.2))";
	hoopsview->render_mesh_group (group, render_options);

	group = new VolumeMeshElementGroup (mesh, "lse", "constant faces");
	group->fhs = osse_handler->get_constant_fhs ();
	render_options.face_color = "(diffuse=green,transmission = (r=0.2 g=0.2 b=0.2))";
	hoopsview->render_mesh_group (group, render_options);

	group = new VolumeMeshElementGroup (mesh, "lse", "result edges");
	group->ehs = result_ehs;
	render_options.edge_color = "green";
	render_options.edge_width = 6;
	render_options.face_color = NULL;
	hoopsview->render_mesh_group (group, render_options);

	group = new VolumeMeshElementGroup (mesh, "lse", "bound edges");
	group->ehs = bound_result_ehs;
	render_options.edge_color = "green";
	render_options.edge_width = 6;
	render_options.face_color = NULL;
	hoopsview->render_mesh_group (group, render_options);
}

void MeshEditWidget::on_local_inflate_for_lse ()
{
	if (ossi_handler) delete ossi_handler;
	ossi_handler = new OneSimpleSheetInflationHandler (mesh, body, hoopsview);
	ossi_handler->init ();

	if (!ossi_handler->analyze_input_edges (osse_handler->get_extract_result_ehs ())){
		QMessageBox::critical (this, tr("错误"), ossi_handler->get_last_err (), QMessageBox::Ok);
		return;
	}

	ossi_handler->set_const_face (osse_handler->get_interface ());
	ossi_handler->set_inter_face (osse_handler->get_const_face ());
	ossi_handler->set_constant_fhs (osse_handler->get_interface_fhs ());
	ossi_handler->set_minimum_depth (ui.spinBox_Extract_Depth->value ());
	auto hexa_set = ossi_handler->get_hexa_set ();
	auto inflation_fhs = ossi_handler->get_inflation_fhs ();

	auto new_sheet = ossi_handler->sheet_inflation ();
	if (!new_sheet){
		QMessageBox::critical (this, tr("错误"), ossi_handler->get_last_err (), QMessageBox::Ok);
		return;
	}

	JC::retrieve_sheets (mesh, sheet_set);

	hoopsview->derender_all_mesh_groups ();
	JC::smooth_volume_mesh (mesh, body, 3);
	hoopsview->rerender_VolumeMesh (mesh);

	MeshRenderOptions render_options;
	render_options.cell_color = "red";
	hoopsview->render_mesh_group (new_sheet, render_options);
}

void MeshEditWidget::on_sep_hex_mesh ()
{
	std::unordered_set<OvmCeH> all_chs;
	for (auto c_it = mesh->cells_begin (); c_it != mesh->cells_end (); ++c_it){
		all_chs.insert (*c_it);
	}

	auto sheet = osse_handler->get_sheet_to_extract ();

	std::unordered_set<OvmCeH> chs_on_sheet = sheet->chs;

	auto fGetOneSet = [&] (OvmCeH seed_ch)->std::unordered_set<OvmCeH>{
		std::queue<OvmCeH> spread_set;
		std::unordered_set<OvmCeH> one_set;
		one_set.insert (seed_ch);
		std::unordered_set<OvmCeH> adj_chs;
		JC::get_adj_hexas_around_hexa (mesh, seed_ch, adj_chs);
		foreach (auto &ch, adj_chs){
			if (JC::contains (chs_on_sheet, ch)) continue;
			spread_set.push (ch);
			one_set.insert (ch);
		}

		while (!spread_set.empty ()){
			auto test_ch = spread_set.front ();
			spread_set.pop ();
			adj_chs.clear ();
			JC::get_adj_hexas_around_hexa (mesh, test_ch, adj_chs);
			foreach (auto &ch, adj_chs){
				if (JC::contains (chs_on_sheet, ch)) continue;
				if (JC::contains (one_set, ch)) continue;
				spread_set.push (ch);
				one_set.insert (ch);
			}
		}
		return one_set;
	};

	std::vector<std::unordered_set<OvmCeH> > all_hex_sets;
	foreach (auto &ch, chs_on_sheet) all_chs.erase (ch);

	std::unordered_set<OvmFaH> all_bound_fhs;
	foreach (auto &ch, all_chs){
		std::unordered_set<OvmFaH> tmp_fhs;
		JC::get_adj_faces_around_hexa (mesh, ch, tmp_fhs);
		foreach (auto &fh, tmp_fhs){
			if (mesh->is_boundary (fh)) all_bound_fhs.insert (fh);
		}
	}

	while (!all_chs.empty ()){
		auto seed_ch = *(all_chs.begin ());
		auto one_set = fGetOneSet (seed_ch);
		foreach (auto &ch, one_set)
			all_chs.erase (ch);

		all_hex_sets.push_back (one_set);
	}

	char *colors[] = {"(r=0 g=0.67 b=1)", "(r=1 g=0.33 b=0)", "(r=0.67 g=0 b=1)", "yellow", "blue"};

	for (int i = 0; i != all_hex_sets.size (); ++i){
		auto group = new VolumeMeshElementGroup (mesh, "lse", "sep hex sets");
		group->chs = all_hex_sets[i];
		MeshRenderOptions render_options;
		render_options.cell_color = colors[i % 4];
		render_options.cell_size = 0.85;
		hoopsview->render_mesh_group (group, render_options);
	}

	std::vector<std::unordered_set<OvmFaH> > fhs_patches;
	std::unordered_set<OvmEgH> all_bound_ehs;
	foreach (auto &fh, all_bound_fhs){
		JC::get_adj_edges_around_face (mesh, fh, all_bound_ehs);
	}

	auto group = new VolumeMeshElementGroup (mesh, "lse", "sep quad edges");
	group->ehs = all_bound_ehs;
	MeshRenderOptions render_options;
	render_options.edge_width = 3;
	render_options.edge_color = "black";
	hoopsview->render_mesh_group (group, render_options);

	JC::get_separate_fhs_patches (mesh, all_bound_fhs, std::unordered_set<OvmEgH>(), fhs_patches);
	for (int i = 0; i != fhs_patches.size (); ++i){
		group = new VolumeMeshElementGroup (mesh, "lse", "sep quad sets");
		group->fhs = fhs_patches[i];
		render_options.face_color = colors[i % 5];
		hoopsview->render_mesh_group (group, render_options);
	}
}

void MeshEditWidget::on_show_int_diagram_for_lse ()
{
	auto sheet = osse_handler->get_sheet_to_extract ();
	auto sd = new SheetIntDiagramsDashboard (this);
	SheetSet ss;
	ss.insert (sheet);
	sd->set_sheet_set (mesh, ss);
	sd->show ();
}
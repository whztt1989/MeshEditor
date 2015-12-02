#include "StdAfx.h"
#include "mesheditcontroller.h"
#include "hoopsview.h"
#include <QMessageBox>

void MeshEditController::on_direct_select_for_cc ()
{
	ui.stackedWidget->setCurrentIndex (0);
}

void MeshEditController::on_select_by_sheets_for_cc ()
{
	ui.stackedWidget->setCurrentIndex (1);
}

void MeshEditController::on_select_face_for_cc ()
{
	hoopsview->begin_select_by_click ();
	hoopsview->set_vertices_selectable (false);
	hoopsview->set_edges_selectable (false);
	hoopsview->set_faces_selectable (true);
	hoopsview->show_mesh_faces (true);

}

void MeshEditController::on_select_face_ok_for_cc ()
{
	selected_fhs.clear ();
	if (!hoopsview->get_selected_elements (NULL, NULL, &selected_fhs)){
		QMessageBox::warning (this, tr("警告"), tr("未选中任何网格面！"), QMessageBox::Ok);
		return;
	}
	hoopsview->clear_selection ();
	hoopsview->begin_camera_manipulate ();

	auto tmp_selected_fhs = selected_fhs;
	selected_sheets.clear ();
	auto E_SHEET_PTR = mesh->request_edge_property<unsigned long> ("sheetptr");

	while (!tmp_selected_fhs.empty ()){
		auto seed_fh = JC::pop_begin_element (tmp_selected_fhs);
		auto heh_vec = mesh->face (seed_fh).halfedges ();
		foreach (auto &heh, heh_vec){
			auto eh = mesh->edge_handle (heh);
			auto sheet = (DualSheet*)(E_SHEET_PTR[eh]);
			assert (sheet);
			selected_sheets.insert (sheet);
		}

		auto column = JC::retrieve_column (mesh, seed_fh);
		foreach (auto &fh, column->ordered_fhs){
			auto locate = std::find (tmp_selected_fhs.begin (), tmp_selected_fhs.end (), fh);
			if (locate != tmp_selected_fhs.end ())
				tmp_selected_fhs.erase (locate);
		}
		columns_to_collapse.insert (column);
	}

	auto fGetAllEdges = [&] (std::unordered_set<OvmCeH> &chs, std::unordered_set<OvmEgH> &ehs){
		foreach (auto &ch, chs){
			auto hfh_vec = mesh->cell (ch).halffaces ();
			foreach (auto &hfh, hfh_vec){
				auto heh_vec = mesh->halfface (hfh).halfedges ();
				foreach (auto &heh, heh_vec)
					ehs.insert (mesh->edge_handle (heh));
			}
		}
	};

	//render
	hoopsview->derender_all_mesh_groups ();

	foreach (auto &sheet, selected_sheets){
		std::unordered_set<OvmEgH> all_ehs_on_sheet;
		fGetAllEdges (sheet->chs, all_ehs_on_sheet);

		MeshRenderOptions render_options;
		render_options.edge_color = "yellow";
		render_options.edge_width = 2;
		VolumeMeshElementGroup *group = new VolumeMeshElementGroup (mesh, "cc", "sheets");
		group->ehs = all_ehs_on_sheet;

		hoopsview->render_mesh_group (group, render_options);
	}

	foreach (auto &column, columns_to_collapse){
		MeshRenderOptions render_options;
		render_options.edge_color = NULL;
		render_options.cell_color = "green";

		auto group = new VolumeMeshElementGroup (mesh, "cc", "selected columns");
		JC::vector_to_unordered_set (column->ordered_chs, group->chs);
		JC::vector_to_unordered_set (column->ordered_fhs, group->fhs);
		hoopsview->render_mesh_group (group, render_options);
	}

	hoopsview->show_mesh_faces (false);
}

void MeshEditController::on_select_edges_for_cc ()
{
	hoopsview->begin_select_by_click ();
	hoopsview->set_vertices_selectable (false);
	hoopsview->set_edges_selectable (true);
	hoopsview->set_faces_selectable (false);
	hoopsview->show_mesh_faces (true);
	hoopsview->show_mesh_edges (true);
}

void MeshEditController::on_select_edges_ok_for_cc ()
{
	selected_ehs.clear ();
	if (!hoopsview->get_selected_elements (NULL, &selected_ehs, NULL)){
		QMessageBox::warning (this, tr("警告"), tr("未选中任何网格边！"), QMessageBox::Ok);
		return;
	}
	hoopsview->clear_selection ();
	hoopsview->begin_camera_manipulate ();

	auto E_SHEET_PTR = mesh->request_edge_property<unsigned long> ("sheetptr");
	selected_sheets.clear ();

	auto fGetAllEdges = [&] (std::unordered_set<OvmCeH> &chs, std::unordered_set<OvmEgH> &ehs){
		foreach (auto &ch, chs){
			auto hfh_vec = mesh->cell (ch).halffaces ();
			foreach (auto &hfh, hfh_vec){
				auto heh_vec = mesh->halfface (hfh).halfedges ();
				foreach (auto &heh, heh_vec)
					ehs.insert (mesh->edge_handle (heh));
			}
		}
	};

	hoopsview->derender_all_mesh_groups ();

	foreach (auto &eh, selected_ehs){
		DualSheet *sheet = (DualSheet*)(E_SHEET_PTR[eh]);
		assert (sheet);
		selected_sheets.insert (sheet);

		std::unordered_set<OvmEgH> all_ehs_on_sheet;
		fGetAllEdges (sheet->chs, all_ehs_on_sheet);

		MeshRenderOptions render_options;
		render_options.edge_color = "yellow";
		render_options.edge_width = 2;
		VolumeMeshElementGroup *group = new VolumeMeshElementGroup (mesh, "cc", "sheets");
		group->ehs = all_ehs_on_sheet;

		hoopsview->render_mesh_group (group, render_options);
	}

	columns_to_collapse = JC::retrieve_columns (mesh, selected_sheets);

	foreach (auto &column, columns_to_collapse){
		auto group = new VolumeMeshElementGroup (mesh, "cc", "intersect columns");
		JC::vector_to_unordered_set (column->ordered_chs, group->chs);
		JC::vector_to_unordered_set (column->ordered_fhs, group->fhs);
		MeshRenderOptions render_options;
		render_options.edge_color = NULL;
		render_options.cell_color = "green";
		hoopsview->render_mesh_group (group, render_options);
	}

	hoopsview->show_mesh_faces (false);

}

void MeshEditController::on_rotate_for_cc ()
{
	hoopsview->begin_camera_manipulate ();
}

void MeshEditController::on_continue_select_collapse_vhs_for_cc ()
{
	hoopsview->begin_select_by_click ();
}

void MeshEditController::on_select_collapse_vhs_for_cc ()
{
	if (columns_to_collapse.empty ()) return;
	std::unordered_set<OvmVeH> all_vhs_on_columns;
	foreach (auto &column, columns_to_collapse){
		auto vhs = JC::get_adj_vertices_around_face (mesh, column->ordered_fhs.front ());
		foreach (auto &vh, vhs)
			all_vhs_on_columns.insert (vh);
	}
	hoopsview->derender_all_mesh_groups ();
	std::unordered_set<OvmEgH> all_ehs_on_columns;
	foreach (auto &column, columns_to_collapse){
		foreach (auto &ch, column->ordered_chs){
			std::unordered_set<OvmEgH> adj_edges;
			JC::get_adj_edges_around_cell (mesh, ch, adj_edges);
			foreach (auto &eh, adj_edges)
				all_ehs_on_columns.insert (eh);
		}
	}

	auto group = new VolumeMeshElementGroup (mesh, "cc", "intersect vertices");
	MeshRenderOptions render_options;
	render_options.vertex_size = 1;
	render_options.vertex_color = "green";
	group->vhs = all_vhs_on_columns;
	hoopsview->render_mesh_group (group, render_options);
	hoopsview->set_mesh_group_selectability (group, true, false, false);

	group = new VolumeMeshElementGroup (mesh, "cc", "edges on columns");
	MeshRenderOptions render_options2;
	render_options2.edge_color = "red";
	render_options2.edge_width = 3;
	group->ehs = all_ehs_on_columns;
	hoopsview->render_mesh_group (group, render_options2);
	
	hoopsview->begin_select_by_click ();

	hoopsview->show_mesh_faces (false);
	hoopsview->show_mesh_edges (true);
}

void MeshEditController::on_select_collapse_vhs_ok_for_cc ()
{
	selected_vhs.clear ();
	if (!hoopsview->get_selected_elements (&selected_vhs, NULL, NULL)){
		QMessageBox::warning (this, tr("警告"), tr("未选中任何网格点！"), QMessageBox::Ok);
		return;
	}
	hoopsview->clear_selection ();
	hoopsview->begin_camera_manipulate ();

	auto fDiagonalVhs = [&] (OvmVeH vh1, OvmVeH vh2, OvmFaH &comm_fh)->bool{
		for (auto voh_it = mesh->voh_iter (vh1); voh_it; ++voh_it){
			OvmVeH vh = mesh->halfedge (*voh_it).to_vertex ();
			if (vh == vh2) return false;
		}
		comm_fh = mesh->InvalidFaceHandle;
		std::unordered_set<OvmFaH> adj_faces1, adj_faces2;
		JC::get_adj_faces_around_vertex (mesh, vh1, adj_faces1);
		JC::get_adj_faces_around_vertex (mesh, vh2, adj_faces2);
		foreach (auto &fh, adj_faces1){
			if (JC::contains (adj_faces2, fh)){
				comm_fh = fh;
				break;
			}
		}
		if (comm_fh == mesh->InvalidFaceHandle)
			return false;
		return true;
	};
	collapse_vhs_pairs.clear ();
	while (!selected_vhs.empty ()){
		OvmVeH test_vh = JC::pop_begin_element (selected_vhs);
		OvmVeH paired_vh = mesh->InvalidVertexHandle;
		OvmFaH comm_fh = mesh->InvalidFaceHandle;
		foreach (auto &vh, selected_vhs){
			if (fDiagonalVhs (test_vh, vh, comm_fh)){
				paired_vh = vh;
				break;
			}
		}
		if (paired_vh == mesh->InvalidVertexHandle){
			QMessageBox::warning (this, "错误", QString ("点%1无法配对！").arg (test_vh.idx ()));
			return;
		}
		std::pair<OvmVeH, OvmVeH> vh_pair;
		vh_pair.first = test_vh; vh_pair.second = paired_vh;
		selected_vhs.erase (std::find (selected_vhs.begin (),selected_vhs.end (), paired_vh));
		
		DualColumn *asso_column = NULL;
		foreach (auto &column, columns_to_collapse){
			if (JC::contains (column->ordered_fhs, comm_fh)){
				asso_column = column;break;
			}
		}
		if (asso_column == NULL){
			QMessageBox::critical (this, "错误", QString("面%1找不到对应的Column！").arg (comm_fh.idx ()));
			return;
		}
		auto adj_vhs1 = JC::get_adj_vertices_around_face (mesh, asso_column->ordered_fhs.front ()),
			adj_vhs2 = JC::get_adj_vertices_around_face (mesh, asso_column->ordered_fhs.back ());
		collapse_vhs_pairs.insert (std::make_pair (asso_column, vh_pair));
	}

	hoopsview->derender_mesh_groups ("cc", "collapse vhs pair", true);
	hoopsview->derender_mesh_groups ("cc", "intersect vertices", true);

	char *colors[] = {"green", "blue", "magenta", "pink"};
	int idx = 0;
	foreach (auto &p, collapse_vhs_pairs){
		char *color = colors[idx++ % 4];
		auto group = new VolumeMeshElementGroup (mesh, "cc", "collapse vhs pair");
		group->vhs.insert (p.second.first); group->vhs.insert (p.second.second);
		MeshRenderOptions render_options;
		render_options.vertex_color = color;
		hoopsview->render_mesh_group (group, render_options);

	}
}

void MeshEditController::on_columns_collapse_for_cc ()
{
	if (collapse_vhs_pairs.empty ()) return;

	std::set<std::pair<OvmVeH, OvmVeH> > vhs_pair_on_ehs_on_sheets;
	foreach (auto &sheet, selected_sheets){
		foreach (auto &eh, sheet->ehs){
			std::pair<OvmVeH, OvmVeH> vh_pair;
			vh_pair.first = mesh->edge (eh).to_vertex ();
			vh_pair.second = mesh->edge (eh).from_vertex ();
			vhs_pair_on_ehs_on_sheets.insert (vh_pair);
		}
	}

	old_new_vhs_mapping.clear ();

	JC::columns_collapse (mesh, collapse_vhs_pairs, old_new_vhs_mapping);
	foreach (auto &p, collapse_vhs_pairs)
		delete p.first;

	hoopsview->derender_all_mesh_groups ();
	JC::smooth_volume_mesh (mesh, body, 5);
	update_mesh ();

	auto E_SHEET_PTR = mesh->request_edge_property<unsigned long> ("sheetptr");
	SheetSet sheet_set;
	JC::retrieve_sheets (mesh, sheet_set);

	auto fGetNewEh = [&] (std::pair<OvmVeH, OvmVeH> &vh_pair)->OvmEgH{
		auto new_vh1 = old_new_vhs_mapping[vh_pair.first],
			new_vh2 = old_new_vhs_mapping[vh_pair.second];
		auto heh = mesh->halfedge (new_vh1, new_vh2);
		if (heh != mesh->InvalidHalfEdgeHandle)
			return mesh->edge_handle (heh);
		else
			return mesh->InvalidEdgeHandle;
	};

	SheetSet resultant_sheets;
	while (!vhs_pair_on_ehs_on_sheets.empty ()){
		auto seed_pair = JC::pop_begin_element (vhs_pair_on_ehs_on_sheets);
		auto new_eh = fGetNewEh (seed_pair);

		while (new_eh == mesh->InvalidEdgeHandle){
			if (vhs_pair_on_ehs_on_sheets.empty ()) break;
			seed_pair = JC::pop_begin_element (vhs_pair_on_ehs_on_sheets);
			new_eh = fGetNewEh (seed_pair);
		}
		if (new_eh == mesh->InvalidEdgeHandle) break;
		auto sheet = (DualSheet *)(E_SHEET_PTR[new_eh]);
		resultant_sheets.insert (sheet);
	}

	//render
	hoopsview->derender_all_mesh_groups ();
	char *colors[] = {"red", "green", "blue", "magenta", "pink"};
	int idx = 0;
	MeshRenderOptions render_options;
	foreach (auto &sheet, resultant_sheets){		
		render_options.cell_color = colors[idx++ % 5];
		hoopsview->render_mesh_group (sheet, render_options);

	}
	hoopsview->show_mesh_faces (false);
}

void MeshEditController::on_postprocess_for_cc ()
{
	selected_fhs.clear (); selected_sheets.clear ();
	selected_ehs.clear (); selected_vhs.clear ();
	hoopsview->derender_all_mesh_groups ();
	hoopsview->show_mesh_faces (true);
	hoopsview->show_mesh_edges (true);
}
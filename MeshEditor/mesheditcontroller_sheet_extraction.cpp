#include "StdAfx.h"
#include "mesheditcontroller.h"
#include "hoopsview.h"
#include <QMessageBox>
#include <QTime>
#include <QInputDialog>

void MeshEditController::on_select_edges_for_se ()
{
	hoopsview->begin_select_by_click ();
	hoopsview->set_vertices_selectable (false);
	hoopsview->set_edges_selectable (true);
	hoopsview->set_faces_selectable (false);
	hoopsview->show_mesh_faces (true);
}

//void MeshEditController::on_select_edges_ok_for_se ()
//{
//	selected_ehs.clear ();
//	if (!hoopsview->get_selected_elements (NULL, &selected_ehs, NULL)){
//		QMessageBox::warning (this, tr("警告"), tr("未选中任何网格边！"), QMessageBox::Ok);
//		return;
//	}
//	if (selected_ehs.size () != 1){
//		QMessageBox::warning (this, tr("警告"), tr("只能选中一条网格边！"), QMessageBox::Ok);
//		return;
//	}
//
//	hoopsview->clear_selection ();
//	hoopsview->begin_camera_manipulate ();
//
//	//hoopsview->derender_all_mesh_groups ();
//	SheetSet sheet_set;
//	JC::retrieve_sheets (mesh, sheet_set);
//
//	auto E_SHEET_PTR = mesh->request_edge_property<unsigned long> ("sheetptr");
//	auto selected_eh = selected_ehs.front ();
//	DualSheet *sheet = (DualSheet*)(E_SHEET_PTR[selected_eh]);
//	assert (sheet);
//	sheet_to_extract = sheet;
//
//	//MeshRenderOptions render_options;
//	//render_options.cell_color = "red";
//	//hoopsview->render_mesh_group (sheet, render_options);
//	//hoopsview->show_mesh_faces (false);
//	//hoopsview->show_mesh_faces_transparent ();
//	QStringList slist;
//	slist<<"red"
//		<<"green"
//		<<"blue"
//		<<"yellow"
//		<<"pink";
//	QString colorStr = QInputDialog::getItem (this, "Choose color", "Choose the sheet's color",
//		slist);
//	if (!fhs_for_rendering.empty ()){
//		//////////////////////////////////////////////////////////////////////////
//		//局部化sheet抽取时，有现成的辅助sheet
//
//		//hoopsview->derender_all_mesh_groups ();
//		//std::unordered_set<OvmEgH> ehs_on_sheet, ehs_on_fhs;
//		//foreach (auto &ch, sheet_to_extract->chs){
//		//	JC::get_adj_edges_around_cell (mesh, ch, ehs_on_sheet);
//		//}
//		//foreach (auto &fh, fhs_for_rendering){
//		//	JC::get_adj_edges_around_face (mesh, fh, ehs_on_fhs);
//		//}
//		//OvmEgH seed_eh;
//		//foreach (auto &eh, ehs_on_fhs){
//		//	if (JC::contains (ehs_on_sheet, eh)){
//		//		seed_eh = eh;
//		//		break;
//		//	}
//		//}
//		//std::unordered_set<OvmFaH> adj_fhs_to_seed, adj_fhs_to_seed_tmp;
//		//std::unordered_set<OvmCeH> adj_chs_to_seed, adj_chs_to_seed_tmp;
//		//JC::get_adj_faces_around_edge (mesh, seed_eh, adj_fhs_to_seed_tmp);
//		//JC::get_adj_hexas_around_edge (mesh, seed_eh, adj_chs_to_seed_tmp);
//		//foreach (auto &fh, adj_fhs_to_seed_tmp){
//		//	if (JC::contains (fhs_for_rendering, fh))
//		//		adj_fhs_to_seed.insert (fh);
//		//}
//		//foreach (auto &ch, adj_chs_to_seed_tmp){
//		//	if (JC::contains (sheet_to_extract->chs, ch))
//		//		adj_chs_to_seed.insert (ch);
//		//}
//
//		//std::unordered_set<OvmEgH> other_ehs;
//		//foreach (auto &fh, adj_fhs_to_seed){
//		//	std::unordered_set<OvmEgH> ehssss;
//		//	JC::get_adj_edges_around_face (mesh, fh, ehssss);
//		//	foreach (auto &eh, ehssss){
//		//		if (eh != seed_eh)
//		//			other_ehs.insert (eh);
//		//	}
//		//}
//		//foreach (auto &ch, adj_chs_to_seed){
//		//	std::unordered_set<OvmEgH> ehssss;
//		//	JC::get_adj_edges_around_cell (mesh, ch, ehssss);
//		//	foreach (auto &eh, ehssss){
//		//		if (eh != seed_eh)
//		//			other_ehs.insert (eh);
//		//	}
//		//}
//		//
//		//auto group = new VolumeMeshElementGroup (mesh, "se", "exm1");
//		//MeshRenderOptions render_options;
//		//render_options.edge_width = 6;
//		//render_options.edge_color = "orange";
//		//group->ehs.insert (seed_eh);
//		//hoopsview->render_mesh_group (group, render_options);
//
//		//group = new VolumeMeshElementGroup (mesh, "se", "exm1");
//		//render_options.edge_color = "blue green";
//		//group->ehs = other_ehs;
//		//hoopsview->render_mesh_group (group, render_options);
//		//hoopsview->show_mesh_edges (false);
//		//hoopsview->show_mesh_faces (false);
//
////////////////////////////////////////////////////////////////////////////
//		//std::unordered_set<OvmFaH> fhs_on_sheet_to_extract;
//		//foreach (auto &ch, sheet_to_extract->chs){
//		//	JC::get_adj_faces_around_hexa (mesh, ch, fhs_on_sheet_to_extract);
//		//}
//
//		//OvmFaH seed_fh;
//		//foreach (auto &fh, fhs_for_rendering){
//		//	if (JC::contains (fhs_on_sheet_to_extract, fh)){
//		//		seed_fh = fh;break;
//		//	}
//		//}
//
//		//std::unordered_set<OvmCeH> chs_adj_to_seed;
//		//std::unordered_set<OvmFaH> fhs_adj_to_seed, fhs_adj_to_seed_tmp;
//		//JC::get_adj_hexas_incident_face (mesh, seed_fh, chs_adj_to_seed);
//		//JC::get_adj_faces_around_face_edge (mesh, seed_fh, fhs_adj_to_seed_tmp);
//		//foreach (auto &fh, fhs_adj_to_seed_tmp){
//		//	if (JC::contains (fhs_for_rendering, fh))
//		//		fhs_adj_to_seed.insert (fh);
//		//}
//		//foreach (auto &fh, fhs_adj_to_seed){
//		//	fhs_adj_to_seed_tmp.clear ();
//		//	JC::get_adj_faces_around_face_edge (mesh, fh, fhs_adj_to_seed_tmp);
//		//	foreach (auto &fh2, fhs_adj_to_seed_tmp){
//		//		if (JC::contains (fhs_for_rendering, fh2))
//		//			fhs_adj_to_seed.insert (fh2);
//		//	}
//		//}
//
//		//std::unordered_set<OvmEgH> ehs_on_seed_fh, other_ehs;
//		//JC::get_adj_edges_around_face (mesh, seed_fh, ehs_on_seed_fh);
//		//foreach (auto &ch, chs_adj_to_seed){
//		//	std::unordered_set<OvmEgH> ehs_tmp;
//		//	JC::get_adj_edges_around_cell (mesh, ch, ehs_tmp);
//		//	foreach (auto &eh, ehs_tmp){
//		//		if (!JC::contains (ehs_on_seed_fh, eh)) other_ehs.insert (eh);
//		//	}
//		//}
//		//foreach (auto &fh, fhs_adj_to_seed){
//		//	std::unordered_set<OvmEgH> ehs_tmp;
//		//	JC::get_adj_edges_around_face (mesh, fh, ehs_tmp);
//		//	foreach (auto &eh, ehs_tmp){
//		//		if (!JC::contains (ehs_on_seed_fh, eh)) other_ehs.insert (eh);
//		//	}
//		//}
//
//		//hoopsview->derender_all_mesh_groups ();
//		//auto group = new VolumeMeshElementGroup (mesh, "se", "exm1");
//		//MeshRenderOptions render_options;
//		//render_options.edge_width = 6;
//		//render_options.edge_color = "orange";
//		//group->ehs = ehs_on_seed_fh;
//		//hoopsview->render_mesh_group (group, render_options);
//
//		//group = new VolumeMeshElementGroup (mesh, "se", "exm1");
//		//render_options.edge_color = "blue green";
//		//group->ehs = other_ehs;
//		//hoopsview->render_mesh_group (group, render_options);
//		//hoopsview->show_mesh_edges (false);
//		//hoopsview->show_mesh_faces (false);
//	}
//
//	MeshRenderOptions render_options;
//	render_options.cell_color = new char[500];
//	strcpy (render_options.cell_color, colorStr.toAscii ().data ());
//	hoopsview->render_mesh_group (sheet_to_extract, render_options);
//	hoopsview->show_mesh_faces (false);
//	foreach (auto &ch, sheet_to_extract->chs)
//		chs_for_rendering.insert (ch);
//	delete[] render_options.cell_color;
//}

void MeshEditController::on_select_edges_ok_for_se ()
{
	selected_ehs.clear ();
	if (!hoopsview->get_selected_elements (NULL, &selected_ehs, NULL)){
		QMessageBox::warning (this, tr("警告"), tr("未选中任何网格边！"), QMessageBox::Ok);
		return;
	}

	hoopsview->clear_selection ();
	hoopsview->begin_camera_manipulate ();

	//hoopsview->derender_all_mesh_groups ();
	SheetSet sheet_set;
	JC::retrieve_sheets (mesh, sheet_set);

	auto E_SHEET_PTR = mesh->request_edge_property<unsigned long> ("sheetptr");
	QStringList slist;
	slist<<"red"
		<<"green"
		<<"blue"
		<<"yellow"
		<<"pink";
	QString colorStr = QInputDialog::getItem (this, "Choose color", "Choose the sheet's color",
		slist);

	foreach (auto &eh, selected_ehs){
		DualSheet *sheet = (DualSheet*)(E_SHEET_PTR[eh]);
		assert (sheet);
		sheet_to_extract = sheet;

		MeshRenderOptions render_options;
		render_options.cell_color = new char[500];
		strcpy (render_options.cell_color, colorStr.toAscii ().data ());
		hoopsview->render_mesh_group (sheet_to_extract, render_options);
		hoopsview->show_mesh_faces (false);
		foreach (auto &ch, sheet_to_extract->chs)
			chs_for_rendering.insert (ch);
		delete[] render_options.cell_color;
	}
}

void MeshEditController::on_sheet_extraction_for_se ()
{
	std::unordered_set<OvmFaH> result_fhs;
	if (!JC::one_simple_sheet_extraction (mesh, sheet_to_extract, result_fhs)){
		QMessageBox::warning (this, "错误", "Sheet抽取失败！");
		return;
	}
	//QMessageBox::information (this, "运行时间", QString ("总共运行%1s").arg (((double)time.elapsed ())/1000.0));

	hoopsview->derender_all_mesh_groups ();
	JC::smooth_volume_mesh (mesh, body, 5);
	update_mesh ();

	SheetSet sheet_set;
	JC::retrieve_sheets (mesh, sheet_set);

	auto group = new VolumeMeshElementGroup (mesh, "se", "resultant faces of extraction");
	group->fhs = result_fhs;
	MeshRenderOptions render_options;
	render_options.face_color = "green";
	foreach (auto &fh, result_fhs){
		JC::get_adj_edges_around_face (mesh, fh, group->ehs);
	}
	render_options.edge_width = 6;
	render_options.edge_color = "black";
	hoopsview->render_mesh_group (group, render_options);
	fhs_for_rendering = result_fhs;

	////渲染边界
	//group = new VolumeMeshElementGroup (mesh, "se", "resultant boundary ehs of extraction");
	//std::hash_map<OvmEgH, int> bound_ehs;
	//foreach (auto &fh, result_fhs){
	//	auto heh_vec = mesh->face (fh).halfedges ();
	//	foreach (auto &heh, heh_vec){
	//		auto eh = mesh->edge_handle (heh);
	//		bound_ehs[eh]++;
	//	}
	//}
	//foreach (auto &p, bound_ehs){
	//	if (p.second == 1)
	//		group->ehs.insert (p.first);
	//}
	//render_options.face_color = NULL;
	//render_options.edge_color = "blue";
	//render_options.edge_width = 1;
	//hoopsview->render_mesh_group (group, render_options);
	hoopsview->show_mesh_faces (false);
}

void MeshEditController::on_postprocess_for_se ()
{
	selected_ehs.clear (); selected_vhs.clear ();
	hoopsview->derender_all_mesh_groups ();
	hoopsview->show_mesh_faces (true);
	if (osse_handler){
		delete ossi_handler;
		ossi_handler = NULL;
	}
	emit new_sheet_inflated((unsigned long)new_sheet);
}

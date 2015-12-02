#include "StdAfx.h"
#include "mesheditcontroller.h"
#include "hoopsview.h"
#include <QMessageBox>
#include <limits>
#include "HUtility.h"
#include "HOpMoveHandle.h"

void MeshEditController::on_select_smooth_whole_for_mo ()
{
	ui.stackedWidget_Smooth_Methods->setCurrentIndex (0);
}

void MeshEditController::on_select_pair_node_method_for_mo ()
{
	ui.stackedWidget_Smooth_Methods->setCurrentIndex (1);
}

void MeshEditController::on_select_smooth_faces_for_mo ()
{
	ui.stackedWidget_Smooth_Methods->setCurrentIndex (2);
}

void MeshEditController::on_init_move_node_for_mo ()
{
	if (!mesh->vertex_property_exists<bool> ("vertexfixedforsmooth")){
		auto tmp = mesh->request_vertex_property<bool> ("vertexfixedforsmooth", false);
		mesh->set_persistent(tmp);
	}
	auto V_FIXED = mesh->request_vertex_property<bool> ("vertexfixedforsmooth");
	auto V_ENTITY_PTR = mesh->request_vertex_property<unsigned long>("entityptr");
	for (auto v_it = mesh->vertices_begin (); v_it != mesh->vertices_end (); ++v_it){
		if (!mesh->is_boundary (*v_it)) continue;
		auto entity = (ENTITY*)(V_ENTITY_PTR[*v_it]);
		if (entity == NULL) continue;
		if ((unsigned long)entity == -1) continue;
		if (is_FACE (entity)){
			V_FIXED[*v_it] = false;
		}
	}
}

void MeshEditController::on_select_node_for_move_for_mo ()
{
	if (!mesh->vertex_property_exists<unsigned long> ("entityptr")) return;
	auto V_ENTITY_PTR = mesh->request_vertex_property<unsigned long> ("entityptr");
	if (!mesh->vertex_property_exists<bool> ("vertexfixedforsmooth")){
		auto tmp = mesh->request_vertex_property<bool> ("vertexfixedforsmooth", false);
		mesh->set_persistent(tmp);
	}
	auto V_FIXED = mesh->request_vertex_property<bool> ("vertexfixedforsmooth");

	std::unordered_set<OvmVeH> vhs_on_geom_faces, vhs_fixed;
	for (auto v_it = mesh->vertices_begin (); v_it != mesh->vertices_end (); ++v_it){
		if (!mesh->is_boundary (*v_it)) continue;
		auto entity = (ENTITY*)(V_ENTITY_PTR[*v_it]);
		if (entity == NULL) continue;
		if ((unsigned long)entity == -1) continue;
		if (is_FACE (entity)){
			if (V_FIXED[*v_it])
				vhs_fixed.insert (*v_it);
			else
				vhs_on_geom_faces.insert (*v_it);
		}
	}

	hoopsview->derender_all_mesh_groups ();
	auto group = new VolumeMeshElementGroup (mesh, "mo", "geom faces vertices");
	group->vhs = vhs_on_geom_faces;
	MeshRenderOptions render_options;
	render_options.vertex_color = "green";
	render_options.vertex_size = 0.7;
	hoopsview->render_mesh_group (group, render_options);
	hoopsview->set_mesh_group_selectability (group, true, false, false);

	group = new VolumeMeshElementGroup (mesh, "mo", "geom faces vertices");
	group->vhs = vhs_fixed;
	render_options.vertex_color = "red";
	render_options.vertex_size = 0.5;
	hoopsview->render_mesh_group (group, render_options);
	hoopsview->set_mesh_group_selectability (group, true, false, false);

	hoopsview->begin_select_by_click ();

	hoopsview->show_mesh_faces (true);
	hoopsview->show_mesh_edges (true);
}

void MeshEditController::on_cancel_select_node_for_move_for_mo ()
{
	hoopsview->clear_selection ();
	hoopsview->begin_camera_manipulate ();
	hoopsview->derender_all_mesh_groups ();
}

void MeshEditController::on_move_node_for_mo ()
{
	if (!mesh->vertex_property_exists<unsigned long> ("entityptr")) return;
	auto V_ENTITY_PTR = mesh->request_vertex_property<unsigned long> ("entityptr");

	selected_vhs.clear ();
	if (!hoopsview->get_selected_elements (&selected_vhs, NULL, NULL)){
		QMessageBox::warning (this, tr("警告"), tr("未选中任何网格点！"), QMessageBox::Ok);
		return;
	}
	//if (selected_vhs.size () != 1){
	//	QMessageBox::warning (this, tr("警告"), tr("只能一次移动一个点！"), QMessageBox::Ok);
	//	return;
	//
	hoopsview->clear_selection ();
	hoopsview->begin_camera_manipulate ();
	hoopsview->derender_all_mesh_groups ();

	hoopsview->move_vertices (mesh, selected_vhs);
}

void MeshEditController::on_move_node_ok_for_mo ()
{
	hoopsview->move_vertices_ok (body);
	update_mesh ();
}

void MeshEditController::on_select_node_pair_for_mo ()
{
	if (!mesh->vertex_property_exists<unsigned long> ("entityptr")) return;
	auto V_ENTITY_PTR = mesh->request_vertex_property<unsigned long> ("entityptr");
	
	std::unordered_set<OvmVeH> vhs_not_on_geom_vertices;
	std::hash_map<ENTITY*, OvmVeH> geom_mapping;
	for (auto v_it = mesh->vertices_begin (); v_it != mesh->vertices_end (); ++v_it){
		if (!mesh->is_boundary (*v_it)) continue;
		auto entity = (ENTITY*)(V_ENTITY_PTR[*v_it]);
		if (entity == NULL) continue;
		if ((unsigned long)entity == -1) continue;
		if (!is_VERTEX (entity))
			vhs_not_on_geom_vertices.insert (*v_it);
		else
			geom_mapping[entity] = *v_it;
	}
	ENTITY_LIST vertices_list;
	api_get_vertices (body, vertices_list);
	std::set<VERTEX*> unmatched_vertices;
	for (int i = 0; i != vertices_list.count (); ++i){
		auto vt = vertices_list[i];
		if (geom_mapping.find (vt) == geom_mapping.end ())
			unmatched_vertices.insert ((VERTEX*)vt);
	}
	if (unmatched_vertices.empty ()){
		QMessageBox::information (this, "提示", "所有几何点均已经匹配网格节点！");
		return;
	}

	hoopsview->derender_all_mesh_groups ();
	auto group = new VolumeMeshElementGroup (mesh, "mo", "unmatched geom vertices");
	group->vhs = vhs_not_on_geom_vertices;
	MeshRenderOptions render_options;
	render_options.vertex_color = "green";
	render_options.vertex_size = 0.7;
	hoopsview->render_mesh_group (group, render_options);
	hoopsview->set_mesh_group_selectability (group, true, false, false);

	hoopsview->begin_select_by_click ();

	hoopsview->show_mesh_faces (true);
	hoopsview->show_mesh_edges (true);
}

void MeshEditController::on_pair_node_ok_for_mo ()
{
	if (!mesh->vertex_property_exists<unsigned long> ("entityptr")) return;
	auto V_ENTITY_PTR = mesh->request_vertex_property<unsigned long> ("entityptr");

	selected_vhs.clear ();
	if (!hoopsview->get_selected_elements (&selected_vhs, NULL, NULL)){
		QMessageBox::warning (this, tr("警告"), tr("未选中任何网格点！"), QMessageBox::Ok);
		return;
	}
	hoopsview->clear_selection ();
	hoopsview->begin_camera_manipulate ();

	std::hash_map<ENTITY*, OvmVeH> geom_mapping;
	for (auto v_it = mesh->vertices_begin (); v_it != mesh->vertices_end (); ++v_it){
		if (!mesh->is_boundary (*v_it)) continue;
		auto entity = (ENTITY*)(V_ENTITY_PTR[*v_it]);
		if (entity == NULL) continue;
		if (is_VERTEX (entity))
			geom_mapping[entity] = *v_it;
	}

	//////////////////////////////////////////////////////////////////////////
	//std::hash_map<ENTITY*, OvmVeH> geom_mapping;
	//for (auto v_it = mesh->vertices_begin (); v_it != mesh->vertices_end (); ++v_it){
	//	if (!mesh->is_boundary (*v_it) || is_FACE (m_face)) continue;
	//	auto entity = (ENTITY*)(V_ENTITY_PTR[*v_it]);
	//	if (entity == NULL) continue;
	//	if (is_VERTEX (entity))
	//		geom_mapping[entity] = *v_it;
	//}
	//////////////////////////////////////////////////////////////////////////

	ENTITY_LIST vertices_list;
	api_get_vertices (body, vertices_list);
	std::set<VERTEX*> unmatched_vertices;
	for (int i = 0; i != vertices_list.count (); ++i){
		auto vt = vertices_list[i];
		if (geom_mapping.find (vt) == geom_mapping.end ())
			unmatched_vertices.insert ((VERTEX*)vt);
	}

	hoopsview->derender_all_mesh_groups ();
	if (unmatched_vertices.empty ()) return;
	foreach (auto &vh, selected_vhs){
		auto vh_pos = mesh->vertex (vh);
		double closest_dist = std::numeric_limits<double>::max ();
		VERTEX *closest_vt = NULL;
		OvmVec3d closest_pos (0,0,0);
		foreach (auto vt, unmatched_vertices){
			auto pt = vt->geometry ()->coords ();
			auto pos = SPA2POS(pt);
			auto dist = (vh_pos - pos).length ();
			if (dist < closest_dist){
				closest_dist = dist;
				closest_vt = vt;
				closest_pos = pos;
			}
		}
		if (closest_vt == NULL){
			QMessageBox::warning (this, "错误", "该节点找不到几何点！");
			return;
		}
		V_ENTITY_PTR[vh] = (unsigned long)closest_vt;
		unmatched_vertices.erase (closest_vt);
		mesh->set_vertex (vh, closest_pos);
	}

	update_mesh ();
}

void MeshEditController::on_smooth_mesh ()
{
	int smooth_times = ui.spinBox_Smooth_Times->value ();
	JC::smooth_volume_mesh (mesh, body, smooth_times, hoopsview);
	update_mesh ();
}

void MeshEditController::on_select_geom_face_for_mo ()
{
	hoopsview->show_acis_faces (true);
	hoopsview->set_acis_faces_selectable (true);
	hoopsview->show_mesh_faces (false);
	hoopsview->begin_select_by_click ();
}

void MeshEditController::on_smooth_geom_face_for_mo ()
{
	std::vector<FACE*> selected_faces;
	if (!hoopsview->get_selected_acis_entities (NULL, NULL, &selected_faces)){
		QMessageBox::warning (this, tr("警告"), tr("未选中任何几何面！"), QMessageBox::Ok);
		return;
	}

	JC::smooth_acis_faces (mesh, selected_faces, 5, hoopsview);
	update_mesh ();
}

void MeshEditController::on_continue_select_geom_face_for_mo ()
{
	hoopsview->begin_select_by_click ();
}

void MeshEditController::on_clear_select_faces_for_mo ()
{
	hoopsview->clear_selection ();
	hoopsview->set_acis_faces_selectable (false);
	hoopsview->show_acis_faces (false);
	hoopsview->show_mesh_faces (true);
	hoopsview->begin_camera_manipulate ();
}

void MeshEditController::on_rotate_for_mo ()
{
	hoopsview->begin_camera_manipulate ();
}
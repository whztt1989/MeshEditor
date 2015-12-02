#include "StdAfx.h"
#include "mesheditwidget.h"

void MeshEditWidget::on_analyze_for_lc ()
{
	if (ossi_handler) delete ossi_handler;
	ossi_handler = new OneSimpleSheetInflationHandler (mesh, body, hoopsview);
	ossi_handler->init ();

	std::vector<OvmEgH> selected_ehs;
	std::vector<OvmFaH> selected_fhs;
	if (!hoopsview->get_selected_elements (std::vector<OvmVeH>(), selected_ehs, selected_fhs)){
		QMessageBox::warning (this, tr("警告"), tr("未选中任何网格边！"), QMessageBox::Ok);
		return;
	}
	if (selected_fhs.empty ()) return;
	std::unordered_set<OvmEgH> selected_ehs_set;
	JC::vector_to_unordered_set (selected_ehs, selected_ehs_set);

	if (!ossi_handler->analyze_input_edges (selected_ehs_set)){
		QMessageBox::critical (this, tr("错误"), ossi_handler->get_last_err (), QMessageBox::Ok);
		return;
	}

	FACE *inter_face = NULL;
	auto V_ENTITY_PTR = mesh->request_vertex_property<unsigned long> ("entityptr");
	foreach (OvmEgH eh, selected_ehs){
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
	ossi_handler->update_inter_face_fhs ();

	FACE *constant_face = NULL;

	
	foreach (auto &fh, selected_fhs){
		auto adj_vhs = JC::get_adj_vertices_around_face (mesh, fh);
		foreach (auto &vh, adj_vhs){
			ENTITY *entity = (ENTITY*)(V_ENTITY_PTR[vh]);
			if (is_FACE (entity))
				constant_face = (FACE *)entity;
		}
	}
	ossi_handler->set_const_face (constant_face);
	ossi_handler->update_const_face_fhs ();

	ossi_handler->retrieve_modify_allowed_fhs_patches ();
	ossi_handler->retrieve_interface_loops ();
	ossi_handler->classify_end_vhs ();
	ossi_handler->evaluate_vhs_on_modify_allowed_fhs ();

	hoopsview->clear_selection ();
	//render
	auto group = new VolumeMeshElementGroup (mesh, "lc", "select edges");
	group->ehs = selected_ehs_set;
	MeshRenderOptions render_options;
	render_options.edge_width = 3;
	hoopsview->render_mesh_group (group, render_options);
}

void MeshEditWidget::on_set_depth_for_lc ()
{
	ossi_handler->set_minimum_depth (ui.spinBox_Depth_For_LC->value ());

}

void MeshEditWidget::on_search_inter_path_for_lc ()
{

	std::vector<std::pair<OvmEgH, OvmEgH> > edge_pairs;
	auto path = ossi_handler->get_intersect_path (edge_pairs);

	//render
	auto group = new VolumeMeshElementGroup (mesh, "lc", "intersect path");
	JC::vector_to_unordered_set (path, group->ehs);
	MeshRenderOptions render_options;
	render_options.edge_color = "red";
	hoopsview->render_mesh_group (group, render_options);

	char *colors[] = {"yellow", "blue", "pink", "purple"};
	for (int i = 0; i != 4; ++i){
		QString str = QString ("cross edge %1").arg (i);
		group = new VolumeMeshElementGroup (mesh, "lc", str);
		group->ehs.insert (edge_pairs[i].first);
		group->ehs.insert (edge_pairs[i].second);
		render_options.edge_color = colors[i];
		hoopsview->render_mesh_group (group, render_options);
	}

	render_options.edge_color = NULL;
	for (int i = 0; i != 4; ++i){
		QString str = QString ("cross hexas %1").arg (i);
		group = new VolumeMeshElementGroup (mesh, "lc", str);
		group->chs = ossi_handler->inter_cross_hexas[i];
		render_options.cell_color = colors[i];
		hoopsview->render_mesh_group (group, render_options);
	}
}

void MeshEditWidget::on_complete_non_int_loop_for_lc ()
{
	auto added_ehs = ossi_handler->connect_nonintersect_loop_parts ();
	//render
	auto group = new VolumeMeshElementGroup (mesh, "lc", "nonintersect parts");
	group->ehs = added_ehs;
	MeshRenderOptions render_options;
	render_options.edge_color = "magenta";
	hoopsview->render_mesh_group (group, render_options);
}

void MeshEditWidget::on_complete_int_loop_for_lc ()
{
	auto added_ehs = ossi_handler->connect_intersect_loop_parts ();
	//render
	auto group = new VolumeMeshElementGroup (mesh, "lc", "intersect parts");
	group->ehs = added_ehs;
	MeshRenderOptions render_options;
	render_options.edge_color = "red";
	hoopsview->render_mesh_group (group, render_options);
	completed_loop_ehs = ossi_handler->input_ehs;
}
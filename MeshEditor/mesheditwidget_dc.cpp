#include "StdAfx.h"
#include "mesheditwidget.h"

void MeshEditWidget::on_set_start_face_for_dc ()
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

	if (depth_cut_handler)
		delete depth_cut_handler;
	depth_cut_handler = new DepthCutHandler (mesh, body);
	depth_cut_handler->start_face = interface_face;
	depth_cut_handler->get_start_fhs ();

	hoopsview->derender_all_mesh_groups ();
	auto group = new VolumeMeshElementGroup (mesh, "dc", "start faces");
	group->fhs = depth_cut_handler->start_fhs;
	MeshRenderOptions render_options;
	render_options.face_color = "(diffuse=red,transmission = (r=0.2 g=0.2 b=0.2))";
	hoopsview->render_mesh_group (group, render_options);

}

void MeshEditWidget::on_set_depth_for_dc ()
{
	int depth_num = ui.spinBox_Depth_For_DC->value ();
	depth_cut_handler->depth_num = depth_num;
	depth_cut_handler->get_hexas_within_depth ();

	hoopsview->derender_mesh_groups ("dc", "joint faces");
	hoopsview->derender_mesh_groups ("dc", "hexas within depth");

	auto group = new VolumeMeshElementGroup (mesh, "dc", "joint faces");
	group->fhs = depth_cut_handler->joint_fhs;
	MeshRenderOptions render_options;
	render_options.face_color = "(diffuse=green,transmission = (r=0.2 g=0.2 b=0.2))";
	hoopsview->render_mesh_group (group, render_options);

	group = new VolumeMeshElementGroup (mesh, "dc", "hexas within depth");
	group->chs = depth_cut_handler->hexas_within_depth;
	render_options.face_color = NULL;
	render_options.cell_color = "(diffuse=blue,transmission = (r=0.2 g=0.2 b=0.2))";
	hoopsview->render_mesh_group (group, render_options);
}

void MeshEditWidget::on_create_cut_face_for_dc ()
{
	depth_cut_handler->create_cutting_face ();

	HC_Open_Segment_By_Key (hoopsview->GetHoopsView ()->GetModelKey ());{
		HC_Open_Segment ("acismodel");{
			HC_Flush_Geometry ("...");
			HA_Render_Entity (depth_cut_handler->cut_face, ".");
		}HC_Close_Segment ();
	}HC_Close_Segment ();
	hoopsview->GetHoopsView ()->SetGeometryChanged ();
	hoopsview->GetHoopsView ()->Update ();
}

void MeshEditWidget::on_depth_cut_for_dc ()
{
	depth_cut_handler->depth_cutting ();

	hoopsview->derender_all_mesh_groups ();
	hoopsview->derender_VolumeMesh ();
	hoopsview->render_VolumeMesh (depth_cut_handler->new_mesh);

	HC_Open_Segment_By_Key (hoopsview->GetHoopsView ()->GetModelKey ());{
		HC_Open_Segment ("acismodel");{
			HC_Flush_Geometry ("...");
			//HC_Set_Color ("faces=(diffuse=green,transmission = (r=0.2 g=0.2 b=0.2))");
			HA_Render_Entity (depth_cut_handler->body, ".");
		}HC_Close_Segment ();
	}HC_Close_Segment ();

	hoopsview->GetHoopsView ()->SetGeometryChanged ();
	hoopsview->GetHoopsView ()->Update ();
}

void MeshEditWidget::on_save_for_dc ()
{
	depth_cut_handler->save_data ();
	QMessageBox::information (this, "提示", "保存成功!");
}
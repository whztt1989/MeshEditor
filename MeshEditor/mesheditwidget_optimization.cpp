#include "stdafx.h"
#include "mesheditwidget.h"

void MeshEditWidget::on_start_smoothing ()
{
	int smooth_rounds = ui.spinBox_Smooth_Rounds->value ();
	JC::smooth_volume_mesh (mesh, body, smooth_rounds);
	hoopsview->rerender_VolumeMesh (mesh);
	hoopsview->update_mesh_groups ();
}

void MeshEditWidget::on_select_geom_faces ()
{
	hoopsview->show_acis_faces ( true);
	hoopsview->set_acis_faces_selectable ( true);
	hoopsview->show_mesh_faces (false);
	hoopsview->begin_select_by_click ();
}

void MeshEditWidget::on_smooth_faces ()
{
	std::vector<FACE*> selected_faces;
	if (!hoopsview->get_selected_acis_entities (NULL, NULL, &selected_faces)){
		QMessageBox::warning (this, tr("警告"), tr("未选中任何几何面！"), QMessageBox::Ok);
		return;
	}

	JC::smooth_acis_faces (mesh, selected_faces, 5, hoopsview);
	hoopsview->rerender_VolumeMesh (mesh);

	if (new_sheet){
		//hoopsview->derender_one_mesh_group (new_sheet);
		hoopsview->update_mesh_group (new_sheet);
	}
}

void MeshEditWidget::on_clear_select_geom_faces ()
{
	hoopsview->clear_selection ();
	hoopsview->set_acis_faces_selectable ( false);
	hoopsview->show_acis_faces ( false);
	hoopsview->show_mesh_faces (true);
	hoopsview->begin_camera_manipulate ();
}
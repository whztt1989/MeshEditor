#include "StdAfx.h"
#include "mousecontrolwidget.h"
#include "hoopsview.h"
#include "mesheditwidget.h"
#include <geom_utl.hxx>

MouseControlWidget::MouseControlWidget(HoopsView *_hoopsview, QWidget *parent)
	: hoopsview (_hoopsview), QWidget(parent)
{
	ui.setupUi(this);
	setup_actions ();
	ui.stackedWidget->setCurrentIndex (1);
	mesh_edit_widget = (MeshEditWidget *)parent;
	ui.radioButton_Camera_Manipulate->toggle ();
}

MouseControlWidget::~MouseControlWidget()
{

}

void MouseControlWidget::start_selecting_vertices ()
{
	ui.radioButton_Selection->toggle ();
	ui.radioButton_Select_Vertices->toggle ();
	ui.stackedWidget->setCurrentIndex (0);
	on_set_vertices_selectable ();
	on_begin_selection ();
}

void MouseControlWidget::start_selecting_edges ()
{
	ui.radioButton_Selection->toggle ();
	ui.radioButton_Select_Edges->toggle ();
	ui.stackedWidget->setCurrentIndex (0);
	on_set_edges_selectable ();
	on_begin_selection ();
}

void MouseControlWidget::start_selecting_faces ()
{
	ui.radioButton_Selection->toggle ();
	ui.radioButton_Select_Faces->toggle ();
	ui.stackedWidget->setCurrentIndex (0);
	on_set_faces_selectable ();
	on_begin_selection ();
}

void MouseControlWidget::start_camera_manipulate ()
{
	ui.stackedWidget->setCurrentIndex (1);
	hoopsview->begin_camera_manipulate ();
}
void MouseControlWidget::setup_actions ()
{
	connect (ui.radioButton_Selection, SIGNAL (clicked ()), SLOT (on_begin_selection ()));
	connect (ui.radioButton_Camera_Manipulate, SIGNAL (clicked ()), SLOT (on_begin_camera_manipulate ()));
	connect (ui.radioButton_Cutting, SIGNAL (clicked ()), SLOT (on_begin_cutting ()));
	connect (ui.pushButton_Compute_Cutting, SIGNAL (clicked ()), SLOT (on_calc_cutting ()));
	connect (ui.pushButton_Restore_Cutting, SIGNAL (clicked ()), SLOT (on_restore_cutting ()));

	connect (ui.radioButton_Select_By_Click, SIGNAL (clicked ()), hoopsview, SLOT (begin_select_by_click ()));
	connect (ui.radioButton_Select_By_Rectangle, SIGNAL (clicked ()), hoopsview, SLOT (begin_select_by_rectangle ()));
	connect (ui.radioButton_Select_By_Polygon, SIGNAL (clicked ()), hoopsview, SLOT (begin_select_by_polygon ()));

	connect (ui.radioButton_Select_Vertices, SIGNAL (clicked ()), SLOT (on_set_vertices_selectable (bool)));
	connect (ui.radioButton_Select_Edges, SIGNAL (clicked ()), SLOT (on_set_edges_selectable (bool)));
	connect (ui.radioButton_Select_Faces, SIGNAL (clicked ()), SLOT (on_set_faces_selectable (bool)));

	connect (ui.pb_Op_OK, SIGNAL (clicked ()), SLOT (on_operate_ok ()));
	//connect (ui.radioButton_Select_Cells, SIGNAL (toggled (bool)), hoopsview, SLOT (set_cells_selectable (bool)));
}

void MouseControlWidget::on_set_vertices_selectable ()
{
	hoopsview->set_vertices_selectable (true);
	hoopsview->set_edges_selectable (false);
	hoopsview->set_faces_selectable (false);
}

void MouseControlWidget::on_set_edges_selectable ()
{
	hoopsview->set_vertices_selectable (false);
	hoopsview->set_edges_selectable (true);
	hoopsview->set_faces_selectable (false);
}

void MouseControlWidget::on_set_faces_selectable ()
{
	hoopsview->set_vertices_selectable (false);
	hoopsview->set_edges_selectable (false);
	hoopsview->set_faces_selectable (true);
}


void MouseControlWidget::on_begin_selection ()
{
	ui.stackedWidget->setCurrentIndex (0);
	if (ui.radioButton_Select_By_Click->isChecked ())
		hoopsview->begin_select_by_click ();
	else if (ui.radioButton_Select_By_Rectangle->isChecked ())
		hoopsview->begin_select_by_rectangle ();
	else if (ui.radioButton_Select_By_Polygon->isChecked ())
		hoopsview->begin_select_by_polygon ();
}

void MouseControlWidget::on_begin_camera_manipulate ()
{
	start_camera_manipulate ();
}

void MouseControlWidget::on_begin_cutting ()
{
	ui.stackedWidget->setCurrentIndex (2);
	hoopsview->begin_cutting ();
}

void MouseControlWidget::on_calc_cutting ()
{
	VolumeMesh *mesh = mesh_edit_widget->get_mesh ();
	HC_KEY cutting_plane_key = hoopsview->get_cutting_plane_key ();
	if (cutting_plane_key == INVALID_KEY) return;

	int points_count = 0;
	HPoint points[4], transformed_points[4];
	HC_Show_Polygon (cutting_plane_key, &points_count, points);
	float matrix_aa[100] = {0};
	HC_Open_Segment_By_Key (hoopsview->GetHoopsView ()->GetModelKey ());{
		HC_Open_Segment ("OpCutMesh");{
		//HC_Show_Modelling_Matrix (matrix_aa);
		HC_Show_Net_Modelling_Matrix (matrix_aa);
		}HC_Close_Segment ();
	}HC_Close_Segment ();
	HC_Compute_Transformed_Points (points_count, points, matrix_aa, transformed_points);
	OvmVec3d centre (0, 0, 0), ovm_points[4];
	for (int i = 0; i != 4; ++i){
		ovm_points[i] = OvmVec3d (transformed_points[i].x, transformed_points[i].y, transformed_points[i].z);
		centre += ovm_points[i];
	}
	centre /= 4;
	OvmVec3d vec1 = ovm_points[1] - ovm_points[0],
		vec2 = ovm_points[2] - ovm_points[1];
	OvmVec3d nor = vec1 % vec2;

	SPAposition root (centre[0], centre[1], centre[2]);
	SPAvector vec_nor (nor[0], nor[1], nor[2]);
	SPAunit_vector unit_vec_nor = normalise (vec_nor);

	std::vector<OvmVeH> one_side_vhs;
	for (auto v_it = mesh->vertices_begin (); v_it != mesh->vertices_end (); ++v_it){
		auto pos = mesh->vertex (*v_it);
		auto spa_pos = POS2SPA (pos);
		if (side_of_plane (root, unit_vec_nor, spa_pos) > 0){
			one_side_vhs.push_back (*v_it);
		}
	}

	cut_left_chs.clear ();
	foreach (auto &vh, one_side_vhs){
		JC::get_adj_hexas_around_vertex (mesh, vh, cut_left_chs);
	}

	hoopsview->show_VolumeMesh (false);
	HC_Open_Segment_By_Key (hoopsview->GetHoopsView ()->GetModelKey ());{
		HC_Open_Segment ("cutmesh");{
			HC_Set_Color ("faces=grey");
			HC_Set_Visibility ("edges=on");
			HC_Set_Rendering_Options ("no lighting interpolation");
			JC::insert_boundary_shell (mesh, cut_left_chs);
		}HC_Close_Segment ();
	}HC_Close_Segment ();
	ui.radioButton_Camera_Manipulate->setChecked (true);
	on_begin_camera_manipulate ();
	hoopsview->clear_cutting_plane ();
}

void MouseControlWidget::on_restore_cutting ()
{
	hoopsview->clear_cutting_plane ();
	HC_Open_Segment_By_Key (hoopsview->GetHoopsView ()->GetModelKey ());{
		HC_Delete_Segment ("cutmesh");
	}HC_Close_Segment ();
	hoopsview->show_VolumeMesh (true);
	ui.radioButton_Camera_Manipulate->setChecked (true);
	on_begin_camera_manipulate ();
}

void MouseControlWidget::on_operate_ok ()
{
	emit opt_ok ();
	hide ();
}
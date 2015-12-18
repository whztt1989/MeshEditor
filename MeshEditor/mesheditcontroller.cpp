#include "StdAfx.h"
#include "mesheditcontroller.h"
#include "hoopsview.h"
#include <QMessageBox>
#include "MeshMatchingHandler.h"
#include "ha_bridge.h"

MeshEditController::MeshEditController(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ossi_handler = NULL;
	//sheet_to_extract = assist_sheet = NULL;
	//column_to_collapse = NULL;
	selected_chord = NULL;
	sheet_to_extract = NULL;
	direct_get_inflation_quad_set = false;

	ui.toolBox->setCurrentIndex (0);
	ui.stackedWidget_Smooth_Methods->setCurrentIndex (0);

	group_controller = NULL;
	mouse_controller = NULL;
	choices_controller = new ChoicesSelectionWidget (this);
	choices_controller->setWindowFlags (choices_controller->windowFlags () | Qt::Tool);

	connect (ui.pb_Get_Int_Quads_For_Pre, SIGNAL (clicked ()), SLOT (on_get_int_face_quads ()));
	connect (ui.pb_Adjust_Int_Quads_For_Pre, SIGNAL (clicked ()), SLOT (on_adjust_int_face_quads ()));
	connect (ui.pb_Show_Group_Manager, SIGNAL (clicked ()), SLOT (on_show_group_manager ()));
	connect (ui.cb_Show_Interfaces, SIGNAL (clicked ()), SLOT (on_show_interfaces ()));
	connect (ui.cb_Show_Body_Vertices, SIGNAL (clicked ()), SLOT (on_show_body_vertices ()));
	connect (ui.cb_Show_Body_Edges, SIGNAL (clicked ()), SLOT (on_show_body_edges ()));
	connect (ui.cb_Show_Body_Faces, SIGNAL (clicked ()), SLOT (on_show_body_faces ()));
	connect (ui.cb_Show_Bnd_Vertices, SIGNAL (clicked ()), SLOT (on_show_bnd_vertices ()));
	connect (ui.cb_Show_Bnd_Edges, SIGNAL (clicked ()), SLOT (on_show_bnd_edges ()));
	connect (ui.cb_Show_Bnd_Faces, SIGNAL (clicked ()), SLOT (on_show_bnd_faces ()));

	connect (ui.pb_Direct_Get_Quad_Set, SIGNAL (clicked ()), SLOT (on_get_quad_set_directly_for_si ()));
	connect (ui.pb_Select_Edges_For_SI, SIGNAL (clicked ()), SLOT (on_select_edges_for_si ()));
	connect (ui.pb_Read_Loop_Ehs, SIGNAL (clicked ()), SLOT (on_read_loop_ehs_for_si ()));
	connect (ui.pb_Save_Loop_Ehs, SIGNAL (clicked ()), SLOT (on_save_loop_ehs_for_si ()));
	connect (ui.pb_Sweep_Get_Quad_Set, SIGNAL (clicked ()), SLOT (on_get_quad_set_by_sweep_for_si ()));
	connect (ui.pb_PCS_Depth_Get_Quad_Set, SIGNAL (clicked ()),
		SLOT (on_pcs_depth_get_quad_set_for_si ()));
	
	connect (ui.pushButton_Modify_Quad_Set_For_SI, SIGNAL (clicked ()),
		SLOT (on_modify_quad_set_for_si ()));
	connect (ui.pushButton_Add_Quad_For_SI, SIGNAL (clicked ()),
		SLOT (on_add_quad_for_si ()));
	connect (ui.pushButton_Remove_Quad_For_SI, SIGNAL (clicked ()),
		SLOT (on_remove_quad_for_si ()));
	connect (ui.pushButton_Modify_Quad_Set_OK_For_SI, SIGNAL (clicked ()),
		SLOT (on_modify_quad_set_ok_for_si ()));
	connect (ui.pb_Determine_Shrink_Set, SIGNAL (clicked ()), SLOT (on_determine_shrink_set_for_si ()));
	connect (ui.pushButton_Sheet_Inflation_For_SI, SIGNAL (clicked ()), SLOT (on_sheet_inflation_for_si ()));
	connect (ui.pushButton_postprocess_for_SI, SIGNAL (clicked ()), SLOT (on_postprocess_for_si ()));

	connect (ui.pushButton_Select_Edges_For_SE, SIGNAL (clicked ()), SLOT (on_select_edges_for_se ()));
	connect (ui.pushButton_Select_Edges_OK_For_SE, SIGNAL (clicked ()), SLOT (on_select_edges_ok_for_se ()));
	connect (ui.pushButton_Extract_Sheet_For_SE, SIGNAL (clicked ()),
		SLOT (on_sheet_extraction_for_se ()));
	connect (ui.pushButton_Postprocess_For_SE, SIGNAL (clicked ()), SLOT (on_postprocess_for_se ()));

	connect (ui.radioButton_Direct_Select_For_CC, SIGNAL (clicked ()),
		SLOT (on_direct_select_for_cc ()));
	connect (ui.radioButton_Select_By_Sheet_For_CC, SIGNAL (clicked ()),
		SLOT (on_select_by_sheets_for_cc ()));
	connect (ui.pushButton_Select_Face_For_CC, SIGNAL (clicked ()),
		SLOT (on_select_face_for_cc ()));
	connect (ui.pushButton_Select_Face_OK_For_CC, SIGNAL (clicked ()),
		SLOT (on_select_face_ok_for_cc ()));
	connect (ui.pushButton_Select_Edges_For_CC, SIGNAL (clicked ()),
		SLOT (on_select_edges_for_cc ()));
	connect (ui.pushButton_Select_Edges_OK_For_CC, SIGNAL (clicked ()),
		SLOT (on_select_edges_ok_for_cc ()));
	connect (ui.pushButton_Rotate_1_For_CC, SIGNAL (clicked ()),
		SLOT (on_rotate_for_cc ()));
	connect (ui.pushButton_Rotate_2_For_CC, SIGNAL (clicked ()),
		SLOT (on_rotate_for_cc ()));
	connect (ui.pushButton_Rotate_3_For_CC, SIGNAL (clicked ()),
		SLOT (on_rotate_for_cc ()));
	connect (ui.pushButton_Continue_Select_Vertices_For_CC, SIGNAL (clicked ()),
		SLOT (on_continue_select_collapse_vhs_for_cc ()));
	connect (ui.pushButton_Select_Collapse_Vertices_For_CC, SIGNAL (clicked ()),
		SLOT (on_select_collapse_vhs_for_cc ()));
	connect (ui.pushButton_Select_Collapse_Vertices_OK_For_CC, SIGNAL (clicked ()),
		SLOT (on_select_collapse_vhs_ok_for_cc ()));
	connect (ui.pushButton_Columns_Collapse_For_CC, SIGNAL (clicked ()),
		SLOT (on_columns_collapse_for_cc ()));
	connect (ui.pushButton_Postprocess_For_CC, SIGNAL (clicked ()),
		SLOT (on_postprocess_for_cc ()));

	connect (ui.pushButton_Select_Edges_For_CO, SIGNAL (clicked ()),
		SLOT (on_select_edges_for_co ()));
	connect (ui.pushButton_Select_Edges_OK_For_CO, SIGNAL (clicked ()),
		SLOT (on_select_edges_ok_for_co ()));
	connect (ui.pb_Show_Chord_Info_For_CO, SIGNAL (clicked ()),
		SLOT (on_show_chord_info_for_co ()));
	connect (ui.pb_Clear_Selection_For_CO, SIGNAL (clicked ()),
		SLOT (on_clear_selection_for_co ()));
	connect (ui.pb_Select_Chord_By_Idx_For_CO, SIGNAL (clicked ()),
		SLOT (on_select_chord_by_idx_for_co ()));
	connect (ui.pb_Select_Edge_For_Adjust_Chord, SIGNAL (clicked ()), SLOT (on_select_edge_for_adjust_chord ()));
	connect (ui.pb_Select_As_1st_Edge_For_Chord, SIGNAL (clicked ()), SLOT (on_select_as_1st_edge_for_chord ()));
	connect (ui.pb_Select_As_2nd_Edge_For_Chord, SIGNAL (clicked ()), SLOT (on_select_as_2nd_edge_for_chord ()));


	connect (ui.radioButton_Smooth_Whole_For_MO, SIGNAL (clicked ()),
		SLOT (on_select_smooth_whole_for_mo ()));
	connect (ui.radioButton_Pair_Geom_Node_For_MO, SIGNAL (clicked ()),
		SLOT (on_select_pair_node_method_for_mo ()));
	connect (ui.radioButton_Smooth_Faces_For_MO, SIGNAL (clicked ()),
		SLOT (on_select_smooth_faces_for_mo ()));
	connect (ui.pushButton_Init_For_Move_Node_For_MO, SIGNAL (clicked ()),
		SLOT (on_init_move_node_for_mo ()));
	connect (ui.pushButton_Select_Node_For_Move_For_MO, SIGNAL (clicked ()),
		SLOT (on_select_node_for_move_for_mo ()));
	connect (ui.pushButton_Cancel_Select_Node_For_Move_For_MO, SIGNAL (clicked ()),
		SLOT (on_cancel_select_node_for_move_for_mo ()));
	connect (ui.pushButton_Move_Node_For_MO, SIGNAL (clicked ()),
		SLOT (on_move_node_for_mo ()));
	connect (ui.pushButton_Move_Node_OK_For_MO, SIGNAL (clicked ()),
		SLOT (on_move_node_ok_for_mo ()));
	connect (ui.pushButton_Select_Node_Pair_For_MO, SIGNAL (clicked ()),
		SLOT (on_select_node_pair_for_mo ()));
	connect (ui.pushButton_Pair_Node_OK_For_MO, SIGNAL (clicked ()),
		SLOT (on_pair_node_ok_for_mo ()));
	connect (ui.pushButton_Smooth, SIGNAL (clicked ()), SLOT (on_smooth_mesh ()));
	connect (ui.pushButton_Select_Geom_Face_For_MO, SIGNAL (clicked ()),
		SLOT (on_select_geom_face_for_mo ()));
	connect (ui.pushButton_Smooth_Geom_Face_For_MO, SIGNAL (clicked ()),
		SLOT (on_smooth_geom_face_for_mo ()));
	connect (ui.pushButton_Rotate_1_For_MO, SIGNAL (clicked ()),
		SLOT (on_rotate_for_mo ()));
	connect (ui.pushButton_Continue_Select_Face_For_MO, SIGNAL (clicked ()),
		SLOT (on_continue_select_geom_face_for_mo ()));
	connect (ui.pushButton_Clear_Select_Faces_For_MO, SIGNAL (clicked ()),
		SLOT (on_clear_select_faces_for_mo ()));

	connect (ui.pushButton_Select_Edges_For_CL, SIGNAL (clicked ()),
		SLOT (on_select_edges_for_cl ()));
	connect (ui.pushButton_Rotate_Camera_For_CL, SIGNAL (clicked ()),
		SLOT (on_rotate_for_cl ()));
	connect (ui.pushButton_Select_Edges_OK_For_CL, SIGNAL (clicked ()),
		SLOT (on_select_edges_ok_for_cl ()));
	connect (ui.pushButton_Cluster_For_CL, SIGNAL (clicked ()),
		SLOT (on_cluster_for_cl ()));
}

MeshEditController::~MeshEditController()
{

}

void MeshEditController::on_get_int_face_quads ()
{
	get_interface_quads ();

	hoopsview->derender_mesh_groups ("mm", "int quads", true);

	auto group = new VolumeMeshElementGroup (mesh, "mm", "int quads");
	group->fhs = int_quads;
	MeshRenderOptions render_options;
	render_options.face_color = "red";
	hoopsview->render_mesh_group (group, render_options);
}

std::unordered_set<OvmFaH> MeshEditController::get_interface_quads ()
{
	JC::get_fhs_on_acis_faces (mesh, inter_faces, int_quads);

	return int_quads;
}

void MeshEditController::on_adjust_int_face_quads ()
{
	MM::adjust_interface_bondary (mesh, int_quads, inter_faces);
	update_mesh ();
	JC::attach_mesh_elements_to_ACIS_entities (mesh, body, SPAresabs * 1000);
}

void MeshEditController::on_show_group_manager ()
{
	if (!group_controller){
		group_controller = new GroupControlWidget (hoopsview, this);
		group_controller->fresh_group_list ();
		group_controller->setWindowFlags (group_controller->windowFlags () | Qt::Tool);
	}
	group_controller->show ();
}

void MeshEditController::on_show_interfaces ()
{
	if (ui.cb_Show_Interfaces->isChecked ()){
		HC_Open_Segment_By_Key (hoopsview->GetHoopsView ()->GetModelKey ());{
			HC_Open_Segment ("interfaces");{
				HC_Set_Visibility ("lines=on");
			}HC_Close_Segment ();
		}HC_Close_Segment ();
	}else{
		HC_Open_Segment_By_Key (hoopsview->GetHoopsView ()->GetModelKey ());{
			HC_Open_Segment ("interfaces");{
				HC_Set_Visibility ("lines=off");
			}HC_Close_Segment ();
		}HC_Close_Segment ();
	}

	hoopsview->GetHoopsView ()->Update ();
}

void MeshEditController::on_show_body_vertices ()
{
	if (ui.cb_Show_Body_Vertices->isChecked ()){
		hoopsview->show_acis_vertices (true);
	}else{
		hoopsview->show_acis_vertices (false);
	}

	hoopsview->GetHoopsView ()->Update ();
}

void MeshEditController::on_show_body_edges ()
{
	if (ui.cb_Show_Body_Edges->isChecked ()){
		hoopsview->show_acis_edges (true);
	}else{
		hoopsview->show_acis_edges (false);
	}

	hoopsview->GetHoopsView ()->Update ();
}

void MeshEditController::on_show_body_faces ()
{
	if (ui.cb_Show_Body_Faces->isChecked ()){
		hoopsview->show_acis_faces (true);
	}else{
		hoopsview->show_acis_faces (false);
	}

	hoopsview->GetHoopsView ()->Update ();
}

void MeshEditController::on_show_bnd_vertices ()
{
	HC_KEY mesh_key = POINTER_TO_KEY(mesh);
	HC_Open_Segment_By_Key (mesh_key);{
		HC_Open_Segment ("meshvertices");{
			if (ui.cb_Show_Bnd_Vertices->isChecked ()){
				HC_UnSet_Visibility ();
				HC_Set_Visibility ("markers=on");		
			}else{
				HC_Set_Visibility ("everything=off");
			}
		}HC_Close_Segment ();
	}HC_Close_Segment ();

	hoopsview->GetHoopsView ()->Update ();
}

void MeshEditController::on_show_bnd_edges ()
{
	HC_KEY mesh_key = POINTER_TO_KEY(mesh);
	HC_Open_Segment_By_Key (mesh_key);{
		HC_Open_Segment ("meshedges");{
			if (ui.cb_Show_Bnd_Edges->isChecked ()){
				HC_UnSet_Visibility ();
				HC_Set_Visibility ("lines=on");		
			}else{
				HC_Set_Visibility ("everything=off");
			}
		}HC_Close_Segment ();
	}HC_Close_Segment ();

	hoopsview->GetHoopsView ()->Update ();
}

void MeshEditController::on_show_bnd_faces ()
{
	HC_KEY mesh_key = POINTER_TO_KEY(mesh);
	HC_Open_Segment_By_Key (mesh_key);{
		HC_Open_Segment ("meshfaces");{
			if (ui.cb_Show_Bnd_Faces->isChecked ()){
				HC_UnSet_Visibility ();
				HC_Set_Visibility ("faces=on");		
			}else{
				HC_Set_Visibility ("everything=off");
			}
		}HC_Close_Segment ();
	}HC_Close_Segment ();

	hoopsview->GetHoopsView ()->Update ();
}


void MeshEditController::update_mesh ()
{
	HC_KEY mesh_key = POINTER_TO_KEY(mesh);
	HC_Open_Segment_By_Key (mesh_key);{
		HC_Flush_Geometry ("...");
		JC::render_volume_mesh_boundary (mesh);
	}HC_Close_Segment ();

	hoopsview->GetHoopsView ()->SetGeometryChanged ();
	hoopsview->GetHoopsView ()->Update ();
}

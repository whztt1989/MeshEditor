#include "StdAfx.h"
#include "mesheditwidget.h"
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QDockWidget>

MeshEditWidget::MeshEditWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	hoopsview = ui.hoopsview;
	setup_actions ();
	mesh = NULL; body = NULL;
	ui.toolBox->setEnabled (false);
	//ossi_handler = NULL;
	osse_handler = NULL;
	depth_cut_handler = NULL;
	new_sheet = NULL;

	pd_group_color = QColor("red");

	ui.label_group_color->setStyleSheet (QString("background-color: rgb(%1, %2, %3);")
		.arg (pd_group_color.red ()).arg (pd_group_color.green ()).arg (pd_group_color.blue ()));

	pd_temp_group = NULL;
	pd_line_weight = 6.0; pd_cell_ratio = 0.8;
	ui.doubleSpinBox_Line_Weight->setValue (pd_line_weight);

}

MeshEditWidget::~MeshEditWidget()
{

}

void MeshEditWidget::load_mesh_and_model (QString mesh_path, QString model_path)
{
	mesh = JC::load_volume_mesh (mesh_path);
	body = JC::load_acis_model (model_path);
	JC::init_volume_mesh (mesh, body, SPAresabs * 1000);

	hoopsview->render_VolumeMesh (mesh);
	JC::retrieve_sheets (mesh, sheet_set);

	hoopsview->show_boundary (true);
	hoopsview->show_boundary_vertices (false);
	hoopsview->show_boundary_edges (true);
	hoopsview->show_boundary_faces (true);
	hoopsview->show_boundary_cells (false);
	hoopsview->show_boundary_vertices_indices (false);
	hoopsview->show_boundary_edges_indices (false);
	hoopsview->show_boundary_faces_indices (false);
	hoopsview->show_boundary_cells_indices (false);

	hoopsview->show_inner (false);
	hoopsview->show_inner_vertices (false);
	hoopsview->show_inner_edges (true);
	hoopsview->show_inner_faces (false);
	hoopsview->show_inner_cells (false);
	hoopsview->show_inner_vertices_indices (false);
	hoopsview->show_inner_edges_indices (false);
	hoopsview->show_inner_faces_indices (false);
	hoopsview->show_inner_cells_indices (false);
	hoopsview->set_edges_selectable (true);

	HC_Open_Segment_By_Key (hoopsview->GetHoopsView ()->GetModelKey ());{
		HC_Open_Segment ("body");{
			HC_Set_Visibility ("markers=off,vertices=off,lines=off,faces=off");
			HA_Render_Entity (body, ".");

		}HC_Close_Segment ();
	}HC_Close_Segment ();
}

void MeshEditWidget::setup_actions ()
{
	file_controller = new FileControlWidget (this);
	file_controller->set_directory_and_filters ("../testdata/meshedit", tr("MeshEditor文件(*.xml);;OVM文件(*.ovm);;SAT文件(*.sat)"));
	mesh_render_controller = new MeshRenderControlWidget (hoopsview, this);
	mouse_controller = new MouseControlWidget (hoopsview, this);
	group_controller = new GroupControlWidget (hoopsview, this);


	std::vector<QToolBar*> one_line_toolbar;

	auto toolbar = new QToolBar (tr("工具栏"), this);
	toolbar->addWidget (file_controller);
	toolbar->addSeparator ();
	toolbar->addWidget (mesh_render_controller);
	toolbar->addSeparator ();
	toolbar->addWidget (mouse_controller);
	toolbar->addSeparator ();
	toolbar->addWidget (group_controller);
	toolbar->setAllowedAreas (Qt::TopToolBarArea | Qt::BottomToolBarArea);
	one_line_toolbar.push_back (toolbar);
	toolbars.push_back (one_line_toolbar);

	ui.stackedWidget_for_QS->setCurrentIndex (0);

	connect (file_controller, SIGNAL (open_file (QString)), SLOT (on_open_file (QString)));
	connect (file_controller, SIGNAL (save_file ()), SLOT (on_save_file ()));
	connect (file_controller, SIGNAL (save_file_as (QString)), SLOT (on_save_file_as (QString)));
	connect (file_controller, SIGNAL (file_close ()), SLOT (on_file_close ()));


	connect (ui.pushButton_Highlight_Mesh_Elements, SIGNAL (clicked ()), SLOT (on_highlight_mesh_elements ()));
	connect (ui.pushButton_Clear_Highlight_Mesh_Elements, SIGNAL (clicked ()), SLOT (on_clear_highlight_mesh_elements ()));

	connect (ui.pushButton_Select_Edges_OK_For_Auto_LSI, SIGNAL (clicked ()), SLOT (on_select_edges_ok_for_auto_lsi ()));
	connect (ui.pushButton_Get_Surf_Patches_For_Auto_LSI, SIGNAL (clicked ()), SLOT (on_get_surf_patches_for_auto_lsi ()));

	connect (ui.pushButton_Set_Depth_Control_For_Auto_LSI, SIGNAL (clicked ()), SLOT (on_set_depth_control_for_auto_lsi ()));
	connect (ui.pushButton_Select_Constant_Faces_OK_For_Auto_LSI, SIGNAL (clicked ()), SLOT (on_select_constant_faces_ok_for_auto_lsi ()));
	connect (ui.pushButton_Get_Hexa_Set, SIGNAL (clicked ()), SLOT (on_get_hexa_set_for_auto_lsi ()));
	connect (ui.pushButton_Get_Quad_Set_For_Auto_LSI, SIGNAL (clicked ()), SLOT (on_get_quad_set_for_auto_lsi ()));
	connect (ui.pushButton_Sheet_Inflate_For_Auto_LSI, SIGNAL (clicked ()), SLOT (on_sheet_inflate_for_auto_lsi ()));
	connect (ui.pushButton_Reprocess_For_Auto_LSI, SIGNAL (clicked ()), SLOT (on_reprocess_for_auto_lsi ()));
	connect (ui.pushButton_Get_QS_By_PCS_For_QS, SIGNAL (clicked ()), SLOT (on_get_qs_by_pcs_for_qs ()));
	connect (ui.pushButton_Get_QS_By_Sweeping_For_QS, SIGNAL (clicked ()), SLOT (on_get_qs_by_sweeping_for_qs ()));

	connect (ui.pushButton_Select_Sheet_For_LSE, SIGNAL (clicked ()), SLOT (on_select_sheet_for_lse ()));
	connect (ui.pushButton_Select_Interface_Faces_For_LSE, SIGNAL (clicked ()), SLOT (on_select_interface_faces_for_lse ()));
	connect (ui.pushButton_Select_Constant_Faces_For_LSE, SIGNAL (clicked ()), SLOT (on_select_constant_faces_for_lse ()));
	connect (ui.pushButton_Extract_All_For_LSE, SIGNAL (clicked ()), SLOT (on_extract_all_for_lse ()));
	connect (ui.pushButton_Local_Inflation_For_LSE, SIGNAL (clicked ()), SLOT (on_local_inflate_for_lse ()));
	connect (ui.pushButton_Sep_Hex_mesh, SIGNAL (clicked ()), SLOT (on_sep_hex_mesh ()));
	connect (ui.pushButton_Show_Int_Diagram_For_LSE, SIGNAL (clicked ()), SLOT (on_show_int_diagram_for_lse ()));

	connect (ui.pushButton_Analyze_For_LC, SIGNAL (clicked ()), SLOT (on_analyze_for_lc ()));
	connect (ui.pushButton_Set_Min_Depth_For_LC, SIGNAL (clicked ()), SLOT (on_set_depth_for_lc ()));
	connect (ui.pushButton_Search_Intersect_Path_For_LC, SIGNAL (clicked ()), SLOT (on_search_inter_path_for_lc ()));
	connect (ui.pushButton_Non_Int_Loop_Connect_For_LC, SIGNAL (clicked ()), SLOT (on_complete_non_int_loop_for_lc ()));
	connect (ui.pushButton_Int_Loop_Connect_For_LC, SIGNAL (clicked ()), SLOT (on_complete_int_loop_for_lc ()));

	connect (ui.pushButton_Set_Start_Face_For_DC, SIGNAL (clicked ()), SLOT (on_set_start_face_for_dc ()));
	connect (ui.pushButton_Set_Depth_For_DC, SIGNAL (clicked ()), SLOT (on_set_depth_for_dc ()));
	connect (ui.pushButton_Create_Cut_Face_For_DC, SIGNAL (clicked ()), SLOT (on_create_cut_face_for_dc ()));
	connect (ui.pushButton_Depth_Cut_For_DC, SIGNAL (clicked ()), SLOT (on_depth_cut_for_dc ()));
	connect (ui.pushButton_Save_For_DC, SIGNAL (clicked ()), SLOT (on_save_for_dc ()));

	connect (ui.pushButton_Select_Edges_OK_For_QS, SIGNAL (clicked ()), SLOT (on_select_edges_ok_for_qs ()));
	connect (ui.pushButton_Select_Constrait_Faces_OK_For_QS, SIGNAL (clicked ()), SLOT (on_select_constraint_faces_ok_for_qs ()));
//	connect (ui.pushButton_Complete_Loop_For_QS, SIGNAL (clicked ()), SLOT (on_complete_loop_for_qs ()));
//	connect (ui.pushButton_Optimize_Loop_For_QS, SIGNAL (clicked ()), SLOT (on_optimize_loop_for_qs ()));
	connect (ui.pushButton_Get_Quad_Set_For_QS, SIGNAL (clicked ()), SLOT (on_get_quad_set_for_qs ()));
	connect (ui.pushButton_Optimize_Quad_Set_For_QS, SIGNAL (clicked ()), SLOT (on_optimize_quad_set_for_qs ()));
//	connect (ui.pushButton_Search_Int_Path_For_QS, SIGNAL (clicked ()), SLOT (on_search_int_path_for_qs ()));
	connect (ui.pushButton_select_cross_hexa_OK, SIGNAL (clicked ()), SLOT (on_select_cross_hexa_ok_for_qs ()));
	connect (ui.pushButton_Inflate_Sheet_For_QS, SIGNAL (clicked ()), SLOT (on_inflate_sheet_for_qs ()));
	connect (ui.pushButton_Postprocess_For_QS, SIGNAL (clicked ()), SLOT (on_postprocess_for_qs ()));
	connect (ui.pushButton_Add_Hexs_For_QS, SIGNAL (clicked ()), SLOT (on_add_hexs_for_qs ()));
	connect (ui.pushButton_Remove_Hexs_For_QS, SIGNAL (clicked ()), SLOT (on_remove_hexs_for_qs ()));
	connect (ui.pushButton_Modify_Quad_Set_OK_For_QS, SIGNAL (clicked ()), SLOT (on_modify_quad_set_ok_for_qs ()));
	connect (ui.pushButton_Optimize_Once_For_QS, SIGNAL (clicked ()), SLOT (on_optimize_once_for_qs ()));
	connect (ui.pushButton_Current_Quality_For_QS, SIGNAL (clicked ()), SLOT (on_current_quality_for_qs ()));
	connect (ui.pushButton_Pair_One_Int_Vhs_OK_for_QS, SIGNAL (clicked ()), SLOT (on_pair_one_int_vhs_ok_for_qs()));
	connect (ui.pushButton_Pair_Int_Vhs_Return_For_QS, SIGNAL (clicked ()), SLOT (on_pair_int_vhs_return_for_qs ()));
	connect (ui.pushButton_Close_Loop_For_QS, SIGNAL (clicked ()), SLOT (on_close_loop_for_qs ()));
	connect (ui.pushButton_Append_Loop_For_QS, SIGNAL (clicked ()), SLOT (on_append_loop_for_qs ()));

//	connect (ui.pushButton_Which_Type_For_QS, SIGNAL (clicked ()), SLOT (on_which_type_for_qs ()));

	connect (ui.pushButton_Read_Data_For_QS, SIGNAL (clicked ()), SLOT (on_read_data_for_qs ()));
	connect (ui.pushButton_Save_Data_For_QS, SIGNAL (clicked ()), SLOT (on_save_data_for_qs ()));

	connect (ui.pushButton_Next_Int_Edge_For_QS, SIGNAL (clicked ()), SLOT (on_select_next_int_edge_for_qs ()));
	connect (ui.pushButton_Select_Int_Edges_OK_For_QS, SIGNAL (clicked ()), SLOT (on_select_int_edges_ok_for_qs ()));

//	connect (ui.pushButton_Get_Corres_Edges_For_QS, SIGNAL (clicked ()), SLOT (on_get_corres_edges_for_qs ()));

	connect (ui.pushButton_Pair_Int_Vhs_For_QS, SIGNAL (clicked ()), SLOT (on_pair_int_vhs_for_qs ()));

	connect (ui.pushButton_Test1, SIGNAL (clicked ()), SLOT (on_test1 ()));
	connect (ui.pushButton_Test2, SIGNAL (clicked ()), SLOT (on_test2 ()));

	connect (ui.pushButton_Start_Smoothing, SIGNAL (clicked ()), SLOT (on_start_smoothing ()));
	connect (ui.pushButton_Select_Geom_Face, SIGNAL (clicked ()), SLOT (on_select_geom_faces ()));
	connect (ui.pushButton_Clear_Selected_Faces, SIGNAL (clicked ()), SLOT (on_clear_select_geom_faces ()));
	connect (ui.pushButton_Smooth_Faces, SIGNAL (clicked ()), SLOT (on_smooth_faces ()));

	connect (ui.pushButton_Set_Color_For_PD, SIGNAL (clicked ()), SLOT (on_set_color_for_pd ()));
	connect (ui.pushButton_Add_Element_For_PD, SIGNAL (clicked ()), SLOT (on_add_element_for_pd ()));
	connect (ui.pushButton_Remove_Element_For_PD, SIGNAL (clicked ()), SLOT (on_remove_element_for_pd ()));
	connect (ui.pushButton_Add_Element_Group_For_PD, SIGNAL (clicked ()), SLOT (on_add_element_group_for_pd ()));
	connect (ui.doubleSpinBox_Line_Weight, SIGNAL (valueChanged (double)), SLOT (on_set_line_weight_for_pd ()));
	connect (ui.doubleSpinBox_Cell_Ratio, SIGNAL (valueChanged (double)), SLOT (on_set_cell_ratio_for_pd ()));
	connect (ui.pushButton_Add_Cells_For_PD, SIGNAL (clicked ()), SLOT (on_add_cells_for_pd ()));
	connect (ui.pushButton_Remove_Cells_For_PD, SIGNAL (clicked ()), SLOT (on_remove_cells_for_pd ()));
	connect (ui.pushButton_Recursive_Spreading, SIGNAL (clicked ()), SLOT (on_recursive_spreading ()));
	connect (ui.pushButton_Loop_Sep, SIGNAL (clicked ()), SLOT (on_loop_sep ()));
	connect (ui.pushButton_Get_st_Node, SIGNAL (clicked ()), SLOT (on_get_st_node ()));
	connect (ui.pushButton_Finally_Sep, SIGNAL (clicked ()), SLOT (on_finally_sep ()));
	connect (ui.pushButton_Sep_Hex_Seeds, SIGNAL (clicked ()), SLOT (on_sep_hex_seeds ()));
	connect (ui.pushButton_Int_Sep_Hex_Sets, SIGNAL (clicked ()), SLOT (on_int_sep_hex_seeds ()));
	connect (ui.pushButton_Get_Init_Quad_Set_For_PD, SIGNAL (clicked ()), SLOT (on_get_init_quad_set_for_pd ()));
	connect (ui.pushButton_Algo_Test_For_PD, SIGNAL (clicked ()), SLOT (on_algo_test_for_pd ()));
	connect (ui.pushButton_Show_Sheet_Edge_Face_For_PD, SIGNAL (clicked ()), SLOT (on_show_sheet_edges_and_faces_for_pd ()));
	connect (ui.pushButton_Append_Edges_on_Faces_For_PD, SIGNAL (clicked ()), SLOT (on_append_edges_on_faces_for_pd ()));
	connect (ui.pushButton_Turn_To_Inflation_For_PD, SIGNAL (clicked ()), SLOT (on_turn_to_inflation_for_pd ()));
}

void MeshEditWidget::on_open_file (QString file_path)
{
	QFileInfo fi(file_path);
	if (fi.suffix () == "xml"){
		QString file_type, data_name;
		std::vector<std::pair<QString, QString> > mesh_model_paths;
		if (!JC::parse_xml_file (file_path, file_type, data_name, mesh_model_paths)){
			QMessageBox::critical (this, tr("文件打开错误！"), 
				tr("解析错误！该文件不是一个有效的MeshEditor文件！"), QMessageBox::Ok);
			return;
		}
		if (file_type != "meshedit"){
			QMessageBox::critical (this, tr("文件打开错误！"), 
				tr("该文件不是一个有效的MeshEdit文件！"), QMessageBox::Ok);
		}
		xml_file_path = file_path; 
		mesh_file_path = mesh_model_paths.front ().first; 
		model_file_path = mesh_model_paths.front ().second;
		load_mesh_and_model (mesh_file_path, model_file_path);
	}else if (fi.suffix () == "ovm"){
		mesh = JC::load_volume_mesh (file_path);

		hoopsview->render_VolumeMesh (mesh);
		JC::retrieve_sheets (mesh, sheet_set);

		hoopsview->show_boundary (true);
		hoopsview->show_boundary_vertices (false);
		hoopsview->show_boundary_edges (true);
		hoopsview->show_boundary_faces (true);
		hoopsview->show_boundary_cells (false);
		hoopsview->show_boundary_vertices_indices (false);
		hoopsview->show_boundary_edges_indices (false);
		hoopsview->show_boundary_faces_indices (false);
		hoopsview->show_boundary_cells_indices (false);

		hoopsview->show_inner (false);
		hoopsview->show_inner_vertices (false);
		hoopsview->show_inner_edges (true);
		hoopsview->show_inner_faces (false);
		hoopsview->show_inner_cells (false);
		hoopsview->show_inner_vertices_indices (false);
		hoopsview->show_inner_edges_indices (false);
		hoopsview->show_inner_faces_indices (false);
		hoopsview->show_inner_cells_indices (false);
		hoopsview->set_edges_selectable (true);
	}else{
		body = JC::load_acis_model (file_path);
		JC::init_volume_mesh (mesh, body, SPAresabs * 1000);

		HC_Open_Segment_By_Key (hoopsview->GetHoopsView ()->GetModelKey ());{
			HC_Open_Segment ("body");{
				HC_Set_Visibility ("markers=off,vertices=off,lines=off,faces=off");
				HA_Render_Entity (body, ".");

			}HC_Close_Segment ();
		}HC_Close_Segment ();
	}

	file_controller->file_accepted (file_path);
	ui.toolBox->setEnabled (true);
}

void MeshEditWidget::on_save_file ()
{
	if (!JC::save_volume_mesh (mesh, mesh_file_path)){
		QMessageBox::warning (this, tr("警告"), tr("网格模型保存失败！"), QMessageBox::Ok);
		return;
	}else{
		QMessageBox::information (this, tr("提示"), tr("网格模型保存成功！"), QMessageBox::Ok);
	}
}

void MeshEditWidget::on_save_file_as (QString file_path)
{
	
	//if (!JC::save_VolumeMesh (mesh, mesh_file_path)){
	//	QMessageBox::warning (this, tr("警告"), tr("网格模型保存失败！"), QMessageBox::Ok);
	//	return;
	//}else{
	//	QMessageBox::information (this, tr("提示"), tr("网格模型保存成功！"), QMessageBox::Ok);
	//}
}

void MeshEditWidget::on_file_close ()
{
	hoopsview->derender_VolumeMesh ();
	delete mesh;
	mesh = NULL;
	ui.toolBox->setEnabled (false);
}

void MeshEditWidget::on_highlight_mesh_elements ()
{
	auto fSepStr = [] (QString str, std::vector<int> &indices){
		if (str == "")
			return;
		indices.clear ();
		auto strlist = str.split (",");
		foreach (auto str, strlist)
			indices.push_back (str.toInt ());
	};
	std::unordered_set<OvmVeH> vertices;
	std::unordered_set<OvmEgH> edges;
	std::unordered_set<OvmFaH> faces;
	std::unordered_set<OvmCeH> hexas;

	auto str = ui.lineEdit_Vertices->text ();
	std::vector<int> indices;
	if (str != ""){
		fSepStr (str, indices);
		
		foreach (int idx, indices){
			vertices.insert (OvmVeH (idx));
		}
	}


	str = ui.lineEdit_Edges->text ();
	if (str != ""){
		fSepStr (str, indices);
		
		foreach (int idx, indices){
			edges.insert (OvmEgH (idx));
		}
	}

	str = ui.lineEdit_Faces->text ();
	if (str != ""){
		fSepStr (str, indices);
		
		foreach (int idx, indices){
			faces.insert (OvmFaH (idx));
		}
	}

	str = ui.lineEdit_Hexas->text ();
	if (str != ""){
		fSepStr (str, indices);
		
		foreach (int idx, indices){
			hexas.insert (OvmCeH (idx));
		}
	}

	//render
	auto group = new VolumeMeshElementGroup (mesh, "highlight", "highlight elements");
	group->vhs = vertices;
	group->ehs = edges;
	group->fhs = faces;
	group->chs = hexas;
	MeshRenderOptions render_options;
	render_options.show_cell_indices = true;
	render_options.show_vertex_indices = true;
	render_options.show_face_indices = true;
	render_options.show_edge_indices = true;
	hoopsview->render_mesh_group (group, render_options);
	ui.lineEdit_Vertices->clear ();
	ui.lineEdit_Edges->clear ();
	ui.lineEdit_Faces->clear ();
	ui.lineEdit_Hexas->clear ();

	mesh_render_controller->render_wireframe ();


}

void MeshEditWidget::on_clear_highlight_mesh_elements ()
{
	ui.lineEdit_Vertices->clear ();
	ui.lineEdit_Edges->clear ();
	ui.lineEdit_Faces->clear ();
	ui.lineEdit_Hexas->clear ();
	
	hoopsview->derender_mesh_groups ("highlight", NULL, true);
}

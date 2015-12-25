#include "StdAfx.h"
#include <Windows.h>
#include "ManualMeshMatchWidget.h"
#include "bool_api_options.hxx"
#include "MeshMatchingHandler.h"
#include "chordmatchwidget.h"
#include <QDir>
#include <QProgressDialog>
#include "rgbcolor.hxx"
#include <at_int.hxx>

ManualMeshMatchWidget::ManualMeshMatchWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	hoopsview1 = ui.hoopsview1; 
	hoopsview2 = ui.hoopsview2;
	mesh_edit_controller1 = ui.mesh_edit_controller1;
	mesh_edit_controller2 = ui.mesh_edit_controller2;
	//assembly_viewer = new MeshMatchAssemblyViewer (this);
	//assembly_viewer->setWindowFlags (Qt::Dialog|Qt::WindowMinimizeButtonHint);
	//assembly_viewer->hide ();
	//assembly_viewer->mm_widget = this;
	//hoopsview3 = assembly_viewer->get_hoopsview ();
	chord_match_widget = new ChordMatchWidget (this);
	chord_match_widget->setWindowFlags (Qt::Tool);
	chord_match_widget->hide ();
	prev_sel_chord1 = prev_sel_chord2 = NULL;
	mm_handler = new MeshMatchingHandler (this);

	connect (chord_match_widget, SIGNAL (chord_pair_highlight (uint, uint)),
		SLOT (on_highlight_chord_pair (uint, uint)));
	connect (chord_match_widget, SIGNAL (delete_chord_pair (uint, uint)),
		SLOT (on_delete_chord_pair (uint, uint)));
	connect (chord_match_widget, SIGNAL (close_widget ()),
		SLOT (on_close_chord_pair_widget ()));

	connect (ui.pushButton_Open, SIGNAL (clicked ()), SLOT (on_open ()));
	connect (ui.pushButton_Open_2, SIGNAL (clicked ()), SLOT (on_open2 ()));
	connect (ui.pushButton_Init_Matching, SIGNAL (clicked ()), SLOT (on_init_matching ()));
	connect (ui.pushButton_Match_Two_Chords, SIGNAL (clicked ()), 
		SLOT (on_match_two_chords ()));
	connect (ui.pushButton_Redraw_Matched_Chords, SIGNAL (clicked ()),
		SLOT (on_update_matched_chords ()));
	connect (ui.pushButton_Get_Polyline, SIGNAL (clicked ()), SLOT (on_get_polyline ()));
	connect (ui.pushButton_Check_Match, SIGNAL (clicked ()),
		SLOT (on_check_match ()));
	connect (ui.pushButton_Show_Interface, SIGNAL (clicked ()),
		SLOT (on_show_interface ()));
	connect (ui.pushButton_Show_Chord_Pairs, SIGNAL (clicked ()),
		SLOT (on_show_chord_pairs ()));
	connect (ui.pushButton_Save, SIGNAL (clicked ()),
		SLOT (on_save ()));
	connect (ui.pushButton_Auto_Match, SIGNAL (clicked ()),
		SLOT (on_auto_match ()));
	connect (ui.pushButton_Merge, SIGNAL (clicked ()),
		SLOT (on_merge ()));
	connect (ui.cb_Show_Left_Side, SIGNAL (clicked ()),
		SLOT (on_show_left ()));
	connect (ui.cb_Show_Right_Side, SIGNAL (clicked ()),
		SLOT (on_show_right ()));
}

ManualMeshMatchWidget::~ManualMeshMatchWidget()
{
	delete mm_handler;
}


VolumeMesh *combine_meshes (VolumeMesh *mesh1, VolumeMesh *mesh2, std::unordered_set<OvmCeH> &chs)
{
	std::hash_map<OvmVeH, OvmVeH> old_new_vh_mapping;
	std::unordered_set<OvmVeH> all_old_vhs;
	foreach (auto &ch, chs){
		JC::get_adj_vertices_around_cell (mesh2, ch, all_old_vhs);
	}
	foreach (auto &vh, all_old_vhs){
		auto pt = mesh2->vertex (vh);
		pt[2] = pt[2] - 30;
		auto new_vh = mesh1->add_vertex (pt);
		old_new_vh_mapping.insert (std::make_pair (vh, new_vh));
	}

	foreach (auto &ch, chs){
		std::vector<OvmVeH> vhs;
		for (auto hv_it = mesh2->hv_iter (ch); hv_it; ++hv_it){
			auto old_vh = *hv_it;
			auto new_vh = old_new_vh_mapping[old_vh];
			vhs.push_back (new_vh);
		}
		mesh1->add_cell (vhs);
	}

	return mesh1;
}

void ManualMeshMatchWidget::on_open2 ()
{
	//dir_str = QFileDialog::getExistingDirectory (this, tr("选择模型所在文件夹"));
	//auto fGetLeftChs = [] (VolumeMesh *whole_mesh, BODY *match_body)->std::unordered_set<OvmCeH>{
	//	std::unordered_set<OvmCeH> left_chs;
	//	for (auto c_it = whole_mesh->cells_begin (); c_it != whole_mesh->cells_end (); ++c_it){
	//		auto centre_pt = whole_mesh->barycenter (*c_it);
	//		SPAposition acis_pos = POS2SPA(centre_pt);
	//		point_containment pc;
	//		api_point_in_body (acis_pos, match_body, pc);
	//		if (pc == point_inside) continue;
	//		left_chs.insert (*c_it);
	//	}

	//	return left_chs;
	//};

	//if (dir_str != ""){
	//	QString part1mesh = dir_str + "\\part1.ovm",
	//		part2mesh = dir_str + "\\part2.ovm",
	//		part1body = dir_str + "\\part1.sat",
	//		part2body = dir_str + "\\part2.sat",
	//		cylinderbody = dir_str + "\\cylinder.SAT",
	//		cylinder_mesh = dir_str + "\\cylinder.ovm";

	//	mesh1 = JC::load_volume_mesh (part1mesh.toAscii ().data ());
	//	mesh2 = JC::load_volume_mesh (part2mesh.toAscii ().data ());
	//	auto cy_mesh = JC::load_volume_mesh (cylinder_mesh.toAscii ().data ());
	//	body1 = JC::load_acis_model (part1body);
	//	body2 = JC::load_acis_model (part2body);
	//	BODY *cylinder = JC::load_acis_model (cylinderbody);
	//	auto block_chs = fGetLeftChs (mesh1, cylinder);
	//	//for (auto c_it = mesh1->cells_begin (); c_it != mesh1->cells_end (); ++c_it){
	//	//	if (!JC::contains (cylinder_chs, *c_it))
	//	//		block_chs.insert (*c_it);
	//	//}

	//	//QMessageBox::information (this, "INFO", QString("cy %1 bl %2").arg (cylinder_chs.size ()).arg (block_chs.size ()));
	//	//common_interface = get_interface ();

	//	HC_Open_Segment ("/match models");{
	//		mesh1key = HC_Open_Segment ("");{
	//			HC_Set_Rendering_Options ("no lighting interpolation");
	//			HC_Set_Color ("faces=green");
	//			//HC_Translate_Object (0, 10, 0);
	//			//JC::render_volume_mesh_boundary (mesh1);

	//			JC::render_volume_mesh (cy_mesh);
	//		}HC_Close_Segment ();
	//		mesh2key = HC_Open_Segment ("");{
	//			HC_Set_Rendering_Options ("no lighting interpolation");
	//			HC_Set_Color ("faces=grey");
	//			//HC_Translate_Object (0, -10, 0);
	//			JC::render_volume_mesh_boundary (mesh2);
	//		}HC_Close_Segment ();

	//	}HC_Close_Segment ();

	//	HC_Open_Segment_By_Key (hoopsview1->GetHoopsView ()->GetModelKey ());{
	//		HC_Open_Segment ("meshes");{
	//			HC_Set_Visibility ("markers=off,lines=on,faces=on");
	//			HC_Include_Segment_By_Key (mesh1key);
	//			JC::render_hexa_set (mesh1, block_chs);

	//		}HC_Close_Segment ();

	//	}HC_Close_Segment ();
	//	hoopsview1->GetHoopsView ()->SetGeometryChanged ();
	//	hoopsview1->GetHoopsView ()->ZoomToExtents ();
	//	hoopsview1->GetHoopsView ()->Update ();

	//	HC_Open_Segment_By_Key (hoopsview2->GetHoopsView ()->GetModelKey ());{
	//		HC_Open_Segment ("meshes");{
	//			HC_Set_Visibility ("markers=off,lines=on,faces=on");
	//			HC_Include_Segment_By_Key (mesh2key);
	//		}HC_Close_Segment ();
	//	}HC_Close_Segment ();

	//	//	//HC_Include_Segment_By_Key (body2key);
	//	//}HC_Close_Segment ();
	//	hoopsview2->GetHoopsView ()->SetGeometryChanged ();
	//	hoopsview2->GetHoopsView ()->ZoomToExtents ();
	//	hoopsview2->GetHoopsView ()->Update ();

	//	mesh_edit_controller1->mesh = mesh1;
	//	mesh_edit_controller1->hoopsview = hoopsview1;
	//	mesh_edit_controller1->body = body1;

	//	mesh_edit_controller2->mesh = mesh2;
	//	mesh_edit_controller2->hoopsview = hoopsview2;
	//	mesh_edit_controller2->body = body2;
	//}
}

void ManualMeshMatchWidget::on_open ()
{
	auto dir_str = QFileDialog::getExistingDirectory (this, tr("选择模型所在文件夹"));

	if (dir_str != ""){
		if (!mm_handler->load_mesh_matching_data (dir_str)){
			QMessageBox::warning (this, "错误", "加载文件错误！");
			return;
		}


		auto fSetupHoopsView = [&](MeshEditController *mc, HoopsView *hv, VolumeMesh *mesh, BODY *body, std::set<FACE*> intf){
			HC_KEY mesh_key = INVALID_KEY;
			HC_KEY new_key = POINTER_TO_KEY(mesh);
			HC_Open_Segment ("/match models");{
				mesh_key = HC_Open_Segment ("");{
					HC_Set_Rendering_Options ("no lighting interpolation");
					HC_Set_Color ("faces=grey");
					JC::render_volume_mesh_boundary (mesh);
				}HC_Close_Segment ();

				HC_Renumber_Key (mesh_key, new_key, "g");

				HC_Open_Segment ("");{
					HA_Set_Rendering_Options ("preserve color=off,merge faces=off");
					HA_Render_Entity (body, ".");
				}HC_Close_Segment ();
			}HC_Close_Segment ();

			HC_Open_Segment_By_Key (hv->GetHoopsView ()->GetModelKey ());{
				HC_Open_Segment ("meshes");{
					HC_Set_Visibility ("markers=off,lines=on,faces=on");
					HC_Set_Rendering_Options ("no lighting interpolation");
					HC_Set_Color ("faces=grey");
					HC_Include_Segment_By_Key (new_key);
				}HC_Close_Segment ();

				HC_Open_Segment ("acismodel");{
					HC_Open_Segment ("acisvertices");{
						HC_Set_Visibility ("markers=off");
						HC_Set_Marker_Size (0.5);
						HC_Set_Color ("markers=black");
						auto num = HA_Compute_Geometry_Key_Count (body, "vertices");
						auto keys = new HC_KEY[num];
						HA_Compute_Geometry_Keys (body, num, keys, "vertices");

						for (int i = 0; i != num; ++i){
							HC_Reference_Geometry_By_Key (keys[i]);
						}
					}HC_Close_Segment ();

					HC_Open_Segment ("acisedges");{
						HC_Set_Visibility ("lines=off");
						HC_Set_Line_Weight (6);
						HC_Set_Color ("lines=yellow");

						auto num = HA_Compute_Geometry_Key_Count (body, "edges");
						auto keys = new HC_KEY[num];
						HA_Compute_Geometry_Keys (body, num, keys, "edges");

						for (int i = 0; i != num; ++i){
							HC_Reference_Geometry_By_Key (keys[i]);
						}
					}HC_Close_Segment ();

					HC_Open_Segment ("acisfaces");{
						HC_Set_Visibility ("faces=off");
						HC_Set_Color ("faces=pink");

						auto num = HA_Compute_Geometry_Key_Count (body, "faces");
						auto keys = new HC_KEY[num];
						HA_Compute_Geometry_Keys (body, num, keys, "faces");

						for (int i = 0; i != num; ++i){
							HC_Reference_Geometry_By_Key (keys[i]);
						}
					}HC_Close_Segment ();
				}HC_Close_Segment ();

				HC_Open_Segment ("interfaces");{
					HC_Set_Visibility ("lines=on");
					HC_Set_Line_Weight (10);
					HC_Set_Color ("lines=blue");
					std::set<ENTITY*> all_edges;
					foreach (auto f, intf){
						ENTITY_LIST edges_list;
						api_get_edges (f, edges_list);
						while (auto e = edges_list.next ()) all_edges.insert (e);
					}

					HC_KEY keys[1];
					foreach (auto e, all_edges){
						HA_Compute_Geometry_Keys (e, 1, keys);
						HC_Reference_Geometry_By_Key (keys[0]);
					}
				}HC_Close_Segment ();
			}HC_Close_Segment ();

			hv->GetHoopsView ()->SetGeometryChanged ();
			hv->GetHoopsView ()->ZoomToExtents ();
			hv->GetHoopsView ()->Update ();

			mc->mesh = mesh;
			mc->hoopsview = hv;
			mc->body = body;
			mc->inter_faces = intf;

			std::unordered_set<OvmVeH> left_vhs;
			JC::init_volume_mesh (mesh, body, SPAresabs * 1000, &left_vhs);
		};

		fSetupHoopsView (mesh_edit_controller1, hoopsview1, mm_handler->mesh1 (), mm_handler->body1 (), mm_handler->interfaces1 ());
		fSetupHoopsView (mesh_edit_controller2, hoopsview2, mm_handler->mesh2 (), mm_handler->body2 (), mm_handler->interfaces2 ());

		draw_matched_chords (mm_handler->matched_chord_pairs);
		//debug
		mm_handler->set_hoopsview1 (hoopsview1);
		mm_handler->set_hoopsview2 (hoopsview2);

		auto mm_data1 = mm_handler->mm_data1,
			mm_data2 = mm_handler->mm_data2;
		foreach (auto ehs, mm_data1->ordered_ehs_on_edges){
			auto ordered_ehs = ehs.second;
			if (ordered_ehs.size () ==1 && ordered_ehs.front () == OvmEgH(-1)){
				hoopsview1->render_one_acis_edge (ehs.first);
			}
		}

		foreach (auto ehs, mm_data2->ordered_ehs_on_edges){
			auto ordered_ehs = ehs.second;
			if (ordered_ehs.size () ==1 && ordered_ehs.front () == OvmEgH(-1)){
				hoopsview2->render_one_acis_edge (ehs.first);
			}
		}

		//QString sta = QString ("Hex1:%1 Hex2:%2").arg (mesh1->n_cells ()).arg (mesh2->n_cells ());
		//QMessageBox::information (this, "INFO", sta);

		//auto fGetVector = [](VolumeMesh *m, OvmEgH eh, OvmVeH vh)->OvmVec3d{
		//	OvmHaEgH heh = m->halfedge_handle (eh, 0);
		//	if (m->halfedge (heh).from_vertex () != vh)
		//		heh = m->halfedge_handle (eh, 1);
		//	OvmVeH vh1 = m->halfedge (heh).from_vertex (),
		//		vh2 = m->halfedge (heh).to_vertex ();
		//	auto vec = m->vertex (vh2) - m->vertex (vh1);
		//	return vec.normalize ();
		//};

		//auto fJacobian = [&](VolumeMesh *m, OvmCeH ch)->double{


		//	auto adj_vhs = JC::get_adj_vertices_around_cell (m, ch);
		//	std::unordered_set<OvmEgH> all_adj_ehs;
		//	JC::get_adj_edges_around_cell (m, ch, all_adj_ehs);
		//	double min_jac = std::numeric_limits<double>::max ();
		//	foreach (auto &cur_vh, adj_vhs){
		//		std::unordered_set<OvmEgH> adj_ehs, adj_ehs_tmp;
		//		JC::get_adj_edges_around_vertex (m, cur_vh, adj_ehs_tmp);
		//		foreach (auto &eh, adj_ehs_tmp){
		//			if (JC::contains (all_adj_ehs, eh)) adj_ehs.insert (eh);
		//		}
		//		assert (adj_ehs.size () == 3);
		//		foreach (auto &adj_eh, adj_ehs){
		//			OvmVec3d vec1 = fGetVector (m, adj_eh, cur_vh);
		//			std::vector<OvmEgH> other_ehs;
		//			foreach (auto &tmp, adj_ehs){
		//				if (tmp != adj_eh) other_ehs.push_back (tmp);
		//			}
		//			auto vec2 = fGetVector (m, other_ehs.front (), cur_vh),
		//				vec3 = fGetVector (m, other_ehs.back (), cur_vh);
		//			double jac = std::abs (vec1 | (vec2 % vec3));
		//			if (jac < min_jac) min_jac = jac;
		//		}
		//	}
		//	return min_jac;
		//};

		//double min_jac1 = std::numeric_limits<double>::max ();
		//for (auto c_it = mesh1->cells_begin (); c_it != mesh1->cells_end (); ++c_it){
		//	auto jac = fJacobian (mesh1, *c_it);
		//	if (jac < min_jac1) min_jac1 = jac;
		//}
		//double min_jac2 = std::numeric_limits<double>::max ();
		//for (auto c_it = mesh2->cells_begin (); c_it != mesh2->cells_end (); ++c_it){
		//	auto jac = fJacobian (mesh2, *c_it);
		//	if (jac < min_jac2) min_jac2 = jac;
		//}

		//sta = QString ("Hex1 jac:%1 Hex2 jac:%2").arg (min_jac1).arg (min_jac2);
		//QMessageBox::information (this, "Jac", sta);
		//return;



	}
}

void ManualMeshMatchWidget::on_show_left ()
{
	if (ui.cb_Show_Left_Side->isChecked ()){
		hoopsview1->show ();
		mesh_edit_controller1->show ();
	}else{
		hoopsview1->hide ();
		mesh_edit_controller1->hide ();
	}

}

void ManualMeshMatchWidget::on_show_right ()
{
	if (ui.cb_Show_Right_Side->isChecked ()){
		hoopsview2->show ();
		mesh_edit_controller2->show ();
	}else{
		hoopsview2->hide ();
		mesh_edit_controller2->hide ();
	}
}

void ManualMeshMatchWidget::on_init_matching ()
{
	mm_handler->init_match ();
	draw_matched_chords (mm_handler->matched_chord_pairs);
}

void ManualMeshMatchWidget::on_update_matched_chords ()
{
	draw_matched_chords (mm_handler->matched_chord_pairs);
}

void ManualMeshMatchWidget::on_match_two_chords ()
{
	DualChord *chord1 = mesh_edit_controller1->get_selected_chord (),
		*chord2 = mesh_edit_controller2->get_selected_chord ();
	if (chord1 == NULL || chord2 == NULL){
		QMessageBox::warning (this, "错误", "必须选中两条chord！");
		return;
	}

	if (!mm_handler->add_two_matched_chords (chord1, chord2)){
		QMessageBox::warning (this, tr("警告"), tr("这两条chord不能够匹配！"));
		return;
	}else{
		hoopsview1->derender_chord (chord1);
		hoopsview2->derender_chord (chord2);
		draw_matched_chords (mm_handler->matched_chord_pairs);
	}
}

void ManualMeshMatchWidget::on_check_match ()
{
	if (mm_handler->check_match ()){
		QMessageBox::information (this, "提示", "可以匹配！");
	}else{
		QMessageBox::warning (this, "提示", "不能匹配！");
	}

}

void ManualMeshMatchWidget::on_show_interface ()
{
	//assembly_viewer->matched_chord_pairs = matched_chord_pairs;
	//assembly_viewer->show ();
}

void ManualMeshMatchWidget::on_show_chord_pairs ()
{
	chord_match_widget->set_matched_pairs (mm_handler->matched_chord_pairs);
	chord_match_widget->show ();
}

void ManualMeshMatchWidget::on_highlight_chord_pair (uint chord_ptr1, uint chord_ptr2)
{
	DualChord *chord1 = (DualChord*)chord_ptr1,
		*chord2 = (DualChord*)chord_ptr2;
	if (prev_sel_chord1 && prev_sel_chord2){
		hoopsview1->derender_chord (prev_sel_chord1);
		hoopsview2->derender_chord (prev_sel_chord2);
		hoopsview1->render_chord (prev_sel_chord1, "green", 3);
		hoopsview2->render_chord (prev_sel_chord2, "green", 3);
	}
	hoopsview1->derender_chord (chord1);
	hoopsview2->derender_chord (chord2);
	prev_sel_chord1 = chord1; prev_sel_chord2 = chord2;

	hoopsview1->render_chord (chord1, "yellow", 6);
	hoopsview2->render_chord (chord2, "yellow", 6);

	hoopsview1->derender_mesh_groups ("tmp", "ordered ehs");
	hoopsview2->derender_mesh_groups ("tmp", "ordered ehs");
	auto group = new VolumeMeshElementGroup (chord1->mesh, "tmp", "ordered ehs");
	group->ehs.insert (chord1->ordered_ehs.front ());

	MeshRenderOptions render_options;
	render_options.edge_color = "red";
	render_options.edge_width = 12;
	hoopsview1->render_mesh_group (group, render_options);
	
	group = new VolumeMeshElementGroup (chord1->mesh, "tmp", "ordered ehs");
	group->ehs.insert (chord1->ordered_ehs[1]);

	render_options.edge_color = "blue";
	hoopsview1->render_mesh_group (group, render_options);

	group = new VolumeMeshElementGroup (chord2->mesh, "tmp", "ordered ehs");
	group->ehs.insert (chord2->ordered_ehs.front ());

	render_options.edge_color = "red";
	hoopsview2->render_mesh_group (group, render_options);

	group = new VolumeMeshElementGroup (chord2->mesh, "tmp", "ordered ehs");
	group->ehs.insert (chord2->ordered_ehs[1]);

	render_options.edge_color = "blue";
	hoopsview2->render_mesh_group (group, render_options);
}

void ManualMeshMatchWidget::on_delete_chord_pair (uint chord_ptr1, uint chord_ptr2)
{
	DualChord *chord1 = (DualChord*)chord_ptr1,
		*chord2 = (DualChord*)chord_ptr2;
	hoopsview1->derender_chord (chord1);
	hoopsview2->derender_chord (chord2);
	mm_handler->matched_chord_pairs.erase (std::make_pair (chord1, chord2));
	prev_sel_chord1 = NULL;
	prev_sel_chord2 = NULL;
	draw_matched_chords (mm_handler->matched_chord_pairs);
	hoopsview1->derender_mesh_groups ("tmp", "ordered ehs");
	hoopsview2->derender_mesh_groups ("tmp", "ordered ehs");
}

void ManualMeshMatchWidget::on_close_chord_pair_widget ()
{
	if (prev_sel_chord1 && prev_sel_chord2){
		hoopsview1->derender_chord (prev_sel_chord1);
		hoopsview2->derender_chord (prev_sel_chord2);
		hoopsview1->render_chord (prev_sel_chord1, "green", 3);
		hoopsview2->render_chord (prev_sel_chord2, "green", 3);
	}
	prev_sel_chord1 = prev_sel_chord2 = NULL;
}

void ManualMeshMatchWidget::on_save ()
{
	auto dir_path = QFileDialog::getExistingDirectory (this, tr("选择目录"), tr("请选择要保存的目录"));
	while (QDir(dir_path).entryInfoList (QDir::NoDotAndDotDot|QDir::AllEntries).count () != 0){
		QMessageBox::warning (this, tr("错误"), tr("选择的目录不为空！请选择一个空文件夹！"));
		dir_path = QFileDialog::getExistingDirectory (this, tr("选择目录"), tr("请选择要保存的目录"));
	}


	if (!mm_handler->save_mesh_matching_data (dir_path))
		QMessageBox::warning (this, tr("错误"), tr("保存失败！"));
	else
		QMessageBox::information (this, tr("提醒"), tr("保存成功！"));
}

DualChord *ManualMeshMatchWidget::get_matched_chord (DualChord *chord)
{
	return mm_handler->get_matched_chord (chord);
}

void ManualMeshMatchWidget::on_get_polyline ()
{
	DualChord *chord1 = mesh_edit_controller1->get_selected_chord (),
		*chord2 = mesh_edit_controller2->get_selected_chord ();
	if (chord1 && chord2){
		QMessageBox::warning (this, "错误", "不能两边同时选中！");
		return;
	}
	auto chord_to_match = chord1? chord1 : chord2;
	auto hoopsview_to_draw = chord1? hoopsview2 : hoopsview1;
	auto mesh = chord1? mm_handler->mesh2 () : mm_handler->mesh1 ();

	auto cpd = mm_handler->get_chord_pos_desc (chord_to_match);
	auto trans_cpd = mm_handler->translate_chord_pos_desc (cpd);

	hoopsview_to_draw->derender_mesh_groups ("sheet inflation", "candidate start geom vertices");
	hoopsview_to_draw->derender_mesh_groups ("sheet inflation", "candidate end geom vertices");
	hoopsview_to_draw->derender_mesh_groups ("sheet inflation", "candidate interval ehs");
	hoopsview_to_draw->derender_mesh_groups ("sheet inflation", "best polyline");

	MeshRenderOptions render_options;

	auto interval_ehs = mm_handler->get_candidate_interval_ehs (trans_cpd);

	foreach (auto ehs, interval_ehs){
		auto group = new VolumeMeshElementGroup (mesh, "sheet inflation", "candidate interval ehs");
		foreach (auto eh, ehs)
			group->ehs.insert (eh);

		render_options.edge_color = "pink";
		render_options.edge_width = 6;
		hoopsview_to_draw->render_mesh_group (group, render_options);
		hoopsview_to_draw->set_mesh_group_selectability (group, false, true, false);
	}

	if (!chord_to_match->is_closed){
		auto candidate_vertices = mm_handler->get_candidate_end_geom_vertices (trans_cpd);
		auto group = new VolumeMeshElementGroup (mesh, "sheet inflation", "candidate start geom vertices");
		foreach (auto vh, candidate_vertices.first)
			group->vhs.insert (vh);

		
		render_options.vertex_color = "red";
		render_options.vertex_size = 1.0;
		hoopsview_to_draw->render_mesh_group (group, render_options);
		hoopsview_to_draw->set_mesh_group_selectability (group, true, false, false);

		group = new VolumeMeshElementGroup (mesh, "sheet inflation", "candidate end geom vertices");
		foreach (auto vh, candidate_vertices.second)
			group->vhs.insert (vh);

		render_options.vertex_color = "blue";
		render_options.vertex_size = 1.0;
		hoopsview_to_draw->render_mesh_group (group, render_options);
		hoopsview_to_draw->set_mesh_group_selectability (group, true, false, false);

		auto best_polyline = mm_handler->get_polyline_for_chord_inflation (mesh, candidate_vertices, interval_ehs);
		group = new VolumeMeshElementGroup (mesh, "sheet inflation", "best polyline");
		foreach (auto eh, best_polyline.second)
			group->ehs.insert (eh);

		render_options.edge_color = "purple";
		render_options.edge_width = 12;
		hoopsview_to_draw->render_mesh_group (group, render_options);

	}

	hoopsview_to_draw->show_mesh_faces (true);
}

void ManualMeshMatchWidget::on_auto_match ()
{
	//hoopsview1->derender_all_chords ();
	//hoopsview2->derender_all_chords ();

	//if (dir_str.isEmpty () || dir_str == "") return;
	//auto matching_dir_str = dir_str + "/matching/";
	//QDir matching_dir (matching_dir_str);
	//if (!matching_dir.exists ()) return;

	//matching_dir.setFilter(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
	//QFileInfoList list = matching_dir.entryInfoList();

	//int subdir_count = list.count();
	//if(subdir_count <= 0)
	//	return;

	//std::vector<QString> subdir_strs;
	//for(int i = 0; i != subdir_count; ++i){
	//	QFileInfo file_info = list.at(i);
	//	QString subdir_path_str = file_info.filePath();
	//	subdir_strs.push_back (subdir_path_str);
	//}

	//foreach (auto str, subdir_strs){
	//	QString part1mesh = str + "\\part1.ovm",
	//		part2mesh = str + "\\part2.ovm";

	//	delete mesh1; delete mesh2;
	//	mesh1 = JC::load_volume_mesh (part1mesh.toAscii ().data ());
	//	mesh2 = JC::load_volume_mesh (part2mesh.toAscii ().data ());
	//	mesh_edit_controller1->mesh = mesh1;
	//	mesh_edit_controller2->mesh = mesh2;

	//	HC_Open_Segment ("/match models");{
	//		HC_Open_Segment_By_Key (mesh1key);{
	//			HC_Flush_Geometry ("...");
	//			JC::render_volume_mesh_boundary (mesh1);
	//		}HC_Close_Segment ();
	//		HC_Open_Segment_By_Key (mesh2key);{
	//			HC_Flush_Geometry ("...");
	//			JC::render_volume_mesh_boundary (mesh2);
	//		}HC_Close_Segment ();
	//	}HC_Close_Segment ();

	//	hoopsview1->GetHoopsView ()->SetGeometryChanged ();
	//	hoopsview1->GetHoopsView ()->Update ();

	//	hoopsview2->GetHoopsView ()->SetGeometryChanged ();
	//	hoopsview2->GetHoopsView ()->Update ();

	//	//get_interface_quads ();

	//	//if (mm_handler) delete mm_handler;
	//	//MMData data1, data2;
	//	//data1.inter_patch = interface_fhs1;
	//	//data2.inter_patch = interface_fhs2;
	//	//data1.mesh = mesh1; data2.mesh = mesh2;
	//	//mm_handler = new MeshMatchingHandler (&data1, &data2, common_interface);
	//	//mm_handler->init_match ();

	//	//matched_chord_pairs = mm_handler->matched_chord_pairs;

	//	//unmatched_chords1 = mm_handler->chord_set1;
	//	//unmatched_chords2 = mm_handler->chord_set2;

	//	//draw_matched_chords (matched_chord_pairs);

	//	QTime time;
	//	time= QTime::currentTime();
	//	qsrand(time.msec()+time.second()*1000);



	//	int xxx=qrand()%3000;

	//	Sleep (1000 + xxx);
	//}
}

void ManualMeshMatchWidget::on_merge ()
{
	//HC_Open_Segment ("/match models");{
	//	HC_Open_Segment_By_Key (mesh1key);{
	//		//HC_Flush_Geometry ("...");
	//		JC::render_volume_mesh_boundary (mesh2);
	//	}HC_Close_Segment ();
	//}HC_Close_Segment ();

	//hoopsview1->GetHoopsView ()->SetGeometryChanged ();
	//hoopsview1->GetHoopsView ()->Update ();

	//hoopsview2->hide ();
}

void ManualMeshMatchWidget::draw_matched_chords (ChordPairs &matched_chord_pairs)
{
	hoopsview1->derender_all_chords ();
	hoopsview2->derender_all_chords ();
	char *colors[] = {
		"red", "blue", "pink", "yellow", "black", "magenta", "dark green", "orange red",
		"violet blue","cerulean", "tangerine", "periwinkle"
	};
	int i = 0;
	foreach (auto &p, matched_chord_pairs){
		//hoopsview1->render_chord (p.first, "red", 4);
		//hoopsview2->render_chord (p.second, "red", 4);
		//hoopsview1->render_chord (p.first, colors[i % 12], 6);
		//hoopsview2->render_chord (p.second, colors[i % 12], 6);
		hoopsview1->render_chord (p.first, "green", 3);
		hoopsview2->render_chord (p.second, "green", 3);
		i++;
	}
}
#include "StdAfx.h"
#include <Windows.h>
#include "ManualMeshMatchWidget.h"
#include "bool_api_options.hxx"
#include "MeshMatchingHandler.h"
#include "chordmatchwidget.h"
#include <QDir>
#include <QProgressDialog>
#include "rgbcolor.hxx"

ManualMeshMatchWidget::ManualMeshMatchWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	body1 = body2 = NULL;
	mm_handler = NULL;
	hoopsview1 = ui.hoopsview1; hoopsview2 = ui.hoopsview2;
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
	connect (ui.pushButton_See_Only_Left, SIGNAL (clicked ()),
		SLOT (on_see_only_left ()));
}

ManualMeshMatchWidget::~ManualMeshMatchWidget()
{

}

void my_render_volume_mesh_boundary (VolumeMesh *mesh, VolumeMesh *meshwhole, BODY *body)
{
	std::unordered_set<OvmCeH> out_body_chs;
	for (auto c_it = meshwhole->cells_begin (); c_it != meshwhole->cells_end (); ++c_it){
		auto pt = meshwhole->barycenter (*c_it);
		point_containment pc;
		api_point_in_body (POS2SPA(pt), body, pc);
		if (pc == point_inside) continue;
		out_body_chs.insert (*c_it);
	}
	std::unordered_set<OvmFaH> out_body_bound_fhs;
	foreach (auto &ch, out_body_chs){
		auto hfh_vec = meshwhole->cell (ch).halffaces ();
		foreach (auto &hfh, hfh_vec){
			auto fh = meshwhole->face_handle (hfh);
			if (JC::contains (out_body_bound_fhs, fh))
				out_body_bound_fhs.erase (fh);
			else
				out_body_bound_fhs.insert (fh);
		}
	}

	std::unordered_set<OvmEgH> out_body_bound_ehs;
	foreach (auto &fh, out_body_bound_fhs){
		auto heh_vec = meshwhole->face (fh).halfedges ();
		foreach (auto &heh, heh_vec){
			auto eh = meshwhole->edge_handle (heh);
			out_body_bound_ehs.insert (eh);
		}
	}

	auto fRenderWorker = [&] (VolumeMesh *m, std::unordered_set<OvmVeH> &vs, 
		std::unordered_set<OvmEgH> &es, std::unordered_set<OvmFaH> &fs){
			HC_Open_Segment ("meshvertices");{
				foreach (auto vh, vs){
					auto pt = m->vertex (vh);
					HC_Insert_Marker (pt[0], pt[1], pt[2]);
				}
			}HC_Close_Segment ();
			HC_Open_Segment ("meshedges");{
				//HC_Set_Visibility ("lines=off,edges=off,faces=off");
				HC_Set_Line_Weight (2);
				foreach (auto eh, es){
					auto eg = m->edge (eh);
					auto pt1 = m->vertex (eg.from_vertex ()), pt2 = m->vertex (eg.to_vertex ());
					HC_Insert_Line (pt1[0], pt1[1], pt1[2], pt2[0], pt2[1], pt2[2]);
				}
			}HC_Close_Segment ();
			HC_Open_Segment ("meshfaces");{
				foreach (auto fh, fs){
					auto hfh = m->halfface_handle (fh, 0);
					if (!m->is_boundary (hfh))
						hfh = m->opposite_halfface_handle (hfh);

					QVector<HPoint> pts;
					for (auto fv_it = m->hfv_iter (hfh); fv_it; ++fv_it)
					{
						auto pt = m->vertex (*fv_it);
						pts.push_back (HPoint (pt[0], pt[1], pt[2]));
					}
					HC_Insert_Polygon (pts.size (), pts.data ());
				}
			}HC_Close_Segment ();
	};
	//flush all the contents in this segment using wild cards
	HC_Flush_Segment ("...");
	fRenderWorker (meshwhole, std::unordered_set<OvmVeH>(), out_body_bound_ehs, out_body_bound_fhs);
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
	dir_str = QFileDialog::getExistingDirectory (this, tr("选择模型所在文件夹"));
	auto fGetLeftChs = [] (VolumeMesh *whole_mesh, BODY *match_body)->std::unordered_set<OvmCeH>{
		std::unordered_set<OvmCeH> left_chs;
		for (auto c_it = whole_mesh->cells_begin (); c_it != whole_mesh->cells_end (); ++c_it){
			auto centre_pt = whole_mesh->barycenter (*c_it);
			SPAposition acis_pos = POS2SPA(centre_pt);
			point_containment pc;
			api_point_in_body (acis_pos, match_body, pc);
			if (pc == point_inside) continue;
			left_chs.insert (*c_it);
		}

		return left_chs;
	};

	if (dir_str != ""){
		QString part1mesh = dir_str + "\\part1.ovm",
			part2mesh = dir_str + "\\part2.ovm",
			part1body = dir_str + "\\part1.sat",
			part2body = dir_str + "\\part2.sat",
			cylinderbody = dir_str + "\\cylinder.SAT",
			cylinder_mesh = dir_str + "\\cylinder.ovm";

		mesh1 = JC::load_volume_mesh (part1mesh.toAscii ().data ());
		mesh2 = JC::load_volume_mesh (part2mesh.toAscii ().data ());
		auto cy_mesh = JC::load_volume_mesh (cylinder_mesh.toAscii ().data ());
		body1 = JC::load_acis_model (part1body);
		body2 = JC::load_acis_model (part2body);
		BODY *cylinder = JC::load_acis_model (cylinderbody);
		auto block_chs = fGetLeftChs (mesh1, cylinder);
		//for (auto c_it = mesh1->cells_begin (); c_it != mesh1->cells_end (); ++c_it){
		//	if (!JC::contains (cylinder_chs, *c_it))
		//		block_chs.insert (*c_it);
		//}
		
		//QMessageBox::information (this, "INFO", QString("cy %1 bl %2").arg (cylinder_chs.size ()).arg (block_chs.size ()));
		//common_interface = get_interface ();

		HC_Open_Segment ("/match models");{
			mesh1key = HC_Open_Segment ("");{
				HC_Set_Rendering_Options ("no lighting interpolation");
				HC_Set_Color ("faces=green");
				//HC_Translate_Object (0, 10, 0);
				//JC::render_volume_mesh_boundary (mesh1);
				
				JC::render_volume_mesh (cy_mesh);
			}HC_Close_Segment ();
			mesh2key = HC_Open_Segment ("");{
				HC_Set_Rendering_Options ("no lighting interpolation");
				HC_Set_Color ("faces=grey");
				//HC_Translate_Object (0, -10, 0);
				JC::render_volume_mesh_boundary (mesh2);
			}HC_Close_Segment ();

		}HC_Close_Segment ();

		HC_Open_Segment_By_Key (hoopsview1->GetHoopsView ()->GetModelKey ());{
			HC_Open_Segment ("meshes");{
				HC_Set_Visibility ("markers=off,lines=on,faces=on");
				HC_Include_Segment_By_Key (mesh1key);
				JC::render_hexa_set (mesh1, block_chs);

			}HC_Close_Segment ();

		}HC_Close_Segment ();
		hoopsview1->GetHoopsView ()->SetGeometryChanged ();
		hoopsview1->GetHoopsView ()->ZoomToExtents ();
		hoopsview1->GetHoopsView ()->Update ();

		HC_Open_Segment_By_Key (hoopsview2->GetHoopsView ()->GetModelKey ());{
			HC_Open_Segment ("meshes");{
				HC_Set_Visibility ("markers=off,lines=on,faces=on");
				HC_Include_Segment_By_Key (mesh2key);
			}HC_Close_Segment ();
		}HC_Close_Segment ();

		//	//HC_Include_Segment_By_Key (body2key);
		//}HC_Close_Segment ();
		hoopsview2->GetHoopsView ()->SetGeometryChanged ();
		hoopsview2->GetHoopsView ()->ZoomToExtents ();
		hoopsview2->GetHoopsView ()->Update ();

		mesh_edit_controller1->mesh = mesh1;
		mesh_edit_controller1->hoopsview = hoopsview1;
		mesh_edit_controller1->body = body1;

		mesh_edit_controller2->mesh = mesh2;
		mesh_edit_controller2->hoopsview = hoopsview2;
		mesh_edit_controller2->body = body2;
	}
}

void ManualMeshMatchWidget::on_open ()
{
	dir_str = QFileDialog::getExistingDirectory (this, tr("选择模型所在文件夹"));

	if (dir_str != ""){
		QString part1mesh = dir_str + "\\part1.ovm",
			part1body = dir_str + "\\part1.sat",
			part2mesh = dir_str + "\\part2.ovm",
			part2body = dir_str + "\\part2.sat";

		QFileInfo fi1 (part1mesh), fi2 (part1body), fi3 (part2mesh), fi4 (part2body);
		if (!fi1.exists () || !fi2.exists () || !fi3.exists () || !fi4.exists ()){
			QMessageBox::warning (this, "错误", "打不开文件！");
			return;
		}

		mesh1 = JC::load_volume_mesh (part1mesh.toAscii ().data ());
		body1 = JC::load_acis_model (part1body);
		mesh2 = JC::load_volume_mesh (part2mesh.toAscii ().data ());
		body2 = JC::load_acis_model (part2body);
		
		all_interfaces = get_interfaces ();
		if (all_interfaces.empty ()){
			QMessageBox::critical (this, "错误", "两个模型没有贴合部分！");
			delete mesh1; delete  mesh2;
			mesh1 = mesh2 = NULL;
			return;
		}

		common_interfaces.clear (); interfaces1.clear (); interfaces2.clear ();
		ENTITY_LIST intf_list1, intf_list2;
		foreach (auto &p, all_interfaces){
			interfaces1.insert (p.first);
			interfaces2.insert (p.second);
			intf_list1.add (p.first); intf_list2.add (p.second);
			common_interfaces.insert (p.first);
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

		fSetupHoopsView (mesh_edit_controller1, hoopsview1, mesh1, body1, interfaces1);
		fSetupHoopsView (mesh_edit_controller2, hoopsview2, mesh2, body2, interfaces2);

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

void ManualMeshMatchWidget::on_see_only_left ()
{
	hoopsview1->show ();
	mesh_edit_controller1->show ();
		mesh_edit_controller2->hide ();
	hoopsview2->hide ();
}

void ManualMeshMatchWidget::on_init_matching ()
{
	if (mm_handler) delete mm_handler;
	MMData data1, data2;
	data1.inter_patch = mesh_edit_controller1->get_interface_quads ();
	data2.inter_patch = mesh_edit_controller2->get_interface_quads ();
	data1.mesh = mesh1; data2.mesh = mesh2;
	mm_handler = new MeshMatchingHandler (&data1, &data2, common_interfaces);
	mm_handler->init_match ();

	matched_chord_pairs = mm_handler->matched_chord_pairs;

	unmatched_chords1 = mm_handler->chord_set1;
	unmatched_chords2 = mm_handler->chord_set2;

	draw_matched_chords (matched_chord_pairs);
}

void ManualMeshMatchWidget::on_update_matched_chords ()
{
	unmatched_chords1.clear ();
	unmatched_chords2.clear ();
	JC::retrieve_chords (mesh1, interface_fhs1, unmatched_chords1);
	JC::retrieve_chords (mesh2, interface_fhs2, unmatched_chords2);

	auto bk_matched_chord_pairs = matched_chord_pairs;
	matched_chord_pairs.clear ();
	foreach (auto &p, bk_matched_chord_pairs){
		auto locate1 = unmatched_chords1.find (p.first);
		auto locate2 = unmatched_chords2.find (p.second);

		if (locate1 != unmatched_chords1.end () &&
			locate2 != unmatched_chords2.end ()){
				unmatched_chords1.erase (p.first);
				unmatched_chords2.erase (p.second);
				matched_chord_pairs.insert (p);
		}
	}

	draw_matched_chords (matched_chord_pairs);
}

bool face_coincident (FACE *f1, FACE *f2, double myresabs)
{
	ENTITY_LIST vertices1, vertices2;
	api_get_vertices (f1, vertices1); api_get_vertices (f2, vertices2);
	std::vector<VERTEX*> std_vertices1, std_vertices2;
	JC::entity_list_to_vector (vertices1, std_vertices1);
	JC::entity_list_to_vector (vertices2, std_vertices2);

	//check vertices
	foreach (VERTEX *v1, std_vertices1){
		auto pFuncFindCoincidentVertex = [&v1, &myresabs](const VERTEX *v2)-> bool{
			SPAposition pos1 = v1->geometry ()->coords (),
				pos2 = v2->geometry ()->coords();
			return (same_point (pos1, pos2, myresabs));
		};
		auto locate = std::find_if (std_vertices2.begin (), std_vertices2.end (), pFuncFindCoincidentVertex);
		if (locate != std_vertices2.end ())
			std_vertices2.erase (locate);
		else return false;
	}

	//check edges
	ENTITY_LIST edges1, edges2;
	api_get_edges (f1, edges1); api_get_edges (f2,edges2);
	std::vector<SPAposition> mid_positions1, mid_positions2;
	std::vector<EDGE*> std_edges1, std_edges2;
	JC::entity_list_to_vector (edges1, std_edges1);
	JC::entity_list_to_vector (edges2, std_edges2);
	std::for_each (std_edges1.begin (), std_edges1.end (), [&mid_positions1](EDGE*e){mid_positions1.push_back (e->mid_pos ());});
	std::for_each (std_edges2.begin (), std_edges2.end (), [&mid_positions2](EDGE*e){mid_positions2.push_back (e->mid_pos ());});
	foreach (SPAposition p1, mid_positions1){
		auto pFuncFindCoincidentPoint = [&p1, &myresabs](SPAposition p2)-> bool{
			return (same_point (p1, p2, myresabs));
		};
		auto locate = std::find_if (mid_positions2.begin (), mid_positions2.end (), pFuncFindCoincidentPoint);
		if (locate != mid_positions2.end ())
			mid_positions2.erase (locate);
		else
			return false;
	}
	return true;
}

std::vector<std::pair<FACE*, FACE*> > find_interface (BODY *body1, BODY *body2, double myresabs)
{
	std::pair<FACE*, FACE*> intf_pair;
	intf_pair.first = intf_pair.second = NULL;
	std::vector<std::pair<FACE*, FACE*> > pairs;
	ENTITY_LIST face_list1, face_list2;
	api_get_faces (body1, face_list1); api_get_faces (body2, face_list2);
	for (int i = 0; i != face_list1.count (); ++i){
		FACE *f1 = (FACE*)face_list1[i];
		for (int j = 0; j != face_list2.count (); ++j){
			FACE *f2 = (FACE*)face_list2[j];
			if (face_coincident (f1, f2, myresabs)){
				intf_pair.first = f1; intf_pair.second = f2;
				pairs.push_back (intf_pair);
			}
		}
	}
	return pairs;
}

std::set<std::pair<FACE *, FACE*> > ManualMeshMatchWidget::get_interfaces ()
{
	std::set<std::pair<FACE *, FACE*> > interfaces;
	logical is_touch;
	api_entity_entity_touch (body1, body2, is_touch);
	if (is_touch == FALSE){
		QMessageBox::warning (this, "错误", "两个模型没有接触面！");
		return interfaces;
	}
	BoolOptions boolopts;
	boolopts.set_near_coincidence_fuzz (SPAresabs * 1000);
	api_initialize_booleans ();
	ENTITY_LIST faces_list1, faces_list2;
	std::vector<FACE*> faces1, faces2;
	outcome o = api_imprint (body2, body1, &boolopts);
	if (!o.ok ()){
		auto str = o.get_error_info ()->error_message ();
		str = str;
	}
	api_terminate_booleans ();

	auto interface_pairs = find_interface (body1, body2, SPAresabs * 1000);

	JC::vector_to_set (interface_pairs, interfaces);

	return interfaces;
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
		hoopsview1->render_chord (p.first, "green", 6);
		hoopsview2->render_chord (p.second, "green", 6);
		i++;
	}
}

void ManualMeshMatchWidget::on_match_two_chords ()
{
	DualChord *chord1 = mesh_edit_controller1->get_selected_chord (),
		*chord2 = mesh_edit_controller2->get_selected_chord ();
	if (chord1 == NULL || chord2 == NULL){
		QMessageBox::warning (this, "错误", "必须选中两条chord！");
		return;
	}
	hoopsview1->derender_chord (chord1);
	hoopsview2->derender_chord (chord2);
	matched_chord_pairs.insert (std::make_pair (chord1,chord2));
}

void ManualMeshMatchWidget::on_check_match ()
{
	//if (mm_handler) delete mm_handler;
	////get_interface_quads (); 
	//MMData data1, data2;
	//data1.inter_patch = interface_fhs1;
	//data2.inter_patch = interface_fhs2;
	//data1.mesh = mesh1; data2.mesh = mesh2;
	//mm_handler = new MeshMatchingHandler (&data1, &data2, common_interface);
	//mm_handler->matched_chord_pairs = matched_chord_pairs;
	//if (mm_handler->check_match ()){
	//	QMessageBox::information (this, "提示", "可以匹配！");
	//}else{
	//	QMessageBox::warning (this, "提示", "不能匹配！");
	//}

}

void ManualMeshMatchWidget::on_show_interface ()
{
	//assembly_viewer->matched_chord_pairs = matched_chord_pairs;
	//assembly_viewer->show ();
}

void ManualMeshMatchWidget::on_show_chord_pairs ()
{
	chord_match_widget->set_matched_pairs (matched_chord_pairs);
	chord_match_widget->show ();
}

void ManualMeshMatchWidget::on_highlight_chord_pair (uint chord_ptr1, uint chord_ptr2)
{
	DualChord *chord1 = (DualChord*)chord_ptr1,
		*chord2 = (DualChord*)chord_ptr2;
	if (prev_sel_chord1 && prev_sel_chord2){
		hoopsview1->derender_chord (prev_sel_chord1);
		hoopsview2->derender_chord (prev_sel_chord2);
		hoopsview1->render_chord (prev_sel_chord1, "yellow", 2);
		hoopsview2->render_chord (prev_sel_chord2, "pink", 2);
	}
	hoopsview1->derender_chord (chord1);
	hoopsview2->derender_chord (chord2);
	prev_sel_chord1 = chord1; prev_sel_chord2 = chord2;

	hoopsview1->render_chord (chord1, "green", 4);
	hoopsview2->render_chord (chord2, "green", 4);
}

void ManualMeshMatchWidget::on_delete_chord_pair (uint chord_ptr1, uint chord_ptr2)
{
	DualChord *chord1 = (DualChord*)chord_ptr1,
		*chord2 = (DualChord*)chord_ptr2;
	hoopsview1->derender_chord (chord1);
	hoopsview2->derender_chord (chord2);
	matched_chord_pairs.erase (std::make_pair (chord1, chord2));
}

void ManualMeshMatchWidget::on_close_chord_pair_widget ()
{
	if (prev_sel_chord1 && prev_sel_chord2){
		hoopsview1->derender_chord (prev_sel_chord1);
		hoopsview2->derender_chord (prev_sel_chord2);
		hoopsview1->render_chord (prev_sel_chord1, "yellow", 2);
		hoopsview2->render_chord (prev_sel_chord2, "pink", 2);
	}
	prev_sel_chord1 = prev_sel_chord2 = NULL;
}

void ManualMeshMatchWidget::on_save ()
{
	QDir dir;
	QString dir_name = QInputDialog::getText (this, 
		"目录名字", "输入保存目录名字");
	if (dir_name == "") return;
	while (dir.exists ("../testdata/" + dir_name)){
		dir_name = QInputDialog::getText (this, 
			"目录名字", "目录名字已经存在！输入保存目录名字");
		if (dir_name == "") return;
	}

	bool ok = dir.mkdir ("../testdata/" + dir_name);
	if (!ok){
		QMessageBox::warning (this, "错误", "创建文件夹失败！");
		return;
	}
	JC::save_volume_mesh (mesh1, "../testdata/" + dir_name + "/part1.ovm");
	JC::save_volume_mesh (mesh2, "../testdata/" + dir_name + "/part2.ovm");
	JC::save_acis_entity (body1, 
		QString("../testdata/" + dir_name + "/part1.sat")
		.toAscii ().data ());
	JC::save_acis_entity (body2, 
		QString("../testdata/" + dir_name + "/part2.sat")
		.toAscii ().data ());
	QMessageBox::information (this, "提醒", "保存成功！");
}

DualChord *ManualMeshMatchWidget::get_matched_chord (DualChord *chord)
{
	foreach (auto &p, matched_chord_pairs){
		if (p.first == chord) return p.second;
		else if (p.second == chord) return p.first;
	}
	return NULL;
}

std::vector<DualChord*> ManualMeshMatchWidget::get_intersect_seq (DualChord *chord, bool &self_int)
{
	std::vector<DualChord*> int_seq;
	ChordPairs reversed_matched_chord_pairs;
	foreach (auto &p, matched_chord_pairs)
		reversed_matched_chord_pairs.insert (std::make_pair (p.second, p.first));
	self_int = false;

	auto fGetIntSeq = [&] (VolumeMesh *mesh, ChordPairs & cp, std::vector<DualChord*> &is){
		auto E_CHORD_PTR = mesh->request_edge_property<unsigned long> ("chordptr");
		foreach (auto &fh, chord->ordered_fhs){
			auto heh_vec = mesh->face (fh).halfedges ();
			std::set<DualChord*> chords;
			foreach (auto &heh, heh_vec){
				auto eh = mesh->edge_handle (heh);
				DualChord *tmp_chord = (DualChord*)(E_CHORD_PTR[eh]);
				assert (tmp_chord);
				chords.insert (tmp_chord);
			}
			assert (chords.size () <= 2);
			if (chords.size () == 1){
				is.push_back (NULL);
				self_int = true;
			}else if (chords.size () == 2){
				chords.erase (chord);
				is.push_back (*(chords.begin ()));
			}else
				assert (false);
		}
	};
	if (chord->mesh == mesh1)
		fGetIntSeq (mesh1, matched_chord_pairs, int_seq);
	else
		fGetIntSeq (mesh2, reversed_matched_chord_pairs, int_seq);
	return int_seq;
}

void ManualMeshMatchWidget::on_get_polyline ()
{
	DualChord *chord1 = mesh_edit_controller1->get_selected_chord (),
		*chord2 = mesh_edit_controller2->get_selected_chord ();
	if (chord1 && chord2){
		QMessageBox::warning (this, "错误", "不能两边同时选中！");
		return;
	}
	auto fGetPolyline = [&] (VolumeMesh *mesh_from, DualChord *chord, VolumeMesh *mesh_to)->std::vector<OvmEgH>{
		//获得相交序列
		bool self_int;
		auto int_seq = get_intersect_seq (chord, self_int);

		std::vector<DualChord*> trans_int_seq;
		int self_int_count = 0;
		foreach (auto &int_chord, int_seq){
			if (int_chord == NULL){
				self_int_count++;
				if (self_int_count == 2)
					trans_int_seq.push_back (NULL);
				else if (self_int_count > 2)
					assert (false);
			}
			else{
				auto trans_chord = get_matched_chord (int_chord);
				assert (trans_chord);
				trans_int_seq.push_back (trans_chord);
			}
		}

		//获得起始候选点和终止候选点
		//auto E_INT_EG_PTR = mesh->request_edge_property<unsigned long> ("intedgeptr");
		//auto start_asso_eg = (EDGE*)(E_INT_EG_PTR[chord->ordered_ehs.front ()]);
		//auto end_asso_eg = (EDGE*)(E_INT_EG_PTR[chord->ordered_ehs.back ()]);

	};
}

void ManualMeshMatchWidget::on_auto_match ()
{
	hoopsview1->derender_all_chords ();
	hoopsview2->derender_all_chords ();

	if (dir_str.isEmpty () || dir_str == "") return;
	auto matching_dir_str = dir_str + "/matching/";
	QDir matching_dir (matching_dir_str);
	if (!matching_dir.exists ()) return;

	matching_dir.setFilter(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
	QFileInfoList list = matching_dir.entryInfoList();

	int subdir_count = list.count();
	if(subdir_count <= 0)
		return;

	std::vector<QString> subdir_strs;
	for(int i = 0; i != subdir_count; ++i){
		QFileInfo file_info = list.at(i);
		QString subdir_path_str = file_info.filePath();
		subdir_strs.push_back (subdir_path_str);
	}

	foreach (auto str, subdir_strs){
		QString part1mesh = str + "\\part1.ovm",
			part2mesh = str + "\\part2.ovm";

		delete mesh1; delete mesh2;
		mesh1 = JC::load_volume_mesh (part1mesh.toAscii ().data ());
		mesh2 = JC::load_volume_mesh (part2mesh.toAscii ().data ());
		mesh_edit_controller1->mesh = mesh1;
		mesh_edit_controller2->mesh = mesh2;

		HC_Open_Segment ("/match models");{
			HC_Open_Segment_By_Key (mesh1key);{
				HC_Flush_Geometry ("...");
				JC::render_volume_mesh_boundary (mesh1);
			}HC_Close_Segment ();
			HC_Open_Segment_By_Key (mesh2key);{
				HC_Flush_Geometry ("...");
				JC::render_volume_mesh_boundary (mesh2);
			}HC_Close_Segment ();
		}HC_Close_Segment ();

		hoopsview1->GetHoopsView ()->SetGeometryChanged ();
		hoopsview1->GetHoopsView ()->Update ();

		hoopsview2->GetHoopsView ()->SetGeometryChanged ();
		hoopsview2->GetHoopsView ()->Update ();

		//get_interface_quads ();

		//if (mm_handler) delete mm_handler;
		//MMData data1, data2;
		//data1.inter_patch = interface_fhs1;
		//data2.inter_patch = interface_fhs2;
		//data1.mesh = mesh1; data2.mesh = mesh2;
		//mm_handler = new MeshMatchingHandler (&data1, &data2, common_interface);
		//mm_handler->init_match ();

		//matched_chord_pairs = mm_handler->matched_chord_pairs;

		//unmatched_chords1 = mm_handler->chord_set1;
		//unmatched_chords2 = mm_handler->chord_set2;

		//draw_matched_chords (matched_chord_pairs);

		QTime time;
		time= QTime::currentTime();
		qsrand(time.msec()+time.second()*1000);



		int xxx=qrand()%3000;

		Sleep (1000 + xxx);
	}
}

void ManualMeshMatchWidget::on_merge ()
{
	HC_Open_Segment ("/match models");{
		HC_Open_Segment_By_Key (mesh1key);{
			//HC_Flush_Geometry ("...");
			JC::render_volume_mesh_boundary (mesh2);
		}HC_Close_Segment ();
	}HC_Close_Segment ();

	hoopsview1->GetHoopsView ()->SetGeometryChanged ();
	hoopsview1->GetHoopsView ()->Update ();

	hoopsview2->hide ();
}
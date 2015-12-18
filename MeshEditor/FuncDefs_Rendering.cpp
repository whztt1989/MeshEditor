#include "stdafx.h"
#include "FuncDefs.h"

namespace JC{
	void render_volume_mesh (VolumeMesh *mesh)
	{
		std::unordered_set<int> bvs, bes, bfs, bcs;	//boundary elements
		std::unordered_set<int> ivs, ies, ifs, ics;	//inner elements
		for (auto v_it = mesh->vertices_begin (); v_it != mesh->vertices_end (); ++v_it)
		{
			if (mesh->is_boundary (*v_it)) bvs.insert ((*v_it).idx ());
			else ivs.insert ((*v_it).idx ());
		}

		for (auto e_it = mesh->edges_begin (); e_it != mesh->edges_end (); ++e_it)
		{
			if (mesh->is_boundary (*e_it)) bes.insert ((*e_it).idx ());
			else ies.insert ((*e_it).idx ());
		}
		for (auto f_it = mesh->faces_begin (); f_it != mesh->faces_end (); ++f_it){
			if (mesh->is_boundary (*f_it)) bfs.insert ((*f_it).idx ());
			else ifs.insert ((*f_it).idx ());
		}
		for (auto c_it = mesh->cells_begin (); c_it != mesh->cells_end (); ++c_it){
			auto hfh_vec = mesh->cell (*c_it).halffaces ();
			bool is_bound = false;
			foreach (auto hfh, hfh_vec){
				if (mesh->is_boundary (mesh->face_handle (hfh))){
					bcs.insert ((*c_it).idx ());
					is_bound = true;
					break;
				}
			}
			if (!is_bound)
				ics.insert ((*c_it).idx ());
		}

		double cell_shrink_ratio = 0.6;

		char text_buf[20];
		auto fRenderWorker = [&mesh, &text_buf, &cell_shrink_ratio] (std::unordered_set<int> &vs, 
			std::unordered_set<int> &es, std::unordered_set<int> &fs, std::unordered_set<int> &cs){
				HC_Open_Segment ("meshvertices");{
					foreach (int idx, vs){
						OvmVeH vh = OvmVeH (idx);
						auto pt = mesh->vertex (vh);
						HC_KEY old_key = HC_Insert_Marker (pt[0], pt[1], pt[2]);
						HC_Open_Geometry (old_key);{
							HC_Set_User_Data (0, &idx, sizeof (int));
						}HC_Close_Geometry ();

						//HC_Open_Segment ("indices");{
						//	HC_Set_Visibility ("everything=off");
						//	sprintf (text_buf, "v%d", idx);
						//	HC_Insert_Text (pt[0], pt[1], pt[2], text_buf);
						//}HC_Close_Segment ();
					}
				}HC_Close_Segment ();
				HC_Open_Segment ("meshedges");{
					HC_Set_Line_Weight (2);
					//HC_Set_Visibility ("lines=off,edges=off,faces=off");
					foreach (int idx, es){
						OvmEgH eh = OvmEgH (idx);
						auto eg = mesh->edge (eh);
						auto pt1 = mesh->vertex (eg.from_vertex ()), pt2 = mesh->vertex (eg.to_vertex ());
						HC_KEY old_key = HC_Insert_Line (pt1[0], pt1[1], pt1[2], pt2[0], pt2[1], pt2[2]);
						HC_Open_Geometry (old_key);{
							HC_Set_User_Data (0, &idx, sizeof (int));
						}HC_Close_Geometry ();
						//HC_Open_Segment ("indices");{
						//	HC_Set_Visibility ("everything=off");
						//	sprintf (text_buf, "e%d", idx);
						//	auto mid = (pt1 + pt2) / 2;
						//	HC_Insert_Text (mid[0], mid[1], mid[2], text_buf);
						//}HC_Close_Segment ();
					}
				}HC_Close_Segment ();
				HC_Open_Segment ("meshfaces");{
					//HC_Set_Visibility ("lines=off,edges=off,faces=on");
					//HC_Set_Selectability ("faces=on!");
					//HC_Set_Face_Pattern ("##");
					foreach (int idx, fs){
						OvmFaH fh = OvmFaH (idx);
						auto f = mesh->face (fh);
						auto hfh = mesh->halfface_handle (idx, 0);
						if (!mesh->is_boundary (hfh))
							hfh = mesh->opposite_halfface_handle (hfh);

						QVector<HPoint> pts;
						OvmVec3d centre = OvmVec3d (0, 0, 0);
						for (auto fv_it = mesh->hfv_iter (hfh); fv_it; ++fv_it)
						{
							auto pt = mesh->vertex (*fv_it);
							centre += pt;
							pts.push_back (HPoint (pt[0], pt[1], pt[2]));
						}
						HC_KEY old_key = HC_Insert_Polygon (pts.size (), pts.data ());
						centre /= pts.size ();
						//assert (*f_it > 0);
						HC_Open_Geometry (old_key);{
							HC_Set_User_Data (0, &idx, sizeof (int));
						}HC_Close_Geometry ();
						//HC_Open_Segment ("indices");{
						//	HC_Set_Visibility ("everything=off");
						//	sprintf (text_buf, "f%d", idx);
						//	HC_Insert_Text (centre[0], centre[1], centre[2], text_buf);
						//}HC_Close_Segment ();
					}
				}HC_Close_Segment ();

				HC_Open_Segment ("meshcells");{
					HC_Set_Rendering_Options ("no lighting interpolation");
					HC_Set_Color ("edges=black");
					//foreach (int c_idx, cs){
					//	OvmCeH ch = OvmCeH (c_idx);
					//	auto centre_pos = mesh->barycenter (ch);
					//	//HC_KEY old_key = HC_Insert_Marker (centre_pos[0], centre_pos[1], centre_pos[2]);
					//	QVector<HPoint> pts;
					//	int count = 0;
					//	QMap<OvmVeH, int> indices_map;
					//	for (auto hv_it = mesh->hv_iter (ch); hv_it; ++hv_it, ++count){
					//		auto pt = mesh->vertex (*hv_it);
					//		auto new_pt = centre_pos + (pt - centre_pos) * cell_shrink_ratio;
					//		pts.push_back (HPoint (new_pt[0], new_pt[1], new_pt[2]));
					//		indices_map.insert (*hv_it, count);
					//	}

					//	auto hfh_vec = mesh->cell (ch).halffaces ();
					//	QVector<int> face_list;
					//	foreach (OvmHaFaH hfh, hfh_vec){
					//		auto heh_vec = mesh->halfface (hfh).halfedges ();
					//		face_list.push_back (heh_vec.size ());
					//		foreach (OvmHaEgH heh, heh_vec){
					//			auto vh = mesh->halfedge (heh).to_vertex ();
					//			face_list.push_back (indices_map[vh]);
					//		}
					//	}
					//	HC_KEY old_key = HC_Insert_Shell (pts.size (), pts.data (), face_list.size (), face_list.data ());
					//	HC_Open_Geometry (old_key);{
					//		HC_Set_User_Data (0, &c_idx, sizeof (int));
					//	}HC_Close_Geometry ();
						//HC_Open_Segment ("indices");{
						//	HC_Set_Visibility ("everything=off");
						//	sprintf (text_buf, "c%d", c_idx);
						//	HC_Insert_Text (pts.front ().x, pts.front ().y, pts.front ().z, text_buf);
						//}HC_Close_Segment ();
					//}
				}HC_Close_Segment ();
		};
		//flush all the contents in this segment using wild cards
		

		HC_Open_Segment ("boundary");{
			HC_Flush_Segment ("...");
			fRenderWorker (bvs, bes, bfs, bcs);
		}HC_Close_Segment ();


		HC_Open_Segment ("inner");{
			HC_Flush_Segment ("...");
			fRenderWorker (ivs, ies, ifs, ics);
		}HC_Close_Segment ();
	}

	void render_hexa_set (VolumeMesh *mesh, std::unordered_set<OvmCeH> &chs)
	{
		std::unordered_set<int> bvs, bes, bfs, bcs;	//boundary elements
		std::unordered_set<OvmFaH> bound_fhs;
		foreach (auto &ch, chs){
			auto hfhs = mesh->cell (ch).halffaces ();
			foreach (auto &hfh, hfhs){
				auto fh = mesh->face_handle (hfh);
				if (contains (bound_fhs, fh)) bound_fhs.erase (fh);
				else bound_fhs.insert (fh);
			}
		}

		foreach (auto &fh, bound_fhs){
			bfs.insert (fh.idx ());
			std::unordered_set<OvmEgH> bound_ehs;
			get_adj_edges_around_face (mesh, fh, bound_ehs);
			foreach (auto &eh, bound_ehs){
				bes.insert (eh.idx ());
			}
		}


		char text_buf[20];
		auto fRenderWorker = [&mesh, &text_buf] (std::unordered_set<int> &vs, 
			std::unordered_set<int> &es, std::unordered_set<int> &fs, std::unordered_set<int> &cs){
				HC_Open_Segment ("meshvertices");{
					foreach (int idx, vs){
						OvmVeH vh = OvmVeH (idx);
						auto pt = mesh->vertex (vh);
						HC_KEY old_key = HC_Insert_Marker (pt[0], pt[1], pt[2]);
						HC_Open_Geometry (old_key);{
							HC_Set_User_Data (0, &idx, sizeof (int));
						}HC_Close_Geometry ();
					}
				}HC_Close_Segment ();
				HC_Open_Segment ("meshedges");{
					HC_Set_Line_Weight (3);
					//HC_Set_Visibility ("lines=off,edges=off,faces=off");
					foreach (int idx, es){
						OvmEgH eh = OvmEgH (idx);
						auto eg = mesh->edge (eh);
						auto pt1 = mesh->vertex (eg.from_vertex ()), pt2 = mesh->vertex (eg.to_vertex ());
						HC_KEY old_key = HC_Insert_Line (pt1[0], pt1[1], pt1[2], pt2[0], pt2[1], pt2[2]);
						HC_Open_Geometry (old_key);{
							HC_Set_User_Data (0, &idx, sizeof (int));
						}HC_Close_Geometry ();
					}
				}HC_Close_Segment ();
				HC_Open_Segment ("meshfaces");{
					foreach (int idx, fs){
						OvmFaH fh = OvmFaH (idx);
						auto f = mesh->face (fh);
						auto hfh = mesh->halfface_handle (idx, 0);
						if (!mesh->is_boundary (hfh))
							hfh = mesh->opposite_halfface_handle (hfh);

						QVector<HPoint> pts;
						OvmVec3d centre = OvmVec3d (0, 0, 0);
						for (auto fv_it = mesh->hfv_iter (hfh); fv_it; ++fv_it)
						{
							auto pt = mesh->vertex (*fv_it);
							centre += pt;
							pts.push_back (HPoint (pt[0], pt[1], pt[2]));
						}
						HC_KEY old_key = HC_Insert_Polygon (pts.size (), pts.data ());
						centre /= pts.size ();
						//assert (*f_it > 0);
						HC_Open_Geometry (old_key);{
							HC_Set_User_Data (0, &idx, sizeof (int));
						}HC_Close_Geometry ();
					}
				}HC_Close_Segment ();
		};
		//flush all the contents in this segment using wild cards


		HC_Open_Segment ("boundary");{
			HC_Flush_Segment ("...");
			fRenderWorker (bvs, bes, bfs, bcs);
		}HC_Close_Segment ();
	}

	void render_volume_mesh_boundary (VolumeMesh *mesh)
	{
		std::unordered_set<int> bvs, bes, bfs;	//boundary elements
		for (auto bf_it = mesh->bf_iter (); bf_it; ++bf_it){
			bfs.insert ((*bf_it).idx ());
		}

		foreach (auto &fh, bfs){
			auto heh_vec = mesh->face (fh).halfedges ();
			foreach (auto &heh, heh_vec){
				auto eh = mesh->edge_handle (heh);
				auto eg = mesh->edge (eh);
				auto vh1 = eg.to_vertex (),
					vh2 = eg.from_vertex ();
				bes.insert (eh);
				bvs.insert (vh1); bvs.insert (vh2);
			}
		}

		auto fRenderWorker = [&] (std::unordered_set<int> &vs, 
			std::unordered_set<int> &es, std::unordered_set<int> &fs){
				HC_Open_Segment ("meshvertices");{
					foreach (int idx, vs){
						OvmVeH vh = OvmVeH (idx);
						auto pt = mesh->vertex (vh);
						HC_KEY old_key = HC_Insert_Marker (pt[0], pt[1], pt[2]);
						HC_Open_Geometry (old_key);{
							HC_Set_User_Data (0, &idx, sizeof (int));
						}HC_Close_Geometry ();
					}
				}HC_Close_Segment ();
				HC_Open_Segment ("meshedges");{
					//HC_Set_Visibility ("lines=off,edges=off,faces=off");
					HC_Set_Line_Weight (2);
					foreach (int idx, es){
						OvmEgH eh = OvmEgH (idx);
						auto eg = mesh->edge (eh);
						auto pt1 = mesh->vertex (eg.from_vertex ()), pt2 = mesh->vertex (eg.to_vertex ());
						HC_KEY old_key = HC_Insert_Line (pt1[0], pt1[1], pt1[2], pt2[0], pt2[1], pt2[2]);
						HC_Open_Geometry (old_key);{
							HC_Set_User_Data (0, &idx, sizeof (int));
						}HC_Close_Geometry ();
					}
				}HC_Close_Segment ();
				HC_Open_Segment ("meshfaces");{
					foreach (int idx, fs){
						OvmFaH fh = OvmFaH (idx);
						auto f = mesh->face (fh);
						auto hfh = mesh->halfface_handle (idx, 0);
						if (!mesh->is_boundary (hfh))
							hfh = mesh->opposite_halfface_handle (hfh);

						QVector<HPoint> pts;
						for (auto fv_it = mesh->hfv_iter (hfh); fv_it; ++fv_it)
						{
							auto pt = mesh->vertex (*fv_it);
							pts.push_back (HPoint (pt[0], pt[1], pt[2]));
						}
						HC_KEY old_key = HC_Insert_Polygon (pts.size (), pts.data ());
						HC_Open_Geometry (old_key);{
							HC_Set_User_Data (0, &idx, sizeof (int));
						}HC_Close_Geometry ();
					}
				}HC_Close_Segment ();
		};
		//flush all the contents in this segment using wild cards
		//HC_Flush_Segment ("...");
		fRenderWorker (bvs, bes, bfs);
	}

	void render_mesh_group (VolumeMeshElementGroup *group)
	{

	}

	HC_KEY insert_boundary_shell (VolumeMesh *mesh, std::unordered_set<OvmVeH> &bound_vhs,
		std::unordered_set<OvmHaFaH> &bound_hfhs)
	{
		HC_KEY shell_key = INVALID_KEY;
		std::hash_map<OvmVeH, int> vhs_idx_mapping;
		HPoint * pts = new HPoint[bound_vhs.size ()];
		int idx = 0;
		foreach (auto &vh, bound_vhs){
			auto pos = mesh->vertex (vh);
			pts[idx] = HPoint (pos[0], pos[1], pos[2]);
			vhs_idx_mapping.insert (std::make_pair (vh, idx));
			idx++;
		}

		int *face_list = new int[5 * bound_hfhs.size ()];

		idx = 0;
		foreach (auto &hfh, bound_hfhs){
			face_list[idx++] = mesh->valence (mesh->face_handle (hfh));
			for (auto hfv_it = mesh->hfv_iter (hfh); hfv_it; ++hfv_it){
				face_list[idx++] = vhs_idx_mapping[*hfv_it];
			}
		}

		shell_key = HC_Insert_Shell (bound_vhs.size (), pts, idx, face_list);
		delete[] pts;
		delete[] face_list;
		return shell_key;
	}

	HC_KEY insert_boundary_shell (VolumeMesh *mesh)
	{
		std::unordered_set<OvmFaH> bound_fhs;
		for (auto bf_it = mesh->bf_iter (); bf_it; ++bf_it)
			bound_fhs.insert (*bf_it);
		std::unordered_set<OvmVeH> bound_vhs;
		std::unordered_set<OvmHaFaH> bound_hfhs;
		foreach (auto &fh, bound_fhs){
			auto hfh = mesh->halfface_handle (fh, 0);
			if (!mesh->is_boundary (hfh))
				hfh = mesh->opposite_halfface_handle (hfh);
			bound_hfhs.insert (hfh);
			for (auto hfv_it = mesh->hfv_iter (hfh); hfv_it; ++hfv_it)
				bound_vhs.insert (*hfv_it);
		}
		
		return insert_boundary_shell (mesh, bound_vhs, bound_hfhs);
	}

	HC_KEY insert_boundary_shell (VolumeMesh *mesh, std::unordered_set<OvmCeH> &chs)
	{
		std::unordered_set<OvmVeH> bound_vhs;
		std::unordered_set<OvmHaFaH> inner_bound_hfhs, out_bound_hfhs;
		collect_boundary_element (mesh, chs, &bound_vhs, NULL, &inner_bound_hfhs);
		foreach (auto &hfh, inner_bound_hfhs)
			out_bound_hfhs.insert (mesh->opposite_halfface_handle (hfh));
		return insert_boundary_shell (mesh, bound_vhs, out_bound_hfhs);
	}
}
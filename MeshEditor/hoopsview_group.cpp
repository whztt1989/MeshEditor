#include "stdafx.h"
#include "hoopsview.h"
#include "HSelectionSet.h"

HC_KEY HoopsView::render_mesh_group (VolumeMeshElementGroup *group, MeshRenderOptions &render_options)
{
	if (mesh_group_exists (group))
		return INVALID_KEY;

	HC_KEY group_key = POINTER_TO_KEY (group);
	HC_KEY old_key = INVALID_KEY;
	double hexa_shrink_ratio = render_options.cell_size;
	char type[100], idx_str[50];

	auto fRenderVertices = [&](){
		HC_Open_Segment ("groupvertices");{
			if (render_options.vertex_color){
				char color_buf[100];
				sprintf (color_buf, "markers=%s", render_options.vertex_color);
				HC_Set_Color (color_buf);
			}
			if (render_options.vertex_size > 0){
				HC_Set_Marker_Size (render_options.vertex_size);
			}
			for (auto v_it = group->vhs.begin (); v_it != group->vhs.end (); ++v_it){
				int idx = (*v_it).idx ();
				auto pt = group->mesh->vertex (*v_it);
				HC_KEY old_key = HC_Insert_Marker (pt[0], pt[1], pt[2]);
				HC_Open_Geometry (old_key);{
					HC_Set_User_Data (0, &idx, sizeof (int));
				}HC_Close_Geometry ();
				if (render_options.show_vertex_indices){
					sprintf (idx_str, "v%d", idx);
					HC_Insert_Text (pt[0], pt[1], pt[2], idx_str);
				}
			}
		}HC_Close_Segment ();
	};

	auto fRenderEdges = [&](){
		HC_Open_Segment ("groupedges");{
			if (render_options.edge_color){
				char color_buf[100];
				sprintf (color_buf, "lines=%s", render_options.edge_color);
				HC_Set_Color (color_buf);
			}
			if (render_options.edge_width != -1){
				HC_Set_Line_Weight (render_options.edge_width);
			}
			for (auto e_it = group->ehs.begin (); e_it != group->ehs.end (); ++e_it){
				int idx = (*e_it).idx ();
				auto eg = group->mesh->edge (*e_it);
				auto pt1 = group->mesh->vertex (eg.from_vertex ()), 
					pt2 = group->mesh->vertex (eg.to_vertex ());
				HC_KEY old_key = HC_Insert_Line (pt1[0], pt1[1], pt1[2], pt2[0], pt2[1], pt2[2]);
				HC_Open_Geometry (old_key);{
					HC_Set_User_Data (0, &idx, sizeof (int));
				}HC_Close_Geometry ();
				if (render_options.show_edge_indices){
					sprintf (idx_str, "e%d", idx);
					auto midpos = (pt1 + pt2) / 2;
					HC_Insert_Text (midpos[0], midpos[1], midpos[2], idx_str);
				}
			}
		}HC_Close_Segment ();
	};

	auto fRenderFaces = [&](){
		HC_Open_Segment ("groupfaces");{
			if (render_options.face_color){
				char color_buf[100];
				sprintf (color_buf, "faces=%s", render_options.face_color);
				HC_Set_Color (color_buf);
			}
			for (auto f_it = group->fhs.begin (); f_it != group->fhs.end (); ++f_it)
			{
				int idx = (*f_it).idx ();
				auto f = group->mesh->face (*f_it);
				auto hfh = group->mesh->halfface_handle (*f_it, 0);
				HPoint pts[4];
				int i = 0;
				for (auto fv_it = group->mesh->hfv_iter (hfh); fv_it; ++fv_it, ++i)
				{
					auto pt = group->mesh->vertex (*fv_it);
					pts[i] = HPoint (pt[0], pt[1], pt[2]);
				}
				HC_KEY old_key = HC_Insert_Polygon (4, pts);
				HC_Open_Geometry (old_key);{
					HC_Set_User_Data (0, &idx, sizeof (int));
				}HC_Close_Geometry ();
				if (render_options.show_face_indices){
					sprintf (idx_str, "f%d", idx);
					auto midpos = group->mesh->barycenter (*f_it);
					HC_Insert_Text (midpos[0], midpos[1], midpos[2], idx_str);
				}
			}
		}HC_Close_Segment ();
	};

	auto fRenderCells = [&](){
		HC_Open_Segment ("groupcells");{
			HC_Set_Visibility ("markers=off,lines=on,edges=on,faces=on");
			HC_Set_Rendering_Options ("no lighting interpolation");
			if (render_options.cell_color){
				char color_buf[1000];
				sprintf (color_buf, "edges=%s,faces=%s", render_options.cell_color, render_options.cell_color);
				HC_Set_Color (color_buf);
			}
			foreach (auto &ch, group->chs){
				auto centre_pos = group->mesh->barycenter (ch);

				auto hfh_vec = group->mesh->cell (ch).halffaces ();
				HPoint face_pts[4];
				foreach (auto &hfh, hfh_vec){
					int idx = group->mesh->face_handle (hfh).idx ();
					auto heh_vec = group->mesh->halfface (hfh).halfedges ();
					int i = 0;
					foreach (auto &heh, heh_vec){
						auto vh = group->mesh->halfedge (heh).to_vertex ();
						auto pt = group->mesh->vertex (vh);
						auto new_pt = centre_pos + 
							(pt - centre_pos) * hexa_shrink_ratio;
						face_pts[i++] = HPoint (new_pt[0], new_pt[1], new_pt[2]);
					}
					HC_KEY old_key = HC_Insert_Polygon (4, face_pts);
					HC_Open_Geometry (old_key);{
						HC_Set_User_Data (0, &idx, sizeof (int));
					}HC_Close_Geometry ();
				}
			}//end foreach (auto &ch, group->chs){...
		}HC_Close_Segment ();
	};

	OPEN_GROUPS_SEG
		old_key = HC_Open_Segment ("");{
			sprintf (type, "group type=%s,group name=%s", group->type.toAscii ().data (), 
				group->name.toAscii ().data ());
			HC_Set_User_Options (type);
			HC_Set_Visibility ("text=on");
			if (render_options.text_size != -1)
				HC_Set_Text_Size (render_options.text_size);
			if (render_options.text_color){
				char color_buf[100];
				sprintf (color_buf, "text=%s", render_options.text_color);
				HC_Set_Color (color_buf);
			}
			fRenderVertices ();
			if (group->type != "sheet")
				fRenderEdges ();
			//if (group->type != "column")
				fRenderFaces ();
			fRenderCells ();

		}HC_Close_Segment ();
	CLOSE_GROUPS_SEG
	HC_Renumber_Key (old_key, group_key, "global");

	m_pHView->SetGeometryChanged ();
	m_pHView->Update ();
	return group_key;
}

void HoopsView::derender_one_mesh_group (VolumeMeshElementGroup *group)
{
	HC_KEY group_key = POINTER_TO_KEY(group);
	//since group_key is local to groups_key,
	//in order to delete it, we should first open groups_key
	OPEN_GROUPS_SEG
		char status[100];
		//we check the status of group key to see whether it is valid or not
		HC_Show_Key_Status (group_key, status);
		if (QString(status) != "invalid")
			HC_Delete_By_Key (group_key);
	CLOSE_GROUPS_SEG
	m_pHView->SetGeometryChanged ();
	m_pHView->Update ();
}

void HoopsView::derender_mesh_groups (const char *group_type, const char *group_name, bool delete_groups)
{
	char type[500], name[500];
	char child[500], pathname[500];
	std::vector<QString> mesh_groups_to_delete;
	OPEN_GROUPS_SEG
		HC_Begin_Segment_Search ("*");{
			while (HC_Find_Segment (child)){
				HC_Open_Segment (child);
				HC_Show_One_User_Option ("group type", type);
				HC_Show_One_User_Option ("group name", name);
				HC_Close_Segment ();
				if (strcmp (group_type, type) == 0){
					if (group_name){
						if (!streq (group_name, name))
							continue;
					}
					mesh_groups_to_delete.push_back (child);
				}
			}
		}HC_End_Segment_Search ();
	CLOSE_GROUPS_SEG

	for (int i = 0; i != mesh_groups_to_delete.size (); ++i){
		HC_KEY key = HC_Open_Segment (mesh_groups_to_delete[i].toAscii ().data ());
		HC_Close_Segment ();
		if (delete_groups){
			auto group = (VolumeMeshElementGroup*)(KEY_TO_POINTER(key));
			delete group;
		}
		HC_Delete_By_Key (key);
	}

	m_pHView->SetGeometryChanged ();
	m_pHView->Update ();
}

void HoopsView::derender_all_mesh_groups ()
{
	std::vector<QString> mesh_groups_to_delete;
	char type[500], name[500];
	char child[500], pathname[500];
	OPEN_GROUPS_SEG
		HC_Begin_Segment_Search ("*");{
			while (HC_Find_Segment (child)){
				mesh_groups_to_delete.push_back (child);
			}
	}HC_End_Segment_Search ();
	CLOSE_GROUPS_SEG
	//HC_Delete_By_Key (groups_key);
	for (int i = 0; i != mesh_groups_to_delete.size (); ++i){
		HC_KEY key = HC_Open_Segment (mesh_groups_to_delete[i].toAscii ().data ());
		HC_Close_Segment ();
		auto group = (VolumeMeshElementGroup*)(KEY_TO_POINTER(key));
		if (group->type != "sheet" && group->type != "chord" && group->type != "column")
			delete group;
		HC_Delete_By_Key (key);
	}

	m_pHView->SetGeometryChanged ();
	m_pHView->Update ();
}

std::set<VolumeMeshElementGroup*> HoopsView::retrieve_mesh_groups (const char *group_type, const char *group_name)
{
	std::vector<QString> mesh_groups_to_retrieve;
	char type[500], name[500];
	char child[500], pathname[500];
	std::set<VolumeMeshElementGroup*> res;

	OPEN_GROUPS_SEG
		HC_Begin_Segment_Search ("*");{
			while (HC_Find_Segment (child)){
				HC_Open_Segment (child);
				HC_Show_One_User_Option ("group type", type);
				HC_Show_One_User_Option ("group name", name);
				HC_Close_Segment ();
				if (strcmp (group_type, type) == 0){
					if (group_name){
						if (!streq (group_name, name))
							continue;
					}
					mesh_groups_to_retrieve.push_back (child);
				}
			}
	}HC_End_Segment_Search ();
	CLOSE_GROUPS_SEG

	for (int i = 0; i != mesh_groups_to_retrieve.size (); ++i){
		HC_KEY key = HC_Open_Segment (mesh_groups_to_retrieve[i].toAscii ().data ());
		HC_Close_Segment ();
		auto group = (VolumeMeshElementGroup*)(KEY_TO_POINTER(key));
		res.insert (group);
	}
	return res;
}

bool HoopsView::mesh_group_exists (VolumeMeshElementGroup *group)
{
	HC_KEY group_key = POINTER_TO_KEY (group);
	char status[100];
	OPEN_GROUPS_SEG
		HC_Show_Key_Status (group_key, status);
	CLOSE_GROUPS_SEG
	if (strncmp (status, "invalid", 1) == 0) return false;
	else return true;
}

void HoopsView::get_mesh_groups (std::vector<VolumeMeshElementGroup*> &groups, 
	std::vector<VolumeMeshElementGroup*> &invisible_groups,
	std::vector<VolumeMeshElementGroup*> &highlighted_groups,
	const char *group_type /* = NULL */, const char *group_name /* = NULL */)
{
	char type[100], name[100], visibility[100];
	char child[200], pathname[200];
	auto fSearch = [&] (std::vector<VolumeMeshElementGroup*> &mygroups){
		HC_Begin_Segment_Search ("*");{
			while (HC_Find_Segment (child)){
				HC_KEY key = HC_Open_Segment (child);
				HC_Show_One_User_Option ("group type", type);
				HC_Show_One_User_Option ("group name", name);
				sprintf (visibility, "");
				if (HC_Show_Existence ("visibility"))
					HC_Show_Visibility (visibility);
				HC_Close_Segment ();
				if (group_type){
					if (!streq (group_type, type))
						continue;
				}
				if (group_name){
					if (!streq (group_name, name))
						continue;
				}
				
				VolumeMeshElementGroup *group = (VolumeMeshElementGroup*)(KEY_TO_POINTER (key));
				if (streq (visibility, "off"))
					invisible_groups.push_back (group);
				else
					mygroups.push_back (group);
			}
		}HC_End_Segment_Search ();
	};
	OPEN_GROUPS_SEG
		fSearch (groups);
	CLOSE_GROUPS_SEG

	OPEN_HIGHLIGHT_SEG
		fSearch (highlighted_groups);
	CLOSE_HIGHLIGHT_SEG;
}

void HoopsView::show_mesh_group (VolumeMeshElementGroup *group, bool show)
{
	if (!mesh_group_exists (group)) return;

	HC_KEY group_key = POINTER_TO_KEY (group);
	HC_Open_Segment_By_Key (group_key);{
		if (show){
			HC_UnSet_Visibility ();
			HC_UnSet_One_Rendering_Option("attribute lock");
		}else{
			HC_Set_Visibility ("off");
			HC_Set_Rendering_Options ("attribute lock = visibility");
		}
	}HC_Close_Segment ();
	m_pHView->Update ();
}

void HoopsView::update_mesh_group (VolumeMeshElementGroup *group)
{
	if (!mesh_group_exists (group)) return;
	double hexa_shrink_ratio = 0.9;

	HC_KEY group_key = POINTER_TO_KEY (group);
	char type[255];
	HC_KEY key = INVALID_KEY;
	int idx = -1;

	auto fRenderVertices = [&](){
		HC_Open_Segment ("groupvertices");{
			HC_Flush_Geometry ("...");
			for (auto v_it = group->vhs.begin (); v_it != group->vhs.end (); ++v_it){
				int idx = (*v_it).idx ();
				auto pt = group->mesh->vertex (*v_it);
				HC_KEY old_key = HC_Insert_Marker (pt[0], pt[1], pt[2]);
				HC_Open_Geometry (old_key);{
					HC_Set_User_Data (0, &idx, sizeof (int));
				}HC_Close_Geometry ();
			}
		}HC_Close_Segment ();
	};

	auto fRenderEdges = [&](){
		HC_Open_Segment ("groupedges");{
			HC_Flush_Geometry ("...");
			for (auto e_it = group->ehs.begin (); e_it != group->ehs.end (); ++e_it){
				int idx = (*e_it).idx ();
				auto eg = group->mesh->edge (*e_it);
				auto pt1 = group->mesh->vertex (eg.from_vertex ()), 
					pt2 = group->mesh->vertex (eg.to_vertex ());
				HC_KEY old_key = HC_Insert_Line (pt1[0], pt1[1], pt1[2], pt2[0], pt2[1], pt2[2]);
				HC_Open_Geometry (old_key);{
					HC_Set_User_Data (0, &idx, sizeof (int));
				}HC_Close_Geometry ();
			}
		}HC_Close_Segment ();
	};

	auto fRenderFaces = [&](){
		HC_Open_Segment ("groupfaces");{
			HC_Flush_Geometry ("...");
			for (auto f_it = group->fhs.begin (); f_it != group->fhs.end (); ++f_it)
			{
				int idx = (*f_it).idx ();
				auto f = group->mesh->face (*f_it);
				auto hfh = group->mesh->halfface_handle (*f_it, 0);
				HPoint pts[4];
				int i = 0;
				for (auto fv_it = group->mesh->hfv_iter (hfh); fv_it; ++fv_it, ++i)
				{
					auto pt = group->mesh->vertex (*fv_it);
					pts[i] = HPoint (pt[0], pt[1], pt[2]);
				}
				HC_KEY old_key = HC_Insert_Polygon (4, pts);
				HC_Open_Geometry (old_key);{
					HC_Set_User_Data (0, &idx, sizeof (int));
				}HC_Close_Geometry ();
			}
		}HC_Close_Segment ();
	};

	auto fRenderCells = [&](){
		HC_Open_Segment ("groupcells");{
			HC_Flush_Geometry ("...");
			foreach (auto &ch, group->chs){
				auto centre_pos = group->mesh->barycenter (ch);

				auto hfh_vec = group->mesh->cell (ch).halffaces ();
				HPoint face_pts[4];
				foreach (auto &hfh, hfh_vec){
					int idx = group->mesh->face_handle (hfh).idx ();
					auto heh_vec = group->mesh->halfface (hfh).halfedges ();
					int i = 0;
					foreach (auto &heh, heh_vec){
						auto vh = group->mesh->halfedge (heh).to_vertex ();
						auto pt = group->mesh->vertex (vh);
						auto new_pt = centre_pos + 
							(pt - centre_pos) * hexa_shrink_ratio;
						face_pts[i++] = HPoint (new_pt[0], new_pt[1], new_pt[2]);
					}
					HC_KEY old_key = HC_Insert_Polygon (4, face_pts);
					HC_Open_Geometry (old_key);{
						HC_Set_User_Data (0, &idx, sizeof (int));
					}HC_Close_Geometry ();
				}
			}//end foreach (auto &ch, group->chs){...
		}HC_Close_Segment ();
	};

	HC_Open_Segment_By_Key (group_key);{
		fRenderVertices ();
		if (group->type != "sheet")
			fRenderEdges ();
		//if (group->type != "column")
		fRenderFaces ();
		fRenderCells ();
	}HC_Close_Segment ();

	m_pHView->SetGeometryChanged ();
	m_pHView->Update ();
}

void HoopsView::update_mesh_groups ()
{
	std::vector<VolumeMeshElementGroup*> groups;
	std::vector<VolumeMeshElementGroup*> invisible_groups;
	std::vector<VolumeMeshElementGroup*> highlighted_groups;
	get_mesh_groups (groups, invisible_groups, highlighted_groups);
	foreach (auto group, groups){
		update_mesh_group (group);
	}

	foreach (auto group, invisible_groups){
		update_mesh_group (group);
	}

	foreach (auto group, highlighted_groups){
		update_mesh_group (group);
	}
}

void HoopsView::update_mesh_group_rendering (VolumeMeshElementGroup *group, MeshGroupRenderParam *param)
{
	if (!mesh_group_exists (group)) return;

	HC_KEY group_key = POINTER_TO_KEY (group);
	HC_Open_Segment_By_Key (group_key);{
		HC_Open_Segment ("groupvertices");{
			if (param->vertex_size != 0){
				HC_Set_Marker_Size (param->vertex_size);
			}
			if (param->vertex_color != ""){
				QString color_str = QString ("markers=%1").arg (param->vertex_color);
				HC_Set_Color (color_str.toAscii ().data ());
			}
			if (!param->vertex_visible){
				HC_Set_Visibility ("everything=off");
			}else{
				HC_UnSet_Visibility ();
			}
		}HC_Close_Segment ();

		HC_Open_Segment ("groupedges");{
			if (param->edge_weight != 0){
				HC_Set_Line_Weight (param->edge_weight);
			}
			if (param->edge_color != ""){
				QString color_str = QString ("lines=%1").arg (param->edge_color);
				HC_Set_Color (color_str.toAscii ().data ());
			}
			if (!param->edge_visible){
				HC_Set_Visibility ("everything=off");
			}else{
				HC_UnSet_Visibility ();
			}
		}HC_Close_Segment ();

		HC_Open_Segment ("groupfaces");{
			if (param->face_color != ""){
				QString color_str = QString ("faces=%1").arg (param->face_color);
				HC_Set_Color (color_str.toAscii ().data ());
			}
			if (!param->face_visible){
				HC_Set_Visibility ("everything=off");
			}else{
				HC_UnSet_Visibility ();
			}
		}HC_Close_Segment ();

		HC_Open_Segment ("groupcells");{
			if (param->cell_color != ""){
				QString color_str = QString ("faces=%1,edges=%2").arg (param->cell_color).arg (param->cell_color);
				HC_Set_Color (color_str.toAscii ().data ());
			}
			if (!param->cell_visible){
				HC_Set_Visibility ("everything=off");
			}else{
				HC_UnSet_Visibility ();
			}
		}HC_Close_Segment ();
	}HC_Close_Segment ();
}

void HoopsView::highlight_mesh_group (VolumeMeshElementGroup *group)
{
	if (is_mesh_group_highlighted (group)) return;

	HC_KEY group_key = POINTER_TO_KEY (group);
	OPEN_HIGHLIGHT_SEG
			HC_Move_By_Key (group_key, ".");
	CLOSE_HIGHLIGHT_SEG
	
	m_pHView->Update ();
}

void HoopsView::dehighlight_mesh_group (VolumeMeshElementGroup *group)
{
	if (!is_mesh_group_highlighted (group)) return;

	HC_KEY group_key = POINTER_TO_KEY (group);
	OPEN_GROUPS_SEG
		HC_Move_By_Key (group_key, ".");
	CLOSE_GROUPS_SEG

		m_pHView->Update ();
}

bool HoopsView::is_mesh_group_highlighted (VolumeMeshElementGroup *group)
{
	HC_KEY group_key = POINTER_TO_KEY (group);
	char child[255];
	bool result = false;
	OPEN_HIGHLIGHT_SEG
			HC_Begin_Segment_Search ("*");{
				while (HC_Find_Segment (child)){
					HC_KEY key = HC_Open_Segment (child);
					HC_Close_Segment ();
					if (key == group_key){
						result = true;
						break;
					}
				}
			}HC_End_Segment_Search ();
	CLOSE_HIGHLIGHT_SEG
	return result;
}

void HoopsView::set_mesh_group_selectability (VolumeMeshElementGroup *group, bool vertices, bool edges, bool faces)
{
	if (!mesh_group_exists (group)) return;

	HC_KEY group_key = POINTER_TO_KEY (group);
	char *bv = vertices? "on": "off";
	char *be = edges? "on": "off";
	char *bf = faces? "on": "off";
	QString str = QString ("markers=%1,lines=%2,faces=%3").arg (bv).arg (be).arg (bf);
	HC_Open_Segment_By_Key (group_key);{
		HC_Set_Selectability (str.toAscii ().data ());
	}HC_Close_Segment ();
	m_pHView->Update ();
}

void HoopsView::set_mesh_group_selectability (const char *group_type, const char *group_name, 
	bool vertices, bool edges, bool faces)
{
	auto groups = retrieve_mesh_groups (group_type, group_name);
	foreach (auto &group, groups)
		set_mesh_group_selectability (group, vertices, edges, faces);
}

void HoopsView::set_selected_elements (VolumeMeshElementGroup *group)
{
	
}
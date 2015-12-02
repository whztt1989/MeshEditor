#include "stdafx.h"
#include "SheetIntDiagram.h"
#include <set>
#include <map>
#include <queue>
#include <algorithm>

OvmHaFaH get_incident_fh_on_cell (VolumeMesh *mesh, OvmCeH ch, OvmHaEgH heh)
{
	OvmHaFaH hfh = mesh->InvalidHalfFaceHandle;
	for (auto hehf_it = mesh->hehf_iter (heh); hehf_it; ++hehf_it)
	{
		OvmHaFaH test_hfh = *hehf_it;
		OvmCeH test_ch = mesh->incident_cell (test_hfh);
		if (test_ch == ch)
		{
			hfh = test_hfh;
			break;
		}
	}
	return hfh;
}

std::set<OvmEgH> get_parallel_edge_handles_on_cell (VolumeMesh *mesh, OvmCeH ch, OvmEgH eh)
{
	std::set<OvmEgH> parallel_ehs;
	parallel_ehs.insert (eh);
	OvmHaEgH start_heh = mesh->halfedge_handle (eh, 0);
	OvmHaFaH incident_hfh = get_incident_fh_on_cell (mesh, ch, start_heh);
	OvmHaEgH next_heh = mesh->opposite_halfedge_handle (
		mesh->next_halfedge_in_halfface (
		mesh->next_halfedge_in_halfface (start_heh, incident_hfh), incident_hfh));
	while (next_heh != start_heh){
		parallel_ehs.insert (mesh->edge_handle (next_heh));
		incident_hfh = get_incident_fh_on_cell (mesh, ch, next_heh);
		next_heh = mesh->opposite_halfedge_handle (
			mesh->next_halfedge_in_halfface (
			mesh->next_halfedge_in_halfface (next_heh, incident_hfh), incident_hfh));
	}
	return parallel_ehs;
}

std::set<OvmEgH> get_parallel_edge_handles_on_face (VolumeMesh *mesh, OvmFaH fh, OvmEgH eh)
{
	std::set<OvmEgH> parallel_ehs;
	parallel_ehs.insert (eh);
	OvmHaFaH hfh = mesh->halfface_handle (fh, 0);
	auto hehs_vec = mesh->halfface (hfh).halfedges ();
	OvmHaEgH heh = mesh->InvalidHalfEdgeHandle;
	for (auto heh_it = hehs_vec.begin (); heh_it != hehs_vec.end (); ++heh_it)
	{
		if (mesh->edge_handle (*heh_it) == eh)
		{
			heh = *heh_it;
			break;
		}
	}
	assert (heh != mesh->InvalidHalfEdgeHandle);

	heh = mesh->next_halfedge_in_halfface (mesh->next_halfedge_in_halfface (heh, hfh), hfh);
	parallel_ehs.insert (mesh->edge_handle (heh));
	return parallel_ehs;
}

SheetIntDiagram *get_sheet_int_diagram (VolumeMesh *mesh, DualSheet *sheet)
{
	SheetIntDiagram *sheet_int_diagram = new SheetIntDiagram ();
	OpenMesh::VPropHandleT<OvmCeH> OM_V_CH;
	OpenMesh::VPropHandleT<OvmFaH> OM_V_FH;
	OpenMesh::EPropHandleT<DualSheet*> OM_E_SHEET_PTR;
	sheet_int_diagram->add_property (OM_V_CH, "cellhandle");
	sheet_int_diagram->add_property (OM_V_FH, "facehandle");
	sheet_int_diagram->add_property (OM_E_SHEET_PTR, "sheetptr");

	assert (mesh->edge_property_exists<unsigned long> ("sheetptr"));
	auto OVM_E_SHEET_PTR = mesh->request_edge_property<unsigned long> ("sheetptr");

	auto sheet_ehs = sheet->ehs;
	//collect inside halfedges with the same direction
	std::set<OvmHaEgH> hehs_with_same_dire;
	OvmHaEgH seed_heh = mesh->halfedge_handle (*(sheet_ehs.begin ()), 0);
	sheet_ehs.erase (sheet_ehs.begin ());
	hehs_with_same_dire.insert (seed_heh);
	std::queue<OvmHaEgH> spread_set;
	spread_set.push (seed_heh);
	while (!sheet_ehs.empty ()){
		seed_heh = spread_set.front ();
		spread_set.pop ();
		for (auto hehf_it = mesh->hehf_iter (seed_heh); hehf_it; ++hehf_it)
		{
			OvmHaEgH oppo_heh_on_hf = mesh->opposite_halfedge_handle (
				mesh->next_halfedge_in_halfface (
				mesh->next_halfedge_in_halfface (seed_heh, *hehf_it), *hehf_it));
			OvmEgH oppo_eh_on_hf = mesh->edge_handle (oppo_heh_on_hf);
			auto locate = sheet_ehs.find (oppo_eh_on_hf);
			if (locate != sheet_ehs.end ()){
				sheet_ehs.erase (locate);
				hehs_with_same_dire.insert (oppo_heh_on_hf);
				spread_set.push (oppo_heh_on_hf);
			}
		}
	}//end while (!inside_ehs.empty ())...

	std::map<std::set<OvmEgH>, SidVeH> sid_vertices_mapping;
	for (auto heh_it = hehs_with_same_dire.begin (); heh_it != hehs_with_same_dire.end (); ++heh_it)
	{
		OvmHaEgH heh = *heh_it;
		OvmEgH eh = mesh->edge_handle (heh);
		std::vector<SidVeH> sid_vhs;
		for (auto hehf_it = mesh->hehf_iter (heh); hehf_it; ++hehf_it)
		{
			OvmFaH fh = mesh->face_handle (*hehf_it);
			if (mesh->is_boundary (fh)){
				auto parallel_ehs = get_parallel_edge_handles_on_face (mesh, fh, eh);
				auto locate = sid_vertices_mapping.find (parallel_ehs);
				if (locate == sid_vertices_mapping.end ())
				{
					SidVeH new_vh = sheet_int_diagram->add_vertex (SheetIntDiagram::Point (0, 0, 0));
					sid_vertices_mapping.insert (std::make_pair (parallel_ehs, new_vh));
					sid_vhs.push_back (new_vh);
					sheet_int_diagram->property (OM_V_FH, new_vh) = fh;
				}else{
					sid_vhs.push_back (locate->second);
				}
			}
			OvmCeH ch = mesh->incident_cell (*hehf_it);
			if (ch == mesh->InvalidCellHandle)
				continue;
			auto parallel_ehs = get_parallel_edge_handles_on_cell (mesh, ch, eh);
			auto locate = sid_vertices_mapping.find (parallel_ehs);
			if (locate == sid_vertices_mapping.end ())
			{
				SidVeH new_vh = sheet_int_diagram->add_vertex (SheetIntDiagram::Point (0, 0, 0));
				sid_vertices_mapping.insert (std::make_pair (parallel_ehs, new_vh));
				sid_vhs.push_back (new_vh);
				sheet_int_diagram->property (OM_V_CH, new_vh) = ch;
			}else{
				sid_vhs.push_back (locate->second);
			}
		}

		SidFaH new_fh = sheet_int_diagram->add_face (sid_vhs);
		assert (sheet_int_diagram->is_valid_handle (new_fh));
	}

	//set the sheet pointers
	std::set<SidEgH> inside_ehs;
	for (auto e_it = sheet_int_diagram->edges_begin (); e_it != sheet_int_diagram->edges_end (); ++e_it)
	{
		if (!sheet_int_diagram->is_boundary (e_it.handle ()))
			inside_ehs.insert (e_it.handle ());
	}

	while (!inside_ehs.empty ())
	{
		SidEgH seed_eh = *(inside_ehs.begin ());
		DualSheet *one_sheet = NULL;
		std::vector<SidEgH> one_sid_polyline;
		one_sid_polyline.push_back (seed_eh);
		inside_ehs.erase (inside_ehs.begin ());

		SidHaEgH seed_heh = sheet_int_diagram->halfedge_handle (seed_eh, 0);
		SidVeH vh1 = sheet_int_diagram->from_vertex_handle (seed_heh),
			vh2 = sheet_int_diagram->to_vertex_handle (seed_heh);

		//get the intersecting sheet
		//first get one quadrilateral in the intersecting column
		OvmFaH column_slice = mesh->InvalidFaceHandle;
		if (sheet_int_diagram->is_boundary (vh1))
			column_slice = sheet_int_diagram->property (OM_V_FH, vh1);
		else if (sheet_int_diagram->is_boundary (vh2))
			column_slice = sheet_int_diagram->property (OM_V_FH, vh2);
		else
		{
			OvmCeH ch1 = sheet_int_diagram->property (OM_V_CH, vh1),
				ch2 = sheet_int_diagram->property (OM_V_CH, vh2);
			column_slice = JC::get_common_face_handle (mesh, ch1, ch2);
		}
		assert (column_slice != mesh->InvalidFaceHandle);
		auto hehs_vec = mesh->face (column_slice).halfedges ();
		//the quadrilateral is the intersecting area of two sheets
		//one is the input sheet, the other is the sheet we want
		std::set<DualSheet*> intersect_sheets_on_slice;
		for (int i = 0; i != 4; ++i)
		{
			auto tmp_eh = mesh->edge_handle (hehs_vec[i]);
			DualSheet *tmp_s = (DualSheet*)(OVM_E_SHEET_PTR[tmp_eh]);
			assert (tmp_s);
			intersect_sheets_on_slice.insert (tmp_s);
		}
		assert (intersect_sheets_on_slice.size () <= 2);
		if (intersect_sheets_on_slice.size () == 2)
			intersect_sheets_on_slice.erase (sheet);
		one_sheet = *(intersect_sheets_on_slice.begin ());

		SidHaEgH heh = seed_heh;
		while (!sheet_int_diagram->is_boundary (vh1)){
			heh = sheet_int_diagram->prev_halfedge_handle (
				sheet_int_diagram->opposite_halfedge_handle (sheet_int_diagram->prev_halfedge_handle (heh)));
			vh1 = sheet_int_diagram->from_vertex_handle (heh);
			SidEgH cur_eh = sheet_int_diagram->edge_handle (heh);
			one_sid_polyline.insert (one_sid_polyline.begin (), cur_eh);
			auto locate = inside_ehs.find (cur_eh);
			assert (locate != inside_ehs.end ());
			inside_ehs.erase (locate);
		}
		heh = seed_heh;
		while (!sheet_int_diagram->is_boundary (vh2)){
			heh = sheet_int_diagram->next_halfedge_handle (
				sheet_int_diagram->opposite_halfedge_handle (sheet_int_diagram->next_halfedge_handle (heh)));
			vh2 = sheet_int_diagram->to_vertex_handle (heh);
			SidEgH cur_eh = sheet_int_diagram->edge_handle (heh);
			one_sid_polyline.push_back (cur_eh);
			auto locate = inside_ehs.find (cur_eh);
			assert (locate != inside_ehs.end ());
			inside_ehs.erase (locate);
		}

		for (auto e_it = one_sid_polyline.begin (); e_it != one_sid_polyline.end (); ++e_it)
			sheet_int_diagram->property (OM_E_SHEET_PTR, *e_it) = one_sheet;
	}
	return sheet_int_diagram;
}

SheetIntDiagram *get_quad_set_int_diagram (VolumeMesh *mesh, std::unordered_set<OvmFaH> &qs)
{
	SheetIntDiagram *sheet_int_diagram = new SheetIntDiagram ();
	OpenMesh::VPropHandleT<OvmCeH> OM_V_CH;
	OpenMesh::VPropHandleT<OvmFaH> OM_V_FH;
	OpenMesh::EPropHandleT<DualSheet*> OM_E_SHEET_PTR;
	sheet_int_diagram->add_property (OM_V_CH, "cellhandle");
	sheet_int_diagram->add_property (OM_V_FH, "facehandle");
	sheet_int_diagram->add_property (OM_E_SHEET_PTR, "sheetptr");

	assert (mesh->edge_property_exists<unsigned long> ("sheetptr"));
	auto OVM_E_SHEET_PTR = mesh->request_edge_property<unsigned long> ("sheetptr");

	//获得qs上的所有网格边
	std::unordered_set<OvmEgH> all_ehs_in_qs;
	foreach (auto &q, qs){
		JC::get_adj_edges_around_face (mesh, q, all_ehs_in_qs);
	}

	//获得qs上的所有网格点
	std::unordered_set<OvmVeH> all_vhs_on_qs;
	foreach (auto &q, qs) JC::get_adj_vertices_around_face (mesh, q, all_vhs_on_qs);

	std::map<std::set<OvmVeH>, SidVeH> sid_vertices_mapping;

	////每一个all_vhs_on_qs上的点都对应相交图中的一个面
	//foreach (auto &vh, all_vhs_on_qs){
	//	std::unordered_set<OvmFaH> tmp_adj_fhs, adj_fhs;
	//	JC::get_adj_faces_around_vertex (mesh, vh, tmp_adj_fhs);
	//	foreach (auto &tmp_fh, tmp_adj_fhs){
	//		if (JC::contains (qs, tmp_fh)) adj_fhs.insert (tmp_fh);
	//	}
	//	std::vector<SidVeH> sid_vhs;

	//	//foreach (auto &)
	//	for (auto hehf_it = mesh->hehf_iter (heh); hehf_it; ++hehf_it)
	//	{
	//		OvmFaH fh = mesh->face_handle (*hehf_it);
	//		if (mesh->is_boundary (fh)){
	//			auto parallel_ehs = get_parallel_edge_handles_on_face (mesh, fh, eh);
	//			auto locate = sid_vertices_mapping.find (parallel_ehs);
	//			if (locate == sid_vertices_mapping.end ())
	//			{
	//				SidVeH new_vh = sheet_int_diagram->add_vertex (SheetIntDiagram::Point (0, 0, 0));
	//				sid_vertices_mapping.insert (std::make_pair (parallel_ehs, new_vh));
	//				sid_vhs.push_back (new_vh);
	//				sheet_int_diagram->property (OM_V_FH, new_vh) = fh;
	//			}else{
	//				sid_vhs.push_back (locate->second);
	//			}
	//		}
	//		OvmCeH ch = mesh->incident_cell (*hehf_it);
	//		if (ch == mesh->InvalidCellHandle)
	//			continue;
	//		auto parallel_ehs = get_parallel_edge_handles_on_cell (mesh, ch, eh);
	//		auto locate = sid_vertices_mapping.find (parallel_ehs);
	//		if (locate == sid_vertices_mapping.end ())
	//		{
	//			SidVeH new_vh = sheet_int_diagram->add_vertex (SheetIntDiagram::Point (0, 0, 0));
	//			sid_vertices_mapping.insert (std::make_pair (parallel_ehs, new_vh));
	//			sid_vhs.push_back (new_vh);
	//			sheet_int_diagram->property (OM_V_CH, new_vh) = ch;
	//		}else{
	//			sid_vhs.push_back (locate->second);
	//		}
	//	}

	//	SidFaH new_fh = sheet_int_diagram->add_face (sid_vhs);
	//	assert (sheet_int_diagram->is_valid_handle (new_fh));
	//}

	////set the sheet pointers
	//std::set<SidEgH> inside_ehs;
	//for (auto e_it = sheet_int_diagram->edges_begin (); e_it != sheet_int_diagram->edges_end (); ++e_it)
	//{
	//	if (!sheet_int_diagram->is_boundary (e_it.handle ()))
	//		inside_ehs.insert (e_it.handle ());
	//}

	//while (!inside_ehs.empty ())
	//{
	//	SidEgH seed_eh = *(inside_ehs.begin ());
	//	DualSheet *one_sheet = NULL;
	//	std::vector<SidEgH> one_sid_polyline;
	//	one_sid_polyline.push_back (seed_eh);
	//	inside_ehs.erase (inside_ehs.begin ());

	//	SidHaEgH seed_heh = sheet_int_diagram->halfedge_handle (seed_eh, 0);
	//	SidVeH vh1 = sheet_int_diagram->from_vertex_handle (seed_heh),
	//		vh2 = sheet_int_diagram->to_vertex_handle (seed_heh);

	//	//get the intersecting sheet
	//	//first get one quadrilateral in the intersecting column
	//	OvmFaH column_slice = mesh->InvalidFaceHandle;
	//	if (sheet_int_diagram->is_boundary (vh1))
	//		column_slice = sheet_int_diagram->property (OM_V_FH, vh1);
	//	else if (sheet_int_diagram->is_boundary (vh2))
	//		column_slice = sheet_int_diagram->property (OM_V_FH, vh2);
	//	else
	//	{
	//		OvmCeH ch1 = sheet_int_diagram->property (OM_V_CH, vh1),
	//			ch2 = sheet_int_diagram->property (OM_V_CH, vh2);
	//		column_slice = JC::get_common_face_handle (mesh, ch1, ch2);
	//	}
	//	assert (column_slice != mesh->InvalidFaceHandle);
	//	auto hehs_vec = mesh->face (column_slice).halfedges ();
	//	//the quadrilateral is the intersecting area of two sheets
	//	//one is the input sheet, the other is the sheet we want
	//	std::set<DualSheet*> intersect_sheets_on_slice;
	//	for (int i = 0; i != 4; ++i)
	//	{
	//		auto tmp_eh = mesh->edge_handle (hehs_vec[i]);
	//		DualSheet *tmp_s = (DualSheet*)(OVM_E_SHEET_PTR[tmp_eh]);
	//		assert (tmp_s);
	//		intersect_sheets_on_slice.insert (tmp_s);
	//	}
	//	assert (intersect_sheets_on_slice.size () <= 2);
	//	if (intersect_sheets_on_slice.size () == 2)
	//		intersect_sheets_on_slice.erase (sheet);
	//	one_sheet = *(intersect_sheets_on_slice.begin ());

	//	SidHaEgH heh = seed_heh;
	//	while (!sheet_int_diagram->is_boundary (vh1)){
	//		heh = sheet_int_diagram->prev_halfedge_handle (
	//			sheet_int_diagram->opposite_halfedge_handle (sheet_int_diagram->prev_halfedge_handle (heh)));
	//		vh1 = sheet_int_diagram->from_vertex_handle (heh);
	//		SidEgH cur_eh = sheet_int_diagram->edge_handle (heh);
	//		one_sid_polyline.insert (one_sid_polyline.begin (), cur_eh);
	//		auto locate = inside_ehs.find (cur_eh);
	//		assert (locate != inside_ehs.end ());
	//		inside_ehs.erase (locate);
	//	}
	//	heh = seed_heh;
	//	while (!sheet_int_diagram->is_boundary (vh2)){
	//		heh = sheet_int_diagram->next_halfedge_handle (
	//			sheet_int_diagram->opposite_halfedge_handle (sheet_int_diagram->next_halfedge_handle (heh)));
	//		vh2 = sheet_int_diagram->to_vertex_handle (heh);
	//		SidEgH cur_eh = sheet_int_diagram->edge_handle (heh);
	//		one_sid_polyline.push_back (cur_eh);
	//		auto locate = inside_ehs.find (cur_eh);
	//		assert (locate != inside_ehs.end ());
	//		inside_ehs.erase (locate);
	//	}

	//	for (auto e_it = one_sid_polyline.begin (); e_it != one_sid_polyline.end (); ++e_it)
	//		sheet_int_diagram->property (OM_E_SHEET_PTR, *e_it) = one_sheet;
	//}
	return sheet_int_diagram;
}

void smooth_sheet_int_diagram (SheetIntDiagram *sheet_int_diagram, double radius, int smooth_rounds)
{
	//first get boundary vertices
	std::set<SidVeH> boundary_vertices, inside_vertices;
	for (auto v_it = sheet_int_diagram->vertices_begin (); v_it != sheet_int_diagram->vertices_end (); ++v_it)
	{
		if (sheet_int_diagram->is_boundary (*v_it))
			boundary_vertices.insert (*v_it);
		else
			inside_vertices.insert (*v_it);
	}
	std::vector<SidVeH> sorted_boundary_vertices;
	sorted_boundary_vertices.push_back (*(boundary_vertices.begin ()));
	boundary_vertices.erase (boundary_vertices.begin ());
	while (!boundary_vertices.empty ()){
		SidVeH vh = sorted_boundary_vertices.back ();
		bool has_found = false;
		for (auto v_it = boundary_vertices.begin (); v_it != boundary_vertices.end (); ++v_it)
		{
			SidHaEgH heh = sheet_int_diagram->find_halfedge (vh, *v_it);
			if (sheet_int_diagram->is_valid_handle (heh))
			{
				sorted_boundary_vertices.push_back (*v_it);
				boundary_vertices.erase (v_it);
				has_found = true;
				break;
			}
		}
		assert (has_found);
	}

	//disperse the boundary vertices on the circle
	double average_angle = M_PI * 2 / sorted_boundary_vertices.size ();
	for (int i = 0; i != sorted_boundary_vertices.size (); ++i)
	{
		double x = radius * std::cos (average_angle * i);
		double y = radius * std::sin (average_angle * i);
		sheet_int_diagram->set_point (sorted_boundary_vertices[i], SheetIntDiagram::Point (x, y, 0));
	}

	//smooth inner vertices
	double rounds = 0;
	while (rounds++ < smooth_rounds){
		for (auto v_it = inside_vertices.begin (); v_it != inside_vertices.end (); ++v_it)
		{
			SheetIntDiagram::Point centre = SheetIntDiagram::Point (0, 0, 0);
			int adj_vhs_count = 0;
			for (auto vv_it = sheet_int_diagram->vv_iter (*v_it); vv_it; ++vv_it)
			{
				centre += sheet_int_diagram->point (vv_it.handle ());
				adj_vhs_count++;
			}
			centre /= adj_vhs_count;
			sheet_int_diagram->set_point (*v_it, centre);
		}
	}
}
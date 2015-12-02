#include "StdAfx.h"
#include "DepthCutHandler.h"
#include "vector_utils.hxx"
#include "geom_utl.hxx"
#include <QMessageBox>
#include "utf_utils.h"
DepthCutHandler::DepthCutHandler(VolumeMesh *_mesh, BODY *_body)
	: mesh (_mesh), body (_body)
{
	cut_face = NULL;
	new_mesh = NULL;
}


DepthCutHandler::~DepthCutHandler(void)
{
}

void DepthCutHandler::get_start_fhs ()
{
	start_fhs.clear ();
	JC::get_fhs_on_acis_face (mesh, start_face, start_fhs);
}

void DepthCutHandler::get_hexas_within_depth ()
{
	hexas_within_depth.clear (); joint_fhs.clear ();

	int depth = 0;
	auto curr_front_fhs = start_fhs;
	while (depth++ < depth_num){
		//获得curr_front_fhs上的点
		std::unordered_set<OvmVeH> curr_front_vhs;
		foreach (auto &fh, curr_front_fhs){
			auto heh_vec = mesh->face (fh).halfedges ();
			foreach (auto &heh, heh_vec)
				curr_front_vhs.insert (mesh->halfedge (heh).to_vertex ());
		}
		//由这些点得到所有的相邻六面体
		foreach (auto &vh, curr_front_vhs){
			JC::get_adj_hexas_around_vertex (mesh, vh, hexas_within_depth);
		}
		//得到表面四边形面集
		curr_front_fhs.clear ();
		std::unordered_set<OvmFaH> bound_fhs;
		JC::collect_boundary_element (mesh, hexas_within_depth, NULL, NULL, &bound_fhs);
		//剔除掉bound_fhs中在interface_fhs上以及在侧面表面上的四边形(其实就是在几何表面的四边形)
		foreach (auto &fh, bound_fhs){
			if (!mesh->is_boundary (fh))
				curr_front_fhs.insert (fh);
		}
	}
	joint_fhs = curr_front_fhs;
}

void DepthCutHandler::create_cutting_face ()
{
	QVector<SPAposition> pts;
	std::unordered_set<OvmVeH> vhs, inside_vhs;
	OvmVec3d mid_pos(0, 0, 0);
	foreach (auto &fh, joint_fhs){
		auto adj_vhs = JC::get_adj_vertices_around_face (mesh, fh);
		foreach (auto &vh, adj_vhs){
			if (!mesh->is_boundary (vh)) {
				inside_vhs.insert (vh);
				continue;
			}
			if (JC::contains (vhs, vh)) continue;

			vhs.insert (vh);
			auto pos = mesh->vertex (vh);
			mid_pos += pos;
			pts.push_back (POS2SPA (pos));
		}
	}
	mid_pos /= pts.size ();

	SPAposition plane_pt;
	SPAunit_vector plane_normal;
	if (get_plane_from_points (pts.size (), pts.data (), plane_pt, plane_normal) == FALSE){
		QMessageBox::warning (NULL, "ERROR", "get plane error!");
		return;
	}
	//获取start_fhs上的一个点，来判断plane_normal的方向
	auto fGetTestPoint = [&] ()->SPAposition{
		OvmFaH test_fh = *(start_fhs.begin ());
		OvmVeH vh = mesh->halfedge (mesh->face (test_fh).halfedges ().front ()).from_vertex ();
		OvmVec3d pos = mesh->vertex (vh);
		return POS2SPA(pos);
	};
	SPAposition test_pos = fGetTestPoint();
	if (side_of_plane (plane_pt, plane_normal, test_pos) > 0){
		plane_normal = -plane_normal;
	}

	SPAposition body_box_min_pt, body_box_max_pt;
	api_get_entity_box (body, body_box_min_pt, body_box_max_pt);
	double max_offset = 0;
	int offset = std::abs (body_box_min_pt.x () - body_box_max_pt.x ());
	if (max_offset < offset) max_offset = offset;
	offset = std::abs (body_box_min_pt.y () - body_box_max_pt.y ());
	if (max_offset < offset) max_offset = offset;
	offset = std::abs (body_box_min_pt.z () - body_box_max_pt.z ());
	if (max_offset < offset) max_offset = offset;

	api_face_plane (plane_pt, max_offset * 1.2, max_offset * 1.2, &plane_normal, cut_face);
	ENTITY_LIST vertices_list;
	api_get_vertices (cut_face, vertices_list);

	SPAposition plane_centre (0, 0, 0);
	for (int i = 0; i != vertices_list.count (); ++i){
		auto pos = ((VERTEX*)(vertices_list[i]))->geometry ()->coords ();
		plane_centre.set_x (plane_centre.x () + pos.x ());
		plane_centre.set_y (plane_centre.y () + pos.y ());
		plane_centre.set_z (plane_centre.z () + pos.z ());
	}
	plane_centre.set_x (plane_centre.x () / vertices_list.count ());
	plane_centre.set_y (plane_centre.y () / vertices_list.count ());
	plane_centre.set_z (plane_centre.z () / vertices_list.count ());

	auto my_trans_transf = translate_transf (POS2SPA(mid_pos) - plane_centre);
	api_apply_transf (cut_face, my_trans_transf);

	foreach (auto &vh, inside_vhs){
		SPAposition closest_pt;
		double dist;
		auto pos = mesh->vertex (vh);
		api_entity_point_distance (cut_face, POS2SPA(pos), closest_pt, dist);
		mesh->set_vertex (vh, SPA2POS(closest_pt));
	}

}

void DepthCutHandler::depth_cutting ()
{
	BODY *open_shell = NULL;
	FACE *faces[1] = {cut_face};
	api_mk_by_faces (NULL, 1, faces, open_shell);
	api_intersect (open_shell, body);

	//保存
	if (new_mesh) delete new_mesh;

	new_mesh = new VolumeMesh ();
	std::unordered_set<OvmVeH> all_old_vhs;
	std::hash_map<OvmVeH, OvmVeH> vhs_mapping;
	foreach (auto &ch, hexas_within_depth){
		auto adj_vhs = JC::get_adj_vertices_around_cell (mesh, ch);
		foreach (auto &vh, adj_vhs)
			all_old_vhs.insert (vh);
	}

	foreach (auto &vh, all_old_vhs){
		auto new_vh = new_mesh->add_vertex (mesh->vertex (vh));
		vhs_mapping.insert (std::make_pair (vh, new_vh));
	}

	std::vector<std::vector<OvmVeH> > chs_recipe;
	foreach (auto &ch, hexas_within_depth){
		std::vector<OvmVeH> adj_vhs;
		for (auto hv_it = mesh->hv_iter (ch); hv_it; ++hv_it)
			adj_vhs.push_back (vhs_mapping[*hv_it]);
		chs_recipe.push_back (adj_vhs);
	}

	foreach (auto &vhs_ary, chs_recipe){
		new_mesh->add_cell (vhs_ary);
	}
}

void DepthCutHandler::save_data ()
{
	JC::save_acis_entity (body, "new_body.sat");
	JC::save_volume_mesh (new_mesh, "new_mesh.ovm");
}
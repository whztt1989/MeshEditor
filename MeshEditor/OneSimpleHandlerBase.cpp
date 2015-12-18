#include "StdAfx.h"
#include "OneSimpleHandlerBase.h"
#include "FaceFacePath.h"

OneSimpleHandlerBase::OneSimpleHandlerBase(VolumeMesh *_mesh, std::unordered_set<OvmEgH> &_input_ehs)
	: mesh (_mesh), input_ehs (_input_ehs)
{
	constant_fhs_ptr = NULL;
	min_depth = 1;
}


OneSimpleHandlerBase::~OneSimpleHandlerBase(void)
{
}

void OneSimpleHandlerBase::get_fhs_on_boundary ()
{
	boundary_fhs.clear ();
	for (auto bf_it = mesh->bf_iter (); bf_it; ++bf_it){
		boundary_fhs.insert (*bf_it);
	}
}

void OneSimpleHandlerBase::get_fhs_on_interface ()
{
	interface_fhs.clear ();
	auto V_ENTITY_PTR = mesh->request_vertex_property<unsigned int> ("entityptr");

	for (auto bf_it = mesh->bf_iter (); bf_it; ++bf_it){
		auto vhs_on_fh = JC::get_adj_vertices_around_face (mesh, *bf_it);
		FACE *f = JC::get_associated_geometry_face_of_boundary_fh (mesh, *bf_it);
		assert (f);
		if (f == inter_face) interface_fhs.insert (*bf_it);
	}
}

void OneSimpleHandlerBase::get_optimize_allowed_fhs ()
{
	for (auto bf_it = mesh->bf_iter (); bf_it; ++bf_it){
		auto fh = *bf_it;
		if (!JC::contains (*constant_fhs_ptr, fh) && !JC::contains (interface_fhs, fh)){
			optimize_allowed_fhs.insert (fh);
		}
	}
}

void OneSimpleHandlerBase::get_inflation_fhs_from_hexa_set (std::unordered_set<OvmCeH> &hexa_set)
{
	inflation_fhs.clear ();
	std::unordered_set<OvmFaH> bound_fhs;
	JC::collect_boundary_element (mesh, hexa_set, NULL, NULL, &bound_fhs);
	foreach (auto &fh, bound_fhs){
		if (!mesh->is_boundary (fh)) inflation_fhs.insert (fh);
	}
}

void OneSimpleHandlerBase::get_intersect_allowed_vhs ()
{
	std::unordered_set<OvmFaH> bound_fhs_less_than_min_depth;
	depth_control (bound_fhs_less_than_min_depth);

	intersect_allowed_vhs.clear ();
	std::unordered_set<OvmFaH> intersect_allowed_fhs;

	for (auto bf_it = mesh->bf_iter (); bf_it; ++bf_it){
		auto fh = *bf_it;
		if (!JC::contains (*constant_fhs_ptr, fh) && !JC::contains (bound_fhs_less_than_min_depth, fh)){
			intersect_allowed_fhs.insert (fh);
		}
	}

	std::unordered_set<OvmEgH> allowed_ehs;

	//先把内部边和边界边统计出来。内部边是被两个面所共享的，而边界边只和一个面相邻
	std::unordered_set<OvmEgH> ehs_count;
	foreach (auto &fh, intersect_allowed_fhs){
		auto heh_vec = mesh->face (fh).halfedges ();
		foreach (auto &heh, heh_vec){
			auto eh = mesh->edge_handle (heh);
			if (JC::contains (ehs_count, eh)){
				ehs_count.erase (eh);
				allowed_ehs.insert (eh);
			}else{
				ehs_count.insert (eh);
			}
		}
	}

	//然后统计内部点和边界点。内部点只和内部边相邻，而边界点至少和一条边界边相邻
	foreach (auto &eh, allowed_ehs){
		OvmVeH vh1 = mesh->edge (eh).from_vertex (),
			vh2 = mesh->edge (eh).to_vertex ();
		intersect_allowed_vhs.insert (vh1); intersect_allowed_vhs.insert (vh1);
	}

	//由于已经删掉了内部边，所以ehs_count只剩下边界边
	foreach (OvmEgH eh, ehs_count){
		OvmVeH vh1 = mesh->edge (eh).from_vertex (),
			vh2 = mesh->edge (eh).to_vertex ();
		intersect_allowed_vhs.erase (vh1); intersect_allowed_vhs.erase (vh1);
	}
}

void OneSimpleHandlerBase::depth_control (std::unordered_set<OvmFaH> &unsuitable_fhs)
{
	//对深度进行控制
	std::unordered_set<OvmCeH> chs_less_than_min_depth;
	int depth = 0;
	auto curr_front_fhs = interface_fhs;
	while (depth++ < min_depth){
		//获得curr_front_fhs上的点
		std::unordered_set<OvmVeH> curr_front_vhs;
		foreach (auto &fh, curr_front_fhs){
			auto heh_vec = mesh->face (fh).halfedges ();
			foreach (auto &heh, heh_vec)
				curr_front_vhs.insert (mesh->halfedge (heh).to_vertex ());
		}
		//由这些点得到所有的相邻六面体
		foreach (auto &vh, curr_front_vhs){
			JC::get_adj_hexas_around_vertex (mesh, vh, chs_less_than_min_depth);
		}
		//得到表面四边形面集
		curr_front_fhs.clear ();
		unsuitable_fhs.clear ();
		std::unordered_set<OvmFaH> bound_fhs;
		JC::collect_boundary_element (mesh, chs_less_than_min_depth, NULL, NULL, &bound_fhs);
		//剔除掉bound_fhs中在interface_fhs上以及在侧面表面上的四边形(其实就是在几何表面的四边形)
		foreach (auto &fh, bound_fhs){
			if (!mesh->is_boundary (fh))
				curr_front_fhs.insert (fh);
			else
				unsuitable_fhs.insert (fh);
		}
	}
}

void OneSimpleHandlerBase::optimize_diagonal_hexa_sets (std::unordered_set<OvmCeH> &diagonal_hexa_set1, 
	std::unordered_set<OvmCeH> &diagonal_hexa_set2)
{
	std::unordered_set<OvmEgH> intersect_path_set;
	JC::vector_to_unordered_set (intersect_path, intersect_path_set);
	JC::optimize_shrink_set (mesh, diagonal_hexa_set1, optimize_allowed_fhs, 
		diagonal_hexa_set2, intersect_path_set);
	JC::optimize_shrink_set (mesh, diagonal_hexa_set2, optimize_allowed_fhs, 
		diagonal_hexa_set1, intersect_path_set);
}

void OneSimpleHandlerBase::get_diagonal_quad_patches (std::unordered_set<OvmFaH> &mid_patch, 
	std::unordered_set<OvmFaH> &diagonal_patch1,
	std::unordered_set<OvmFaH> &diagonal_patch2)
{
	std::vector<std::set<OvmVeH> > vhs_on_patches;
	vhs_on_patches.resize (separated_fhs_patches.size ());
	for (int i = 0; i != separated_fhs_patches.size (); ++i){
		auto &patch = separated_fhs_patches[i];
		std::set<OvmVeH> vhs_on_patch;
		foreach (auto &fh, patch){
			auto heh_vec = mesh->face (fh).halfedges ();
			foreach (auto &heh, heh_vec)
				vhs_on_patches[i].insert (mesh->halfedge (heh).to_vertex ());
		}
	}

	for (int i = 0; i != separated_fhs_patches.size (); ++i){
		auto &patch1 = separated_fhs_patches[i];
		auto &vhs_on_patch1 = vhs_on_patches[i];
		std::vector<int> diagonal_adj_patch_indices;
		std::vector<OvmVeH> comm_vhs_vec;
		for (int j = 0; j != separated_fhs_patches.size (); ++j){
			if (j == i) continue;

			auto &patch2 = separated_fhs_patches[j];
			auto &vhs_on_patch2 = vhs_on_patches[j];

			std::vector<OvmVeH> comm_vhs;
			std::set_intersection (vhs_on_patch1.begin (), vhs_on_patch1.end (), 
				vhs_on_patch2.begin (), vhs_on_patch2.end (), std::back_inserter (comm_vhs));

			if (comm_vhs.size () == 1){
				diagonal_adj_patch_indices.push_back (j);
				comm_vhs_vec.push_back (comm_vhs.front ());
			}
		}
		if (diagonal_adj_patch_indices.size () == 2){
			mid_patch = patch1;
			if (comm_vhs_vec.front () == int_vhs.front ()){
				diagonal_patch1 = separated_fhs_patches[diagonal_adj_patch_indices.front ()];
				diagonal_patch2 = separated_fhs_patches[diagonal_adj_patch_indices.back ()];
			}else{
				diagonal_patch1 = separated_fhs_patches[diagonal_adj_patch_indices.back ()];
				diagonal_patch2 = separated_fhs_patches[diagonal_adj_patch_indices.front ()];
			}

			return;
		}
	}
}

bool OneSimpleHandlerBase::search_constrained_cross_hexa_sets (CrossFaceSetPairs &cross_face_set_pairs)
{
	CrossCrossPath cc_path (mesh, cross_face_set_pairs, int_vhs.front (), int_vhs.back ());
	cross_hexas.clear ();
	intersect_path.clear ();
	cc_path.shortest_path (intersect_path, cross_hexas);
	if (intersect_path.empty ()){
		last_err_str = QObject::tr ("路径搜索失败！");
		return false;
	}

	return true;
}

void OneSimpleHandlerBase::get_cross_faces_pairs (std::unordered_set<OvmFaH> &diagonal_patch1, 
	std::unordered_set<OvmFaH> &diagonal_patch2, 
	CrossFaceSetPairs &cross_face_set_pairs)
{
	std::vector<OvmEgH> ehs1, ehs2;
	std::vector<std::unordered_set<OvmFaH> > fhs_sets1, fhs_sets2;
	JC::get_cw_boundary_edges_faces_around_vertex (mesh, int_vhs.front (), (*vh_adj_ehs_mapping)[int_vhs.front ()],
		ehs1, fhs_sets1);
	JC::get_ccw_boundary_edges_faces_around_vertex (mesh, int_vhs.back (), (*vh_adj_ehs_mapping)[int_vhs.back ()],
		ehs2, fhs_sets2);

	auto front_set1 = fhs_sets1.front ();
	while (!JC::intersects (front_set1, diagonal_patch1)){
		fhs_sets1.erase (fhs_sets1.begin ());
		fhs_sets1.push_back (front_set1);
		front_set1 = fhs_sets1.front ();
	}

	auto front_set2 = fhs_sets2.front ();
	while (!JC::intersects (front_set2, diagonal_patch2)){
		fhs_sets2.erase (fhs_sets2.begin ());
		fhs_sets2.push_back (front_set2);
		front_set2 = fhs_sets2.front ();
	}

	for (int i = 0; i != 4; ++i){
		std::pair<std::unordered_set<OvmFaH>, std::unordered_set<OvmFaH> > one_pair;
		one_pair.first = fhs_sets1[i]; one_pair.second = fhs_sets2[i];
		cross_face_set_pairs.push_back (one_pair);
	}
}

void OneSimpleHandlerBase::optimize_surface_quad_set (std::unordered_set<OvmFaH> &fhs_need_optimize, std::unordered_set<OvmFaH> barrier_fhs)
{
	//barrier_fhs是表面四边形上不能够加入到fhs_need_optimize中的四边形，
	//由于仅是点相邻也不行，所以在做优化之前，需要将barrier_fhs扩大一圈，
	//即将barrier_fhs边界上点相邻的四边形也放入到barrier_fhs中
	std::unordered_set<OvmEgH> boundary_ehs_of_barrier_fhs;
	std::unordered_set<OvmVeH> boundary_vhs_of_barrier_fhs;

	foreach (auto &fh, barrier_fhs){
		auto heh_vec = mesh->face (fh).halfedges ();
		foreach (auto &heh, heh_vec){
			auto eh = mesh->edge_handle (heh);
			if (JC::contains (boundary_ehs_of_barrier_fhs, eh))
				boundary_ehs_of_barrier_fhs.erase (eh);
			else
				boundary_ehs_of_barrier_fhs.insert (eh);
		}
	}

	foreach (auto &eh, boundary_ehs_of_barrier_fhs){
		auto vh1 = mesh->edge (eh).from_vertex (),
			vh2 = mesh->edge (eh).to_vertex ();
		boundary_vhs_of_barrier_fhs.insert (vh1);
		boundary_vhs_of_barrier_fhs.insert (vh2);
	}

	foreach (auto &vh, boundary_vhs_of_barrier_fhs){
		std::unordered_set<OvmFaH> bound_adj_fhs;
		JC::get_adj_boundary_faces_around_vertex (mesh, vh, bound_adj_fhs);
		foreach (auto &fh, bound_adj_fhs)
			barrier_fhs.insert (fh);
	}

	//curr_optimize_allowed_fhs中存储当前可以用于优化的四边形面集，
	//search_allowed_fhs则存储可以用于搜索的四边形面集。
	//由于fhs_need_optimize可能并不连通，所以需要先将他变成连通的
	std::unordered_set<OvmFaH> curr_optimize_allowed_fhs, search_allowed_fhs = fhs_need_optimize;
	foreach (auto &fh, optimize_allowed_fhs){
		if (!JC::contains (barrier_fhs, fh)){
			search_allowed_fhs.insert (fh);
			if (!JC::contains (fhs_need_optimize, fh))
				curr_optimize_allowed_fhs.insert (fh);
		}
	}

	std::vector<std::unordered_set<OvmFaH> > quad_patches;
	JC::get_separate_fhs_patches (mesh, fhs_need_optimize, std::unordered_set<OvmEgH> (), quad_patches);

	std::unordered_set<OvmFaH> optimized_quad_patch;//最终优化过的合并的四边形面集
	//如果只是一块区域，那么不需要把它们连接起来
	if (quad_patches.size () == 1){
		JC::optimize_quad_patch (mesh, quad_patches[0], curr_optimize_allowed_fhs);
		foreach (auto &fh, quad_patches[0])
			optimized_quad_patch.insert (fh);
	}
	//如果是多块区域，那么就需要将它们先连接起来
	//当前仅处理最多两块的情况
	else{
		assert (quad_patches.size () == 2);
		auto seed_fh1 = *(quad_patches.front ().begin ()),
			seed_fh2 = *(quad_patches.back ().begin ());
		FaceFacePath ff_path (mesh, seed_fh1, seed_fh2, &search_allowed_fhs);
		std::deque<OvmFaH> path;
		ff_path.shortest_path (path);
		//如果无法找到一条路径将两块连接起来的话，那么两块四边形面集需要各自优化
		if (path.empty ()){
			if (JC::intersects (quad_patches[0], optimize_allowed_fhs)){
				JC::optimize_quad_patch (mesh, quad_patches[0], curr_optimize_allowed_fhs);
			}
			if (JC::intersects (quad_patches[1], optimize_allowed_fhs)){
				JC::optimize_quad_patch (mesh, quad_patches[1], curr_optimize_allowed_fhs);
			}
			foreach (auto &quad_patch, quad_patches){
				foreach (auto &fh, quad_patch)
					optimized_quad_patch.insert (fh);
			}
		}else{
			std::unordered_set<OvmFaH> merged_quad_patch = quad_patches.front ();
			foreach (auto &fh, quad_patches.back ())
				merged_quad_patch.insert (fh);
			foreach (auto &fh, path)
				merged_quad_patch.insert (fh);

			JC::optimize_quad_patch (mesh, merged_quad_patch, curr_optimize_allowed_fhs);
			foreach (auto &fh, merged_quad_patch)
				optimized_quad_patch.insert (fh);
		}
	}//end if (quad_patches.size () == 2){
	fhs_need_optimize = optimized_quad_patch;
}

void OneSimpleHandlerBase::optimize_diagonal_hexa_set_boundary_quads (std::unordered_set<OvmCeH> &diagonal_hexa_set1, 
	std::unordered_set<OvmCeH> &diagonal_hexa_set2)
{
	std::vector<std::unordered_set<OvmFaH> > surface_quad_set;
	surface_quad_set.resize (2);

	foreach (auto &ch, diagonal_hexa_set1){
		auto hfh_vec = mesh->cell (ch).halffaces ();
		foreach (auto &hfh, hfh_vec){
			auto fh = mesh->face_handle (hfh);
			if (mesh->is_boundary (fh))
				surface_quad_set[0].insert (fh);
		}
	}

	foreach (auto &ch, diagonal_hexa_set2){
		auto hfh_vec = mesh->cell (ch).halffaces ();
		foreach (auto &hfh, hfh_vec){
			auto fh = mesh->face_handle (hfh);
			if (mesh->is_boundary (fh))
				surface_quad_set[1].insert (fh);
		}
	}


	for (int i = 0; i != 2; ++i){
		auto &this_quad_set = surface_quad_set[i];
		auto &that_quad_set = surface_quad_set[1 - i];
		optimize_surface_quad_set (that_quad_set, that_quad_set);
	}

	JC::get_direct_adjacent_hexas (mesh, surface_quad_set[0], diagonal_hexa_set1);
	JC::get_direct_adjacent_hexas (mesh, surface_quad_set[1], diagonal_hexa_set2);
}

void OneSimpleHandlerBase::depth_control (std::vector<std::unordered_set<OvmFaH> > &new_separated_fhs_patches, 
	std::vector<std::unordered_set<OvmCeH> > &hexa_sets_within_depth)
{

}
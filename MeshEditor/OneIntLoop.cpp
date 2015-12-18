#include "stdafx.h"
#include "OneIntLoop.h"
#include "MeshDijkstra.h"
#include "FaceFacePath.h"

void depth_control (VolumeMesh *mesh, int min_depth, std::unordered_set<OvmFaH> &interface_fhs, 
	std::unordered_set<OvmFaH> &bound_fhs_less_than_min_depth)
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
		bound_fhs_less_than_min_depth.clear ();
		std::unordered_set<OvmFaH> bound_fhs;
		JC::collect_boundary_element (mesh, chs_less_than_min_depth, NULL, NULL, &bound_fhs);
		//剔除掉bound_fhs中在interface_fhs上以及在侧面表面上的四边形(其实就是在几何表面的四边形)
		foreach (auto &fh, bound_fhs){
			if (!mesh->is_boundary (fh))
				curr_front_fhs.insert (fh);
			else
				bound_fhs_less_than_min_depth.insert (fh);
		}
	}

}

bool oil_auto_generation (OneIntLoopInput &oil_input, OneIntLoopOutput &oil_output)
{
	OneIntLoopIntermedia oil_mid;
	if (!_analyze_input_edges (oil_input, oil_mid)){
		oil_output.last_err_str = oil_mid.last_err_str;
		return false;
	}

	oil_mid.has_been_analyzed = true;

	//如果loop不完整，则首先需要进行补全
	if (!oil_mid.is_complete){

		if (!oil_get_interface_quad_patch (oil_input, oil_mid)){
			oil_output.last_err_str = oil_mid.last_err_str;
			return false;
		}

		std::unordered_set<OvmFaH> bound_fhs_less_than_min_depth;
		depth_control (oil_input.mesh, oil_input.min_depth, oil_input.interface_fhs, bound_fhs_less_than_min_depth);

		if (oil_input.modify_allowed_fhs.empty ()){
			for (auto bf_it = oil_input.mesh->bf_iter (); bf_it; ++bf_it){
				auto fh = *bf_it;
				if (!JC::contains (bound_fhs_less_than_min_depth, fh) && !JC::contains (oil_input.constant_fhs, fh))
					oil_input.modify_allowed_fhs.insert (fh);
			}
		}

		oil_get_allowed_vhs_and_ehs (oil_input, oil_mid);

		//若loop不自相交，或者已经有两个交点了，则以普通方式进行补全
		if (!oil_mid.is_intersect || oil_mid.int_vhs.size () == 2){

		}
		//如果loop内部自相交，则需要用补全自相交的方式进行补全
		else{
			if (!oil_get_cross_path_hexa_sets (oil_input, oil_mid)){
				oil_output.last_err_str = oil_mid.last_err_str;
				return false;
			}

			if (!oil_get_diagonal_hexa_sets (oil_input, oil_mid)){
				oil_output.last_err_str = oil_mid.last_err_str;
				return false;
			}

			oil_input.optimize_allowed_fhs = oil_input.modify_allowed_fhs;

			if (!oil_optimize_diagonal_hexa_sets (oil_input, oil_mid)){
				oil_output.last_err_str = oil_mid.last_err_str;
				return false;
			}
			//oil_output.diagonal_hexa_sets = oil_mid.diagonal_hexa_sets;
			std::unordered_set<OvmFaH> bound_fhs;

			JC::collect_boundary_element (oil_input.mesh, oil_mid.diagonal_hexa_sets.front (), NULL, NULL, &bound_fhs);
			JC::collect_boundary_element (oil_input.mesh, oil_mid.diagonal_hexa_sets.back (), NULL, NULL, &bound_fhs);
			foreach (OvmFaH fh, bound_fhs){
				if (oil_input.mesh->is_boundary (fh))
					continue;
				oil_mid.fhs_for_inflation.insert (fh);
			}

			oil_output.fhs_for_inflation = oil_mid.fhs_for_inflation;
		}
	}
	return true;
}

bool oil_auto_deduce_constant_quad_set (OneIntLoopInput &oil_input, OneIntLoopIntermedia &oil_mid)
{
	if (oil_input.face_of_constant == NULL) return false;
	auto mesh = oil_input.mesh;

	assert (mesh->vertex_property_exists<unsigned int> ("entityptr"));
	auto V_ENTITY_PTR = mesh->request_vertex_property<unsigned int>("entityptr");

	oil_input.constant_fhs.clear ();

	for (auto f_it = mesh->bf_iter (); f_it; f_it++)
	{
		auto adj_vhs = JC::get_adj_vertices_around_face (mesh, *f_it);
		foreach (auto &vh, adj_vhs){
			ENTITY *entity = (ENTITY*)(V_ENTITY_PTR[vh]);
			if (entity == oil_input.face_of_constant){
				oil_input.constant_fhs.insert (*f_it);
				break;
			}
		}
	}

	return true;
}

bool oil_auto_get_hexa_set (OneIntLoopInput &oil_input, OneIntLoopIntermedia &oil_mid)
{
	if (!_analyze_input_edges (oil_input, oil_mid)){
		return false;
	}

	oil_mid.has_been_analyzed = true;

	//如果loop不完整，则首先需要进行补全
	if (!oil_mid.is_complete){

		if (!oil_get_interface_quad_patch (oil_input, oil_mid)){
			return false;
		}

		std::unordered_set<OvmFaH> bound_fhs_less_than_min_depth;
		depth_control (oil_input.mesh, oil_input.min_depth, oil_input.interface_fhs, bound_fhs_less_than_min_depth);

		if (oil_input.modify_allowed_fhs.empty ()){
			for (auto bf_it = oil_input.mesh->bf_iter (); bf_it; ++bf_it){
				auto fh = *bf_it;
				if (!JC::contains (oil_input.constant_fhs, fh)){
					if (!JC::contains (oil_input.interface_fhs, fh))
						oil_input.optimize_allowed_fhs.insert (fh);
					if (!JC::contains (bound_fhs_less_than_min_depth, fh))
						oil_input.modify_allowed_fhs.insert (fh);
				}
			}
		}

		oil_get_allowed_vhs_and_ehs (oil_input, oil_mid);

		//若loop不自相交，或者已经有两个交点了，则以普通方式进行补全
		if (!oil_mid.is_intersect){

		}
		else if (oil_mid.int_vhs.size () == 2){
			
		}
		//如果loop内部自相交，则需要用补全自相交的方式进行补全
		else{
			if (!oil_get_cross_path_hexa_sets (oil_input, oil_mid)){
				return false;
			}

			if (!oil_get_diagonal_hexa_sets (oil_input, oil_mid)){
				return false;
			}

			if (!oil_optimize_surface_quad_set (oil_input, oil_mid)){
				return false;
			}

			for (int i = 0; i != 2; ++i){
				std::unordered_set<OvmCeH> hexas;
				JC::get_direct_adjacent_hexas (oil_input.mesh, oil_mid.optimized_quad_patches[i], hexas);
				foreach (auto &ch, hexas)
					oil_mid.diagonal_hexa_sets[i].insert (ch);
			}

			if (!oil_optimize_diagonal_hexa_sets (oil_input, oil_mid)){
				return false;
			}
		}
	}
	return true;
}

bool oil_auto_get_quad_set (OneIntLoopInput &oil_input, OneIntLoopIntermedia &oil_mid)
{
	std::unordered_set<OvmFaH> bound_fhs;

	JC::collect_boundary_element (oil_input.mesh, oil_mid.diagonal_hexa_sets.front (), NULL, NULL, &bound_fhs);
	JC::collect_boundary_element (oil_input.mesh, oil_mid.diagonal_hexa_sets.back (), NULL, NULL, &bound_fhs);
	foreach (OvmFaH fh, bound_fhs){
		if (oil_input.mesh->is_boundary (fh))
			continue;
		oil_mid.fhs_for_inflation.insert (fh);
	}

	return true;
}

bool oil_analyze_input_ehs (OneIntLoopInput &oil_input, OneIntLoopIntermedia &oil_mid)
{
	if (!_analyze_input_edges (oil_input, oil_mid)){
		return false;
	}
	oil_mid.has_been_analyzed = true;
	return true;
}

bool oil_deduce_interface_fhs (OneIntLoopInput &oil_input, OneIntLoopIntermedia &oil_mid)
{
	auto mesh = oil_input.mesh;
	auto V_ENTITY_PTR = mesh->request_vertex_property<unsigned int> ("entityptr");
	oil_mid.face_of_input_ehs = NULL;
	foreach (OvmEgH eh, oil_input.input_ehs){
		OvmVeH vh1 = mesh->edge (eh).from_vertex (),
			vh2 = mesh->edge (eh).to_vertex ();
		ENTITY *entity1 = (ENTITY*)(V_ENTITY_PTR[vh1]),
			*entity2 = (ENTITY*)(V_ENTITY_PTR[vh2]);
		if (is_FACE (entity1)){
			oil_mid.face_of_input_ehs = (FACE*)entity1;
			break;
		}else if (is_FACE (entity2)){
			oil_mid.face_of_input_ehs = (FACE*)entity2;
			break;
		}
	}
	if (oil_mid.face_of_input_ehs == NULL){
		oil_mid.last_err_str = QObject::tr ("找不到输入网格边所在的几何面！无法推导！");
		return false;
	}

	for (auto f_it = mesh->faces_begin (); f_it != mesh->faces_end (); ++f_it){
		if (!mesh->is_boundary (*f_it))
			continue;
		auto vhs_on_fh = JC::get_adj_vertices_around_face (mesh, *f_it);
		FACE *f = JC::get_associated_geometry_face_of_boundary_fh (mesh, *f_it);
		assert (f);
		if (f == oil_mid.face_of_input_ehs)
			oil_mid.deduced_interface_fhs.insert (*f_it);
	}
	return true;
}

bool oil_get_interface_quad_patch (OneIntLoopInput &oil_input, OneIntLoopIntermedia &oil_mid)
{
	if (oil_input.interface_fhs.empty () && !oil_input.auto_deduce_interface_fhs){
		oil_mid.last_err_str = QObject::tr ("设置错误！请设置贴合面面集，或者设置自动推导贴合面四面形面集！");
		return false;
	}

	std::unordered_set<OvmFaH> interface_fhs;
	if (!oil_input.interface_fhs.empty ())
		interface_fhs = oil_input.interface_fhs;
	else{
		if (oil_mid.deduced_interface_fhs.empty ())
			oil_deduce_interface_fhs (oil_input, oil_mid);
		interface_fhs = oil_mid.deduced_interface_fhs;
		oil_input.interface_fhs = interface_fhs;
	}

	std::vector<std::unordered_set<OvmFaH> > separated_fhs_patches;
	JC::get_separate_fhs_patches (oil_input.mesh, interface_fhs, oil_input.input_ehs, separated_fhs_patches);

	if (separated_fhs_patches.size () != 3){
		oil_mid.last_err_str = QObject::tr ("该loop无法获得三块四边形集合！");
		return false;
	}

	//三块区域中，有一块和另两块都通过边相邻，这块称为背景区域，
	//另两块通过唯一的一个点相邻，称为主要区域
	std::set<std::set<int> > cross_candidiates;
	std::set<int> pair_candidiate;
	pair_candidiate.insert (0); pair_candidiate.insert (1);
	cross_candidiates.insert (pair_candidiate);
	pair_candidiate.clear ();
	pair_candidiate.insert (0); pair_candidiate.insert (2);
	cross_candidiates.insert (pair_candidiate);
	pair_candidiate.clear ();
	pair_candidiate.insert (1); pair_candidiate.insert (2);
	cross_candidiates.insert (pair_candidiate);
	foreach (OvmEgH eh, oil_input.input_ehs){
		auto adj_fhs = JC::get_adj_faces_around_edge (oil_input.mesh, eh);
		std::set<int> real_pair;
		foreach (OvmFaH fh, adj_fhs){
			for (int i = 0; i != 3; ++i){
				if (JC::contains (separated_fhs_patches[i], fh))
					real_pair.insert (i);
			}
		}
		assert (real_pair.size () == 2);
		cross_candidiates.erase (real_pair);
	}

	assert (cross_candidiates.size () == 1);
	std::vector<int> cross_idx;
	JC::set_to_vector (*(cross_candidiates.begin ()), cross_idx);
	int background_idx = -1;
	for (int i = 0; i != 3; ++i){
		if (!JC::contains (cross_idx, i)){
			background_idx = i; break;
		}
	}
	oil_mid.diagonal_fhs_patchs.resize (2);
	oil_mid.diagonal_fhs_patchs[0] = separated_fhs_patches[cross_idx.front ()];
	oil_mid.diagonal_fhs_patchs[1] = separated_fhs_patches[cross_idx.back ()];
	oil_mid.background_fhs_patch = separated_fhs_patches[background_idx];
	return true;
}

bool oil_deduce_allowed_fhs (OneIntLoopInput &oil_input, OneIntLoopIntermedia &oil_mid)
{
	if (!oil_input.auto_deduce_allowed_fhs){
		oil_mid.last_err_str = QObject::tr ("不需要推导可修改四边形面集！");
		return false;
	}
	if (oil_mid.has_been_analyzed == false){
		oil_mid.last_err_str = QObject::tr ("输入还没有经过初步分析！");
		return false;
	}

	VolumeMesh *mesh = oil_input.mesh;
	auto &modify_allowed_fhs = oil_input.modify_allowed_fhs;
	auto V_ENTITY_PTR = mesh->request_vertex_property<unsigned int> ("entityptr");

	std::set<FACE*> vhs_on_faces, adj_faces;
	foreach (OvmEgH eh, oil_input.input_ehs){
		OvmVeH vh1 = mesh->edge (eh).from_vertex (),
			vh2 = mesh->edge (eh).to_vertex ();
		ENTITY *entity1 = (ENTITY*)(V_ENTITY_PTR[vh1]),
			*entity2 = (ENTITY*)(V_ENTITY_PTR[vh2]);
		if (is_FACE (entity1))
			vhs_on_faces.insert ((FACE*)entity1);
		else if (is_EDGE (entity1)){
			ENTITY_LIST tmp_faces;
			api_get_faces (entity1, tmp_faces);
			for (int i = 0; i != tmp_faces.count (); ++i)
				adj_faces.insert ((FACE*)(tmp_faces[i]));
		}

		if (is_FACE (entity2))
			vhs_on_faces.insert ((FACE*)entity2);
		else if (is_EDGE (entity2)){
			ENTITY_LIST tmp_faces;
			api_get_faces (entity2, tmp_faces);
			for (int i = 0; i != tmp_faces.count (); ++i)
				adj_faces.insert ((FACE*)(tmp_faces[i]));
		}
	}//end foreach (OvmEgH eh, oil_input.input_ehs){...
	std::vector<FACE *> side_adj_faces;
	foreach (FACE *f, adj_faces){
		if (!JC::contains (vhs_on_faces, f))
			side_adj_faces.push_back (f);
	}

	for (auto f_it = mesh->faces_begin (); f_it != mesh->faces_end (); ++f_it){
		if (!mesh->is_boundary (*f_it)) continue;
		auto adj_vhs = JC::get_adj_vertices_around_face (mesh, *f_it);
		FACE *on_f = NULL;
		foreach (OvmVeH test_vh, adj_vhs){
			ENTITY *entity = (ENTITY*)(V_ENTITY_PTR[test_vh]);
			if (is_FACE (entity)) on_f = (FACE *)on_f;
		}
		assert (on_f);
		if (JC::contains (side_adj_faces, on_f))
			modify_allowed_fhs.insert (*f_it);
	}
	oil_mid.deduced_allowed_fhs = modify_allowed_fhs;
	return true;
}

bool oil_get_allowed_vhs_and_ehs (OneIntLoopInput &oil_input, OneIntLoopIntermedia &oil_mid)
{
	if (oil_mid.has_been_analyzed == false){
		oil_mid.last_err_str = QObject::tr ("输入还没有经过初步分析！");
		return false;
	}
	
	return _get_allowed_vhs_and_ehs (oil_input, oil_mid);
}

bool oil_get_cross_path_hexa_sets (OneIntLoopInput &oil_input, OneIntLoopIntermedia &oil_mid)
{
	if (oil_mid.has_been_analyzed == false){
		oil_mid.last_err_str = QObject::tr ("输入还没有经过初步分析！");
		return false;
	}
	return _get_cross_hexa_sets (oil_input, oil_mid);
}

bool oil_get_diagonal_hexa_sets (OneIntLoopInput &oil_input, OneIntLoopIntermedia &oil_mid)
{
	if (oil_mid.diagonal_fhs_patchs.empty ()){
		oil_mid.last_err_str = QObject::tr ("还没有获得对角四边形面集！");
		return false;
	}
	oil_mid.diagonal_hexa_sets.resize (2);
	for (int i = 0; i != 2; ++i){
		JC::get_direct_adjacent_hexas (oil_input.mesh, oil_mid.diagonal_fhs_patchs[i], oil_mid.diagonal_hexa_sets[i]);
	}

	for (int i = 0; i != 4; ++i){
		if (JC::intersects (oil_mid.cross_hexas[i], oil_mid.diagonal_hexa_sets[0])){
			foreach (OvmCeH ch, oil_mid.cross_hexas[i])
				oil_mid.diagonal_hexa_sets[0].insert (ch);
			continue;
		}
		if (JC::intersects (oil_mid.cross_hexas[i], oil_mid.diagonal_hexa_sets[1])){
			foreach (OvmCeH ch, oil_mid.cross_hexas[i])
				oil_mid.diagonal_hexa_sets[1].insert (ch);
			continue;
		}
	}
	return true;
}

bool oil_optimize_surface_quad_set (OneIntLoopInput &oil_input, OneIntLoopIntermedia &oil_mid)
{
	auto mesh = oil_input.mesh;
	std::vector<std::unordered_set<OvmFaH> > surface_quad_set;
	surface_quad_set.resize (2);
	oil_mid.optimized_quad_patches.clear ();

	for (int i = 0; i != 2; ++i){
		foreach (auto &ch, oil_mid.diagonal_hexa_sets[i]){
			auto hfh_vec = mesh->cell (ch).halffaces ();
			foreach (auto &hfh, hfh_vec){
				auto fh = mesh->face_handle (hfh);
				if (mesh->is_boundary (fh))
					surface_quad_set[i].insert (fh);
			}
		}
	}

	for (int i = 0; i != 2; ++i){
		auto &this_quad_set = surface_quad_set[i];
		auto &that_quad_set = surface_quad_set[1 - i];
		std::unordered_set<OvmFaH> optimize_allowed_fhs, search_allowed_fhs = this_quad_set, barrier_fhs = that_quad_set;
		std::unordered_set<OvmEgH> boundary_ehs_of_that_quad_set;
		std::unordered_set<OvmVeH> boundary_vhs_of_that_quad_set;

		foreach (auto &fh, that_quad_set){
			auto heh_vec = mesh->face (fh).halfedges ();
			foreach (auto &heh, heh_vec){
				auto eh = mesh->edge_handle (heh);
				if (JC::contains (boundary_ehs_of_that_quad_set, eh))
					boundary_ehs_of_that_quad_set.erase (eh);
				else
					boundary_ehs_of_that_quad_set.insert (eh);
			}
		}

		foreach (auto &eh, boundary_ehs_of_that_quad_set){
			auto vh1 = mesh->edge (eh).from_vertex (),
				vh2 = mesh->edge (eh).to_vertex ();
			boundary_vhs_of_that_quad_set.insert (vh1);
			boundary_vhs_of_that_quad_set.insert (vh2);
		}

		foreach (auto &vh, boundary_vhs_of_that_quad_set){
			std::unordered_set<OvmFaH> bound_adj_fhs;
			JC::get_adj_boundary_faces_around_vertex (mesh, vh, bound_adj_fhs);
			foreach (auto &fh, bound_adj_fhs)
				barrier_fhs.insert (fh);
		}

		foreach (auto &fh, oil_input.optimize_allowed_fhs){
			if (!JC::contains (barrier_fhs, fh)){
				search_allowed_fhs.insert (fh);
				if (!JC::contains (this_quad_set, fh))
					optimize_allowed_fhs.insert (fh);
			}
		}

		std::vector<std::unordered_set<OvmFaH> > quad_patches;
		JC::get_separate_fhs_patches (mesh, this_quad_set, std::unordered_set<OvmEgH> (), quad_patches);

		std::unordered_set<OvmFaH> optimized_quad_patch;//最终优化过的合并的四边形面集
		if (quad_patches.size () == 1){
			JC::optimize_quad_patch (mesh, quad_patches[0], optimize_allowed_fhs);
			foreach (auto &fh, quad_patches[0])
				optimized_quad_patch.insert (fh);
		}else{
			auto seed_fh1 = *(quad_patches.front ().begin ()),
				seed_fh2 = *(quad_patches.back ().begin ());
			FaceFacePath ff_path (mesh, seed_fh1, seed_fh2, &search_allowed_fhs);
			std::deque<OvmFaH> path;
			ff_path.shortest_path (path);
			//如果无法找到一条路径将两块连接起来的话，那么两块四边形面集需要各自优化
			if (path.empty ()){
				if (JC::intersects (quad_patches[0], oil_input.optimize_allowed_fhs)){
					JC::optimize_quad_patch (mesh, quad_patches[0], optimize_allowed_fhs);
				}
				if (JC::intersects (quad_patches[1], oil_input.optimize_allowed_fhs)){
					JC::optimize_quad_patch (mesh, quad_patches[1], optimize_allowed_fhs);
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

				JC::optimize_quad_patch (mesh, merged_quad_patch, optimize_allowed_fhs);
				foreach (auto &fh, merged_quad_patch)
					optimized_quad_patch.insert (fh);
			}
		}//end if (quad_patches.size () == 2){
		oil_mid.optimized_quad_patches.push_back (optimized_quad_patch);
	}
	return true;
}

bool oil_optimize_diagonal_hexa_sets (OneIntLoopInput &oil_input, OneIntLoopIntermedia &oil_mid)
{
	if (oil_mid.diagonal_hexa_sets.empty ()){
		oil_mid.last_err_str = QObject::tr ("还没有获得对角六面体集合！");
		return false;
	}
	if (oil_input.optimize_allowed_fhs.empty ()){
		oil_mid.last_err_str = QObject::tr ("还没有获得可用于优化的四边形面集合！");
		return false;
	}
	
	std::unordered_set<OvmEgH> intersect_path;
	JC::vector_to_unordered_set (oil_mid.intersect_path, intersect_path);
	JC::optimize_shrink_set (oil_input.mesh, oil_mid.diagonal_hexa_sets.front (), oil_input.optimize_allowed_fhs, 
		oil_mid.diagonal_hexa_sets.back (), intersect_path);
	JC::optimize_shrink_set (oil_input.mesh, oil_mid.diagonal_hexa_sets.back (), oil_input.optimize_allowed_fhs, 
		oil_mid.diagonal_hexa_sets.front (), intersect_path);
	return true;
}

bool _get_allowed_vhs_and_ehs (OneIntLoopInput &oil_input, OneIntLoopIntermedia &oil_mid)
{
	VolumeMesh *mesh = oil_input.mesh;
	auto &allowed_vhs = oil_mid.allowed_vhs;
	auto &allowed_ehs = oil_mid.allowed_ehs;
	auto &modify_allowed_fhs = oil_input.modify_allowed_fhs;

	//先把内部边和边界边统计出来。内部边是被两个面所共享的，而边界边只和一个面相邻
	std::set<OvmEgH> ehs_count;
	foreach (OvmFaH fh, modify_allowed_fhs){
		auto heh_vec = mesh->face (fh).halfedges ();
		foreach (OvmHaEgH heh, heh_vec){
			OvmEgH eh = mesh->edge_handle (heh);
			if (JC::contains (ehs_count, eh)){
				ehs_count.erase (eh);
				allowed_ehs.insert (eh);
			}else{
				ehs_count.insert (eh);
			}
		}
	}

	//然后统计内部点和边界点。内部点只和内部边相邻，而边界点至少和一条边界边相邻
	foreach (OvmEgH eh, allowed_ehs){
		OvmVeH vh1 = mesh->edge (eh).from_vertex (),
			vh2 = mesh->edge (eh).to_vertex ();
		allowed_vhs.insert (vh1); allowed_vhs.insert (vh1);
	}

	//由于已经删掉了内部边，所以ehs_count只剩下边界边
	foreach (OvmEgH eh, ehs_count){
		OvmVeH vh1 = mesh->edge (eh).from_vertex (),
			vh2 = mesh->edge (eh).to_vertex ();
		allowed_vhs.erase (vh1); allowed_vhs.erase (vh1);
	}
	return true;
}

bool _analyze_input_edges (OneIntLoopInput &oil_input, OneIntLoopIntermedia &oil_mid)
{
	bool is_ehs_valid = true;
	VolumeMesh *mesh = oil_input.mesh;
	auto &input_ehs = oil_input.input_ehs;

	if (!mesh->vertex_property_exists<unsigned int> ("entityptr")){
			oil_mid.last_err_str = QObject::tr ("网格节点没有关联集合实体！");
			return false;
	}

	auto V_ENTITY_PTR = mesh->request_vertex_property<unsigned int> ("entityptr");

	//对输入边上的点相连接的边进行统计
	auto &vh_adj_ehs_mapping = oil_mid.vh_adj_ehs_mapping;
	auto &end_vhs = oil_mid.end_vhs;
	auto &int_vhs = oil_mid.int_vhs;
	vh_adj_ehs_mapping.clear ();
	end_vhs.clear (); int_vhs.clear ();

	foreach (OvmEgH eh, input_ehs){
		vh_adj_ehs_mapping[mesh->edge (eh).from_vertex ()].push_back (eh);
		vh_adj_ehs_mapping[mesh->edge (eh).to_vertex ()].push_back (eh);
	}
	for (auto it = vh_adj_ehs_mapping.begin (); it != vh_adj_ehs_mapping.end (); ++it)
	{
		//如果一个顶点只和一条边相连，则表示选中的边组合不是完整的
		if (it->second.size () == 1)
			end_vhs.insert (it->first);
		
		//如果一个顶点和三条边或者超过四条边相连，则表示输入的边集合是无效的
		else if (it->second.size () == 3 || it->second.size () > 4){
			oil_mid.last_err_str = QString (QObject::tr("%1号顶点的度数为%2，不符合要求！")).arg (it->first.idx ()).arg (it->second.size ());
			is_ehs_valid = false;
			break;
		}
		//如果一个顶点连接的网格边是4的话，需要根据边的围绕顺序将其排序，以便后面恢复loop结构
		else if (it->second.size () == 4){
			int_vhs.insert (it->first);
			std::vector<OvmEgH> sorted_ehs;
			sorted_ehs.push_back (it->second.front ());
			OvmHaEgH iterator_heh = mesh->halfedge_handle (it->second.front (), 0);
			if (mesh->halfedge (iterator_heh).from_vertex () != it->first)
				iterator_heh = mesh->opposite_halfedge_handle (iterator_heh);
			auto fGetPrevBoundHeh = [&](OvmHaEgH heh)->OvmHaEgH{
				OvmHaFaH bound_hfh = mesh->InvalidHalfFaceHandle;
				for (auto hehf_it = mesh->hehf_iter (heh); hehf_it; ++hehf_it){
					if (mesh->is_boundary (*hehf_it)){
						bound_hfh = *hehf_it;
						break;
					}
				}
				assert (bound_hfh != mesh->InvalidHalfFaceHandle);
				return mesh->prev_halfedge_in_halfface (heh, bound_hfh);
			};
			iterator_heh = mesh->opposite_halfedge_handle (fGetPrevBoundHeh (iterator_heh));
			while (mesh->edge_handle (iterator_heh) != it->second.front ()){
				OvmEgH test_eh = mesh->edge_handle (iterator_heh);
				if (std::find (it->second.begin (), it->second.end (), test_eh) != it->second.end ())
					sorted_ehs.push_back (test_eh);
				iterator_heh = mesh->opposite_halfedge_handle (fGetPrevBoundHeh (iterator_heh));
			}
			it->second = sorted_ehs;
		}//end else if (it->second.size () == 4){...
	}

	foreach (OvmVeH vh, end_vhs){
		ENTITY *entity = (ENTITY*)(V_ENTITY_PTR[vh]);
		if (is_VERTEX (entity)){
			oil_mid.last_err_str = QObject::tr ("网格点不能在几何点上！");
			return false;
		}
	}
	//如果int_vhs为空，则该输入网格边集合内部不自相交
	oil_mid.is_intersect = !int_vhs.empty ();
	
	//当前只能处理自交一次的情况，因此交点多于两个时，就不能够处理
	if (int_vhs.size () > 2){
		oil_mid.last_err_str = QObject::tr ("只能处理自交一次的loop！");
		return false;
	}else if (int_vhs.size () == 1){
		oil_mid.is_complete = false;
		oil_mid.int_vhs = int_vhs;
		oil_mid.existed_int_vh = *(int_vhs.begin ());
	}else{
		oil_mid.is_complete = end_vhs.empty ();
	}

	std::vector<std::vector<OvmEgH> > complete_loops;
	//下面将连续loop段落拿出来
	//fTrackThrough函数若返回有效的节点句柄，表示搜索到的是loop片段；
	//若返回无效的句柄，则表示搜索到的是完整的loop
	auto fTrackThrough = [&mesh, &vh_adj_ehs_mapping](OvmEgH start_eh, OvmVeH vh, std::vector<OvmEgH> &path)->OvmVeH{
		auto ehs = vh_adj_ehs_mapping[vh];
		OvmEgH next_eh = start_eh;
		while (true){
			if (ehs.size () == 1) return JC::get_other_vertex_on_edge (mesh, ehs.front (), vh);
			else if (ehs.size () == 2) next_eh = ehs.front () == next_eh? ehs.back ():ehs.front ();
			else next_eh = ehs[(std::find (ehs.begin (), ehs.end (), next_eh) - ehs.begin () + 2) % 4];

			if (next_eh == start_eh) return mesh->InvalidVertexHandle;
			path.push_back (next_eh);
			vh = JC::get_other_vertex_on_edge (mesh, next_eh, vh);
			ehs = vh_adj_ehs_mapping[vh];
		}
	};

	auto input_ehs_bk = input_ehs;
	while (!input_ehs_bk.empty ()){
		OvmEgH start_eh = JC::pop_begin_element (input_ehs_bk);
		std::vector<OvmEgH> ehs_sequence;
		ehs_sequence.push_back (start_eh);

		OvmVeH curr_end_vh = fTrackThrough (start_eh, mesh->edge (start_eh).from_vertex (), ehs_sequence);
		//如果是loop段，则还需要反向搜索，然后将两部分拼接起来
		if (curr_end_vh != mesh->InvalidVertexHandle){
			std::vector<OvmEgH> ehs_sequence_right;
			LoopSegment loop_seg;
			loop_seg.end_vh = curr_end_vh;

			curr_end_vh = fTrackThrough (start_eh, mesh->edge (start_eh).to_vertex (), ehs_sequence_right);

			loop_seg.start_vh = curr_end_vh;
			
			foreach (OvmEgH eh, ehs_sequence_right)
				ehs_sequence.insert (ehs_sequence.begin (), eh);

			loop_seg.seg_ehs = ehs_sequence;

			oil_mid.input_loop_segs.push_back (loop_seg);
		}
		//如果是完整的loop
		else complete_loops.push_back (ehs_sequence);

		foreach (OvmEgH eh, ehs_sequence)
			input_ehs_bk.erase (eh);
	}

	//对一些当前不能处理的情况进行判断
	if (!complete_loops.empty ()){
		if (complete_loops.size () == 1){
			//当前对既有完整loop又有需要补全的loop这种情况无法处理
			if (!oil_mid.input_loop_segs.empty ()){
				oil_mid.last_err_str = QObject::tr("当前无法处理既有完整loop又有需要补全的loop这种情况！");
				is_ehs_valid = false;
			}
			else oil_mid.complete_loop = complete_loops.front ();
		}
		//对于多个完整loop现在也无法处理
		else{
			oil_mid.last_err_str = QObject::tr("当前无法处理多个完整loop这种情况！");
			is_ehs_valid = false;
		}
	}
		
	return is_ehs_valid;
}

bool _get_cross_hexa_sets (OneIntLoopInput &oil_inut, OneIntLoopIntermedia &oil_mid)
{
	VolumeMesh *mesh = oil_inut.mesh;
	std::vector<OvmEgH> ehs;
	std::vector<std::unordered_set<OvmFaH> > fhs_sets;
	auto cross_adj_ehs = oil_mid.vh_adj_ehs_mapping[oil_mid.existed_int_vh];
	JC::get_ccw_boundary_edges_faces_around_vertex (mesh, oil_mid.existed_int_vh, cross_adj_ehs, ehs, fhs_sets);

	//搜索最优的第二个相交点的位置
	OvmVeH new_int_vh = mesh->InvalidVertexHandle;
	MeshDijkstra dijkstra (mesh, oil_mid.existed_int_vh, &(oil_mid.allowed_vhs));
	dijkstra.shortest_path (oil_mid.intersect_path, new_int_vh);
	if (new_int_vh == mesh->InvalidVertexHandle){
		oil_mid.last_err_str = QObject::tr("搜索第二个交点失败！");
		return false;
	}

	//搜索内部沿着自交线的Cross分布
	CrossCrossPath cc_path (mesh, cross_adj_ehs, fhs_sets, oil_mid.existed_int_vh, new_int_vh);
	std::vector<OvmEgH> path;
	CCPSHConf cross_hexas;
	std::vector<std::pair<OvmEgH, OvmEgH> > edge_pairs;
	//由于是补全，所以在第二个交点位置Cross分布不确定，因此是unconstrained搜索
	if (!cc_path.unconstrained_shortest_path (path, cross_hexas, edge_pairs)){
		oil_mid.last_err_str = QObject::tr("搜索交叉路径失败！");
		return false;
	}
	oil_mid.intersect_path = path;
	oil_mid.cross_hexas = cross_hexas;
	oil_mid.edge_pairs = edge_pairs;

	return true;
}
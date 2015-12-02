#include "StdAfx.h"
#include "OneIntHandler.h"
#include "MeshDijkstra.h"
#include <iterator>

OneIntHandler::OneIntHandler(VolumeMesh *_mesh, std::unordered_set<OvmEgH> &_input_ehs, 
	FACE *_inter_face, std::unordered_set<OvmFaH> *_constant_fhs_ptr, 
	std::hash_map<OvmVeH, std::vector<OvmEgH> > *_vh_adj_ehs_mapping, std::vector<OvmVeH> &_int_vhs)
	: OneSimpleHandlerBase (_mesh, _input_ehs)
{
	inter_face = _inter_face;
	constant_fhs_ptr = _constant_fhs_ptr;
	vh_adj_ehs_mapping = _vh_adj_ehs_mapping;
	int_vhs = _int_vhs;
}


OneIntHandler::~OneIntHandler(void)
{
}

std::unordered_set<OvmCeH> OneIntHandler::get_hexa_set ()
{
	all_hexa_set.clear ();
	//获得interface上的网格面
	get_fhs_on_interface ();
	//利用input_ehs对fhs_on_interface进行分块儿
	JC::get_separate_fhs_patches (mesh, interface_fhs, input_ehs, separated_fhs_patches);

	if (separated_fhs_patches.size () != 3){
		last_err_str = QObject::tr ("该loop无法获得三块四边形集合！");
		return all_hexa_set;
	}

	//获得可以进行优化的面集，这部分面集是网格表面上除了不可边的以及贴合面上的四边形面
	get_optimize_allowed_fhs ();

	//获得贴合面上的两个对角四边形面集
	get_diagonal_quad_patches ();

	//获得可以作为第二个交点的点集
	get_intersect_allowed_vhs ();

	//运用路径搜索得到从第一个交点到第二个交点的路径，以及沿着这个路径的四个六面体集合
	search_unconstrained_cross_hexa_sets ();

	get_diagonal_hexa_sets ();

	//对表面四边形进行优化
	optimize_diagonal_hexa_set_boundary_quads (diagonal_hexa_set1, diagonal_hexa_set2);

	//对获得的两个六面体集合进行优化
	optimize_diagonal_hexa_sets (diagonal_hexa_set1, diagonal_hexa_set2);

	//获得可用于sheet Inflation的四边形面集
	all_hexa_set = diagonal_hexa_set1;
	foreach (auto &ch, diagonal_hexa_set2)
		all_hexa_set.insert (ch);

	//all_hexa_set = diagonal_hexa_set1;
	//foreach (auto &ch, diagonal_hexa_set2)
	//	all_hexa_set.insert (ch);

	//std::unordered_set<OvmCeH> barrier_chs;
	//for (int i = 0; i != 4; ++i){
	//	if (!JC::contains (diagonal_idx, i)){
	//		foreach (auto &ch, cross_hexas[i])
	//			barrier_chs.insert (ch);
	//	}
	//}

	//JC::get_direct_adjacent_hexas (mesh, interface_fhs, barrier_chs);

	//JC::get_direct_adjacent_hexas (mesh, *constant_fhs_ptr, barrier_chs);

	//JC::optimize_shrink_set_test (mesh, all_hexa_set, barrier_chs);

	return all_hexa_set;
}

std::unordered_set<OvmFaH> OneIntHandler::get_inflation_fhs ()
{
	inflation_fhs.clear ();
	get_inflation_fhs_from_hexa_set (all_hexa_set);
	return inflation_fhs;
}

DualSheet * OneIntHandler::inflate_new_sheet ()
{
	std::unordered_set<OvmEgH> intersect_path_set;
	JC::vector_to_unordered_set (intersect_path, intersect_path_set);
	auto new_sheets = JC::one_simple_sheet_inflation (mesh, inflation_fhs, all_hexa_set, intersect_path_set);
	return new_sheets.front ();
}

void OneIntHandler::get_diagonal_quad_patches ()
{
	//三块区域中，有一块和另两块都通过边相邻，这块称为背景区域，
	//另两块通过唯一的一个点相邻，称为主要区域
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

	for (int i = 0; i != separated_fhs_patches.size () - 1; ++i){
		auto &patch1 = separated_fhs_patches[i];
		auto &vhs_on_patch1 = vhs_on_patches[i];

		for (int j = i + 1; j != separated_fhs_patches.size (); ++j){
			auto &patch2 = separated_fhs_patches[j];
			auto &vhs_on_patch2 = vhs_on_patches[j];

			std::vector<OvmVeH> comm_vhs;
			std::set_intersection (vhs_on_patch1.begin (), vhs_on_patch1.end (), 
				vhs_on_patch2.begin (), vhs_on_patch2.end (), std::back_inserter (comm_vhs));

			if (comm_vhs.size () == 1){
				diagonal_quad_patch1 = patch1; diagonal_quad_patch2 = patch2;
				return;
			}
		}//end for (int j = i + 1; j != separated_fhs_patches.size (); ++j){...
	}//end for (int i = 0; i != separated_fhs_patches.size () - 1; ++i){...
}

bool OneIntHandler::search_unconstrained_cross_hexa_sets ()
{
	std::vector<OvmEgH> ehs;
	std::vector<std::unordered_set<OvmFaH> > fhs_sets;
	start_int_vh = *(int_vhs.begin ());
	std::vector<OvmEgH> cross_adj_ehs = (*vh_adj_ehs_mapping)[start_int_vh];
	JC::get_ccw_boundary_edges_faces_around_vertex (mesh, start_int_vh, cross_adj_ehs, ehs, fhs_sets);

	//搜索最优的第二个相交点的位置
	end_int_vh = mesh->InvalidVertexHandle;
	MeshDijkstra dijkstra (mesh, start_int_vh, &intersect_allowed_vhs);
	dijkstra.shortest_path (intersect_path, end_int_vh);
	if (end_int_vh == mesh->InvalidVertexHandle){
		last_err_str = QObject::tr("搜索第二个交点失败！");
		return false;
	}

	//搜索内部沿着自交线的Cross分布
	CrossCrossPath cc_path (mesh, cross_adj_ehs, fhs_sets, start_int_vh, end_int_vh);
	cross_eh_pairs.clear ();
	//由于是补全，所以在第二个交点位置Cross分布不确定，因此是unconstrained搜索
	cross_hexas.clear ();
	if (!cc_path.unconstrained_shortest_path (intersect_path, cross_hexas, cross_eh_pairs)){
		last_err_str = QObject::tr("搜索交叉路径失败！");
		return false;
	}
	return true;
}

void OneIntHandler::get_diagonal_hexa_sets ()
{
	JC::get_direct_adjacent_hexas (mesh, diagonal_quad_patch1, diagonal_hexa_set1);
	JC::get_direct_adjacent_hexas (mesh, diagonal_quad_patch2, diagonal_hexa_set2);

	for (int i = 0; i != 4; ++i){
		if (JC::intersects (cross_hexas[i], diagonal_hexa_set1)){
			foreach (auto &ch, cross_hexas[i])
				diagonal_hexa_set1.insert (ch);
			diagonal_idx.insert (i);
			continue;
		}
		if (JC::intersects (cross_hexas[i], diagonal_hexa_set2)){
			foreach (auto &ch, cross_hexas[i])
				diagonal_hexa_set2.insert (ch);
			diagonal_idx.insert (i);
		}
	}
}
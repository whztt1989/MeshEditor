#include "StdAfx.h"
#include "NoIntIncompHandler.h"


NoIntIncompHandler::NoIntIncompHandler(VolumeMesh *_mesh, std::unordered_set<OvmEgH> &_input_ehs,
	FACE *_inter_face, std::unordered_set<OvmFaH> *_constant_fhs_ptr)
	: OneSimpleHandlerBase (_mesh, _input_ehs)
{
	inter_face = _inter_face;
	constant_fhs_ptr = _constant_fhs_ptr;
}


NoIntIncompHandler::~NoIntIncompHandler(void)
{
}

std::unordered_set<OvmCeH> NoIntIncompHandler::get_hexa_set ()
{
	hexa_set.clear ();
	//获得interface上的网格面
	get_fhs_on_interface ();
	//利用input_ehs对fhs_on_interface进行分块儿
	JC::get_separate_fhs_patches (mesh, interface_fhs, input_ehs, separated_fhs_patches);
	//如果separated_fhs_patches只有一块，那说明input_ehs不足以将指定的四边形面集切分开来，因此输入是无效的
	if (separated_fhs_patches.size () == 1){
		last_err_str = QObject::tr("该输入边不能将贴合面切分！");
		return hexa_set;
	}

	//获得可以进行优化的面集，这部分面集是网格表面上除了不可边的以及贴合面上的四边形面
	get_optimize_allowed_fhs ();

	//如果输入不含交点，那么需要从separated_fhs_patches中找出一个和所有的input_ehs相邻的patch，然后进行pillowing
	auto best_patch = get_best_patch ();
	if (best_patch.empty ()){
		last_err_str = QObject::tr("找不到和所有输入边相邻的面集！");
		return hexa_set;
	}

	////获得和best_patch直接相邻的六面体集合，这部分六面体集合是shrink set中必须包含的
	//JC::get_direct_adjacent_hexas (mesh, best_patch, hexa_set);

	////获得这部分六面体集合的表面四边形，然后进行优化
	//foreach (auto &ch, hexa_set){
	//	auto hfh_vec = mesh->cell (ch).halffaces ();
	//	foreach (auto &hfh, hfh_vec){
	//		auto fh = mesh->face_handle (hfh);
	//		if (mesh->is_boundary (fh) && !JC::contains (best_patch, fh)) 
	//			best_patch.insert (fh);
	//	}
	//}
	//JC::optimize_quad_patch (mesh, best_patch, optimize_allowed_fhs);

	////再次获得新的六面体集合
	//hexa_set.clear ();
	//JC::get_direct_adjacent_hexas (mesh, best_patch, hexa_set);

	////优化这个六面体集合
	//JC::optimize_shrink_set (mesh, hexa_set, optimize_allowed_fhs);
	hexa_set.clear ();
	std::unordered_set<OvmCeH> rest_hexa_set;
	depth_control (best_patch, hexa_set, rest_hexa_set);
	JC::get_direct_adjacent_hexas (mesh, *constant_fhs_ptr, rest_hexa_set);

	JC::optimize_shrink_set_test (mesh, hexa_set, rest_hexa_set);

	return hexa_set;
}

void NoIntIncompHandler::depth_control (std::unordered_set<OvmFaH> &best_patch, std::unordered_set<OvmCeH> &min_hexa_set,
	std::unordered_set<OvmCeH> &rest_hexa_set)
{
	std::unordered_set<OvmFaH> rest_patch;
	foreach (auto &fh, interface_fhs){
		if (!JC::contains (best_patch, fh))
			rest_patch.insert (fh);
	}

	int depth = 0;
	auto fGetVertices = [&] (std::unordered_set<OvmFaH> &fhs, 
		std::unordered_set<OvmVeH> &inner_vhs, std::unordered_set<OvmVeH> &all_vhs){
			std::hash_map<OvmEgH, int> ehs_count;
			foreach (auto &fh, fhs){
				auto heh_vec = mesh->face (fh).halfedges ();
				foreach (auto &heh, heh_vec){
					auto eh = mesh->edge_handle (heh);
					ehs_count[eh]++;
				}
			}
			std::unordered_set<OvmVeH> bound_vhs;
			foreach (auto &p, ehs_count){
				auto vh1 = mesh->edge (p.first).from_vertex (),
					vh2 = mesh->edge (p.first).to_vertex ();
				all_vhs.insert (vh1); all_vhs.insert (vh2);
				if (p.second == 1){
					bound_vhs.insert (vh1); bound_vhs.insert (vh2);
				}
			}
			foreach (auto &vh, all_vhs){
				if (!JC::contains (bound_vhs, vh))
					inner_vhs.insert (vh);
			}
	};
	auto cur_patch = best_patch;
	while (depth++ < min_depth){
		std::unordered_set<OvmVeH> inner_vhs, all_vhs;
		fGetVertices (cur_patch, inner_vhs, all_vhs);
		foreach (auto &fh, cur_patch){
			auto hfh = mesh->halfface_handle (fh, 0);
			auto ch = mesh->incident_cell (hfh);
			if (ch != mesh->InvalidCellHandle)
				min_hexa_set.insert (ch);
			hfh =  mesh->halfface_handle (fh, 1);
			ch = mesh->incident_cell (hfh);
			if (ch != mesh->InvalidCellHandle)
				min_hexa_set.insert (ch);
		}
		foreach (auto &vh, inner_vhs){
			std::unordered_set<OvmCeH> adj_chs;
			JC::get_adj_hexas_around_vertex (mesh, vh, adj_chs);
			foreach (auto &ch, adj_chs)
				min_hexa_set.insert (ch);
		}

		inner_vhs.clear (); all_vhs.clear ();
		fGetVertices (rest_patch, inner_vhs, all_vhs);
		foreach (auto &vh, all_vhs){
			std::unordered_set<OvmCeH> adj_chs;
			JC::get_adj_hexas_around_vertex (mesh, vh, adj_chs);
			foreach (auto &ch, adj_chs){
				if (!JC::contains (min_hexa_set, ch))
					rest_hexa_set.insert (ch);
			}
		}

		std::unordered_set<OvmFaH> tmp_fhs, bound_fhs1, bound_fhs2;
		JC::collect_boundary_element (mesh, min_hexa_set, NULL, NULL, &tmp_fhs);
		foreach (auto &fh, tmp_fhs){
			if (!mesh->is_boundary (fh)) bound_fhs1.insert (fh);
		}
		tmp_fhs.clear ();
		JC::collect_boundary_element (mesh, rest_hexa_set, NULL, NULL, &tmp_fhs);
		foreach (auto &fh, tmp_fhs){
			if (!mesh->is_boundary (fh)) bound_fhs2.insert (fh);
		}
		cur_patch.clear ();
		foreach (auto &fh, bound_fhs1){
			if (!JC::contains (bound_fhs2, fh)) cur_patch.insert (fh);
		}
		rest_patch.clear ();
		foreach (auto &fh, bound_fhs2){
			if (!JC::contains (bound_fhs1, fh)) rest_patch.insert (fh);
		}
	}
}

std::unordered_set<OvmFaH> NoIntIncompHandler::get_inflation_fhs ()
{
	inflation_fhs.clear ();
	//获得可用于sheet Inflation的四边形面集
	get_inflation_fhs_from_hexa_set (hexa_set);
	return inflation_fhs;
}

DualSheet * NoIntIncompHandler::inflate_new_sheet ()
{
	auto new_sheets = JC::one_simple_sheet_inflation (mesh, inflation_fhs, hexa_set);
	return new_sheets.front ();
}

std::unordered_set<OvmFaH> NoIntIncompHandler::get_best_patch ()
{
	std::unordered_set<OvmFaH> best_patch;
	int patch_fhs_count = std::numeric_limits<int>::max ();
	foreach (auto &patch, separated_fhs_patches){
		bool is_patch_ok = true;
		foreach (auto &eh, input_ehs){
			std::unordered_set<OvmFaH> adj_fhs;
			JC::get_adj_boundary_faces_around_edge (mesh, eh, adj_fhs);
			if (!JC::intersects (adj_fhs, patch)){
				is_patch_ok = false; break;
			}
		}
		if (is_patch_ok){
			if (patch.size () < patch_fhs_count){
				best_patch = patch; patch_fhs_count = patch.size ();
			}
		}
	}
	return best_patch;
}
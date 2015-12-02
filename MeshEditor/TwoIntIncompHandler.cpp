#include "StdAfx.h"
#include "TwoIntIncompHandler.h"


TwoIntIncompHandler::TwoIntIncompHandler(VolumeMesh *_mesh, std::unordered_set<OvmEgH> &_input_ehs, 
	FACE *_inter_face, std::unordered_set<OvmFaH> *_constant_fhs_ptr, 
	std::hash_map<OvmVeH, std::vector<OvmEgH> > *_vh_adj_ehs_mapping, std::vector<OvmVeH> &_int_vhs)
	: OneSimpleHandlerBase (_mesh, _input_ehs)
{
	inter_face = _inter_face;
	constant_fhs_ptr = _constant_fhs_ptr;
	vh_adj_ehs_mapping = _vh_adj_ehs_mapping;
	int_vhs = _int_vhs;
}


TwoIntIncompHandler::~TwoIntIncompHandler(void)
{
}

std::unordered_set<OvmCeH> TwoIntIncompHandler::get_hexa_set ()
{
	hexa_set.clear ();
	//获得interface上的网格面
	get_fhs_on_interface ();
	//利用input_ehs对fhs_on_interface进行分块儿
	JC::get_separate_fhs_patches (mesh, interface_fhs, input_ehs, separated_fhs_patches);
	//如果separated_fhs_patches只有一块，那说明input_ehs不足以将指定的四边形面集切分开来，因此输入是无效的
	if (separated_fhs_patches.size () == 4){
		last_err_str = QObject::tr("该输入边不能将贴合面切分！");
		return hexa_set;
	}

	//获得可以进行优化的面集，这部分面集是网格表面上除了不可边的以及贴合面上的四边形面
	get_optimize_allowed_fhs ();

	//获得三块四边形区域，其中mid_patch为中间区域，另两块儿分别与它对角相邻，同时patch1和int_vhs[0]相邻，patch2和int_vhs[1]相邻
	std::unordered_set<OvmFaH> mid_patch, diagonal_quad_patch1, diagonal_quad_patch2;
	get_diagonal_quad_patches (mid_patch, diagonal_quad_patch1, diagonal_quad_patch2);
	if (mid_patch.empty () || diagonal_quad_patch1.empty () || diagonal_quad_patch2.empty ()){
		last_err_str = QObject::tr ("该loop无法获得有效的对角四边形集合！");
		return hexa_set;
	}

	//获得两组六面体集合
	std::unordered_set<OvmCeH> mid_hexa_set, diagonal_hexa_set;
	JC::get_direct_adjacent_hexas (mesh, mid_patch, mid_hexa_set);
	JC::get_direct_adjacent_hexas (mesh, diagonal_quad_patch1, diagonal_hexa_set);
	JC::get_direct_adjacent_hexas (mesh, diagonal_quad_patch2, diagonal_hexa_set);

	//获得两个交叉点附近面集合的映射对
	CrossFaceSetPairs cross_face_set_pairs;
	get_cross_faces_pairs (diagonal_quad_patch1, diagonal_quad_patch2, cross_face_set_pairs);

	//运用路径搜索得到从第一个交点到第二个交点的路径，以及沿着这个路径的四个六面体集合
	search_constrained_cross_hexa_sets (cross_face_set_pairs);

	//将搜索到的交叉路径上的六面体集合通过判断是否和上面找到的两个集合相邻来把他们加入到上面的两个六面体集合中
	foreach (auto &one_hexa_set, cross_hexas){
		if (JC::intersects (one_hexa_set, mid_hexa_set)){
			foreach (auto &ch, one_hexa_set)
				mid_hexa_set.insert (ch);
			continue;
		}
		if (JC::intersects (one_hexa_set, diagonal_hexa_set)){
			foreach (auto &ch, one_hexa_set)
				diagonal_hexa_set.insert (ch);
			continue;
		}
	}//end foreach (auto &hexa_set, cross_hexas){...

	//对表面四边形进行优化
	optimize_diagonal_hexa_set_boundary_quads (mid_hexa_set, diagonal_hexa_set);

	//对获得的两个六面体集合进行优化
	optimize_diagonal_hexa_sets (mid_hexa_set, diagonal_hexa_set);

	//获得可用于sheet Inflation的四边形面集
	hexa_set = mid_hexa_set;
	foreach (auto &ch, diagonal_hexa_set)
		hexa_set.insert (ch);

	return hexa_set;
}

std::unordered_set<OvmFaH> TwoIntIncompHandler::get_inflation_fhs ()
{
	inflation_fhs.clear ();
	get_inflation_fhs_from_hexa_set (hexa_set);
	return inflation_fhs;
}

DualSheet * TwoIntIncompHandler::inflate_new_sheet ()
{
	std::unordered_set<OvmEgH> intersect_path_set;
	JC::vector_to_unordered_set (intersect_path, intersect_path_set);
	auto new_sheets = JC::one_simple_sheet_inflation (mesh, inflation_fhs, hexa_set, intersect_path_set);
	return new_sheets.front ();
}
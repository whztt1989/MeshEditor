#include "StdAfx.h"
#include "TwoIntCompHandler.h"
#include "hoopsview.h"
#include "mesh_min_cut.h"

TwoIntCompHandler::TwoIntCompHandler(VolumeMesh *_mesh, std::unordered_set<OvmEgH> &_input_ehs, 
	std::hash_map<OvmVeH, std::vector<OvmEgH> > *_vh_adj_ehs_mapping, std::vector<OvmVeH> &_int_vhs)
	: OneSimpleHandlerBase (_mesh, _input_ehs)
{
	vh_adj_ehs_mapping = _vh_adj_ehs_mapping;
	int_vhs = _int_vhs;
}


TwoIntCompHandler::~TwoIntCompHandler(void)
{
}

std::unordered_set<OvmCeH> TwoIntCompHandler::get_hexa_set ()
{
	hexa_set.clear ();
	get_fhs_on_boundary ();
	//利用input_ehs对fhs_on_interface进行分块儿
	JC::get_separate_fhs_patches (mesh, boundary_fhs, input_ehs, separated_fhs_patches);

	if (separated_fhs_patches.size () != 4){
		last_err_str = QObject::tr ("在有两个自交点的情况下，该loop无法获得四块四边形集合！");
		return hexa_set;
	}

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

	////获得两个交叉点附近面集合的映射对
	//CrossFaceSetPairs cross_face_set_pairs;
	//get_cross_faces_pairs (diagonal_quad_patch1, diagonal_quad_patch2, cross_face_set_pairs);

	////运用路径搜索得到从第一个交点到第二个交点的路径，以及沿着这个路径的四个六面体集合
	//search_constrained_cross_hexa_sets (cross_face_set_pairs);
	
	//char *colors[] = {"yellow", "blue", "pink","green"};

	//for (int i = 0; i != 4; ++i){
	//	auto &one_hexa_set = cross_hexas[i];
	//	auto group = new VolumeMeshElementGroup (mesh, "tmp", "for paper drawing");
	//	group->chs = one_hexa_set;

	//	MeshRenderOptions render_options;
	//	render_options.cell_color = colors[i];
	//	hoopsview->render_mesh_group (group, render_options);
	//}

	//return hexa_set;
	//auto group = new VolumeMeshElementGroup (mesh, "tmp", "for paper drawing");
	//foreach (auto &eh, intersect_path)
	//	group->ehs.insert (eh);

	//MeshRenderOptions render_options;
	//render_options.edge_color = "green";
	//render_options.edge_width = 8;
	//hoopsview->render_mesh_group (group, render_options);

	//将搜索到的交叉路径上的六面体集合通过判断是否和上面找到的两个集合相邻来把他们加入到上面的两个六面体集合中
	int mid_idx = -1, dia_idx = -1;
	std::unordered_set<OvmCeH> other_diag_hexa;
	for (int i = 0; i != 4; ++i){
		auto &one_hexa_set = cross_hexas[i];
		if (JC::intersects (one_hexa_set, mid_hexa_set)){
			foreach (auto &ch, one_hexa_set)
				mid_hexa_set.insert (ch);
			mid_idx = i;
			continue;
		}
		if (JC::intersects (one_hexa_set, diagonal_hexa_set)){
			foreach (auto &ch, one_hexa_set)
				diagonal_hexa_set.insert (ch);
			dia_idx = i;
			continue;
		}
		foreach (auto &ch, one_hexa_set)
			other_diag_hexa.insert (ch);
	}//end foreach (auto &hexa_set, cross_hexas){...

	//if (i==2){
	std::unordered_set<OvmFaH> rest_bnd_fhs;
	for (auto bf_it = mesh->bf_iter (); bf_it; ++bf_it){
		auto fh = *bf_it;
		if (!JC::contains (mid_patch, fh))
			rest_bnd_fhs.insert (fh);
	}
	std::unordered_set<OvmCeH> rest_chs;
	foreach (auto &ch, other_diag_hexa)
		rest_chs.insert (ch);
	JC::get_direct_adjacent_hexas (mesh, rest_bnd_fhs, rest_chs);
	foreach (auto &ch, diagonal_hexa_set)
		rest_chs.insert (ch);

	auto cut_fhs = get_volume_mesh_min_cut (mesh, mid_hexa_set, rest_chs);
	//获得切分后的两块
	auto seed_ch = *(mid_hexa_set.begin ());
	std::queue<OvmCeH> spread_set;
	std::unordered_set<OvmCeH> visied_chs, adj_chs;
	visied_chs.insert (seed_ch);
	auto hfhs = mesh->cell (seed_ch).halffaces ();
	foreach (auto hfh, hfhs){
		auto fh = mesh->face_handle (hfh);
		if (JC::contains (cut_fhs, fh)) continue;
		auto oppo_hfh = mesh->opposite_halfface_handle (hfh);
		auto inci_ch = mesh->incident_cell (oppo_hfh);
		if (inci_ch != mesh->InvalidCellHandle){
			visied_chs.insert (inci_ch);
			mid_hexa_set.insert (inci_ch);
			spread_set.push (inci_ch);
		}
	}

	while (!spread_set.empty ()){
		auto front_ch = spread_set.front ();
		spread_set.pop ();
		hfhs = mesh->cell (front_ch).halffaces ();
		foreach (auto hfh, hfhs){
			auto fh = mesh->face_handle (hfh);
			if (JC::contains (cut_fhs, fh)) continue;
			auto oppo_hfh = mesh->opposite_halfface_handle (hfh);
			auto inci_ch = mesh->incident_cell (oppo_hfh);
			if (inci_ch == mesh->InvalidCellHandle) continue;
			if (JC::contains (visied_chs, inci_ch)) continue;

			visied_chs.insert (inci_ch);
			mid_hexa_set.insert (inci_ch);
			spread_set.push (inci_ch);
		}
	}

	rest_bnd_fhs.clear ();
	for (auto bf_it = mesh->bf_iter (); bf_it; ++bf_it){
		auto fh = *bf_it;
		if (!JC::contains (diagonal_quad_patch1, fh) &&
			!JC::contains (diagonal_quad_patch2, fh))
			rest_bnd_fhs.insert (fh);
	}

	rest_chs.clear ();
	foreach (auto &ch, other_diag_hexa)
		rest_chs.insert (ch);

	JC::get_direct_adjacent_hexas (mesh, rest_bnd_fhs, rest_chs);
	foreach (auto &ch, mid_hexa_set)
		rest_chs.insert (ch);

	//auto cut_fhs2 = get_volume_mesh_min_cut (mesh, diagonal_hexa_set, rest_chs);

	auto group = new VolumeMeshElementGroup (mesh, "tmp", "hexa set 1");

	group->fhs = cut_fhs;
	//group->chs = mid_hexa_set;
//	foreach (auto &fh, cut_fhs2)
//		group->fhs.insert (fh);

	MeshRenderOptions render_options1;
	render_options1.face_color = "red";
	hoopsview->render_mesh_group (group, render_options1);
	hoopsview->show_mesh_faces (false);
	return hexa_set;

	//对获得的两个六面体集合进行优化
	optimize_diagonal_hexa_sets (mid_hexa_set, diagonal_hexa_set);

	//auto tmp_group = JC::load_mesh_element_group ("diag-hex");
	//assert (tmp_group);
	//char *colors[] = {"yellow", "blue", "pink","green"};
	//auto group = new VolumeMeshElementGroup (mesh, "tmp", "hexa set 1");
	//foreach (auto &ch, mid_hexa_set){
	//	if (JC::contains (tmp_group->chs, ch))
	//		group->chs.insert (ch);
	//}
	//MeshRenderOptions render_options1;
	//render_options1.cell_color = colors[mid_idx];
	//hoopsview->render_mesh_group (group, render_options1);

	//group = new VolumeMeshElementGroup (mesh, "tmp", "hexa set 2");
	//foreach (auto &ch, diagonal_hexa_set){
	//	if (JC::contains (tmp_group->chs, ch))
	//		group->chs.insert (ch);
	//}
	//render_options1.cell_color = colors[dia_idx];
	//hoopsview->render_mesh_group (group, render_options1);

	//return hexa_set;

	//获得可用于sheet Inflation的四边形面集
	hexa_set = mid_hexa_set;
	foreach (auto &ch, diagonal_hexa_set)
		hexa_set.insert (ch);
	return hexa_set;
}

std::unordered_set<OvmFaH> TwoIntCompHandler::get_inflation_fhs ()
{
	inflation_fhs.clear ();
	//get_inflation_fhs_from_hexa_set (hexa_set);
	//return inflation_fhs;

	get_fhs_on_boundary ();
	//利用input_ehs对fhs_on_interface进行分块儿
	separated_fhs_patches.clear ();
	JC::get_separate_fhs_patches (mesh, boundary_fhs, input_ehs, separated_fhs_patches);

	if (separated_fhs_patches.size () < 4){
		last_err_str = QObject::tr ("在有两个自交点的情况下，该loop无法获得至少四块四边形集合！");
		return inflation_fhs;
	}

	//获得三块四边形区域，其中mid_patch为中间区域，另两块儿分别与它对角相邻，同时patch1和int_vhs[0]相邻，patch2和int_vhs[1]相邻
	std::unordered_set<OvmFaH> mid_patch, diagonal_quad_patch1, diagonal_quad_patch2;
	get_diagonal_quad_patches (mid_patch, diagonal_quad_patch1, diagonal_quad_patch2);
	if (mid_patch.empty () || diagonal_quad_patch1.empty () || diagonal_quad_patch2.empty ()){
		last_err_str = QObject::tr ("该loop无法获得有效的对角四边形集合！");
		return inflation_fhs;
	}

	//获得两组六面体集合
	std::unordered_set<OvmCeH> mid_hexa_set, diagonal_hexa_set;
	JC::get_direct_adjacent_hexas (mesh, mid_patch, mid_hexa_set);
	JC::get_direct_adjacent_hexas (mesh, diagonal_quad_patch1, diagonal_hexa_set);
	JC::get_direct_adjacent_hexas (mesh, diagonal_quad_patch2, diagonal_hexa_set);

	//将搜索到的交叉路径上的六面体集合通过判断是否和上面找到的两个集合相邻来把他们加入到上面的两个六面体集合中
	int mid_idx = -1, dia_idx = -1;
	std::unordered_set<OvmCeH> other_diag_hexa;
	for (int i = 0; i != 4; ++i){
		auto &one_hexa_set = cross_hexas[i];
		if (JC::intersects (one_hexa_set, mid_hexa_set)){
			foreach (auto &ch, one_hexa_set)
				mid_hexa_set.insert (ch);
			mid_idx = i;
			continue;
		}
		if (JC::intersects (one_hexa_set, diagonal_hexa_set)){
			foreach (auto &ch, one_hexa_set)
				diagonal_hexa_set.insert (ch);
			dia_idx = i;
			continue;
		}
		foreach (auto &ch, one_hexa_set)
			other_diag_hexa.insert (ch);
	}//end foreach (auto &hexa_set, cross_hexas){...

	//if (i==2){
	std::unordered_set<OvmFaH> rest_bnd_fhs;
	for (auto bf_it = mesh->bf_iter (); bf_it; ++bf_it){
		auto fh = *bf_it;
		if (!JC::contains (mid_patch, fh))
			rest_bnd_fhs.insert (fh);
	}
	std::unordered_set<OvmCeH> rest_chs;
	foreach (auto &ch, other_diag_hexa)
		rest_chs.insert (ch);
	JC::get_direct_adjacent_hexas (mesh, rest_bnd_fhs, rest_chs);
	foreach (auto &ch, diagonal_hexa_set)
		rest_chs.insert (ch);

	auto cut_fhs = get_volume_mesh_min_cut (mesh, mid_hexa_set, rest_chs);

	inflation_fhs = cut_fhs;

	//获得切分后的两块
	auto seed_ch = *(mid_hexa_set.begin ());
	std::queue<OvmCeH> spread_set;
	std::unordered_set<OvmCeH> visied_chs, adj_chs;
	visied_chs.insert (seed_ch);
	auto hfhs = mesh->cell (seed_ch).halffaces ();
	foreach (auto hfh, hfhs){
		auto fh = mesh->face_handle (hfh);
		if (JC::contains (cut_fhs, fh)) continue;
		auto oppo_hfh = mesh->opposite_halfface_handle (hfh);
		auto inci_ch = mesh->incident_cell (oppo_hfh);
		if (inci_ch != mesh->InvalidCellHandle){
			visied_chs.insert (inci_ch);
			mid_hexa_set.insert (inci_ch);
			spread_set.push (inci_ch);
		}
	}

	while (!spread_set.empty ()){
		auto front_ch = spread_set.front ();
		spread_set.pop ();
		hfhs = mesh->cell (front_ch).halffaces ();
		foreach (auto hfh, hfhs){
			auto fh = mesh->face_handle (hfh);
			if (JC::contains (cut_fhs, fh)) continue;
			auto oppo_hfh = mesh->opposite_halfface_handle (hfh);
			auto inci_ch = mesh->incident_cell (oppo_hfh);
			if (inci_ch == mesh->InvalidCellHandle) continue;
			if (JC::contains (visied_chs, inci_ch)) continue;

			visied_chs.insert (inci_ch);
			mid_hexa_set.insert (inci_ch);
			spread_set.push (inci_ch);
		}
	}

	rest_bnd_fhs.clear ();
	for (auto bf_it = mesh->bf_iter (); bf_it; ++bf_it){
		auto fh = *bf_it;
		if (!JC::contains (diagonal_quad_patch1, fh) &&
			!JC::contains (diagonal_quad_patch2, fh))
			rest_bnd_fhs.insert (fh);
	}
	hexa_set.clear ();
	hexa_set = mid_hexa_set;

	rest_chs.clear ();
	foreach (auto &ch, other_diag_hexa)
		rest_chs.insert (ch);

	JC::get_direct_adjacent_hexas (mesh, rest_bnd_fhs, rest_chs);
	foreach (auto &ch, mid_hexa_set)
		rest_chs.insert (ch);
	auto cut_fhs2 = get_volume_mesh_min_cut (mesh, diagonal_hexa_set, rest_chs);

//	static int iiii = 0;
//	if (iiii == 1){
//		
//		inflation_fhs = cut_fhs2;

//	}else if (iiii == 2){
		foreach (auto &fh, cut_fhs2)
			inflation_fhs.insert (fh);
//	}


	foreach (auto &ch, diagonal_hexa_set){
		hexa_set.insert (ch);
	}
//	iiii++;
	//auto group = new VolumeMeshElementGroup (mesh, "qs", "hexa set");
	//group->chs = hexa_set;

	//MeshRenderOptions render_options; 
	//render_options.cell_color = "pink";
	//hoopsview->render_mesh_group (group, render_options);
	return inflation_fhs;
}

DualSheet * TwoIntCompHandler::inflate_new_sheet ()
{
	std::unordered_set<OvmEgH> intersect_path_set;
	JC::vector_to_unordered_set (intersect_path, intersect_path_set);
	auto new_sheets = JC::one_simple_sheet_inflation (mesh, inflation_fhs, hexa_set, intersect_path_set);
	return new_sheets.front ();
}
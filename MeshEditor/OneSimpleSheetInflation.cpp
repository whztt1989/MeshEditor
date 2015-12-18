#include "stdafx.h"
#include "OneSimpleSheetInflation.h"
#include <limits>
#include <algorithm>
#include <iterator>
#include "MeshDijkstra.h"
#include "FaceFacePath.h"
#include "NoIntCompHandler.h"
//#include "NoIntIncompHandler.h"
//#include "OneIntHandler.h"
#include "TwoIntCompHandler.h"
//#include "TwoIntIncompHandler.h"
#include "VertexVertexBoundaryPath.h"

OneSimpleSheetInflationHandler::OneSimpleSheetInflationHandler (VolumeMesh *_mesh, BODY *_body, HoopsView *_hoopsview)
	: mesh (_mesh), body (_body), hoopsview (_hoopsview)
{
	inter_face = NULL;
	base_handler = NULL;
	min_depth = 1;

}

OneSimpleSheetInflationHandler::~OneSimpleSheetInflationHandler()
{
	if (base_handler)
		delete base_handler;
}

void OneSimpleSheetInflationHandler::init ()
{
	last_err_str = "";
	end_vhs.clear (); int_vhs.clear ();
	constant_fhs.clear ();
	inflation_fhs.clear ();
	interface_fhs.clear ();
	passed_analyzed = false;
	input_ehs.clear ();
	min_depth = 1;
}

std::unordered_set<OvmCeH> OneSimpleSheetInflationHandler::get_hexa_set ()
{	
	if (!passed_analyzed){
		last_err_str = QObject::tr ("还未经过分析！请先进行输入边分析！");
		return std::unordered_set<OvmCeH> ();
	}
	return base_handler->get_hexa_set ();;
}

std::unordered_set<OvmEgH> OneSimpleSheetInflationHandler::complete_loop ()
{
	std::unordered_set<OvmEgH> completed_loop;
	
	return completed_loop;
}

std::unordered_set<OvmFaH> OneSimpleSheetInflationHandler::get_inflation_fhs ()
{
	inflation_fhs.clear ();
	auto inflation_fhs = base_handler->get_inflation_fhs ();
	if (inflation_fhs.empty ()){
		last_err_str = base_handler->get_last_err ();
		delete base_handler;
	}
	return inflation_fhs;}

DualSheet * OneSimpleSheetInflationHandler::sheet_inflation ()
{
	if (!base_handler) return NULL;
	//下面进行sheet生成
	DualSheet *new_sheet = base_handler->inflate_new_sheet ();
	delete base_handler;
	return new_sheet;
}

void OneSimpleSheetInflationHandler::reprocess ()
{
	auto V_PREV_HANDLE = mesh->request_vertex_property<OvmVeH> ("prevhandle");
	for (auto v_it = mesh->vertices_begin (); v_it != mesh->vertices_end (); ++v_it)
		V_PREV_HANDLE[*v_it] = *v_it;

	auto E_PREV_HANDLE = mesh->request_edge_property<OvmEgH> ("prevhandle");
	for (auto e_it = mesh->edges_begin (); e_it != mesh->edges_end (); ++e_it)
		E_PREV_HANDLE[*e_it] = *e_it;

	auto F_PREV_HANDLE = mesh->request_face_property<OvmFaH> ("prevhandle");
	for (auto c_it = mesh->faces_begin (); c_it != mesh->faces_end (); ++c_it)
		F_PREV_HANDLE[*c_it] = *c_it;

	auto C_PREV_HANDLE = mesh->request_cell_property<OvmCeH> ("prevhandle");
	for (auto c_it = mesh->cells_begin (); c_it != mesh->cells_end (); ++c_it)
		C_PREV_HANDLE[*c_it] = *c_it;
}

bool OneSimpleSheetInflationHandler::analyze_input_edges (std::unordered_set<OvmEgH> &_input_ehs)
{
	input_ehs = _input_ehs;
	if (!mesh->vertex_property_exists<unsigned int> ("entityptr")){
		last_err_str = QObject::tr ("网格节点没有关联集合实体！");
		return false;
	}

	auto V_ENTITY_PTR = mesh->request_vertex_property<unsigned int> ("entityptr");

	//对输入边上的点相连接的边进行统计
	vh_adj_ehs_mapping.clear ();

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
			last_err_str = QString (QObject::tr("%1号顶点的度数为%2，不符合要求！")).arg (it->first.idx ()).arg (it->second.size ());
			return false;
		}
		//如果一个顶点连接的网格边是4的话，需要根据边的围绕顺序将其排序，以便后面恢复loop结构
		else if (it->second.size () == 4){
			int_vhs.push_back (it->first);
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

	//foreach (OvmVeH vh, end_vhs){
	//	ENTITY *entity = (ENTITY*)(V_ENTITY_PTR[vh]);
	//	if (is_VERTEX (entity)){
	//		last_err_str = QObject::tr ("网格点不能在几何点上！");
	//		return false;
	//	}
	//}

	//当前只能处理自交一次的情况，因此交点多于两个时，就不能够处理
	if (int_vhs.size () > 2){
		last_err_str = QObject::tr ("只能处理自交一次的loop！");
		return false;
	}

	//下面将连续loop段落拿出来
	//fTrackThrough函数若返回有效的节点句柄，表示搜索到的是loop片段；
	//若返回无效的句柄，则表示搜索到的是完整的loop
	auto fTrackThrough = [&](OvmEgH start_eh, OvmVeH vh, std::vector<OvmEgH> &path)->OvmVeH{
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

			curr_end_vh = fTrackThrough (start_eh, mesh->edge (start_eh).to_vertex (), ehs_sequence_right);

			foreach (OvmEgH eh, ehs_sequence_right)
				ehs_sequence.insert (ehs_sequence.begin (), eh);

			incomplete_loops.push_back (ehs_sequence);
		}
		//如果是完整的loop
		else complete_loops.push_back (ehs_sequence);

		foreach (OvmEgH eh, ehs_sequence)
			input_ehs_bk.erase (eh);
	}

	////对一些当前不能处理的情况进行判断
	//if (!complete_loops.empty ()){
	//	if (complete_loops.size () == 1){
	//		//当前对既有完整loop又有需要补全的loop这种情况无法处理
	//		if (!incomplete_loops.empty ()){
	//			last_err_str = QObject::tr("当前无法处理既有完整loop又有需要补全的loop这种情况！");
	//			return false;
	//		}
	//	}
	//	//对于多个完整loop现在也无法处理
	//	else{
	//		last_err_str = QObject::tr("当前无法处理多个完整loop这种情况！");
	//		return false;
	//	}
	//}

	passed_analyzed = true;

	if (base_handler) delete base_handler;

	//下面进行分类处理
	if (int_vhs.empty ()){
		//如果输入不含交点，分成两种情况，一种是完整的loop，另一种是需要补全的loop
		if (end_vhs.empty ()){
			base_handler = new NoIntCompHandler (mesh, input_ehs);
		}else{
			//base_handler = new NoIntIncompHandler (mesh, input_ehs, inter_face, &constant_fhs);
		}
	}else if (int_vhs.size () == 1){
		//如果只有一个交点，则需要找到第二个交点
		//base_handler = new OneIntHandler (mesh, input_ehs, inter_face, &constant_fhs, &vh_adj_ehs_mapping, int_vhs);
	}else{
		if (end_vhs.empty ()){
			base_handler = new TwoIntCompHandler (mesh, input_ehs, &vh_adj_ehs_mapping, int_vhs);
		}else{
			//base_handler = new TwoIntIncompHandler (mesh, input_ehs, inter_face, &constant_fhs, &vh_adj_ehs_mapping, int_vhs);
		}
	}

	if (base_handler){
		base_handler->hoopsview = hoopsview;
		base_handler->set_min_depth (min_depth);
		if (!interface_fhs.empty ())
			base_handler->set_inter_face_fhs (interface_fhs);
	}


	return true;
}

void OneSimpleSheetInflationHandler::set_constant_fhs (std::unordered_set<OvmFaH> &_constant_fhs)
{
	constant_fhs = _constant_fhs;
}

void OneSimpleSheetInflationHandler::set_minimum_depth (int _min_depth)
{
	min_depth = _min_depth;
	if (base_handler)
		base_handler->set_min_depth (min_depth);
}

void OneSimpleSheetInflationHandler::set_interface (FACE *_inter_face)
{
	inter_face = _inter_face;
}

void OneSimpleSheetInflationHandler::update_inter_face_fhs ()
{
	interface_fhs.clear ();
	JC::get_fhs_on_acis_face (mesh, inter_face, interface_fhs);
}

void OneSimpleSheetInflationHandler::update_const_face_fhs ()
{
	constant_fhs.clear ();
	JC::get_fhs_on_acis_face (mesh, const_face, constant_fhs);
}


std::vector<std::unordered_set<OvmFaH> > OneSimpleSheetInflationHandler::retrieve_modify_allowed_fhs_patches ()
{
	modify_allowed_fhs_patches.clear ();
	all_modify_allowed_fhs.clear ();
	for (auto bf_it = mesh->bf_iter (); bf_it; ++bf_it){
		if (JC::contains (interface_fhs, *bf_it)) continue;
		if (JC::contains (constant_fhs, *bf_it)) continue;
		all_modify_allowed_fhs.insert (*bf_it);
	}

	JC::get_separate_fhs_patches (mesh, all_modify_allowed_fhs, std::unordered_set<OvmEgH> (), modify_allowed_fhs_patches);

	return modify_allowed_fhs_patches;
}

//获得interface_fhs面集的边界边形成的圈
std::vector<LoopClassify> OneSimpleSheetInflationHandler::retrieve_interface_loops ()
{
	interface_loops.clear ();

	assert (!interface_fhs.empty ());
	std::unordered_set<OvmEgH> boundary_ehs;
	foreach (auto &fh, interface_fhs){
		auto heh_vec = mesh->face (fh).halfedges ();
		foreach (auto &heh, heh_vec){
			auto eh = mesh->edge_handle (heh);
			if (JC::contains (boundary_ehs, eh))
				boundary_ehs.erase (eh);
			else
				boundary_ehs.insert (eh);
		}
	}

	all_interface_fhs_boundary_ehs = boundary_ehs;

	auto fTrackThrough = [&](OvmEgH start_eh, OvmVeH vh, std::vector<OvmEgH> &path){
		OvmVeH other_vh = JC::get_other_vertex_on_edge (mesh, start_eh, vh);
		path.push_back (start_eh);
		while (vh != other_vh){
			OvmEgH next_eh = mesh->InvalidEdgeHandle;
			foreach (auto &eh, boundary_ehs){
				if (JC::contains (path, eh)) continue;
				auto vh1 = mesh->edge (eh).from_vertex (), vh2 = mesh->edge (eh).to_vertex ();
				if (vh1 == vh){
					vh = vh2; next_eh = eh;
					break;
				}else if (vh2 == vh){
					vh = vh1; next_eh = eh;
					break;
				}
			}
			assert (next_eh != mesh->InvalidEdgeHandle);
			path.push_back (next_eh);
		}
	};

	while (!boundary_ehs.empty ()){
		OvmEgH start_eh = *(boundary_ehs.begin ());
		std::vector<OvmEgH> one_loop;
		fTrackThrough (start_eh, mesh->edge (start_eh).to_vertex (), one_loop);

		LoopClassify one_loop_classify;
		one_loop_classify.loop = one_loop;
		interface_loops.push_back (one_loop_classify);

		foreach (auto &eh, one_loop)
			boundary_ehs.erase (eh);
	}

	assert (!modify_allowed_fhs_patches.empty ());
	for (int i = 0; i != interface_loops.size (); ++i){
		OvmEgH eh = interface_loops[i].loop.front ();
		std::unordered_set<OvmFaH> adj_bound_fhs;
		JC::get_adj_boundary_faces_around_edge (mesh, eh, adj_bound_fhs);
		OvmFaH fh_not_on_interface = mesh->InvalidFaceHandle;
		foreach (auto &fh, adj_bound_fhs){
			if (!JC::contains (interface_fhs, fh))
				fh_not_on_interface = fh;
		}
		assert (fh_not_on_interface != mesh->InvalidFaceHandle);
		bool has_found = false;
		for (int j = 0; j != modify_allowed_fhs_patches.size (); ++j){
			if (JC::contains (modify_allowed_fhs_patches[j], fh_not_on_interface)){
				interface_loops[i].fhs_patch_idx = j;
				has_found = true;
				break;
			}
		}
		assert (has_found);
	}


	return interface_loops;
}

std::vector<EndVhPair> OneSimpleSheetInflationHandler::classify_end_vhs ()
{
	end_vh_pairs.clear ();
	std::hash_map<int, std::vector<OvmVeH> > end_vhs_classifies_mapping;
	std::vector<std::unordered_set<OvmVeH> > vhs_on_loops;
	foreach (auto &one_loop, interface_loops){
		std::unordered_set<OvmVeH> vhs_on_one_loop;
		foreach (auto &eh, one_loop.loop){
			vhs_on_one_loop.insert (mesh->edge (eh).from_vertex ());
			vhs_on_one_loop.insert (mesh->edge (eh).to_vertex ());
		}
		vhs_on_loops.push_back (vhs_on_one_loop);
	}
	foreach (auto &vh, end_vhs){
		bool has_found = false;
		for (int i = 0; i != vhs_on_loops.size (); ++i){
			if (JC::contains (vhs_on_loops[i], vh)){
				end_vhs_classifies_mapping[i].push_back (vh);
				has_found = true;
				break;
			}
		}
		assert (has_found);
	}

	foreach (auto &p, end_vhs_classifies_mapping){
		if (p.second.size () != 2){
			assert (false);
		}
		EndVhPair one_end_vh_pair;
		one_end_vh_pair.loop_idx = p.first;
		one_end_vh_pair.vh1 = p.second.front ();
		one_end_vh_pair.vh2 = p.second.back ();
		end_vh_pairs.push_back (one_end_vh_pair);
	}
	return end_vh_pairs;
}

void OneSimpleSheetInflationHandler::evaluate_vhs_on_modify_allowed_fhs ()
{
	std::unordered_set<OvmVeH> all_vhs_on_interface_fhs_boundary;
	foreach (auto &eh, all_interface_fhs_boundary_ehs){
		all_vhs_on_interface_fhs_boundary.insert (mesh->edge (eh).from_vertex ());
		all_vhs_on_interface_fhs_boundary.insert (mesh->edge (eh).to_vertex ());
	}
	std::unordered_set<OvmVeH> all_vhs_on_modify_allowed_fhs;
	foreach (auto &fh, all_modify_allowed_fhs){
		auto adj_vhs = JC::get_adj_vertices_around_face (mesh, fh);
		foreach (auto &vh, adj_vhs){
			if (!JC::contains (all_vhs_on_interface_fhs_boundary, vh))
				all_vhs_on_modify_allowed_fhs.insert (vh);
		}
	}

	//该属性标识顶点离interface的距离
	//interface和constant faces内部的点，该属性都为-1,
	//interface边界上的点的属性为0
	if (!mesh->vertex_property_exists<int> ("node level")){
		auto tmp = mesh->request_vertex_property<int> ("node level", -1);
		mesh->set_persistent (tmp);
	}
	auto V_NODE_LEVEL = mesh->request_vertex_property<int> ("node level");
	std::unordered_set<OvmVeH> visited_vhs;
	
	int level = 0;
	auto curr_front = all_vhs_on_interface_fhs_boundary;
	while (!all_vhs_on_modify_allowed_fhs.empty ()){
		std::unordered_set<OvmVeH> new_front;
		foreach (auto &vh, curr_front){
			V_NODE_LEVEL[vh] = level;
			std::unordered_set<OvmVeH> adj_vhs;
			JC::get_adj_boundary_vertices_around_vertex (mesh, vh, adj_vhs);
			foreach (auto &adj_vh, adj_vhs){
				if (!JC::contains (all_vhs_on_modify_allowed_fhs, adj_vh)) continue;
				if (JC::contains (visited_vhs, adj_vh)) continue;
				new_front.insert (adj_vh);
				visited_vhs.insert (adj_vh);
			}
		}
		curr_front = new_front;
		foreach (auto &vh, new_front)
			all_vhs_on_modify_allowed_fhs.erase (vh);
		level++;
	}
}

std::vector<OvmEgH> OneSimpleSheetInflationHandler::get_intersect_path (std::vector<std::pair<OvmEgH, OvmEgH> > &edge_pairs)
{
	inter_path.clear ();
	inter_cross_hexas.clear ();
	inter_edge_pairs.clear ();

	//首先获得可以查找第二个交叉点的顶点范围
	assert (mesh->vertex_property_exists<int> ("node level"));
	auto V_NODE_LEVEL = mesh->request_vertex_property<int> ("node level");
	auto V_ENTITY_PTR = mesh->request_vertex_property<unsigned int> ("entityptr");

	intersect_allowed_vhs.clear ();
	foreach (auto &end_vh_pair, end_vh_pairs){
		auto &fh_patch = modify_allowed_fhs_patches[interface_loops[end_vh_pair.loop_idx].fhs_patch_idx];
		foreach (auto &fh, fh_patch){
			auto adj_vhs = JC::get_adj_vertices_around_face (mesh, fh);
			foreach (auto &vh, adj_vhs){
				ENTITY *entity = (ENTITY*)V_ENTITY_PTR[vh];
				if (entity == NULL || is_VERTEX (entity) || is_EDGE (entity)) continue;

				if (V_NODE_LEVEL[vh] > min_depth)
					intersect_allowed_vhs.insert (vh);
			}
		}
	}

	MeshDijkstra dijkstra (mesh, int_vhs.front (), &intersect_allowed_vhs);
	dijkstra.shortest_path (inter_path, new_int_vh);
	if (new_int_vh == mesh->InvalidVertexHandle){
		assert (false);
	}

	std::vector<OvmEgH> cross_adj_ehs = vh_adj_ehs_mapping[int_vhs.front ()];
	std::vector<std::unordered_set<OvmFaH> > fhs_sets;
	std::vector<OvmEgH> ehs;
	JC::get_ccw_boundary_edges_faces_around_vertex (mesh, int_vhs.front (), cross_adj_ehs, ehs, fhs_sets);

	CrossCrossPath cc_path (mesh, ehs, fhs_sets, int_vhs.front (), new_int_vh);
	//由于是补全，所以在第二个交点位置Cross分布不确定，因此是unconstrained搜索
	if (!cc_path.unconstrained_shortest_path (inter_path, inter_cross_hexas, inter_edge_pairs)){
		assert (false);
	}


	//查找第二个交点在哪个fhs_patch上
	new_int_fhs_patch_idx = -1;
	for (int i = 0; i != modify_allowed_fhs_patches.size (); ++i){
		foreach (auto &fh, modify_allowed_fhs_patches[i]){
			auto adj_vhs = JC::get_adj_vertices_around_face (mesh, fh);
			foreach (auto &vh, adj_vhs){
				if (vh == new_int_vh){
					new_int_fhs_patch_idx = i;
					goto outer;
				}
			}
		}
	}

outer:
	assert (new_int_fhs_patch_idx != -1);

	//查找哪一对end_vh和new_int_vh在同一片fhs_patch上
	end_vh_pair_idx_on_intersect_patch = -1;
	for (int i = 0; i != interface_loops.size (); ++i){
		int fh_patch_idx = interface_loops[i].fhs_patch_idx;
		if (fh_patch_idx == new_int_fhs_patch_idx){
			for (int j = 0; j != end_vh_pairs.size (); ++j){
				if (i == end_vh_pairs[j].loop_idx){
					end_vh_pair_idx_on_intersect_patch = j;
					goto funcend;
				}
			}
		}
	}
funcend:
	assert (end_vh_pair_idx_on_intersect_patch != -1);

	edge_pairs = inter_edge_pairs;
	return inter_path;
}

std::unordered_set<OvmEgH> OneSimpleSheetInflationHandler::connect_nonintersect_loop_parts ()
{
	std::unordered_set<OvmEgH> forbidden_ehs;
	auto fGetEhs = [&] (std::unordered_set<OvmFaH> &fhs, std::unordered_set<OvmEgH> &ehs){
		foreach (auto &fh, fhs){
			auto heh_vec = mesh->face (fh).halfedges ();
			foreach (auto &heh, heh_vec){
				ehs.insert (mesh->edge_handle (heh));
			}
		}
	};
	fGetEhs (interface_fhs, forbidden_ehs);
	fGetEhs (constant_fhs, forbidden_ehs);

	std::unordered_set<OvmEgH> added_ehs;
	for (int i = 0; i != end_vh_pairs.size (); ++i){
		if (i == end_vh_pair_idx_on_intersect_patch) continue;
		EndVhPair end_vh_pair = end_vh_pairs[i];
		std::unordered_set<OvmEgH> allowed_ehs;
		auto &fhs_patch = modify_allowed_fhs_patches[interface_loops[end_vh_pairs[i].loop_idx].fhs_patch_idx];
		fGetEhs (fhs_patch, allowed_ehs);

		VertexVertexBoundaryPath vv_path (mesh, end_vh_pair.vh1, end_vh_pair.vh2, &allowed_ehs, &forbidden_ehs);
		vv_path.set_min_depth (min_depth);
		std::vector<OvmHaEgH> path;
		vv_path.shortest_path (path);
		foreach (auto &heh, path)
			added_ehs.insert (mesh->edge_handle (heh));
	}

	foreach (auto &eh, added_ehs)
		input_ehs.insert (eh);

	return added_ehs; 
}

int OneSimpleSheetInflationHandler::circle_size ()
{
	auto int_vh = int_vhs.front ();
	auto cross_edges = vh_adj_ehs_mapping[int_vh];

	auto fTrackThrough = [&](OvmEgH start_eh, OvmVeH int_vh, std::vector<OvmEgH> &path)->bool{
		OvmVeH other_vh = JC::get_other_vertex_on_edge (mesh, start_eh, int_vh);
		OvmEgH curr_eh = start_eh;
		path.push_back (curr_eh);
		forever{
			if (other_vh == int_vh){
				return true;
			}
			OvmEgH next_eh = mesh->InvalidEdgeHandle;
			foreach (auto &eh, input_ehs){
				if (eh == curr_eh) continue;
				auto vh1 = mesh->edge (eh).to_vertex (), vh2 = mesh->edge (eh).from_vertex ();
				if (vh1 == other_vh || vh2 == other_vh){
					next_eh = eh; break;
				}
			}
			if (next_eh == mesh->InvalidEdgeHandle){
				return false;
			}
			curr_eh = next_eh;
			path.push_back (curr_eh);
			other_vh = JC::get_other_vertex_on_edge (mesh, curr_eh, other_vh);
		}
	};

	std::vector<OvmEgH> one_circle_loop;
	for (int i = 0; i != 4; ++i){
		std::vector<OvmEgH> tmp_path;
		if (fTrackThrough (cross_edges[i], int_vh, tmp_path)){
			one_circle_loop = tmp_path;
			break;
		}
	}
	return one_circle_loop.size ();
}

std::unordered_set<OvmEgH> OneSimpleSheetInflationHandler::connect_intersect_loop_parts ()
{

	for (int i = 0; i != 4; ++i)
		input_ehs.insert (inter_edge_pairs[i].second);
	//返回true表示从int_vh开始，绕了一个圈，又回到int_vh，end_eh保存了最后的一条边
	//返回false表示从int_vh开始，遍历到了终点，end_vh保存了该终点
	auto fTrackThrough = [&](OvmEgH start_eh, OvmVeH int_vh, OvmVeH &end_vh, OvmEgH &end_eh)->bool{
		OvmVeH other_vh = JC::get_other_vertex_on_edge (mesh, start_eh, int_vh);
		OvmEgH curr_eh = start_eh;
		forever{
			if (other_vh == int_vh){
				end_eh = curr_eh; return true;
			}
			OvmEgH next_eh = mesh->InvalidEdgeHandle;
			foreach (auto &eh, input_ehs){
				if (eh == curr_eh) continue;
				auto vh1 = mesh->edge (eh).to_vertex (), vh2 = mesh->edge (eh).from_vertex ();
				if (vh1 == other_vh || vh2 == other_vh){
					next_eh = eh; break;
				}
			}
			if (next_eh == mesh->InvalidEdgeHandle){
				end_vh = other_vh; return false;
			}
			curr_eh = next_eh;
			other_vh = JC::get_other_vertex_on_edge (mesh, curr_eh, other_vh);
		}
	};

	std::unordered_set<OvmEgH> added_ehs;

	EndVhPair end_vh_pair = end_vh_pairs[end_vh_pair_idx_on_intersect_patch];
	std::pair<OvmEgH, OvmEgH> closure_eh_pair;
	std::vector<std::pair<OvmVeH, OvmVeH> > connect_vh_pairs;
	auto bk_inter_edge_pairs = inter_edge_pairs;

	for (int i = 0; i != 4; ++i){
		if (bk_inter_edge_pairs[i].first == mesh->InvalidEdgeHandle) continue;

		OvmEgH start_eh = bk_inter_edge_pairs[i].first;
		OvmVeH end_vh; OvmEgH end_eh;
		if (fTrackThrough (start_eh, int_vhs.front (), end_vh, end_eh)){
			for (int j = 0; j != 4; ++j){
				if (bk_inter_edge_pairs[j].first == end_eh){
					bk_inter_edge_pairs[i].first = bk_inter_edge_pairs[j].first = mesh->InvalidEdgeHandle;
					closure_eh_pair.first = bk_inter_edge_pairs[i].second;
					closure_eh_pair.second = bk_inter_edge_pairs[j].second;
					break;
				}
			}
		}else{
			std::pair<OvmVeH, OvmVeH> one_connect_vh_pair;
			one_connect_vh_pair.first = end_vh; 
			one_connect_vh_pair.second = JC::get_other_vertex_on_edge (mesh, bk_inter_edge_pairs[i].second, new_int_vh);
			connect_vh_pairs.push_back (one_connect_vh_pair);
			bk_inter_edge_pairs[i].first = mesh->InvalidEdgeHandle;
		}
	}

	std::unordered_set<OvmEgH> allowed_ehs, forbidden_ehs;
	auto fGetEhs = [&] (std::unordered_set<OvmFaH> &fhs, std::unordered_set<OvmEgH> &ehs){
		foreach (auto &fh, fhs){
			auto heh_vec = mesh->face (fh).halfedges ();
			foreach (auto &heh, heh_vec){
				ehs.insert (mesh->edge_handle (heh));
			}
		}
	};
	fGetEhs (interface_fhs, forbidden_ehs);
	fGetEhs (constant_fhs, forbidden_ehs);

	OvmVeH closure_vh1 = JC::get_other_vertex_on_edge (mesh, closure_eh_pair.first, new_int_vh),
		closure_vh2 = JC::get_other_vertex_on_edge (mesh, closure_eh_pair.second, new_int_vh);
	forbidden_ehs.insert (closure_eh_pair.first);
	forbidden_ehs.insert (closure_eh_pair.second);

	VertexVertexBoundaryPath vv_path (mesh, closure_vh1, closure_vh2, NULL, &forbidden_ehs);
	std::vector<OvmHaEgH> path;
	vv_path.shortest_path (path);
	foreach (auto &heh, path){
		auto eh = (mesh->edge_handle (heh));
		added_ehs.insert (eh);
		forbidden_ehs.insert (eh);
	}

	vv_path = VertexVertexBoundaryPath (mesh, connect_vh_pairs.front ().first, connect_vh_pairs.front ().second, NULL, &forbidden_ehs);
	path.clear ();
	vv_path.shortest_path (path);
	foreach (auto &heh, path){
		auto eh = (mesh->edge_handle (heh));
		added_ehs.insert (eh);
		forbidden_ehs.insert (eh);
	}
	vv_path = VertexVertexBoundaryPath (mesh, connect_vh_pairs.back ().first, connect_vh_pairs.back ().second, NULL, &forbidden_ehs);
	path.clear ();
	vv_path.shortest_path (path);
	foreach (auto &heh, path){
		auto eh = (mesh->edge_handle (heh));
		added_ehs.insert (eh);
	}

	foreach (auto &eh, added_ehs)
		input_ehs.insert (eh);
	return added_ehs;
}

#include "stdafx.h"
#include <Windows.h>
#include "mesheditwidget.h"
#include "FaceFacePath.h"
#include "VertexVertexBoundaryPath.h"
#include "swp_opts.hxx"
#include "sweepapi.hxx"
#include "max_flow_min_cut.h"
#include "mesh_min_cut.h"
#include "MeshDijkstra.h"
#include <QDomDocument>
#include <QDomElement>


void MeshEditWidget::on_select_edges_ok_for_qs ()
{
	selected_ehs.clear ();
	if (!hoopsview->get_selected_elements (NULL, &selected_ehs, NULL)){
		QMessageBox::warning (this, tr("警告"), tr("未选中任何网格边！"), QMessageBox::Ok);
		return;
	}

	hoopsview->clear_selection ();
	//hoopsview->derender_all_mesh_groups ();
	//render
	auto group = new VolumeMeshElementGroup (mesh, "qs", "select edges");
	MeshRenderOptions render_options;
	render_options.edge_color = "red";
	render_options.edge_width = 6;
	JC::vector_to_unordered_set (selected_ehs, group->ehs);
	selected_ehs_set = group->ehs;
	qs_loop = selected_ehs_set;
	foreach (auto &eh, selected_ehs_set) quad_set_data.input_edges.insert (eh.idx ());

	hoopsview->render_mesh_group (group, render_options);
}

void MeshEditWidget::on_select_constraint_faces_ok_for_qs ()
{
	selected_fhs.clear ();

	if (!hoopsview->get_selected_elements (NULL, NULL, &selected_fhs)){
		QMessageBox::warning (this, tr("警告"), tr("未选中任何网格面！"), QMessageBox::Ok);
		return;
	}
	JC::vector_to_unordered_set (selected_fhs, selected_fhs_set);
	hoopsview->clear_selection ();
	//render
	auto group = new VolumeMeshElementGroup (mesh, "qs", "select constraint faces");
	MeshRenderOptions render_options;
	render_options.face_color = "yellow";
	group->fhs = selected_fhs_set;
	foreach (auto &fh, selected_fhs) quad_set_data.forbidden_faces.insert (fh.idx ());
	hoopsview->render_mesh_group (group, render_options);
}

int topo_shortest_path (VolumeMesh *mesh, OvmVeH start_vh, std::unordered_set<OvmVeH> &end_vhs, OvmVeH &end_vh){
	std::unordered_set<OvmVeH> visited_vhs;
	int dist = 0;
	visited_vhs.insert (start_vh);
	std::unordered_set<OvmVeH> front_set;
	front_set.insert (start_vh);
	forever{
		dist++;
		std::unordered_set<OvmVeH> new_front_set;
		foreach (auto &vh, front_set){
			for (auto voh_it = mesh->voh_iter (vh); voh_it; ++voh_it){
				auto to_vh = mesh->halfedge (*voh_it).to_vertex ();
				if (JC::contains (visited_vhs, to_vh)) continue;
				new_front_set.insert (to_vh);
				visited_vhs.insert (to_vh);
			}
		}//end foreach (auto &vh, front_set){...
		bool reach_one_end = false;
		foreach (auto &front_vh, new_front_set){
			foreach (auto &one_end_vh, end_vhs){
				if (one_end_vh == front_vh){
					end_vh = one_end_vh;
					reach_one_end = true;
					break;
				}
			}
			if (reach_one_end) break;
		}
		if (reach_one_end) break;

		if (new_front_set.empty ()) {
			dist = -1;
			break;
		}
		front_set = new_front_set;
	}
	return dist;
}

std::vector<std::pair<OvmVeH, OvmVeH> > get_end_node_pairs (VolumeMesh *mesh, std::vector<std::pair<OvmVeH, OvmVeH> > end_node_groups)
{
	std::vector<std::pair<OvmVeH, OvmVeH> > node_pairs;
	std::vector<OvmVeH> all_end_vhs;
	std::map<OvmVeH, OvmVeH> node_pair_mapping;

	foreach (auto &p, end_node_groups){
		all_end_vhs.push_back (p.first); all_end_vhs.push_back (p.second);
		node_pair_mapping[p.first] = p.second;
		node_pair_mapping[p.second] = p.first;
	}

	struct TmpNodePair{
		OvmVeH end_vh1, end_vh2;
		double dist;
		bool operator < (const TmpNodePair &_rhs) const {
			return dist < _rhs.dist;
		}
	};

	
	while (!all_end_vhs.empty ()){
		std::set<TmpNodePair> tmppairs;
		for (int i = 0; i != all_end_vhs.size (); ++i){
			
			OvmVeH end_vh1 = all_end_vhs[i], end_vh2;

			std::unordered_set<OvmVeH> other_end_vhs;
			//获得除了第i个（它自己）以及属于一个pair的那个end_vh之外的其他点
			for (int j = 0; j != all_end_vhs.size (); ++j){
				if (i == j || all_end_vhs[j] == node_pair_mapping[end_vh1]) continue;
				other_end_vhs.insert (all_end_vhs[j]);
			}

			//MeshDijkstra md (mesh, end_vh1, &other_end_vhs);
			//md.search_boundary = true; md.search_inner = false;
			//md.min_valence = -1;
			//std::vector<OvmEgH> best_path;
			//auto dist = md.shortest_path (best_path, end_vh2);
			auto dist = topo_shortest_path (mesh, end_vh1, other_end_vhs, end_vh2);

			TmpNodePair tmppair;
			tmppair.dist = dist;
			tmppair.end_vh1 = end_vh1; tmppair.end_vh2 = end_vh2;
			tmppairs.insert (tmppair);
		}
		auto closest_pair = *(tmppairs.begin ());
		std::pair<OvmVeH, OvmVeH> p;
		p.first = closest_pair.end_vh1; p.second = closest_pair.end_vh2;
		node_pairs.push_back (p);

		all_end_vhs.erase (std::find (all_end_vhs.begin (), all_end_vhs.end (), closest_pair.end_vh1));
		all_end_vhs.erase (std::find (all_end_vhs.begin (), all_end_vhs.end (), closest_pair.end_vh2));
	}

	return node_pairs;
}

bool search_2nd_intersect_vh (VolumeMesh *mesh, OvmVeH start_int_vh, std::vector<OvmEgH> cross_adj_ehs, 
	std::unordered_set<OvmVeH> &intersect_allowed_vhs,
	OvmVeH &end_int_vh, std::vector<OvmEgH> &intersect_path)
{
	std::vector<OvmEgH> ehs;
	std::vector<std::unordered_set<OvmFaH> > fhs_sets;
	JC::get_ccw_boundary_edges_faces_around_vertex (mesh, start_int_vh, cross_adj_ehs, ehs, fhs_sets);

	//搜索最优的第二个相交点的位置
	MeshDijkstra dijkstra (mesh, start_int_vh, &intersect_allowed_vhs);
	dijkstra.shortest_path (intersect_path, end_int_vh);
	if (end_int_vh == mesh->InvalidVertexHandle){
		QMessageBox::warning (NULL, "ERROR", QObject::tr("搜索第二个交点失败！"));
		return false;
	}

	//搜索内部沿着自交线的Cross分布
	CrossCrossPath cc_path (mesh, cross_adj_ehs, fhs_sets, start_int_vh, end_int_vh);
	std::vector<std::pair<OvmEgH, OvmEgH> > cross_eh_pairs;
	//由于是补全，所以在第二个交点位置Cross分布不确定，因此是unconstrained搜索
	CCPSHConf cross_hexas;
	if (!cc_path.unconstrained_shortest_path (intersect_path, cross_hexas, cross_eh_pairs)){
		QMessageBox::warning (NULL, "ERROR", QObject::tr("搜索交叉路径失败！"));
		return false;
	}
	return true;
}

int which_end_node_pair (VolumeMesh *mesh, OvmVeH int_vh, std::vector<std::pair<OvmVeH, OvmVeH> > &end_node_pairs,
	std::unordered_set<OvmFaH> &restricted_fhs)
{
	return 0;
}

void rate_boundary_vhs (VolumeMesh *mesh, std::unordered_set<OvmFaH> &forbidden_fhs,
	std::vector<std::unordered_set<OvmVeH> > &layered_vhs)
{
	if (!mesh->vertex_property_exists<unsigned int> ("entityptr")) return;

	auto V_ENT_PTR = mesh->request_vertex_property<unsigned int> ("entityptr");

	//获得forbidden fhs的外围点
	std::unordered_set<OvmVeH> vhs_on_front, vhs_left, vhs_on_forbidded_fhs_bnd, vhs_visited;
	JC::collect_boundary_element (mesh, forbidden_fhs, &vhs_on_forbidded_fhs_bnd, NULL);

	if (!mesh->vertex_property_exists<int> ("node level")){
		auto tmp = mesh->request_vertex_property<int> ("node level", 0);
		mesh->set_persistent (tmp);
	}
	if (!mesh->mesh_property_exists<int> ("max node level")){
		auto tmp = mesh->request_mesh_property<int> ("max node level", 1);
		mesh->set_persistent (tmp);
	}
	auto V_LEVEL = mesh->request_vertex_property<int> ("node level");
	auto V_MAX_LEVEL = mesh->request_mesh_property<int> ("max node level");
	
	std::unordered_set<OvmFaH> allowed_fhs;
	for (auto bf_it = mesh->bf_iter (); bf_it; ++bf_it){
		if (!JC::contains (forbidden_fhs, *bf_it))
			allowed_fhs.insert (*bf_it);
	}

	std::unordered_set<OvmVeH> all_allowed_vhs;
	foreach (auto &fh, allowed_fhs){
		auto adj_vhs = JC::get_adj_vertices_around_face (mesh, fh);
		foreach (auto &vh, adj_vhs)
			all_allowed_vhs.insert (vh);
	}

	foreach (auto &vh, all_allowed_vhs){
		ENTITY *entity = (ENTITY*)(V_ENT_PTR[vh]);
		assert (entity);
		if (is_VERTEX (entity) || is_EDGE (entity) || JC::contains (vhs_on_forbidded_fhs_bnd, vh)){
			vhs_on_front.insert (vh);
			V_LEVEL[vh] = 0;
		}
		else
			vhs_left.insert (vh);
	}

	std::unordered_set<OvmVeH> vhs_on_new_front;
	foreach (auto &vh, vhs_on_front)
		vhs_visited.insert (vh);

	
	layered_vhs.push_back (vhs_on_front);

	int layer_idx = 0;
	while (!vhs_left.empty ()){
		layer_idx++;
		foreach (auto &vh, vhs_on_front){
			std::unordered_set<OvmVeH> adj_vhs;
			JC::get_adj_boundary_vertices_around_vertex (mesh, vh, adj_vhs);
			foreach (auto &adj_vh, adj_vhs){
				if (JC::contains (vhs_visited, adj_vh)) continue;
				if (!JC::contains (all_allowed_vhs, adj_vh)) continue;
				vhs_on_new_front.insert (adj_vh);
				V_LEVEL[adj_vh] = layer_idx;
				vhs_visited.insert (adj_vh);
			}
		}

		layered_vhs.push_back (vhs_on_new_front);

		foreach (auto &vh, vhs_on_new_front)
			vhs_left.erase (vh);

		vhs_on_front = vhs_on_new_front;
		vhs_on_new_front.clear ();
	}
	V_MAX_LEVEL[0] = layer_idx + 1;
}

void rate_inner_vhs (VolumeMesh *mesh, std::vector<std::unordered_set<OvmVeH> > &layered_vhs)
{
	if (!mesh->vertex_property_exists<int> ("node level")){
		auto tmp = mesh->request_vertex_property<int> ("node level", 0);
		mesh->set_persistent (tmp);
	}
	if (!mesh->mesh_property_exists<int> ("max node level")){
		auto tmp = mesh->request_mesh_property<int> ("max node level", 1);
		mesh->set_persistent (tmp);
	}
	auto V_LEVEL = mesh->request_vertex_property<int> ("node level");
	auto V_MAX_LEVEL = mesh->request_mesh_property<int> ("max node level");

	std::unordered_set<OvmVeH> vhs_on_front, vhs_on_new_front, vhs_left, vhs_visited;
	for (auto v_it = mesh->vertices_begin (); v_it != mesh->vertices_end (); ++v_it){
		auto vh = *v_it;
		if (mesh->is_boundary (vh)){
			vhs_on_front.insert (vh);
			vhs_visited.insert (vh);
			V_LEVEL[vh] = 0;
		}
		else
			vhs_left.insert (vh);
	}

	layered_vhs.push_back (vhs_on_front);

	int layer_idx = 0;
	while (!vhs_left.empty ()){
		layer_idx++;
		foreach (auto &vh, vhs_on_front){
			std::unordered_set<OvmVeH> adj_vhs;
			JC::get_adj_vertices_around_vertex (mesh, vh, adj_vhs);
			foreach (auto &adj_vh, adj_vhs){
				if (JC::contains (vhs_visited, adj_vh)) continue;
				vhs_on_new_front.insert (adj_vh);
				V_LEVEL[adj_vh] = layer_idx;
				vhs_visited.insert (adj_vh);
			}
		}

		layered_vhs.push_back (vhs_on_new_front);

		foreach (auto &vh, vhs_on_new_front)
			vhs_left.erase (vh);

		vhs_on_front = vhs_on_new_front;
		vhs_on_new_front.clear ();
	}
	V_MAX_LEVEL[0] = layer_idx + 1;
}

void rate_all_vhs (VolumeMesh *mesh, std::unordered_set<OvmFaH> &forbidden_fhs,
	std::vector<std::unordered_set<OvmVeH> > &layered_vhs)
{
	if (!mesh->vertex_property_exists<unsigned int> ("entityptr")) return;

	auto V_ENT_PTR = mesh->request_vertex_property<unsigned int> ("entityptr");

	//获得forbidden fhs的外围点
	std::unordered_set<OvmVeH> vhs_on_front, vhs_left, vhs_on_forbidded_fhs_bnd, vhs_visited;
	JC::collect_boundary_element (mesh, forbidden_fhs, &vhs_on_forbidded_fhs_bnd, NULL);

	if (!mesh->vertex_property_exists<int> ("node level")){
		auto tmp = mesh->request_vertex_property<int> ("node level", 0);
		mesh->set_persistent (tmp);
	}
	if (!mesh->mesh_property_exists<int> ("max node level")){
		auto tmp = mesh->request_mesh_property<int> ("max node level", 1);
		mesh->set_persistent (tmp);
	}
	auto V_LEVEL = mesh->request_vertex_property<int> ("node level");
	auto V_MAX_LEVEL = mesh->request_mesh_property<int> ("max node level");

	std::unordered_set<OvmFaH> allowed_fhs;
	for (auto bf_it = mesh->bf_iter (); bf_it; ++bf_it){
		if (!JC::contains (forbidden_fhs, *bf_it))
			allowed_fhs.insert (*bf_it);
	}

	std::unordered_set<OvmVeH> all_allowed_vhs;
	foreach (auto &fh, allowed_fhs){
		auto adj_vhs = JC::get_adj_vertices_around_face (mesh, fh);
		foreach (auto &vh, adj_vhs)
			all_allowed_vhs.insert (vh);
	}

	foreach (auto &vh, all_allowed_vhs){
		ENTITY *entity = (ENTITY*)(V_ENT_PTR[vh]);
		assert (entity);
		if (is_VERTEX (entity) || is_EDGE (entity) || JC::contains (vhs_on_forbidded_fhs_bnd, vh)){
			vhs_on_front.insert (vh);
			V_LEVEL[vh] = 0;
		}
		else
			vhs_left.insert (vh);
	}

	std::unordered_set<OvmVeH> vhs_on_new_front;
	foreach (auto &vh, vhs_on_front)
		vhs_visited.insert (vh);


	layered_vhs.push_back (vhs_on_front);

	int layer_idx = 0;
	while (!vhs_left.empty ()){
		layer_idx++;
		foreach (auto &vh, vhs_on_front){
			std::unordered_set<OvmVeH> adj_vhs;
			JC::get_adj_boundary_vertices_around_vertex (mesh, vh, adj_vhs);
			foreach (auto &adj_vh, adj_vhs){
				if (JC::contains (vhs_visited, adj_vh)) continue;
				if (!JC::contains (all_allowed_vhs, adj_vh)) continue;
				vhs_on_new_front.insert (adj_vh);
				V_LEVEL[adj_vh] = layer_idx;
				vhs_visited.insert (adj_vh);
			}
		}

		layered_vhs.push_back (vhs_on_new_front);

		foreach (auto &vh, vhs_on_new_front)
			vhs_left.erase (vh);

		vhs_on_front = vhs_on_new_front;
		vhs_on_new_front.clear ();
	}
	V_MAX_LEVEL[0] = layer_idx + 1;
}

void MeshEditWidget::on_complete_loop_for_qs ()
{
	//////////////////////////////////////////////////////////////////////////
	//首先对表面顶点进行预处理
	//获得位于顶点和几何边上的网格点
	
	if (!mesh->vertex_property_exists<unsigned int> ("entityptr")) return;

	auto V_ENT_PTR = mesh->request_vertex_property<unsigned int> ("entityptr");

	
	std::vector<std::unordered_set<OvmVeH> > layered_vhs;
	rate_boundary_vhs (mesh, selected_fhs_set, layered_vhs);

	////render
	//std::vector<QString> colors;
	//float color_step = 1.0 / layered_vhs.size ();

	//for (int i = 0; i != layered_vhs.size (); ++i){
	//	QString color = QString ("(r=%1 g=%2 b=0)")
	//		.arg (1.0 - color_step * i)
	//		.arg (0.0 + color_step * i);
	//	char *color_buf = new char[500];
	//	strcpy (color_buf, color.toAscii ().data ());
	//	auto group = new VolumeMeshElementGroup (mesh, "qs", "layered vhs");
	//	MeshRenderOptions render_options;
	//	render_options.vertex_color = color_buf;
	//	render_options.vertex_size = 6;
	//	group->vhs = layered_vhs[i];
	//	hoopsview->render_mesh_group (group, render_options);
	//}

	if (ossi_handler) delete ossi_handler;
	ossi_handler = new OneSimpleSheetInflationHandler (mesh, body, hoopsview);
	ossi_handler->init ();

	std::unordered_set<OvmEgH> selected_ehs_set;
	JC::vector_to_unordered_set (selected_ehs, selected_ehs_set);

	if (!ossi_handler->analyze_input_edges (selected_ehs_set)){
		QMessageBox::critical (this, tr("错误"), ossi_handler->get_last_err (), QMessageBox::Ok);
		return;
	}

	////////////////////////////////////////////////////////////////////////////
	////获得可以变动或者经过的四边形面集
	//std::unordered_set<OvmFaH> modified_allowed_fhs;
	//for (auto bf_it = mesh->bf_iter (); ++bf_it; ++bf_it){
	//	auto bf = *bf_it;
	//	if (!JC::contains (selected_fhs, bf))
	//		modified_allowed_fhs.insert (bf);
	//}

	//////////////////////////////////////////////////////////////////////////
	//进行端点配对
	auto end_vhs = ossi_handler->end_vhs;
	std::vector<std::pair<OvmVeH, OvmVeH> > end_vh_groups;
	for (int i = 0; i != ossi_handler->incomplete_loops.size (); ++i){
		auto this_loop = ossi_handler->incomplete_loops[i];
		std::unordered_set<OvmVeH> vhs_on_this_loop;
		foreach (auto &eh, this_loop){
			auto eg = mesh->edge (eh);
			vhs_on_this_loop.insert (eg.to_vertex ());
			vhs_on_this_loop.insert (eg.from_vertex ());
		}

		std::vector<OvmVeH> end_vhs_on_this_loop;
		foreach (auto &end_vh, end_vhs){
			if (JC::contains (vhs_on_this_loop, end_vh))
				end_vhs_on_this_loop.push_back (end_vh);
		}

		std::pair<OvmVeH, OvmVeH> one_group;
		one_group.first = end_vhs_on_this_loop.front ();
		one_group.second = end_vhs_on_this_loop.back ();

		end_vh_groups.push_back (one_group);
	}

	auto end_vh_pairs = get_end_node_pairs (mesh, end_vh_groups);
	//auto int_vhs = ossi_handler->int_vhs;
	//OvmVeH int_vh1, int_vh2;


	//if (int_vhs.size () == 0){

	//}else if (int_vhs.size () == 1){
	//	int_vh1 = int_vhs.front ();
	//	std::unordered_set<OvmVeH> int_allowed_vhs, int_forbidden_vhs;
	//	foreach (auto &fh, selected_fhs){
	//		auto adj_vhs = JC::get_adj_vertices_around_face (mesh, fh);
	//		foreach (auto &vh, adj_vhs)
	//			int_forbidden_vhs.insert (vh);
	//	}

	//	for (auto v_it = mesh->vertices_begin (); v_it != mesh->vertices_end (); ++v_it){
	//		if (!mesh->is_boundary (*v_it)) continue;
	//		if (!JC::contains (int_forbidden_vhs, *v_it))
	//			int_allowed_vhs.insert (*v_it);
	//	}

	//	std::vector<OvmEgH> cross_adj_ehs = ossi_handler->vh_adj_ehs_mapping[int_vh1];
	//	std::vector<OvmEgH> intersect_path;
	//	search_2nd_intersect_vh (mesh, int_vh1, cross_adj_ehs, int_allowed_vhs, int_vh2, intersect_path);

	//	//看int_vh2应该和那对end_pair搭配

	//}else if (int_vhs.size () == 2){

	//}

	//render
	char *colors[] = {"red", "blue", "yellow", "magenta"};
	for (int i = 0; i != end_vh_pairs.size (); ++i){
		auto &p = end_vh_pairs[i];
		auto group = new VolumeMeshElementGroup (mesh, "qs", "layered vhs");
		MeshRenderOptions render_options;
		render_options.vertex_color = colors[i % 4];
		render_options.vertex_size = 6;
		group->vhs.insert (p.first); 
		group->vhs.insert (p.second); 
		hoopsview->render_mesh_group (group, render_options);
	}
	return;

	for (int i = 0; i != end_vh_pairs.size (); ++i){

	}
}

void MeshEditWidget::on_optimize_loop_for_qs ()
{
	std::unordered_set<OvmFaH> s_fh_set, t_fh_set, all_adj_fh;
	std::unordered_set<OvmVeH> all_vhs;
	foreach (auto &eh, qs_loop){
		auto eg = mesh->edge (eh);
		all_vhs.insert (eg.to_vertex ()); all_vhs.insert (eg.from_vertex ());
	}

	foreach (auto &vh, all_vhs){
		std::unordered_set<OvmFaH> adj_faces;
		JC::get_adj_boundary_faces_around_vertex (mesh, vh, adj_faces);
		foreach (auto &adj_fh, adj_faces)
			all_adj_fh.insert (adj_fh);
	}

	OvmFaH seed_fh = *(all_adj_fh.begin ());
	all_adj_fh.erase (seed_fh);
	std::queue<OvmFaH> spread_set;
	std::unordered_set<OvmFaH> visited_fhs, adj_bnd_fhs;
	visited_fhs.insert (seed_fh);

	JC::get_adj_boundary_faces_around_face (mesh, seed_fh, adj_bnd_fhs);
	foreach (auto &adj_fh, adj_bnd_fhs){
		if (!JC::contains (all_adj_fh, adj_fh)) continue;
		auto comm_eh = JC::get_common_edge_handle (mesh, seed_fh, adj_fh);
		if (JC::contains (selected_ehs_set, comm_eh)) continue;
		
		spread_set.push (adj_fh);
		visited_fhs.insert (adj_fh);
		all_adj_fh.erase (adj_fh);
	}

	while (!spread_set.empty ()){
		auto front_fh = spread_set.front ();
		spread_set.pop ();
		adj_bnd_fhs.clear ();

		JC::get_adj_boundary_faces_around_face (mesh, front_fh, adj_bnd_fhs);
		foreach (auto &adj_fh, adj_bnd_fhs){
			if (!JC::contains (all_adj_fh, adj_fh)) continue;
			if (JC::contains (visited_fhs, adj_fh)) continue;
			auto comm_eh = JC::get_common_edge_handle (mesh, front_fh, adj_fh);
			if (JC::contains (selected_ehs_set, comm_eh)) continue;
			
			spread_set.push (adj_fh);
			visited_fhs.insert (adj_fh);
			all_adj_fh.erase (adj_fh);
		}
	}

	if (all_adj_fh.empty ()){
		QMessageBox::warning (this, "ERROR", "该loop是mobius带！");
		return;
	}

	//render
	auto group = new VolumeMeshElementGroup (mesh, "qs", "s set");
	MeshRenderOptions render_options;
	render_options.face_color = "blue";
	group->fhs = visited_fhs;
	hoopsview->render_mesh_group (group, render_options);
	group = new VolumeMeshElementGroup (mesh, "qs", "t set");
	render_options.face_color = "yellow";
	group->fhs = all_adj_fh;
	hoopsview->render_mesh_group (group, render_options);

	return;

	//s_fh_set = visited_fhs;
	//t_fh_set = all_adj_fh;

	//std::vector<std::unordered_set<OvmFaH> > obstacles;
	//std::unordered_set<OvmFaH> obstacle_fhs;
	//JC::vector_to_unordered_set (selected_fhs, obstacle_fhs);
	//obstacles.push_back (obstacle_fhs);
	//auto cut_ehs = get_quad_mesh_min_cut (mesh, s_fh_set, t_fh_set, obstacles);

	//completed_loop_ehs = cut_ehs;
	//selected_ehs_set = cut_ehs;
	//qs_loop = selected_ehs_set;

	////render
	//hoopsview->derender_all_mesh_groups ();
	//auto group = new VolumeMeshElementGroup (mesh, "qs", "quad min cut");
	//MeshRenderOptions render_options;
	//render_options.edge_color = "blue";
	//render_options.edge_width = 6;
	//group->ehs = cut_ehs;
	//hoopsview->render_mesh_group (group, render_options);
}

void MeshEditWidget::on_search_int_path_for_qs ()
{
	if (qs_loop.empty ()){
		QMessageBox::information (this, "INFO", "qs_loop 为空！");
		return;
	}
	//qs_cross_conf.resize (4);

	//std::vector<std::unordered_set<OvmVeH> > layered_vhs;
	//rate_inner_vhs (mesh, layered_vhs);

	//if (ossi_handler) delete ossi_handler;
	//ossi_handler = new OneSimpleSheetInflationHandler (mesh, body, hoopsview);
	//ossi_handler->init ();

	//if (!ossi_handler->analyze_input_edges (qs_loop)){
	//	QMessageBox::critical (this, tr("错误"), ossi_handler->get_last_err (), QMessageBox::Ok);
	//	return;
	//}

	//auto int_vhs = ossi_handler->int_vhs;
	//if (int_vhs.size () != 2){
	//	QMessageBox::warning (this, "INFO", "自相交点不为2！");
	//	return;
	//}

	//VertexVertexPath vv_path (mesh, int_vhs.front (), int_vhs.back ());
	//std::vector<OvmHaEgH> cross_vv_path;
	//vv_path.shortest_path (cross_vv_path, 4);
	//std::vector<OvmEgH> cross_vv_path_ehs;
	//foreach (auto &heh, cross_vv_path)
	//	cross_vv_path_ehs.push_back (mesh->edge_handle (heh));

	//qs_int_path = cross_vv_path_ehs;

	//ossi_handler->base_handler->intersect_path = cross_vv_path_ehs;

	//std::unordered_set<OvmVeH> vhs_on_path;
	//foreach (auto &heh, cross_vv_path){
	//	auto he = mesh->halfedge (heh);
	//	vhs_on_path.insert (he.to_vertex ());
	//	vhs_on_path.insert (he.from_vertex ());
	//}
	//hoopsview->derender_mesh_groups ("qs", "cross hexas", true);
	//auto group = new VolumeMeshElementGroup (mesh, "qs", "cross hexas");
	//foreach (auto &vh, vhs_on_path){
	//	JC::get_adj_hexas_around_vertex (mesh, vh, group->chs);
	//}
	//all_chs_around_int_path = group->chs;
	//ungrouped_chs_around_int_path = all_chs_around_int_path;

	//MeshRenderOptions render_options;
	//render_options.cell_color = "grey";

	//hoopsview->render_mesh_group (group, render_options);
	//hoopsview->set_mesh_group_selectability (group, false, false, true);

	ui.stackedWidget_for_QS->setCurrentIndex (2);

	if (ossi_handler) delete ossi_handler;
	ossi_handler = new OneSimpleSheetInflationHandler (mesh, body, hoopsview);
	ossi_handler->init ();

	if (!ossi_handler->analyze_input_edges (qs_loop)){
		QMessageBox::critical (this, tr("错误"), ossi_handler->get_last_err (), QMessageBox::Ok);
		return;
	}

	auto int_vhs = ossi_handler->int_vhs;
	//if (int_vhs.size () != 2){
	//	QMessageBox::warning (this, "INFO", "自相交点不为2！");
	//	return;
	//}

	auto int_vh = int_vhs.front ();
	qs_first_int_vh = int_vh;

	//for (auto voh_it = mesh->voh_iter (int_vh); voh_it; ++voh_it){
	//	auto eh = mesh->edge_handle (*voh_it);
	//	if (mesh->is_boundary (eh)) continue;
	//	qs_int_path.push_back (eh);
	//}

	//if (qs_int_path.size () > 1){
	//	QMessageBox::critical (this, tr("错误"), "初始相交边大于一个！", QMessageBox::Ok);
	//	return;
	//}

	auto group = new VolumeMeshElementGroup (mesh, "qs", "int path next");

	qs_vh = qs_first_int_vh;

	MeshRenderOptions render_options;

	for (auto voh_it = mesh->voh_iter (qs_vh); voh_it; ++voh_it){
		auto eh = mesh->edge_handle (*voh_it);

		if (mesh->is_boundary (eh)) continue;
		if (mesh->valence (eh) < 4) continue;
		group->ehs.insert (eh);
		//group->vhs.insert (mesh->halfedge (*voh_it).to_vertex ());
	}

	render_options.edge_color = "green";
	render_options.edge_width = 6;
	//render_options.vertex_color = "pink";
	//render_options.vertex_size = 2;

	hoopsview->render_mesh_group (group, render_options);
	hoopsview->set_mesh_group_selectability (group, false, true, false);
}

void MeshEditWidget::on_select_next_int_edge_for_qs ()
{
	selected_ehs.clear ();
	if (!hoopsview->get_selected_elements (NULL, &selected_ehs, NULL)){
		QMessageBox::warning (this, tr("警告"), tr("未选中任何边！"), QMessageBox::Ok);
		return;
	}

	if (selected_ehs.size () != 1){
		QMessageBox::warning (this, tr("警告"), tr("只能选择一条边！"), QMessageBox::Ok);
		return;
	}

	hoopsview->derender_mesh_groups ("qs", "int path next", true);
	hoopsview->derender_mesh_groups ("qs", "int path", true);

	auto eg = mesh->edge (selected_ehs.front ());
	qs_vh = eg.from_vertex () == qs_vh? eg.to_vertex () : eg.from_vertex ();
	qs_int_path.push_back (selected_ehs.front ());

	auto group = new VolumeMeshElementGroup (mesh, "qs", "int path");
	JC::vector_to_unordered_set (qs_int_path, group->ehs);
	
	MeshRenderOptions render_options;
	render_options.edge_color = "blue";
	render_options.edge_width = 6;
	hoopsview->render_mesh_group (group, render_options);

	group = new VolumeMeshElementGroup (mesh, "qs", "int path next");
	for (auto voh_it = mesh->voh_iter (qs_vh); voh_it; ++voh_it){
		auto eh = mesh->edge_handle (*voh_it);
		if (JC::contains (qs_int_path, eh)) continue;

		if (mesh->is_boundary (eh)) continue;
		if (mesh->valence (eh) < 4) continue;
		group->ehs.insert (eh);
		
	}

	render_options.edge_color = "green";
	render_options.edge_width = 6;

	hoopsview->render_mesh_group (group, render_options);
	hoopsview->set_mesh_group_selectability (group, true, true, true);
}

void MeshEditWidget::on_select_int_edges_ok_for_qs ()
{
	qs_cross_conf.clear ();
	qs_cross_conf.resize (4);

	std::unordered_set<OvmVeH> vhs_on_path;
	foreach (auto &eh, qs_int_path){
		auto eg = mesh->edge (eh);
		vhs_on_path.insert (eg.to_vertex ());
		vhs_on_path.insert (eg.from_vertex ());
	}

	hoopsview->derender_mesh_groups ("qs", "cross hexas", true);
	auto group = new VolumeMeshElementGroup (mesh, "qs", "cross hexas");
	foreach (auto &vh, vhs_on_path){
		JC::get_adj_hexas_around_vertex (mesh, vh, group->chs);
	}
	all_chs_around_int_path = group->chs;
	ungrouped_chs_around_int_path = all_chs_around_int_path;

	MeshRenderOptions render_options;
	render_options.cell_color = "grey";

	hoopsview->render_mesh_group (group, render_options);
	hoopsview->set_mesh_group_selectability (group, false, false, true);
}

void MeshEditWidget::on_select_cross_hexa_ok_for_qs ()
{
	if (all_chs_around_int_path.empty ()){
		QMessageBox::warning (this, "ERR", "all_chs_around_int_path为空！");
		return;
	}
	selected_fhs.clear ();
	if (!hoopsview->get_selected_elements (NULL, NULL, &selected_fhs)){
		QMessageBox::warning (this, tr("警告"), tr("未选中任何网格面！"), QMessageBox::Ok);
		return;
	}

	std::unordered_set<OvmCeH> selected_chs;
	foreach (auto &fh, selected_fhs){
		auto hfh = mesh->halfface_handle (fh, 0);
		auto ch = mesh->incident_cell (hfh);
		if (ch == mesh->InvalidCellHandle){
			hfh = mesh->opposite_halfface_handle (hfh);
			ch = mesh->incident_cell (hfh);
		}
		if (!JC::contains (all_chs_around_int_path, ch)){
			hfh = mesh->opposite_halfface_handle (hfh);
			ch = mesh->incident_cell (hfh);
		}
		selected_chs.insert (ch);
	}

	int hexa_idx = ui.comboBox_cross_hexa_idx->currentText ().toInt ();
	

	foreach (auto &ch, selected_chs){
		ungrouped_chs_around_int_path.erase (ch);
		for (int i = 0; i != 4; ++i){
			qs_cross_conf[i].erase (ch);
		}
		qs_cross_conf[hexa_idx - 1].insert (ch);
	}

	update_cross_hexas ();
}

void MeshEditWidget::on_get_quad_set_for_qs ()
{
	//selected_fhs.clear ();
	//if (!hoopsview->get_selected_elements (NULL, NULL, &selected_fhs)){
	//	QMessageBox::warning (this, tr("警告"), tr("未选中任何网格面！"), QMessageBox::Ok);
	//	return;
	//}

	//OvmFaH seed_fh = selected_fhs.front ();
	//OvmCeH outer_ch = mesh->incident_cell (mesh->halfface_handle (seed_fh, 0));
	//if (outer_ch == mesh->InvalidCellHandle){
	//	outer_ch = mesh->incident_cell (mesh->halfface_handle (seed_fh, 1));
	//}
	//

	//std::unordered_set<OvmEgH> all_loop_ehs;
	//foreach (auto &idx, quad_set_data.input_edges)
	//	all_loop_ehs.insert (OvmEgH(idx));
	//foreach (auto &idx, quad_set_data.closed_edges)
	//	all_loop_ehs.insert (OvmEgH(idx));
	//foreach (auto &idx, quad_set_data.appended_edges)
	//	all_loop_ehs.insert (OvmEgH(idx));

	//std::vector<std::unordered_set<OvmFaH> > sep_fhs;
	//std::unordered_set<OvmFaH> all_fhs;
	//for (auto bf_it = mesh->bf_iter (); bf_it; ++bf_it)
	//	all_fhs.insert (*bf_it);
	//JC::get_separate_fhs_patches (mesh, all_fhs, all_loop_ehs, sep_fhs);

	//std::vector<std::unordered_set<OvmCeH> > sep_chs;
	//std::unordered_set<OvmCeH> outer_chs;
	//foreach (auto &fhs, sep_fhs){
	//	std::unordered_set<OvmCeH> one_sep_chs;
	//	JC::get_direct_adjacent_hexas (mesh, fhs, one_sep_chs);
	//	if (JC::contains (one_sep_chs, outer_ch))
	//		outer_chs = one_sep_chs;
	//	else
	//		sep_chs.push_back (one_sep_chs);
	//}



	//while (true){
	//	bool it_ok = true;

	//	for (int i = 0; i != sep_chs.size () - 1; ++i){
	//		auto &sep_chs1= sep_chs[i];
	//		for (int j = i + 1; j != sep_chs.size (); ++j){
	//			auto &sep_chs2 = sep_chs[j];
	//			if (JC::intersects (sep_chs1, sep_chs2)){
	//				foreach (auto &ch, sep_chs2) sep_chs1.insert (ch);
	//				it_ok = false;
	//				sep_chs.erase (sep_chs.begin () + j);
	//				break;
	//			}
	//		}
	//		if (!it_ok) break;
	//	}
	//	if (it_ok) break;
	//}

	//auto fGetQSet = [&] (const std::unordered_set<OvmCeH> &std_chs_set)->QSet<int>{
	//	QSet<int> q_chs_set;
	//	foreach (auto &ch, std_chs_set) q_chs_set.insert (ch.idx ());
	//	return q_chs_set;
	//};
	//quad_set_data.init_hex_sets.push_back (fGetQSet (outer_chs));
	//foreach (auto &chs, sep_chs)
	//	quad_set_data.init_hex_sets.push_back (fGetQSet (chs));

	//char *colors[] = {"grey", "red", "purple", "blue", "pink", "green"};
	//hoopsview->derender_mesh_groups ("qs", "init hex sets", true);

	//MeshRenderOptions render_options;
	//for (int i = 0; i != quad_set_data.init_hex_sets.size (); ++i){
	//	render_options.cell_color = colors[i];
	//	auto group = new VolumeMeshElementGroup (mesh, "qs", "init hex sets");
	//	foreach (auto idx, quad_set_data.init_hex_sets[i])
	//		group->chs.insert (OvmCeH (idx));
	//	hoopsview->render_mesh_group (group, render_options);
	//}
	//
}


void MeshEditWidget::on_read_data_for_qs ()
{
	char *colors[] = {"red", "blue", "pink", "green"};
	char *colors2[] = {"purple", "blue", "pink", "green"};
	char *colors3[] = {"(r=0 g=0.67 b=0)", "(r=0 g=0.67 b=1)", "(r=1 g=0.33 b=0)", "(r=0.67 g=0 b=1)", "(r=1 g=0.67 b=0)", "(r=0.67 g=0 b=0.50)"};

	QString file_path = QFileDialog::getOpenFileName (this, "选择文件");
	if (file_path != ""){
		quad_set_data.deserialize (file_path);
		//read_qs_data (file_path, qs_loop, qs_obs_fhs, qs_int_path, qs_cross_conf, qs_inf_fhs);
		hoopsview->derender_all_mesh_groups ();

		MeshRenderOptions render_options;
		auto group = new VolumeMeshElementGroup (mesh, "qs", "input edges");
		qs_loop.clear ();
		foreach (auto &idx, quad_set_data.input_edges){
			group->ehs.insert (OvmEgH (idx));
			qs_loop.insert (OvmEgH (idx));
		}
		render_options.edge_color = "red";
		render_options.edge_width = 6;
		hoopsview->render_mesh_group (group, render_options);
		

		if (!quad_set_data.closed_edges.empty ()){
			group = new VolumeMeshElementGroup (mesh, "qs", "closed edges");
			foreach (auto &idx, quad_set_data.closed_edges)
				group->ehs.insert (OvmEgH (idx));
			render_options.edge_color = "blue";
			render_options.edge_width = 6;
			hoopsview->render_mesh_group (group, render_options);
		}

		if (!quad_set_data.appended_edges.empty ()){
			group = new VolumeMeshElementGroup (mesh, "qs", "appended edges");
			foreach (auto &idx, quad_set_data.appended_edges)
				group->ehs.insert (OvmEgH (idx));
			render_options.edge_color = "magenta";
			render_options.edge_width = 6;
			hoopsview->render_mesh_group (group, render_options);
		}

		//画限制面集
		if (!quad_set_data.forbidden_faces.empty ()){
			auto group = new VolumeMeshElementGroup (mesh, "qs", "obs");
			foreach (auto &one_fh, quad_set_data.forbidden_faces){
				group->fhs.insert (OvmFaH (one_fh));
			}
			render_options.face_color = "yellow";
			hoopsview->render_mesh_group (group, render_options);
		}

		//画配对的相交点
		for (int i = 0; i != quad_set_data.int_vh_pairs.size (); ++i) {
			auto &v_p = quad_set_data.int_vh_pairs[i];
			group = new VolumeMeshElementGroup (mesh, "qs", "int vhs pairs");
			group->vhs.insert (v_p.first); group->vhs.insert (v_p.second);
			render_options.vertex_color = colors2[i];
			render_options.vertex_size = 1;
			hoopsview->render_mesh_group (group, render_options);
		}

		//画相交线
		
		render_options.edge_width = 6;
		for (int i = 0; i != quad_set_data.int_paths.size (); ++i){
			auto int_path = quad_set_data.int_paths[i];
			group = new VolumeMeshElementGroup (mesh, "qs", "int path");
			foreach (auto &idx, int_path)
				group->ehs.insert (OvmEgH (idx));

			render_options.edge_color = colors3[i % 6];
			hoopsview->render_mesh_group (group, render_options);
		}
		
		//画相交线处六面体集合
		foreach (auto &one_cross_conf, quad_set_data.int_hex_sets){
			
			for (int i = 0; i != 4; ++i){
				auto &one_hexa_set = one_cross_conf[i];
				auto group = new VolumeMeshElementGroup (mesh, "qs", "cross hexas");
				foreach (auto &idx, one_hexa_set)
					group->chs.insert (OvmCeH (idx));		
				render_options.cell_color = colors[i];
				hoopsview->render_mesh_group (group, render_options);
			}
		}


		if (!qs_inf_fhs.empty ()){
			//hoopsview->derender_mesh_groups ("qs", "inflation fhs", true);
			update_inflation_fhs ();
		}

		QTime time;
		time= QTime::currentTime();
		qsrand(time.msec()+time.second()*1000);



		int xxx=qrand()%3000;

		Sleep (1000 + xxx);
	}
}


void MeshEditWidget::on_save_data_for_qs ()
{
	QString file_path = QFileDialog::getSaveFileName (this, "选择文件");
	if (file_path != ""){
		quad_set_data.input_edges.clear ();
		foreach (auto &eh, qs_loop)
			quad_set_data.input_edges.insert (eh.idx ());

		quad_set_data.serialize (file_path);
		//save_qs_data (qs_loop, qs_obs_fhs, qs_int_path, qs_cross_conf, qs_inf_fhs, file_path);
	}
}

void MeshEditWidget::update_cross_hexas ()
{
	char *colors[] = {"red", "blue", "pink","green"};
	hoopsview->derender_mesh_groups ("qs", "cross hexas", true);

	MeshRenderOptions render_options;
	for (int i = 0; i != 4; ++i){
		auto &one_hexa_set = qs_cross_conf[i];
		auto group = new VolumeMeshElementGroup (mesh, "qs", "cross hexas");
		group->chs = one_hexa_set;		
		render_options.cell_color = colors[i];
		hoopsview->render_mesh_group (group, render_options);
	}

	if (!ungrouped_chs_around_int_path.empty ()){
		auto group = new VolumeMeshElementGroup (mesh, "qs", "cross hexas");
		group->chs = ungrouped_chs_around_int_path;		
		render_options.cell_color = "grey";
		hoopsview->render_mesh_group (group, render_options);
	}

}

void MeshEditWidget::update_inflation_fhs ()
{
	std::map<OvmEgH, std::set<OvmFaH> > eh_sta;
	foreach (auto &fh, qs_inf_fhs){
		std::unordered_set<OvmEgH> adj_ehs;
		JC::get_adj_edges_around_face (mesh, fh, adj_ehs);
		foreach (auto &eh, adj_ehs)
			eh_sta[eh].insert (fh);
	}

	//qs_hexa_set = ossi_handler->base_handler->hexa_set;

	hoopsview->derender_mesh_groups ("qs", "cross hexas", true);
	hoopsview->derender_mesh_groups ("qs", "inflation fhs", true);
	auto group = new VolumeMeshElementGroup (mesh, "qs", "inflation fhs"),
		group2 = new VolumeMeshElementGroup (mesh, "qs", "edges on inflation fhs");

	group->fhs = qs_inf_fhs;

	foreach (auto &p, eh_sta){
		//if (p.second.size () > 1)
		group2->ehs.insert (p.first);
	}

	MeshRenderOptions render_options;
	//render_options.face_color = "(r=0 g=0.67 b=1)";
	render_options.face_color = "red";
	render_options.edge_color = "black";
	render_options.edge_width = 3;
	hoopsview->render_mesh_group (group, render_options);
	hoopsview->show_mesh_faces (false);

	hoopsview->render_mesh_group (group2, render_options);
}

void MeshEditWidget::on_inflate_sheet_for_qs ()
{
	if (qs_inf_fhs.empty ()){
		QMessageBox::warning (this, "ERROR", "qs_inf_fhs为空！");
		return;
	}
	//ossi_handler->base_handler->inflation_fhs = qs_inf_fhs;
	//ossi_handler->base_handler->hexa_set = qs_hexa_set;
	//ossi_handler->base_handler->intersect_path = qs_int_path;

	qs_int_path_set.clear ();

	std::hash_map<OvmEgH, int> ehs_int_counting;
	foreach (auto &fh, qs_inf_fhs){
		std::unordered_set<OvmEgH> adj_ehs;
		JC::get_adj_edges_around_face (mesh, fh, adj_ehs);
		foreach (auto &eh, adj_ehs)
			ehs_int_counting[eh]++;
	}

	qs_int_path_set.clear ();
	foreach (auto &p, ehs_int_counting){
		if (p.second == 4) qs_int_path_set.insert (p.first);
	}
	auto new_sheets = JC::one_simple_sheet_inflation (mesh, qs_inf_fhs, qs_hexa_set, qs_int_path_set);

	//new_sheet = ossi_handler->sheet_inflation ();

	//hoopsview->derender_all_mesh_groups ();
	JC::smooth_volume_mesh (mesh, body, 5, hoopsview);
	hoopsview->rerender_VolumeMesh (mesh);

	char *colors[] = {"red", "(r=0 g=0.67 b=0)", "(r=0 g=0.67 b=1)", "(r=1 g=0.33 b=0)", "(r=0.67 g=0 b=1)", "(r=1 g=0.67 b=0)", "(r=0.67 g=0 b=0.50)"};

	MeshRenderOptions render_options;

	for (int i = 0; i != new_sheets.size (); ++i){
		render_options.cell_color = colors[i % 7];
		hoopsview->render_mesh_group (new_sheets[i], render_options);
	}
	
}

void MeshEditWidget::on_postprocess_for_qs ()
{
	hoopsview->derender_all_mesh_groups ();
	
	//delete ossi_handler;
	ossi_handler = NULL;
	qs_loop.clear (); qs_int_path.clear (); qs_cross_conf.clear (); qs_cross_conf.resize (4); qs_inf_fhs.clear ();
	qs_obs_fhs.clear ();
	selected_vhs.clear (); selected_vhs_set.clear ();
	selected_ehs.clear (); selected_ehs_set.clear ();
	selected_fhs.clear (); selected_fhs_set.clear ();
}


void MeshEditWidget::on_optimize_quad_set_for_qs ()
{
	ui.stackedWidget_for_QS->setCurrentIndex (1);

	hoopsview->derender_mesh_groups ("qs", "inflation fhs", true);
	auto group = new VolumeMeshElementGroup (mesh, "qs", "hexa sets");
	group->chs = qs_hexa_set;
	
	MeshRenderOptions render_options;
	render_options.cell_color = "red";

	hoopsview->render_mesh_group (group, render_options);
	hoopsview->set_mesh_group_selectability (group, false, false, true);

	//////////////////////////////////////////////////////////////////////////
	//不规则边的整体结构显示

	//std::hash_map<OvmEgH, std::unordered_set<OvmFaH> > eh_count_mapping;

	//foreach (auto &fh, qs_inf_fhs){
	//	std::unordered_set<OvmEgH> adj_ehs;
	//	JC::get_adj_edges_around_face (mesh, fh, adj_ehs);
	//	foreach (auto &eh, adj_ehs)
	//		eh_count_mapping[eh].insert (fh);
	//}
	//auto group = new VolumeMeshElementGroup (mesh, "qs", "irregular edges");

	//foreach (auto &p, eh_count_mapping){
	//	if (p.second.size () == 2){
	//		std::vector<OvmFaH> adj_fhs;
	//		JC::unordered_set_to_vector (p.second, adj_fhs);
	//		auto eh_val = JC::edge_valence_change (mesh, p.first, adj_fhs.front (), adj_fhs.back ());
	//		if (std::abs (eh_val) != 0)
	//			group->ehs.insert (p.first);
	//	}
	//}

	//MeshRenderOptions render_options;
	//render_options.edge_color = "red";
	//render_options.edge_width = 6;

	//hoopsview->render_mesh_group (group, render_options);
}

void MeshEditWidget::on_add_hexs_for_qs ()
{
	selected_fhs.clear ();
	if (!hoopsview->get_selected_elements (NULL, NULL, &selected_fhs)){
		QMessageBox::warning (this, tr("警告"), tr("未选中任何网格面！"), QMessageBox::Ok);
		return;
	}

	foreach (auto &fh, selected_fhs){
		if (mesh->is_boundary (fh)) continue;

		auto hfh = mesh->halfface_handle (fh, 0);
		auto ch = mesh->incident_cell (hfh);
		if (JC::contains (qs_hexa_set, ch)){
			hfh = mesh->opposite_halfface_handle (hfh);
			ch = mesh->incident_cell (hfh);
		}
		qs_hexa_set.insert (ch);
	}

	hoopsview->derender_mesh_groups ("qs", "hexa sets", true);

	auto group = new VolumeMeshElementGroup (mesh, "qs", "hexa sets");
	group->chs = qs_hexa_set;
	MeshRenderOptions render_options;
	render_options.cell_color = 
		"red";
	hoopsview->render_mesh_group (group, render_options);
	hoopsview->set_mesh_group_selectability (group, false, false, true);
}

void MeshEditWidget::on_remove_hexs_for_qs ()
{
	selected_fhs.clear ();
	if (!hoopsview->get_selected_elements (NULL, NULL, &selected_fhs)){
		QMessageBox::warning (this, tr("警告"), tr("未选中任何网格面！"), QMessageBox::Ok);
		return;
	}

	foreach (auto &fh, selected_fhs){
		if (mesh->is_boundary (fh)) continue;

		auto hfh = mesh->halfface_handle (fh, 0);
		auto ch = mesh->incident_cell (hfh);
		if (!JC::contains (qs_hexa_set, ch)){
			hfh = mesh->opposite_halfface_handle (hfh);
			ch = mesh->incident_cell (hfh);
		}
		qs_hexa_set.erase (ch);
	}

	hoopsview->derender_mesh_groups ("qs", "hexa sets", true);

	auto group = new VolumeMeshElementGroup (mesh, "qs", "hexa sets");
	group->chs = qs_hexa_set;
	MeshRenderOptions render_options;
	render_options.cell_color = 
		"red";
	hoopsview->render_mesh_group (group, render_options);
	hoopsview->set_mesh_group_selectability (group, false, false, true);
}

void MeshEditWidget::on_modify_quad_set_ok_for_qs ()
{
	ui.stackedWidget_for_QS->setCurrentIndex (0);

	std::unordered_set<OvmFaH> tmp, bnd_fhs;
	JC::collect_boundary_element (mesh, qs_hexa_set, NULL, NULL, &tmp);
	foreach (auto &fh, tmp){
		if (!mesh->is_boundary (fh))
			bnd_fhs.insert (fh);
	}
	qs_inf_fhs = bnd_fhs;

	foreach (auto &fh, qs_inf_fhs)
		quad_set_data.opt_quad_set.insert (fh.idx ());

	foreach (auto &ch, qs_hexa_set)
		quad_set_data.opt_hex_set.insert (ch.idx ());

	hoopsview->derender_mesh_groups ("qs", "hexa sets", true);
	update_inflation_fhs ();
}

void MeshEditWidget::on_get_corres_edges_for_qs ()
{
	if (!ossi_handler){

		ossi_handler = new OneSimpleSheetInflationHandler (mesh, body, hoopsview);
		ossi_handler->init ();

		if (!ossi_handler->analyze_input_edges (qs_loop)){
			QMessageBox::critical (this, tr("错误"), ossi_handler->get_last_err (), QMessageBox::Ok);
			return;
		}
		auto int_vhs = ossi_handler->int_vhs;
		qs_first_int_vh = *(int_vhs.begin ());
	}

	auto adj_ehs = ossi_handler->vh_adj_ehs_mapping[qs_first_int_vh];
	std::vector<OvmVeH> end_vhs;
	foreach (auto &eh, qs_int_path){
		auto eg = mesh->edge (eh);
		auto loc = std::find (end_vhs.begin (), end_vhs.end (), eg.from_vertex ());
		if (loc != end_vhs.end ())
			end_vhs.erase (loc);
		else
			end_vhs.push_back (eg.from_vertex ());
		loc = std::find (end_vhs.begin (), end_vhs.end (), eg.to_vertex ());
		if (loc != end_vhs.end ())
			end_vhs.erase (loc);
		else
			end_vhs.push_back (eg.to_vertex ());
	}

	assert (end_vhs.size () == 2);

	qs_second_int_vh = end_vhs.front () == qs_first_int_vh? end_vhs.back () : end_vhs.front ();

	std::unordered_set<OvmEgH> adj_bnd_ehs;
	JC::get_adj_boundary_edges_around_vertex (mesh, qs_second_int_vh, adj_bnd_ehs);

	auto fGetCrossIdx = [&] (OvmEgH eh)->std::set<int>{
		std::unordered_set<OvmFaH> adj_fhs;
		JC::get_adj_boundary_faces_around_edge (mesh, eh, adj_fhs);
		std::set<int> adj_cross_sets;
		foreach (auto &adj_fh, adj_fhs){
			auto hfh = mesh->halfface_handle (adj_fh, 0);
			auto ch = mesh->incident_cell (hfh);
			if (ch == mesh->InvalidCellHandle){
				hfh = mesh->opposite_halfface_handle (hfh);
				ch = mesh->incident_cell (hfh);
			}
			for (int j = 0; j != 4; ++j){
				if (JC::contains (qs_cross_conf[j], ch)){
					adj_cross_sets.insert (j);
					break;
				}
			}
		}
		return adj_cross_sets;
	};

	
	std::vector<std::pair<OvmEgH, OvmEgH> > eh_pairs;
	foreach (auto &adj_eh, adj_ehs){
		auto adj_cross_sets = fGetCrossIdx (adj_eh);

		bool founded = false;
		foreach (auto &adj_bnd_eh, adj_bnd_ehs){
			auto adj_cross_sets2 = fGetCrossIdx (adj_bnd_eh);
			if (adj_cross_sets == adj_cross_sets2){
				founded = true;
				eh_pairs.push_back (std::make_pair (adj_eh, adj_bnd_eh));
				break;
			}
		}
		assert (founded);
	}

	//////////////////////////////////////////////////////////////////////////
	//render
	char *colors[] = {"brick red", "green", "yellow", "magenta"};
	hoopsview->derender_mesh_groups ("qs", "cross edges");
	MeshRenderOptions render_options;
	for (int i = 0; i != 4; ++i){
		auto group = new VolumeMeshElementGroup (mesh, "qs", "cross edges");
		group->ehs.insert (eh_pairs[i].first);
		group->ehs.insert (eh_pairs[i].second);
		render_options.edge_color = colors[i];
		render_options.edge_width = 12;
		hoopsview->render_mesh_group (group, render_options);
	}
}

void MeshEditWidget::on_optimize_once_for_qs ()
{
	
}

void MeshEditWidget::on_current_quality_for_qs ()
{
	std::unordered_set<OvmFaH> tmp, bnd_fhs;
	JC::collect_boundary_element (mesh, qs_hexa_set, NULL, NULL, &tmp);
	foreach (auto &fh, tmp){
		if (!mesh->is_boundary (fh))
			bnd_fhs.insert (fh);
	}
	auto marks = JC::calc_manifold_edge_marks (mesh, bnd_fhs);
	QMessageBox::information (this, "Marks", QString ("Current edge marks is: %1").arg (marks));
}

void MeshEditWidget::on_which_type_for_qs ()
{
	std::vector<OvmHaEgH> hehs;

	//JC::get_piecewise_halfedges_from_edges (mesh, qs_loop, true, hehs);
}

void MeshEditWidget::on_pair_int_vhs_for_qs ()
{
	ui.stackedWidget_for_QS->setCurrentIndex (2);

	std::hash_map<OvmVeH, std::vector<OvmEgH> > vh_adj_ehs_mapping;
	std::unordered_set<OvmVeH> end_vhs, int_vhs;
	foreach (OvmEgH eh, qs_loop){
		vh_adj_ehs_mapping[mesh->edge (eh).from_vertex ()].push_back (eh);
		vh_adj_ehs_mapping[mesh->edge (eh).to_vertex ()].push_back (eh);
	}
	for (auto it = vh_adj_ehs_mapping.begin (); it != vh_adj_ehs_mapping.end (); ++it)
	{
		//如果一个顶点只和一条边相连，则表示选中的边组合不是完整的
		if (it->second.size () == 1){
			end_vhs.insert (it->first);
		}
		else if (it->second.size () == 4){
			int_vhs.insert (it->first);
		}//end else if (it->second.size () == 4){...
	}

	foreach (auto &p, quad_set_data.int_vh_pairs){
		int_vhs.erase (p.first); int_vhs.erase (p.second);
	}

	auto group = new VolumeMeshElementGroup (mesh, "qs", "intersect vertices");
	MeshRenderOptions render_options;
	render_options.vertex_size = 1;
	render_options.vertex_color = "blue";
	group->vhs = int_vhs;
	hoopsview->render_mesh_group (group, render_options);
	hoopsview->set_mesh_group_selectability (group, true, false, false);
}

void MeshEditWidget::on_pair_one_int_vhs_ok_for_qs ()
{
	selected_vhs.clear ();
	if (!hoopsview->get_selected_elements (&selected_vhs, NULL, NULL)){
		QMessageBox::warning (this, tr("警告"), tr("未选中任何网格点！"), QMessageBox::Ok);
		return;
	}

	if (selected_vhs.size () != 2){
		QMessageBox::warning (this, tr("警告"), tr("请选择两个网格点！"), QMessageBox::Ok);
		return;
	}

	auto group = new VolumeMeshElementGroup (mesh, "qs", "paired intersect vertices");
	MeshRenderOptions render_options;
	render_options.vertex_size = 1;
	render_options.vertex_color = "pink";
	JC::vector_to_unordered_set (selected_vhs, group->vhs);
	hoopsview->render_mesh_group (group, render_options);
	hoopsview->set_mesh_group_selectability (group, true, false, false);

	qs_first_int_vh = *(selected_vhs.begin ());
	qs_second_int_vh = *(selected_vhs.begin () + 1);

	group = new VolumeMeshElementGroup (mesh, "qs", "int path next");

	qs_vh = qs_first_int_vh;

	for (auto voh_it = mesh->voh_iter (qs_vh); voh_it; ++voh_it){
		auto eh = mesh->edge_handle (*voh_it);

		if (mesh->is_boundary (eh)) continue;
		if (mesh->valence (eh) < 4) continue;
		group->ehs.insert (eh);
		//group->vhs.insert (mesh->halfedge (*voh_it).to_vertex ());
	}

	render_options.edge_color = "green";
	render_options.edge_width = 6;

	hoopsview->render_mesh_group (group, render_options);
	hoopsview->set_mesh_group_selectability (group, false, true, false);
}

void MeshEditWidget::on_pair_int_vhs_return_for_qs ()
{
	if (qs_first_int_vh != mesh->InvalidVertexHandle && qs_second_int_vh != mesh->InvalidVertexHandle)
		quad_set_data.int_vh_pairs.push_back (QPair<int,int> (qs_first_int_vh.idx (), qs_second_int_vh.idx ()));

	if (!qs_int_path.empty ()){
		QVector<int> one_int_path;
		foreach (auto &eh, qs_int_path){
			one_int_path.push_back (eh.idx ());
		}

		quad_set_data.int_paths.push_back (one_int_path);

		if (!qs_cross_conf.empty ()){
			QVector<QSet<int> > one_cross_conf;
			foreach (auto &hs, qs_cross_conf){
				QSet<int> tmp_hs;
				foreach (auto &h, hs) tmp_hs.insert (h.idx ());
				one_cross_conf.push_back (tmp_hs);
			}
			quad_set_data.int_hex_sets.push_back (one_cross_conf);
		}
	}

	
	
	hoopsview->derender_mesh_groups ("qs", "cross hexas", true);
	hoopsview->derender_mesh_groups ("qs", "intersect vertices", true);
	hoopsview->derender_mesh_groups ("qs", "paired intersect vertices", true);
	hoopsview->derender_mesh_groups ("qs", "int path", true);

	ui.stackedWidget_for_QS->setCurrentIndex (0);

	qs_first_int_vh = qs_second_int_vh = mesh->InvalidVertexHandle;
	qs_int_path.clear (); qs_cross_conf.clear ();

}

void MeshEditWidget::on_close_loop_for_qs ()
{
	selected_ehs.clear ();
	if (!hoopsview->get_selected_elements (NULL, &selected_ehs, NULL)){
		QMessageBox::warning (this, tr("警告"), tr("未选中任何边！"), QMessageBox::Ok);
		return;
	}

	hoopsview->derender_mesh_groups ("qs", "closed edges", true);

	foreach (auto &eh, selected_ehs)
		quad_set_data.closed_edges.insert (eh.idx ());

	auto group = new VolumeMeshElementGroup (mesh, "qs", "closed edges");
	JC::vector_to_unordered_set (selected_ehs, group->ehs);

	MeshRenderOptions render_options;
	render_options.edge_color = "blue";
	render_options.edge_width = 6;
	hoopsview->render_mesh_group (group, render_options);
}

void MeshEditWidget::on_append_loop_for_qs ()
{
	selected_ehs.clear ();
	if (!hoopsview->get_selected_elements (NULL, &selected_ehs, NULL)){
		QMessageBox::warning (this, tr("警告"), tr("未选中任何边！"), QMessageBox::Ok);
		return;
	}

	hoopsview->derender_mesh_groups ("qs", "appended edges", true);

	foreach (auto &eh, selected_ehs)
		quad_set_data.closed_edges.insert (eh.idx ());

	auto group = new VolumeMeshElementGroup (mesh, "qs", "appended edges");
	JC::vector_to_unordered_set (selected_ehs, group->ehs);

	MeshRenderOptions render_options;
	render_options.edge_color = "magenta";
	render_options.edge_width = 6;
	hoopsview->render_mesh_group (group, render_options);
}

void MeshEditWidget::on_show_int_diagram_for_qs ()
{

}

std::unordered_set<OvmCeH> aget_chs_within_depth (VolumeMesh *mesh, int max_depth, std::unordered_set<OvmFaH> &interface_fhs,
	std::unordered_set<OvmFaH> &adjoint_fhs, std::unordered_set<OvmFaH> &side_fhs)
{
	//对深度进行控制
	std::unordered_set<OvmCeH> chs_less_than_min_depth;
	int depth = 0;
	auto curr_front_fhs = interface_fhs;
	while (depth++ < max_depth){
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
		std::unordered_set<OvmFaH> bound_fhs;
		JC::collect_boundary_element (mesh, chs_less_than_min_depth, NULL, NULL, &bound_fhs);
		//剔除掉bound_fhs中在interface_fhs上以及在侧面表面上的四边形(其实就是在几何表面的四边形)
		foreach (auto &fh, bound_fhs){
			if (!mesh->is_boundary (fh))
				curr_front_fhs.insert (fh);
			else{
				if (!JC::contains (interface_fhs, fh))
					side_fhs.insert (fh);
			}
		}
	}
	adjoint_fhs = curr_front_fhs; 
	return chs_less_than_min_depth;
}

std::unordered_set<OvmCeH> aget_chs_within_depth2 (VolumeMesh *mesh, int max_depth, 
	std::unordered_set<OvmFaH> &fhs, std::unordered_set<OvmFaH> &rest_fhs,
	std::unordered_set<OvmFaH> &adjoint_fhs)
{
	int depth = 0;
	auto curr_front_fhs = fhs, curr_rest_fhs = rest_fhs;
	std::unordered_set<OvmCeH> wanted_chs, rest_chs;
	while (depth++ < max_depth){
		JC::get_direct_adjacent_hexas (mesh, curr_front_fhs, wanted_chs);

		//获得curr_front_fhs上的点
		std::unordered_set<OvmVeH> curr_rest_vhs;
		foreach (auto &fh, curr_rest_fhs){
			auto heh_vec = mesh->face (fh).halfedges ();
			foreach (auto &heh, heh_vec)
				curr_rest_vhs.insert (mesh->halfedge (heh).to_vertex ());
		}
		//由这些点得到所有的相邻六面体
		foreach (auto &vh, curr_rest_vhs){
			JC::get_adj_hexas_around_vertex (mesh, vh, rest_chs);
		}

		foreach (auto &ch, wanted_chs){
			rest_chs.erase (ch);
		}

		//得到表面四边形面集
		curr_front_fhs.clear (); curr_rest_fhs.clear ();
		std::unordered_set<OvmFaH> bound_front_fhs, bound_rest_fhs;
		JC::collect_boundary_element (mesh, wanted_chs, NULL, NULL, &bound_front_fhs);
		JC::collect_boundary_element (mesh, rest_chs, NULL, NULL, &bound_rest_fhs);
		foreach (auto &fh, bound_front_fhs){
			if (!mesh->is_boundary (fh) && !JC::contains (bound_rest_fhs, fh))
				curr_front_fhs.insert (fh);
		}
		foreach (auto &fh, bound_rest_fhs){
			if (!mesh->is_boundary (fh) && !JC::contains (bound_front_fhs, fh))
				curr_rest_fhs.insert (fh);
		}
	}
	std::unordered_set<OvmFaH> bound_fhs;
	JC::collect_boundary_element (mesh, wanted_chs, NULL, NULL, &bound_fhs);
	foreach (auto &fh, bound_fhs){
		if (!mesh->is_boundary (fh))
			adjoint_fhs.insert (fh);
	}

	return wanted_chs;
}

void MeshEditWidget::on_get_qs_by_pcs_for_qs ()
{
	if (selected_ehs.empty ()) return;

	auto seed_eh = selected_ehs.front ();
	std::unordered_set<OvmFaH> bound_fhs_adj_seed_eh;
	JC::get_adj_boundary_faces_around_edge (mesh, seed_eh, bound_fhs_adj_seed_eh);
	auto f = JC::get_associated_geometry_face_of_boundary_fh (mesh, *(bound_fhs_adj_seed_eh.begin ()));

	std::unordered_set<OvmFaH> interface_fhs;
	JC::get_fhs_on_acis_face (mesh, f, interface_fhs);
	std::unordered_set<OvmEgH> sep_ehs;
	JC::vector_to_unordered_set (selected_ehs, sep_ehs);
	std::vector<std::unordered_set<OvmFaH> > fhs_patches;
	JC::get_separate_fhs_patches (mesh, interface_fhs, sep_ehs, fhs_patches);
	if (fhs_patches.size () != 2) return;
	auto smaller_patch = fhs_patches.front ().size () < fhs_patches.back ().size ()?
		fhs_patches.front () : fhs_patches.back ();
	auto larger_patch = fhs_patches.front ().size () < fhs_patches.back ().size ()?
		fhs_patches.back () : fhs_patches.front ();

	int max_depth = QInputDialog::getInt (this, "深度", "输入深度", 3, 1, 10000);
	std::unordered_set<OvmFaH> front_fhs;
	auto chs_within_depth = aget_chs_within_depth2 (mesh, max_depth, smaller_patch, larger_patch, front_fhs);

	qs_inf_fhs = front_fhs;
	qs_hexa_set = chs_within_depth;

	hoopsview->derender_mesh_groups ("qs", "quad set for inflation", true);
	auto group = new VolumeMeshElementGroup (mesh, "qs", "quad set for inflation");
	group->fhs = qs_inf_fhs;
	foreach (auto &fh, group->fhs){
		auto heh_vec = mesh->face (fh).halfedges ();
		foreach (auto &heh, heh_vec){
			auto eh = mesh->edge_handle (heh);
			group->ehs.insert (eh);
		}
	}

	char *colors[] = {"(r=0 g=0.67 b=0)", "(r=0 g=0.67 b=1)", "(r=1 g=0.33 b=0)", "(r=0.67 g=0 b=1)", "(r=1 g=0.67 b=0)", "(r=0.67 g=0 b=0.50)"};

	MeshRenderOptions render_options;
	render_options.face_color = colors[1];
	render_options.edge_color = "black";
	render_options.edge_width = 6;
	hoopsview->render_mesh_group (group, render_options);
	hoopsview->show_mesh_faces (false);
}

void MeshEditWidget::on_get_qs_by_sweeping_for_qs ()
{
	if (selected_ehs.empty ()){
		QMessageBox::warning (this, tr("警告"), tr("未选中任何网格边！"), QMessageBox::Ok);
		return;
	}
	std::unordered_set<OvmFaH> inf_fhs;
	foreach (auto &eh, selected_ehs){
		auto adj_fhs = JC::get_adj_faces_around_edge (mesh, eh);
		foreach (auto &fh, adj_fhs){
			if (!mesh->is_boundary (fh))
				inf_fhs.insert (fh);
		}
	}
	foreach (auto &fh, qs_inf_fhs)
		inf_fhs.insert (fh);

	std::unordered_set<OvmCeH> all_adj_chs;
	foreach (auto &fh, inf_fhs){
		auto hfh = mesh->halfface_handle (fh, 0);
		auto ch = mesh->incident_cell (hfh);
		assert (ch != mesh->InvalidCellHandle);
		all_adj_chs.insert (ch);
		hfh = mesh->halfface_handle (fh, 1);
		ch = mesh->incident_cell (hfh);
		assert (ch != mesh->InvalidCellHandle);
		all_adj_chs.insert (ch);
	}

	auto fGetFrontEhs = [&] (std::unordered_set<OvmFaH> &fhs, std::unordered_set<OvmEgH> &front_ehs)->bool{
		std::unordered_set<OvmEgH> bound_ehs;
		std::map<OvmFaH, std::set<OvmEgH> > fhs_edges_mapping;
		foreach (auto &fh, fhs){
			auto heh_vec = mesh->face (fh).halfedges ();
			foreach (auto &heh, heh_vec){
				auto eh = mesh->edge_handle (heh);
				fhs_edges_mapping[fh].insert (eh);
				if (JC::contains (bound_ehs, eh))
					bound_ehs.erase (eh);
				else
					bound_ehs.insert (eh);
			}
		}
		foreach (auto &eh, bound_ehs){
			if (!mesh->is_boundary (eh))
				front_ehs.insert (eh);
		}
		std::map<OvmFaH, int> front_ehs_fhs_count;
		foreach (auto &eh, front_ehs){
			foreach (auto &p, fhs_edges_mapping){
				if (JC::contains (p.second, eh))
					front_ehs_fhs_count[p.first]++;
			}
		}
		foreach (auto &p, front_ehs_fhs_count){
			if (p.second >= 2) return true;
		}
		if (front_ehs.empty ()) return true;
		return false;
	};


	std::unordered_set<OvmEgH> curr_front_ehs;
	auto should_stop = fGetFrontEhs (inf_fhs, curr_front_ehs);
	while (!should_stop){
		foreach (auto &eh, curr_front_ehs){
			auto adj_fhs = JC::get_adj_faces_around_edge (mesh, eh);
			foreach (auto &fh, adj_fhs){
				auto hfh = mesh->halfface_handle (fh, 0);
				auto ch = mesh->incident_cell (hfh);
				assert (ch != mesh->InvalidCellHandle);
				if (JC::contains (all_adj_chs, ch))
					continue;
				hfh = mesh->halfface_handle (fh, 1);
				ch = mesh->incident_cell (hfh);
				assert (ch != mesh->InvalidCellHandle);
				if (JC::contains (all_adj_chs, ch))
					continue;
				inf_fhs.insert (fh);
				break;
			}
		}
		curr_front_ehs.clear ();
		should_stop = fGetFrontEhs (inf_fhs, curr_front_ehs);
		all_adj_chs.clear ();

		foreach (auto &fh, inf_fhs){
			auto hfh = mesh->halfface_handle (fh, 0);
			auto ch = mesh->incident_cell (hfh);
			assert (ch != mesh->InvalidCellHandle);
			all_adj_chs.insert (ch);
			hfh = mesh->halfface_handle (fh, 1);
			ch = mesh->incident_cell (hfh);
			assert (ch != mesh->InvalidCellHandle);
			all_adj_chs.insert (ch);
		}
	}
	qs_inf_fhs = inf_fhs;

	hoopsview->derender_mesh_groups ("qs", "quad set for inflation", true);
	auto group = new VolumeMeshElementGroup (mesh, "qs", "quad set for inflation");
	group->fhs = inf_fhs;
	foreach (auto &fh, group->fhs){
		auto heh_vec = mesh->face (fh).halfedges ();
		foreach (auto &heh, heh_vec){
			auto eh = mesh->edge_handle (heh);
			group->ehs.insert (eh);
		}
	}

	char *colors[] = {"(r=0 g=0.67 b=0)", "(r=0 g=0.67 b=1)", "(r=1 g=0.33 b=0)", "(r=0.67 g=0 b=1)", "(r=1 g=0.67 b=0)", "(r=0.67 g=0 b=0.50)"};

	MeshRenderOptions render_options;
	render_options.face_color = colors[1];
	render_options.edge_color = "black";
	render_options.edge_width = 6;
	hoopsview->render_mesh_group (group, render_options);
	hoopsview->show_mesh_faces (false);
	hoopsview->begin_select_by_click ();
	hoopsview->set_edges_selectable (true);
}

#include "StdAfx.h"
#include "mesh_min_cut.h"
#include "max_flow_min_cut.h"

void rate_inner_vhs (VolumeMesh *mesh, std::unordered_set<OvmCeH> &s_chs, std::unordered_set<OvmCeH> &t_chs, 
	std::vector<std::unordered_set<OvmVeH> > &layered_vhs)
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

	std::unordered_set<OvmVeH> s_bnd_vhs, t_bnd_vhs;
	std::unordered_set<OvmFaH> tmp, s_bnd_fhs, t_bnd_fhs;
	JC::collect_boundary_element (mesh, s_chs, NULL, NULL, &tmp);
	foreach (auto &fh, tmp){
		if (!mesh->is_boundary (fh))
			s_bnd_fhs.insert (fh);
	}
	foreach (auto &fh, s_bnd_fhs){
		auto adj_vhs = JC::get_adj_vertices_around_face (mesh, fh);
		foreach (auto &vh, adj_vhs)
			s_bnd_vhs.insert (vh);
	}
	tmp.clear ();
	JC::collect_boundary_element (mesh, t_chs, NULL, NULL, &tmp);
	foreach (auto &fh, tmp){
		if (!mesh->is_boundary (fh))
			t_bnd_fhs.insert (fh);
	}
	foreach (auto &fh, t_bnd_fhs){
		auto adj_vhs = JC::get_adj_vertices_around_face (mesh, fh);
		foreach (auto &vh, adj_vhs)
			t_bnd_vhs.insert (vh);
	}

	std::unordered_set<OvmVeH> vhs_on_front, vhs_on_new_front, vhs_left, vhs_visited;

	auto fGetFront = [&] (std::unordered_set<OvmCeH> &chs, std::unordered_set<OvmVeH> &bnd_vhs){
		std::unordered_set<OvmVeH> all_vhs;
		foreach (auto &ch, chs){
			auto adj_vhs = JC::get_adj_vertices_around_cell (mesh, ch);
			foreach (auto &vh, adj_vhs)
				all_vhs.insert (vh);
		}

		foreach (auto &vh, all_vhs){
			vhs_visited.insert (vh);
			V_LEVEL[vh] = 0;
			if (JC::contains (bnd_vhs, vh)){
				vhs_on_front.insert (vh);
			}
		}
	};

	fGetFront (s_chs, s_bnd_vhs);
	fGetFront (t_chs, t_bnd_vhs);

	for (auto v_it = mesh->vertices_begin (); v_it != mesh->vertices_end (); ++v_it){
		auto vh = *v_it;
		if (!JC::contains (vhs_visited, vh))
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

std::unordered_set<OvmFaH> get_volume_mesh_min_cut (VolumeMesh *mesh, std::unordered_set<OvmCeH> &s_chs, std::unordered_set<OvmCeH> &t_chs)
{
	std::vector<std::unordered_set<OvmVeH> > layered_vhs;
	rate_inner_vhs (mesh, s_chs, t_chs, layered_vhs);

	auto V_LEVEL = mesh->request_vertex_property<int> ("node level");
	auto V_MAX_LEVEL = mesh->request_mesh_property<int> ("max node level");
	const int INFINITY_LEVEL = std::numeric_limits<int>::max () / 1000;
	const int MAX_LEVEL = V_MAX_LEVEL[0];

	//建立网格面句柄和无向图中节点序号之间的映射关系
	std::hash_map<OvmCeH, int> c_n_mapping;
	std::hash_map<int, OvmCeH> n_c_mapping;
	std::unordered_set<OvmCeH> normal_chs;

	for (auto c_it = mesh->cells_begin (); c_it != mesh->cells_end (); ++c_it){
		auto ch = *c_it;
		if (!JC::contains (s_chs, ch) && !JC::contains (t_chs, ch)){
			normal_chs.insert (ch);
			int cur_idx = normal_chs.size ();
			c_n_mapping.insert (std::make_pair(ch, cur_idx));
			n_c_mapping.insert (std::make_pair(cur_idx, ch));
		}
	}

	//构建有向图
	int N_node = normal_chs.size () + 2;
	int S_idx = 0, T_idx = N_node - 1;
	CoreSolver<int> graph (N_node, S_idx, T_idx);

	auto fRateFh = [&] (OvmFaH fh) -> int {
		auto adj_vhs = JC::get_adj_vertices_around_face (mesh, fh);
		int all_vh_level = 0;
		foreach (auto &adj_vh, adj_vhs){
			int vh_level = V_LEVEL[adj_vh];
			all_vh_level += MAX_LEVEL - vh_level;
		}
		return (all_vh_level) / adj_vhs.size () * 4;
		//return 1;
	};

	auto fInsertGraphEdges = [&] (std::unordered_set<OvmFaH> &fhs1, std::unordered_set<OvmFaH> &fhs2,
		int idx1, int idx2){
			int all_level = 0;
			if (fhs1.size () < fhs2.size ()){
				foreach (auto &fh1, fhs1){
					if (JC::contains (fhs2, fh1)){
						auto rate_level = fRateFh (fh1);
						all_level += rate_level;
					}
				}
			}else{
				foreach (auto &fh2, fhs2){
					if (JC::contains (fhs1, fh2)){
						auto rate_level = fRateFh (fh2);
						all_level += rate_level;
					}
				}
			                                                                                                                                 }

			if (all_level > 0){
				graph.insert_edge (idx1, idx2, all_level);
			}
	};

	//连接s和t
	std::unordered_set<OvmFaH> s_bnd_fhs, t_bnd_fhs;
	JC::collect_boundary_element (mesh, s_chs, NULL, NULL, &s_bnd_fhs);
	JC::collect_boundary_element (mesh, t_chs, NULL, NULL, &t_bnd_fhs);
	fInsertGraphEdges (s_bnd_fhs, t_bnd_fhs, S_idx, T_idx);

	foreach (auto &nor_ch, normal_chs){
		std::unordered_set<OvmCeH> adj_chs;
		JC::get_adj_hexas_around_hexa (mesh, nor_ch, adj_chs);
		std::unordered_set<OvmFaH> adj_fhs;
		JC::get_adj_faces_around_hexa (mesh, nor_ch, adj_fhs);

		int nor_ch_idx = c_n_mapping[nor_ch];
		bool adj_to_s = false, adj_to_t = false;
		std::unordered_set<OvmCeH> adj_nor_chs;
		foreach (auto &adj_ch, adj_chs){
			if (JC::contains (s_chs, adj_ch))
				adj_to_s = true;
			else if (JC::contains (t_chs, adj_ch))                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
				adj_to_t = true;
			else
				adj_nor_chs.insert (adj_ch);
		}
		if (adj_to_s)
			fInsertGraphEdges (s_bnd_fhs, adj_fhs, S_idx, nor_ch_idx);
		if (adj_to_t)
			fInsertGraphEdges (adj_fhs, t_bnd_fhs, nor_ch_idx, T_idx);
		foreach (auto &adj_nor_ch, adj_nor_chs){
			int adj_nor_ch_idx = c_n_mapping[adj_nor_ch];
			std::unordered_set<OvmFaH> adj_nor_ch_adj_fhs;
			JC::get_adj_faces_around_hexa (mesh, adj_nor_ch, adj_nor_ch_adj_fhs);
			fInsertGraphEdges (adj_fhs, adj_nor_ch_adj_fhs, nor_ch_idx, adj_nor_ch_idx);
		}
	}

	QString max_flow_str = QString ("Max flow is %1").arg (graph.ford_fulkerson());
	auto min_cut = graph.mincut ();

	//给定一个图中的节点序号，求得它的外围面，要分四种情况
	auto fGetTmpBndFhs = [&] (int tmp_idx) -> std::unordered_set<OvmFaH>{
		std::unordered_set<OvmFaH> bnd_fhs;
		if (tmp_idx == S_idx)
			bnd_fhs = s_bnd_fhs;
		else if (tmp_idx == T_idx)
			bnd_fhs = t_bnd_fhs;
		else{
			auto nor_ch = n_c_mapping[tmp_idx];
			JC::get_adj_faces_around_hexa (mesh, nor_ch, bnd_fhs);
		}
		return bnd_fhs;
	};

	auto fGetSharedFhs = [] (std::unordered_set<OvmFaH> &fhs1, std::unordered_set<OvmFaH> &fhs2)->std::unordered_set<OvmFaH>{
		std::unordered_set<OvmFaH> shared_fhs;
		foreach (auto &fh, fhs1){
			if (JC::contains (fhs2, fh))
				shared_fhs.insert (fh);
		}
		return shared_fhs;
	};

	std::unordered_set<OvmFaH> min_cut_fhs;
	for (int i = 0; i != min_cut.size (); ++i){
		auto bnd_fhs1 = fGetTmpBndFhs (min_cut[i].first), 
			bnd_fhs2 = fGetTmpBndFhs (min_cut[i].second);

		auto shared_fhs = fGetSharedFhs (bnd_fhs1, bnd_fhs2);
		foreach (auto &fh, shared_fhs)
			min_cut_fhs.insert (fh);
	}

	return min_cut_fhs;
}

std::unordered_set<OvmFaH> get_local_mesh_min_cut (VolumeMesh *mesh, std::unordered_set<OvmCeH> &all_chs, std::unordered_set<OvmCeH> &s_chs, std::unordered_set<OvmCeH> &t_chs)
{
	//建立网格面句柄和无向图中节点序号之间的映射关系
	std::hash_map<OvmCeH, int> c_n_mapping;
	std::hash_map<int, OvmCeH> n_c_mapping;
	std::unordered_set<OvmCeH> normal_chs;

	for (auto c_it = mesh->cells_begin (); c_it != mesh->cells_end (); ++c_it){
		auto ch = *c_it;
		if (!JC::contains (s_chs, ch) && !JC::contains (t_chs, ch)){
			normal_chs.insert (ch);
			int cur_idx = normal_chs.size ();
			c_n_mapping.insert (std::make_pair(ch, cur_idx));
			n_c_mapping.insert (std::make_pair(cur_idx, ch));
		}
	}

	//构建有向图
	int N_node = normal_chs.size () + 2;
	int S_idx = 0, T_idx = N_node - 1;
	CoreSolver<int> graph (N_node, S_idx, T_idx);

	auto fInsertGraphEdges = [&] (std::unordered_set<OvmFaH> &fhs1, std::unordered_set<OvmFaH> &fhs2,
		int idx1, int idx2){
			int all_level = 0;
			if (fhs1.size () < fhs2.size ()){
				foreach (auto &fh1, fhs1){
					if (JC::contains (fhs2, fh1)){
						auto rate_level = 1;//fRateFh (fh1);
						all_level += rate_level;
					}
				}
			}else{
				foreach (auto &fh2, fhs2){
					if (JC::contains (fhs1, fh2)){
						auto rate_level = 1;//fRateFh (fh2);
						all_level += rate_level;
					}
				}
			}

			if (all_level > 0){
				graph.insert_edge (idx1, idx2, all_level);
			}
	};

	//连接s和t
	std::unordered_set<OvmFaH> s_bnd_fhs, t_bnd_fhs;
	JC::collect_boundary_element (mesh, s_chs, NULL, NULL, &s_bnd_fhs);
	JC::collect_boundary_element (mesh, t_chs, NULL, NULL, &t_bnd_fhs);
	fInsertGraphEdges (s_bnd_fhs, t_bnd_fhs, S_idx, T_idx);

	foreach (auto &nor_ch, normal_chs){
		std::unordered_set<OvmCeH> adj_chs;
		JC::get_adj_hexas_around_hexa (mesh, nor_ch, adj_chs);
		std::unordered_set<OvmFaH> adj_fhs;
		JC::get_adj_faces_around_hexa (mesh, nor_ch, adj_fhs);

		int nor_ch_idx = c_n_mapping[nor_ch];
		bool adj_to_s = false, adj_to_t = false;
		std::unordered_set<OvmCeH> adj_nor_chs;
		foreach (auto &adj_ch, adj_chs){
			if (JC::contains (s_chs, adj_ch))
				adj_to_s = true;
			else if (JC::contains (t_chs, adj_ch))                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
				adj_to_t = true;
			else
				adj_nor_chs.insert (adj_ch);
		}
		if (adj_to_s)
			fInsertGraphEdges (s_bnd_fhs, adj_fhs, S_idx, nor_ch_idx);
		if (adj_to_t)
			fInsertGraphEdges (adj_fhs, t_bnd_fhs, nor_ch_idx, T_idx);
		foreach (auto &adj_nor_ch, adj_nor_chs){
			int adj_nor_ch_idx = c_n_mapping[adj_nor_ch];
			std::unordered_set<OvmFaH> adj_nor_ch_adj_fhs;
			JC::get_adj_faces_around_hexa (mesh, adj_nor_ch, adj_nor_ch_adj_fhs);
			fInsertGraphEdges (adj_fhs, adj_nor_ch_adj_fhs, nor_ch_idx, adj_nor_ch_idx);
		}
	}

	QString max_flow_str = QString ("Max flow is %1").arg (graph.ford_fulkerson());
	auto min_cut = graph.mincut ();

	//给定一个图中的节点序号，求得它的外围面，要分四种情况
	auto fGetTmpBndFhs = [&] (int tmp_idx) -> std::unordered_set<OvmFaH>{
		std::unordered_set<OvmFaH> bnd_fhs;
		if (tmp_idx == S_idx)
			bnd_fhs = s_bnd_fhs;
		else if (tmp_idx == T_idx)
			bnd_fhs = t_bnd_fhs;
		else{
			auto nor_ch = n_c_mapping[tmp_idx];
			JC::get_adj_faces_around_hexa (mesh, nor_ch, bnd_fhs);
		}
		return bnd_fhs;
	};

	auto fGetSharedFhs = [] (std::unordered_set<OvmFaH> &fhs1, std::unordered_set<OvmFaH> &fhs2)->std::unordered_set<OvmFaH>{
		std::unordered_set<OvmFaH> shared_fhs;
		foreach (auto &fh, fhs1){
			if (JC::contains (fhs2, fh))
				shared_fhs.insert (fh);
		}
		return shared_fhs;
	};

	std::unordered_set<OvmFaH> min_cut_fhs;
	for (int i = 0; i != min_cut.size (); ++i){
		auto bnd_fhs1 = fGetTmpBndFhs (min_cut[i].first), 
			bnd_fhs2 = fGetTmpBndFhs (min_cut[i].second);

		auto shared_fhs = fGetSharedFhs (bnd_fhs1, bnd_fhs2);
		foreach (auto &fh, shared_fhs)
			min_cut_fhs.insert (fh);
	}

	return min_cut_fhs;
}

std::unordered_set<OvmEgH> get_quad_mesh_min_cut (VolumeMesh *mesh, 
	std::unordered_set<OvmFaH> &s_fhs, std::unordered_set<OvmFaH> &t_fhs,
	std::vector<std::unordered_set<OvmFaH> > &obstacles)
{
	auto V_LEVEL = mesh->request_vertex_property<int> ("node level");
	auto V_MAX_LEVEL = mesh->request_mesh_property<int> ("max node level");
	const int INFINITY_LEVEL = std::numeric_limits<int>::max () / 1000;
	const int MAX_LEVEL = V_MAX_LEVEL[0];
	//构建有向图
	std::unordered_set<OvmFaH> normal_fhs;
	for (auto bf_it = mesh->bf_iter (); bf_it; ++bf_it){
		auto fh = *bf_it;
		if (JC::contains (s_fhs, fh) || JC::contains (t_fhs, fh)) continue;
		bool in_ob = false;
		foreach (auto &ob, obstacles){
			if (JC::contains (ob, fh)){
				in_ob = true; break;
			}
		}
		if (!in_ob)
			normal_fhs.insert (fh);
	}

	int N_node = normal_fhs.size () + 2 + obstacles.size ();
	int S_idx = 0, T_idx = N_node - 1;
	CoreSolver<int> graph (N_node, S_idx, T_idx);

	auto fWhichOb = [&] (OvmFaH test_fh, int &ob_idx)->bool{
		for (int i = 0; i != obstacles.size (); ++i){
			if (JC::contains (obstacles[i], test_fh)){
				ob_idx = i;
				return true;
			}
		}
		return false;
	};

	//0号节点是s节点，最后一个节点是t节点，ob节点排布在1~ ob.size
	std::hash_map<OvmFaH, int> f_n_mapping;
	std::hash_map<int, OvmFaH> n_f_mapping;
	int cur_idx = 1 + obstacles.size ();

	foreach (auto &fh, normal_fhs){
		f_n_mapping.insert (std::make_pair (fh, cur_idx));
		n_f_mapping.insert (std::make_pair (cur_idx, fh));
		cur_idx++;
	}

	auto fGetBndEhs = [&] (std::unordered_set<OvmFaH> &fhs)->std::unordered_set<OvmEgH>{
		std::unordered_set<OvmEgH> bnd_ehs;
		foreach (auto &fh, fhs){
			auto hehs = mesh->face (fh).halfedges ();
			foreach (auto &heh, hehs){
				auto eh = mesh->edge_handle (heh);
				if (JC::contains (bnd_ehs, eh))	bnd_ehs.erase (eh);
				else bnd_ehs.insert (eh);
			}
		}
		return bnd_ehs;
	};

	auto fRateEh = [&] (OvmEgH eh) -> int {
		auto vh1 = mesh->edge (eh).from_vertex (),
			vh2 = mesh->edge (eh).to_vertex ();
		auto vh1_level = V_LEVEL[vh1],
			vh2_level = V_LEVEL[vh2];
		if (vh1_level == 0 && vh2_level == 0)
			return INFINITY_LEVEL;
		return ((MAX_LEVEL - vh1_level) + (MAX_LEVEL - vh2_level)) / 2;
	};

	auto fInsertGraphEdges = [&] (std::unordered_set<OvmEgH> &ehs1, std::unordered_set<OvmEgH> &ehs2,
		int idx1, int idx2, bool mutual){
		int all_level = 0;
		if (ehs1.size () < ehs2.size ()){
			foreach (auto &eh1, ehs1){
				if (JC::contains (ehs2, eh1)){
					auto rate_level = fRateEh (eh1);
					all_level += rate_level;
				}
			}
		}else{
			foreach (auto &eh2, ehs2){
				if (JC::contains (ehs1, eh2)){
					auto rate_level = fRateEh (eh2);
					all_level += rate_level;
				}
			}
		}

		if (all_level > 0){
			graph.insert_edge (idx1, idx2, all_level);
			if (mutual)
				graph.insert_edge (idx2, idx1, all_level);
		}
	};

	//首先处理s、t和ob之间的连接关系
	auto s_bnd_ehs = fGetBndEhs (s_fhs),
		t_bnd_ehs = fGetBndEhs (t_fhs);

	//s和t之间
	//if (JC::intersects (s_bnd_ehs, t_bnd_ehs))
	//	graph.insert_edge (S_idx, T_idx, 1);

	fInsertGraphEdges (s_bnd_ehs, t_bnd_ehs, S_idx, T_idx, false);

	//s和ob之间 ob和t之间
	std::vector<std::unordered_set<OvmEgH> > ob_bnd_ehs;
	for (int i = 0; i != obstacles.size (); ++i){
		auto one_ob_bnd_ehs = fGetBndEhs (obstacles[i]);
		//if (JC::intersects (s_bnd_ehs, one_ob_bnd_ehs))
		//	graph.insert_edge (S_idx, i, 1);
		//if (JC::intersects (one_ob_bnd_ehs, t_bnd_ehs))
		//	graph.insert_edge (i, T_idx, 1);

		fInsertGraphEdges (s_bnd_ehs, one_ob_bnd_ehs, S_idx, i, false);
		fInsertGraphEdges (one_ob_bnd_ehs, t_bnd_ehs, i, T_idx, false);
		ob_bnd_ehs.push_back (one_ob_bnd_ehs);
	}

	//ob之间
	for (int i = 0; i != obstacles.size () - 1; ++i){
		auto ob_bnd_ehs_i = ob_bnd_ehs[i];
		for (int j = i + 1; j != obstacles.size (); ++j){
			auto ob_bnd_ehs_j = ob_bnd_ehs[j];
			//if (JC::intersects (ob_bnd_ehs_i, ob_bnd_ehs_j)){
			//	graph.insert_edge (1 + i, 1 + j, 1);
			//	graph.insert_edge (1 + j, 1 + i, 1);
			//}
			fInsertGraphEdges (ob_bnd_ehs_i, ob_bnd_ehs_j, i, j, true);
		}
	}

	//普通节点之间
	foreach (auto &fh, normal_fhs){
		auto fh_idx = f_n_mapping[fh];
		std::unordered_set<OvmFaH> adj_fhs;
		JC::get_adj_boundary_faces_around_face (mesh, fh, adj_fhs);
		std::unordered_set<OvmEgH> adj_ehs;
		JC::get_adj_edges_around_face (mesh, fh, adj_ehs);

		bool adj_to_S = false, adj_to_T = false;
		std::set<int> ob_indices;
		std::set<OvmFaH> nor_fhs;
		foreach (auto &adj_fh, adj_fhs){
			int ob_idx = -1;
			if (JC::contains (s_fhs, adj_fh))
				adj_to_S = true;
			else if (JC::contains (t_fhs, adj_fh))
				adj_to_T = true;
			else if (fWhichOb (adj_fh, ob_idx))
				ob_indices.insert (ob_idx);
			else{
				
				nor_fhs.insert (adj_fh);
			}
		}
		if (adj_to_S)
			//graph.insert_edge (S_idx, fh_idx, 1);
			fInsertGraphEdges (s_bnd_ehs, adj_ehs, S_idx, fh_idx, false);
		if (adj_to_T)
			//graph.insert_edge (fh_idx, T_idx, 1);
			fInsertGraphEdges (adj_ehs, t_bnd_ehs, fh_idx, T_idx, false);
		foreach (auto &ob_idx, ob_indices){
			//graph.insert_edge (ob_idx, fh_idx, 1);
			//graph.insert_edge (fh_idx, ob_idx, 1);
			fInsertGraphEdges (ob_bnd_ehs[ob_idx], adj_ehs, ob_idx, fh_idx, true);
		}
		//foreach (auto &adj_fh_idx, nor_indices){
		foreach (auto &adj_fh, nor_fhs){
			//graph.insert_edge (fh_idx, adj_fh_idx, 1);
			auto adj_fh_idx = f_n_mapping[adj_fh];
			std::unordered_set<OvmEgH> adj_ehs2;
			JC::get_adj_edges_around_face (mesh, adj_fh, adj_ehs2);
			fInsertGraphEdges (adj_ehs, adj_ehs2, fh_idx, adj_fh_idx, false);
		}
	}

	//下面求最大流最小割
	QString max_flow_str = QString ("Max flow is %1").arg (graph.ford_fulkerson());
	auto min_cut = graph.mincut ();

	std::unordered_set<OvmEgH> min_cut_ehs;
	
	//给定一个图中的节点序号，求得它的外围边界边，要分四种情况
	auto fGetTmpBndEhs = [&] (int tmp_idx) -> std::unordered_set<OvmEgH>{
		std::unordered_set<OvmEgH> bnd_ehs;
		if (tmp_idx == S_idx)
			bnd_ehs = s_bnd_ehs;
		else if (tmp_idx == T_idx)
			bnd_ehs = t_bnd_ehs;
		else if (1<= tmp_idx && tmp_idx <= obstacles.size ())
			bnd_ehs = ob_bnd_ehs[tmp_idx];
		else{
			auto nor_fh = n_f_mapping[tmp_idx];
			auto hehs = mesh->face (nor_fh).halfedges ();
			foreach (auto &heh, hehs){
				auto eh = mesh->edge_handle (heh);
				bnd_ehs.insert (eh);
			}
		}
		return bnd_ehs;
	};

	auto fGetSharedEhs = [] (std::unordered_set<OvmEgH> &ehs1, std::unordered_set<OvmEgH> &ehs2)->std::unordered_set<OvmEgH>{
		std::unordered_set<OvmEgH> shared_ehs;
		foreach (auto &eh, ehs1){
			if (JC::contains (ehs2, eh))
				shared_ehs.insert (eh);
		}
		return shared_ehs;
	};

	for (int i = 0; i != min_cut.size (); ++i){
		auto bnd_ehs1 = fGetTmpBndEhs (min_cut[i].first), 
			bnd_ehs2 = fGetTmpBndEhs (min_cut[i].second);

		auto shared_ehs = fGetSharedEhs (bnd_ehs1, bnd_ehs2);
		foreach (auto &eh, shared_ehs)
			min_cut_ehs.insert (eh);
	}
	return min_cut_ehs;
}
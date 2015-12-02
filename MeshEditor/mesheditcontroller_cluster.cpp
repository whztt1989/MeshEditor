#include "StdAfx.h"
#include "mesheditcontroller.h"
#include "hoopsview.h"
#include <QMessageBox>
#include <limits>
#include <body.hxx>
#include <lump.hxx>
#include <shell.hxx>
#include <wire.hxx>
#include <mk_ewires_opts.hxx>
#include "max_flow_min_cut.h"

void MeshEditController::on_select_edges_for_cl ()
{
	hoopsview->begin_select_by_click ();
	hoopsview->set_edges_selectable (true);
	hoopsview->show_mesh_faces (true);
}

void MeshEditController::on_rotate_for_cl ()
{
	hoopsview->begin_camera_manipulate ();

}

std::vector<std::unordered_set<OvmFaH> > fhs_patches;

void MeshEditController::on_select_edges_ok_for_cl ()
{
	selected_ehs.clear ();
	fhs_patches.clear ();
	if (!hoopsview->get_selected_elements (std::vector<OvmVeH>(), selected_ehs, std::vector<OvmFaH> ())){
		QMessageBox::warning (this, tr("警告"), tr("未选中任何网格边！"), QMessageBox::Ok);
		return;
	}

	std::unordered_set<OvmFaH> all_fhs;
	std::unordered_set<OvmEgH> seprate_ehs;
	
	for (auto bf_it = mesh->bf_iter (); bf_it; ++bf_it){
		all_fhs.insert (*bf_it);
	}
	JC::vector_to_unordered_set (selected_ehs, seprate_ehs);

	JC::get_separate_fhs_patches (mesh, all_fhs, seprate_ehs, fhs_patches);
	hoopsview->clear_selection ();
	hoopsview->begin_camera_manipulate ();

	hoopsview->derender_all_mesh_groups ();

	////render
	//char *colors[] = {"yellow", "pink", "blue", "green", "red", "purple"};
	//for (int i = 0; i != fhs_patches.size (); ++i){
	//	auto group = new VolumeMeshElementGroup (mesh, "lsi", "loop edges");
	//	//JC::vector_to_unordered_set (selected_ehs, group->ehs);
	//	MeshRenderOptions render_options;
	//	render_options.face_color = colors[i];
	//	foreach (auto fh, fhs_patches[i]){
	//		group->fhs.insert (fh);
	//	}
	//	hoopsview->render_mesh_group (group, render_options);
	//}
	//hoopsview->show_mesh_faces (false);
	auto group = new VolumeMeshElementGroup (mesh, "lsi", "loop edges");
	JC::vector_to_unordered_set (selected_ehs, group->ehs);
	MeshRenderOptions render_options;
	render_options.edge_color = "green";
	render_options.edge_width = 6;
	hoopsview->render_mesh_group (group, render_options);
}

void MeshEditController::on_cluster_for_cl ()
{
	if (selected_ehs.empty () || fhs_patches.empty ()) return;

	if (fhs_patches.size () != 2) return;

	std::vector<std::unordered_set<OvmCeH> > s_t_chs;
	s_t_chs.resize (2);
	for (int i = 0; i != fhs_patches.size (); ++i){
		JC::get_direct_adjacent_hexas (mesh, fhs_patches[i], s_t_chs[i]);
	}
	auto tmp = s_t_chs.front ();
	s_t_chs[0] = s_t_chs[1];
	s_t_chs[1] = tmp;

	//构建有向图
	int N_node = mesh->n_cells () - s_t_chs.front ().size () - s_t_chs.back ().size () + 2;
	int S_idx = 0, T_idx = N_node - 1;
	CoreSolver<int> graph (N_node, S_idx, T_idx);

	std::unordered_set<OvmFaH> bound_fhs1, bound_fhs2;
	JC::collect_boundary_element (mesh, s_t_chs.front (), NULL, NULL, &bound_fhs1);
	JC::collect_boundary_element (mesh, s_t_chs.back (), NULL, NULL, &bound_fhs2);
	if (JC::intersects (bound_fhs1, bound_fhs2)){
		graph.insert_edge (S_idx, T_idx, 1);
	}

	std::hash_map<OvmCeH, int> c_n_mapping;
	std::hash_map<int, OvmCeH> n_c_mapping;
	for (auto c_it = mesh->cells_begin (); c_it != mesh->cells_end (); ++c_it){
		auto ch = *c_it;
		if (JC::contains (s_t_chs.front (), ch) || JC::contains (s_t_chs.back (), ch))
			continue;
		int cur_idx = c_n_mapping.size () + 1;
		c_n_mapping.insert (std::make_pair(ch, cur_idx));
		n_c_mapping.insert (std::make_pair(cur_idx, ch));
	}

	for (auto c_it = mesh->cells_begin (); c_it != mesh->cells_end (); ++c_it){
		auto ch = *c_it;
		if (JC::contains (s_t_chs.front (), ch) || JC::contains (s_t_chs.back (), ch))
			continue;

		std::unordered_set<OvmCeH> adj_chs;
		JC::get_adj_hexas_around_hexa (mesh, ch, adj_chs);
		int cur_idx = c_n_mapping[ch];
		foreach (auto adj_ch, adj_chs){
			if (JC::contains (s_t_chs.front (), adj_ch))
				graph.insert_edge (S_idx, cur_idx, 1);
			else if (JC::contains (s_t_chs.back (), adj_ch))
				graph.insert_edge (cur_idx, T_idx, 1);
			else{
				int adj_ch_idx = c_n_mapping[adj_ch];
				graph.insert_edge (cur_idx, adj_ch_idx, 1);
			}
		}
	}

	QString max_flow_str = QString ("Max flow is %1").arg (graph.ford_fulkerson());
	auto min_cut = graph.mincut ();

	std::unordered_set<OvmFaH> min_cut_fhs;
	for (int i = 0; i != min_cut.size (); ++i){
		if (min_cut[i].first == S_idx && min_cut[i].second == T_idx){
			foreach (auto &fh, bound_fhs1){
				if (JC::contains (bound_fhs2, fh))
					min_cut_fhs.insert (fh);
			}
		}
		else if (min_cut[i].first == S_idx){
			OvmCeH ch = n_c_mapping[min_cut[i].second];
			auto adj_hfhs = mesh->cell (ch).halffaces ();
			foreach (auto hfh, adj_hfhs){
				auto fh = mesh->face_handle (hfh);
				if (JC::contains (bound_fhs1, fh))
					min_cut_fhs.insert (fh);
			}
		}
		else if (min_cut[i].second == T_idx){
			OvmCeH ch = n_c_mapping[min_cut[i].first];
			auto adj_hfhs = mesh->cell (ch).halffaces ();
			foreach (auto hfh, adj_hfhs){
				auto fh = mesh->face_handle (hfh);
				if (JC::contains (bound_fhs2, fh))
					min_cut_fhs.insert (fh);
			}
		}
		else{
			OvmCeH ch1 = n_c_mapping[min_cut[i].first];
			OvmCeH ch2 = n_c_mapping[min_cut[i].second];
			auto com_fh = JC::get_common_face_handle (mesh, ch1, ch2);
			min_cut_fhs.insert (com_fh);
		}
	}

	auto group = new VolumeMeshElementGroup (mesh, "lsi", "min cut faces");
	MeshRenderOptions render_options;
	render_options.face_color = "red";
	group->fhs = min_cut_fhs;
	hoopsview->render_mesh_group (group, render_options);
	hoopsview->show_mesh_faces (false);
}
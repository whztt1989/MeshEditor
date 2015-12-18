#include "stdafx.h"
#include "FuncDefs.h"
#include "hoopsview.h"
#include <intrapi.hxx>
namespace JC{

	void optimize_quad_patch (VolumeMesh *mesh, std::unordered_set<OvmFaH> &quad_patch, std::unordered_set<OvmFaH> &allowed_fhs)
	{
		assert (mesh->vertex_property_exists<unsigned int> ("entityptr"));
		auto V_ENTITY_PTR = mesh->request_vertex_property<unsigned int>("entityptr");
		std::unordered_set<OvmEgH> quad_set_boundary_ehs;

		auto fUpdateBoundaryEhsLocally = [&] (const OvmFaH &fh){
			auto heh_vec = mesh->face (fh).halfedges ();
			foreach (auto &heh, heh_vec){
				auto eh = mesh->edge_handle (heh);
				if (contains (quad_set_boundary_ehs, eh))
					quad_set_boundary_ehs.erase (eh);
				else
					quad_set_boundary_ehs.insert (eh);
			}
		};

		foreach (auto &fh, quad_patch){
			fUpdateBoundaryEhsLocally (fh);
		}

		forever{
			bool optimize_ok = true;

			//首先检查边界上的网格边是否有在几何边上的
			std::unordered_set<OvmFaH> tested_fhs;
			foreach (auto &boundary_eh, quad_set_boundary_ehs){
				std::unordered_set<OvmFaH> adj_boundary_fhs;
				get_adj_boundary_faces_around_edge (mesh, boundary_eh, adj_boundary_fhs);
				assert (adj_boundary_fhs.size () == 2);
				OvmFaH out_adj_fh = mesh->InvalidFaceHandle;
				foreach (auto &test_fh, adj_boundary_fhs){
					if (!contains (quad_patch, test_fh)) out_adj_fh = test_fh;
				}
				assert (out_adj_fh != mesh->InvalidFaceHandle);

				bool should_add_to_quad_set = false;
				auto acis_edge = get_associated_geometry_edge_of_boundary_eh (mesh, boundary_eh, V_ENTITY_PTR);
				if (acis_edge != NULL)
					should_add_to_quad_set = true;
				else{
					std::unordered_set<OvmFaH> adj_bound_fhs_around_fh;
					get_adj_boundary_faces_around_face (mesh, out_adj_fh, adj_bound_fhs_around_fh);
					assert (adj_bound_fhs_around_fh.size () == 4);
					int count = 0;
					foreach (auto &test_fh, adj_bound_fhs_around_fh){
						if (contains (quad_patch, test_fh))
							count++;
					}
					if (count > 1)
						should_add_to_quad_set = true;
				}//end if (acis_edge != NULL)...
				if (should_add_to_quad_set){
					if (contains (allowed_fhs, out_adj_fh)){
						quad_patch.insert (out_adj_fh);
						fUpdateBoundaryEhsLocally (out_adj_fh);
						optimize_ok = false;
						break;
					}
				}
			}//end foreach (auto &boundary_eh, quad_set_boundary_ehs){...
			if (optimize_ok) break;
		}
	}
	void optimize_shrink_set_fast (VolumeMesh *mesh, std::unordered_set<OvmCeH> &hexa_set, 
		std::unordered_set<OvmFaH> &allowed_boundary_fhs, std::unordered_set<OvmCeH> &barrier_set, std::unordered_set<OvmEgH> &allowed_ehs)
	{

	}

	void optimize_shrink_set (VolumeMesh *mesh, std::unordered_set<OvmCeH> &hexa_set, 
		std::unordered_set<OvmFaH> &allowed_boundary_fhs, std::unordered_set<OvmCeH> &barrier_set, std::unordered_set<OvmEgH> &allowed_ehs)
	{
		auto hexa_set_bk = hexa_set;
		assert (mesh->vertex_property_exists<unsigned int> ("entityptr"));
		auto V_ENTITY_PTR = mesh->request_vertex_property<unsigned int>("entityptr");

		std::vector<OvmCeH> fixed_chs, expanded_chs, shrinked_chs;//for debug only
		std::unordered_set<OvmVeH> hexa_set_bound_vhs, tmp_bound_vhs;
		std::unordered_set<OvmEgH> hexa_set_bound_ehs, tmp_bound_ehs;
		std::unordered_set<OvmFaH> hexa_set_bound_fhs, tmp_bound_fhs;
		std::unordered_set<OvmHaFaH> hexa_set_bound_hfhs;
		collect_boundary_element (mesh, hexa_set, &tmp_bound_vhs, &tmp_bound_ehs, &tmp_bound_fhs);
		//collect_boundary_element (mesh, hexa_set, NULL, NULL, &hexa_set_bound_hfhs);
		foreach (auto &vh, tmp_bound_vhs){
			if (!mesh->is_boundary (vh))
				hexa_set_bound_vhs.insert (vh);
		}
		foreach (auto &fh, tmp_bound_fhs){
			if (!mesh->is_boundary (fh))
				hexa_set_bound_fhs.insert (fh);
		}
		foreach (auto &eh, tmp_bound_ehs){
			if (!mesh->is_boundary (eh))
				hexa_set_bound_ehs.insert (eh);
		}

		auto fAllowed = [&] (std::unordered_set<OvmCeH> &hs, OvmCeH ch)->bool{
			if (contains (hs, ch)) return false;

			auto hfh_vec = mesh->cell (ch).halffaces ();
			std::unordered_set<OvmCeH> adj_chs;
			std::unordered_set<OvmEgH> ehs_on_ch;
			foreach (OvmHaFaH hfh, hfh_vec){
				OvmFaH fh = mesh->face_handle (hfh);

				auto heh_vec = mesh->face (fh).halfedges ();
				foreach (auto heh, heh_vec)
					ehs_on_ch.insert (mesh->edge_handle (heh));

				//如果fh是边界面，但是又不在allowed_boundary_fhs里面，则表明该ch是不在允许范围内的边界体
				if (mesh->is_boundary (fh)){
					if(!contains (allowed_boundary_fhs, fh)) return false;
					else continue;
				}
				OvmCeH oppo_ch = mesh->incident_cell (mesh->opposite_halfface_handle (hfh));
				if (oppo_ch == mesh->InvalidCellHandle) continue;
				if (contains (barrier_set, oppo_ch)) return false;
				adj_chs.insert (oppo_ch);
			}

			foreach (OvmEgH eh, ehs_on_ch){
				//if (mesh->is_boundary (eh)) return false;
				auto acis_edge = get_associated_geometry_edge_of_boundary_eh (mesh, eh, V_ENTITY_PTR);
				//if (acis_edge != NULL) return false;
				std::unordered_set<OvmCeH> adj_chs_of_eh;
				get_adj_hexas_around_edge (mesh, eh, adj_chs_of_eh);
				foreach (OvmCeH ch, adj_chs_of_eh){
					if (!contains (adj_chs, ch) && contains (barrier_set, ch) && !contains (allowed_ehs, eh))
						return false;
				}
			}
			return true;
		};

		auto fCellsConnected = [&] (std::unordered_set<OvmCeH> chs)->bool{
			std::queue<OvmCeH> spread_set;
			auto start_ch = pop_begin_element (chs);
			spread_set.push (start_ch);
			while (!spread_set.empty ()){
				auto curr_ch = spread_set.front ();
				spread_set.pop ();
				for (auto cc_it = mesh->cc_iter (curr_ch); cc_it; ++cc_it){
					auto test_ch = *cc_it;
					if (test_ch == mesh->InvalidCellHandle) continue;
					if (!contains (chs, test_ch)) continue;
					chs.erase (test_ch);
					spread_set.push (test_ch);
				}
			}
			return chs.empty ();
		};

		//局部更新边界情况
		auto fUpdateBoundaryFhsLocally = [&] (OvmCeH ch){
			auto hfh_vec = mesh->cell (ch).halffaces ();
			std::unordered_set<OvmEgH> related_ehs;
			foreach (auto &hfh, hfh_vec){
				auto fh = mesh->face_handle (hfh);
				if (mesh->is_boundary (fh)) continue;

				if (contains (hexa_set_bound_fhs, fh))
					hexa_set_bound_fhs.erase (fh);
				else
					hexa_set_bound_fhs.insert (fh);
				auto heh_vec = mesh->face (fh).halfedges ();
				foreach (auto heh, heh_vec)
					related_ehs.insert (mesh->edge_handle (heh));
			}

			foreach (auto &eh, related_ehs){
				if (mesh->is_boundary (eh)) continue;

				auto adj_fhs = get_adj_faces_around_edge (mesh, eh);
				bool is_on_boundary = false;
				foreach (auto &fh, adj_fhs){
					if (contains (hexa_set_bound_fhs, fh)){
						is_on_boundary = true;
						break;
					}
				}
				if (!is_on_boundary)
					hexa_set_bound_ehs.erase (eh);
				else{
					if (!contains (hexa_set_bound_ehs, eh))
						hexa_set_bound_ehs.insert (eh);
				}
			}//end foreach (auto &eh, related_ehs){...

			for (auto cv_it = mesh->cv_iter (ch); cv_it; ++cv_it){
				std::unordered_set<OvmFaH> adj_fhs;
				get_adj_faces_around_vertex (mesh, *cv_it, adj_fhs);
				bool is_on_boundary = false;
				foreach (auto &fh, adj_fhs){
					if (contains (hexa_set_bound_fhs, fh)){
						is_on_boundary = true;
						break;
					}
				}
				if (!is_on_boundary)
					hexa_set_bound_vhs.erase (*cv_it);
				else{
					if (!contains (hexa_set_bound_vhs, *cv_it))
						hexa_set_bound_vhs.insert (*cv_it);
				}
			}
		};

		auto fFixCore = [&] (std::unordered_set<OvmCeH> &hs, std::unordered_set<OvmCeH> &adj_chs)
			->std::vector<OvmCeH>{
				std::vector<OvmCeH> result;
				std::vector<OvmCeH> rest_chs;
				std::unordered_set<OvmCeH> adj_chs_of_vh_in_hs;
				foreach (OvmCeH ch, adj_chs){
					if (contains (hexa_set, ch)) adj_chs_of_vh_in_hs.insert (ch);
					else rest_chs.push_back (ch);
				}

				while (!fCellsConnected (adj_chs_of_vh_in_hs)){
					bool found_remedy = false;
					for (int i = 0; i != rest_chs.size (); ++i){
						auto test_ch = rest_chs[i];
						if (fAllowed (hs, test_ch)){
							hs.insert (test_ch);
							fUpdateBoundaryFhsLocally (test_ch);
							result.push_back (test_ch);
							rest_chs.erase (rest_chs.begin () + i);
							adj_chs_of_vh_in_hs.insert (test_ch);
							found_remedy = true;
							break;
						}
					}
					if (!found_remedy){
						assert (false);
					}
				}
				return result;
		};
		//首先要预处理一下，由于有些情况下，输入的六面体中就有非流形的情况，因此需要把输入先变成流形
		auto fFixManifoldOnce = [&](std::unordered_set<OvmCeH> &hs)->std::vector<OvmCeH>{
			std::vector<OvmCeH> result;
			foreach (OvmVeH vh, hexa_set_bound_vhs){
				std::unordered_set<OvmCeH> adj_chs;
				get_adj_hexas_around_vertex (mesh, vh, adj_chs);

				//循环添加rest_chs_of_vh中的六面体，使得最终围绕vh的六面体集合连通
				result = fFixCore (hs, adj_chs);
				if (!result.empty ()) return result;
			}

			foreach (OvmEgH eh, hexa_set_bound_ehs){
				std::unordered_set<OvmCeH> adj_chs;
				get_adj_hexas_around_edge (mesh, eh, adj_chs);

				//循环添加rest_chs_of_vh中的六面体，使得最终围绕vh的六面体集合连通
				result = fFixCore (hs, adj_chs);
				if (!result.empty ()) return result;
			}
			return result;
		};

		auto fix_result = fFixManifoldOnce (hexa_set);
		while (!fix_result.empty ()){
			foreach (OvmCeH ch, fix_result)
				fixed_chs.push_back (ch);
			fix_result = fFixManifoldOnce (hexa_set);
		}

		auto fIsManifold = [&] (std::unordered_set<OvmCeH> &hs)->bool{
			foreach (OvmVeH vh, hexa_set_bound_vhs){
				std::unordered_set<OvmCeH> adj_chs;
				get_adj_hexas_around_vertex (mesh, vh, adj_chs);

				std::unordered_set<OvmCeH> adj_chs_of_vh_in_hs;
				foreach (OvmCeH ch, adj_chs){
					if (contains (hexa_set, ch)) adj_chs_of_vh_in_hs.insert (ch);
				}

				if (!fCellsConnected (adj_chs_of_vh_in_hs)) return false;
			}

			foreach (OvmEgH eh, hexa_set_bound_ehs){
				std::unordered_set<OvmCeH> adj_chs;
				get_adj_hexas_around_edge (mesh, eh, adj_chs);

				std::unordered_set<OvmCeH> adj_chs_of_vh_in_hs;
				foreach (OvmCeH ch, adj_chs){
					if (contains (hexa_set, ch)) adj_chs_of_vh_in_hs.insert (ch);
				}

				if (!fCellsConnected (adj_chs_of_vh_in_hs)) return false;
			}
			return true;
		};

		auto fExpandOnce = [&] (std::unordered_set<OvmCeH> &hs)->OvmCeH{
			std::unordered_set<OvmCeH> tested_chs;
			foreach (auto &fh, hexa_set_bound_fhs){
				if (mesh->is_boundary (fh)) continue;
				auto hfh = mesh->halfface_handle (fh, 0);
				auto out_adj_ch = mesh->incident_cell (hfh);
				int idx = out_adj_ch.idx ();
				if (contains (hs, out_adj_ch)){
					hfh = mesh->opposite_halfface_handle (hfh);
					out_adj_ch = mesh->incident_cell (hfh);
					assert (!contains (hs, out_adj_ch));
				}

				//为防止单次膨胀中，对同一个六面体多次测试，用tested_chs来存放被测试过的六面体
				if (contains (tested_chs, out_adj_ch)) continue;
				tested_chs.insert (out_adj_ch);

				auto hfh_vec = mesh->cell (out_adj_ch).halffaces ();
				//out_adj_ch上的六个面和hexa_bound_fhs的交集面个数
				//如果多于两个，则可以做进一步测试，否则直接返回不做考虑
				int adj_fhs_on_ch_num = 0;
				bool on_the_boundary = false;
				foreach (auto &hfh, hfh_vec){
					auto test_fh = mesh->face_handle (hfh);
					if (contains (hexa_set_bound_fhs, test_fh))
						adj_fhs_on_ch_num++;
					if (mesh->is_boundary (test_fh))
						on_the_boundary = true;
				}

				if (on_the_boundary) continue;
				if (adj_fhs_on_ch_num >= 2){
					int idx = out_adj_ch.idx ();

					if (fAllowed (hs, out_adj_ch)){
						hs.insert (out_adj_ch);
						fUpdateBoundaryFhsLocally (out_adj_ch);

						if (fIsManifold (hs))
							return out_adj_ch;

						hs.erase (out_adj_ch);
						fUpdateBoundaryFhsLocally (out_adj_ch);
					}//end if (fAllowed (out_adj_ch)){...
				}//end if (out_adj_chs_num == 1){...
			}//end foreach (OvmEgH eh, hexa_set_bound_ehs){...
			return mesh->InvalidCellHandle;
		};

		auto fShrinkOnce = [&] (std::unordered_set<OvmCeH> &hs, double &prev_mark)->OvmCeH{
			std::unordered_set<OvmCeH> tested_chs;
			foreach (OvmEgH eh, hexa_set_bound_ehs){
				if (mesh->is_boundary (eh)) continue;

				std::unordered_set<OvmCeH> adj_chs;
				int in_adj_chs_num = 0;
				OvmCeH in_adj_ch = mesh->InvalidCellHandle;
				get_adj_hexas_around_edge (mesh, eh, adj_chs);
				foreach (OvmCeH ch, adj_chs){
					if (contains (hs, ch)){
						in_adj_ch = ch;
						in_adj_chs_num++;
					}
				}
				if (in_adj_chs_num == 1){
					int idx = in_adj_ch.idx ();
					if (contains (tested_chs, in_adj_ch)) continue;

					tested_chs.insert (in_adj_ch);

					if (contains (hexa_set_bk, in_adj_ch)) continue;

					hs.erase (in_adj_ch);
					fUpdateBoundaryFhsLocally (in_adj_ch);

					if (fIsManifold (hs)){
						double new_mark = calc_manifold_edge_marks (mesh, hexa_set_bound_fhs);
						if (new_mark < prev_mark){
							prev_mark = new_mark;
							return in_adj_ch;
						}
					}
					hs.insert (in_adj_ch);
					fUpdateBoundaryFhsLocally (in_adj_ch);
				}//end if (in_adj_chs.size () == 1){...
			}//end foreach (OvmEgH eh, bound_ehs){...
			return mesh->InvalidCellHandle;
		};

		OvmCeH expaned_ch = fExpandOnce (hexa_set);
		while (expaned_ch != mesh->InvalidCellHandle){
			expanded_chs.push_back (expaned_ch);
			expaned_ch = fExpandOnce (hexa_set);
		}
	}
	void optimize_shrink_set_simple (VolumeMesh *mesh, 
		std::unordered_set<OvmCeH> &hexa_set, 
		std::unordered_set<OvmCeH> &barrier_set)
	{
		//auto hexa_set_bk = hexa_set;
		//assert (mesh->vertex_property_exists<unsigned int> ("entityptr"));

		//std::vector<OvmCeH> fixed_chs, expanded_chs, shrinked_chs;//for debug only
		//std::unordered_set<OvmFaH> hexa_set_bound_fhs, tmp_bound_fhs;
		//collect_boundary_element (mesh, hexa_set, 
		//	NULL, NULL, &tmp_bound_fhs);
		//foreach (auto &fh, tmp_bound_fhs){
		//	if (!mesh->is_boundary (fh))
		//		hexa_set_bound_fhs.insert (fh);
		//}

		//auto fAllowed = [&] (std::unordered_set<OvmCeH> &hs, OvmCeH ch)->bool{
		//	if (contains (hs, ch)) return false;

		//	auto hfh_vec = mesh->cell (ch).halffaces ();
		//	std::unordered_set<OvmCeH> adj_chs;
		//	std::unordered_set<OvmEgH> ehs_on_ch;
		//	foreach (OvmHaFaH hfh, hfh_vec){
		//		OvmFaH fh = mesh->face_handle (hfh);

		//		auto heh_vec = mesh->face (fh).halfedges ();
		//		foreach (auto heh, heh_vec)
		//			ehs_on_ch.insert (mesh->edge_handle (heh));

		//		//如果fh是边界面，但是又不在allowed_boundary_fhs里面，则表明该ch是不在允许范围内的边界体
		//		if (mesh->is_boundary (fh)){
		//			if(!contains (allowed_boundary_fhs, fh)) return false;
		//			else continue;
		//		}
		//		OvmCeH oppo_ch = mesh->incident_cell (mesh->opposite_halfface_handle (hfh));
		//		if (oppo_ch == mesh->InvalidCellHandle) continue;
		//		if (contains (barrier_set, oppo_ch)) return false;
		//		adj_chs.insert (oppo_ch);
		//	}

		//	foreach (OvmEgH eh, ehs_on_ch){
		//		//if (mesh->is_boundary (eh)) return false;
		//		auto acis_edge = get_associated_geometry_edge_of_boundary_eh (mesh, eh, V_ENTITY_PTR);
		//		//if (acis_edge != NULL) return false;
		//		std::unordered_set<OvmCeH> adj_chs_of_eh;
		//		get_adj_hexas_around_edge (mesh, eh, adj_chs_of_eh);
		//		foreach (OvmCeH ch, adj_chs_of_eh){
		//			if (!contains (adj_chs, ch) && contains (barrier_set, ch) && !contains (allowed_ehs, eh))
		//				return false;
		//		}
		//	}
		//	return true;
		//};

		////局部更新边界情况
		//auto fUpdateBoundaryFhsLocally = [&] (OvmCeH ch){
		//	auto hfh_vec = mesh->cell (ch).halffaces ();
		//	foreach (auto &hfh, hfh_vec){
		//		auto fh = mesh->face_handle (hfh);
		//		if (mesh->is_boundary (fh)) continue;

		//		if (contains (hexa_set_bound_fhs, fh))
		//			hexa_set_bound_fhs.erase (fh);
		//		else
		//			hexa_set_bound_fhs.insert (fh);
		//	}
		//};

	}

	void optimize_shrink_set (VolumeMesh *mesh, std::unordered_set<OvmCeH> &hexa_set, 
		std::unordered_set<OvmFaH> &allowed_boundary_fhs)
	{
		std::unordered_set<OvmCeH> barrier_set;
		std::unordered_set<OvmEgH> allowed_ehs;
		optimize_shrink_set (mesh, hexa_set, allowed_boundary_fhs, barrier_set, allowed_ehs);
	}

	void optimize_shrink_set (VolumeMesh *mesh, std::unordered_set<OvmCeH> &hexa_set)
	{
		std::unordered_set<OvmCeH> barrier_set;
		std::unordered_set<OvmEgH> allowed_ehs;
		std::unordered_set<OvmFaH> allowed_boundary_fhs;
		optimize_shrink_set (mesh, hexa_set, allowed_boundary_fhs, barrier_set, allowed_ehs);
	}

	void optimize_shrink_set_test (VolumeMesh *mesh, std::unordered_set<OvmCeH> &hexa_set, std::unordered_set<OvmCeH> &barrier_set)
	{
		assert (mesh->vertex_property_exists<unsigned int> ("entityptr"));
		auto V_ENTITY_PTR = mesh->request_vertex_property<unsigned int>("entityptr");

		std::vector<OvmCeH> fixed_chs, expanded_chs, shrinked_chs;//for debug only
		std::unordered_set<OvmFaH> hexa_set_bound_fhs, tmp_bound_fhs;
		collect_boundary_element (mesh, hexa_set, NULL, NULL, &tmp_bound_fhs);
		foreach (auto &fh, tmp_bound_fhs){
			if (!mesh->is_boundary (fh))
				hexa_set_bound_fhs.insert (fh);
		}

		//局部更新边界情况
		auto fUpdateBoundaryFhsLocally = [&] (OvmCeH ch){
			auto hfh_vec = mesh->cell (ch).halffaces ();
			std::unordered_set<OvmEgH> related_ehs;
			foreach (auto &hfh, hfh_vec){
				auto fh = mesh->face_handle (hfh);
				if (mesh->is_boundary (fh)) continue;

				if (contains (hexa_set_bound_fhs, fh))
					hexa_set_bound_fhs.erase (fh);
				else
					hexa_set_bound_fhs.insert (fh);
			}
		};

		auto fExpandOnce = [&] (std::unordered_set<OvmCeH> &hs, std::vector<OvmCeH> &exp_chs)->bool{
			std::unordered_set<OvmCeH> tested_chs;
			foreach (auto &fh, hexa_set_bound_fhs){
				if (mesh->is_boundary (fh)) continue;
				auto hfh = mesh->halfface_handle (fh, 0);
				auto out_adj_ch = mesh->incident_cell (hfh);
				int idx = out_adj_ch.idx ();
				if (contains (hs, out_adj_ch)){
					hfh = mesh->opposite_halfface_handle (hfh);
					out_adj_ch = mesh->incident_cell (hfh);
					assert (!contains (hs, out_adj_ch));
				}

				//为防止单次膨胀中，对同一个六面体多次测试，用tested_chs来存放被测试过的六面体
				if (contains (tested_chs, out_adj_ch)) continue;
				tested_chs.insert (out_adj_ch);

				auto hfh_vec = mesh->cell (out_adj_ch).halffaces ();
				//out_adj_ch上的六个面和hexa_bound_fhs的交集面个数
				//如果多于两个，则可以做进一步测试，否则直接返回不做考虑
				int adj_fhs_on_ch_num = 0;
				bool on_the_boundary = false;
				foreach (auto &hfh, hfh_vec){
					auto test_fh = mesh->face_handle (hfh);
					if (mesh->is_boundary (test_fh)) 
						on_the_boundary = true;
					if (contains (hexa_set_bound_fhs, test_fh))
						adj_fhs_on_ch_num++;
				}
				if (on_the_boundary) continue;

				if (adj_fhs_on_ch_num >= 2){
					int idx = out_adj_ch.idx ();
					hs.insert (out_adj_ch);
					fUpdateBoundaryFhsLocally (out_adj_ch);
					exp_chs.push_back (out_adj_ch);
					return true;

					//if (!contains (barrier_set, out_adj_ch)){
					//	std::unordered_set<OvmEgH> asso_ehs;
					//	std::unordered_set<OvmCeH> adj_chs;
					//	bool need_consider_hard_edge = false;
					//	auto hfh_vec = mesh->cell (out_adj_ch).halffaces ();
					//	foreach (auto &hfh, hfh_vec){
					//		auto heh_vec = mesh->halfface (hfh).halfedges ();
					//		foreach (auto &heh, heh_vec){
					//			auto eh = mesh->edge_handle (heh);
					//			if (!mesh->is_boundary (eh)) continue;
					//			
					//			auto acis_edge = get_associated_geometry_edge_of_boundary_eh (mesh, eh, V_ENTITY_PTR);
					//			if (acis_edge == NULL) continue;
					//			else
					//				asso_ehs.insert (eh);
					//		}
					//	}
					//	assert (asso_ehs.size () <= 1);

					//	if (asso_ehs.empty ()){
					//		hs.insert (out_adj_ch);
					//		fUpdateBoundaryFhsLocally (out_adj_ch);
					//		exp_chs.push_back (out_adj_ch);
					//		return true;
					//	}else{
					//		auto asso_eh = *(asso_ehs.begin ());
					//		auto acis_edge = get_associated_geometry_edge_of_boundary_eh (mesh, asso_eh, V_ENTITY_PTR);
					//		get_adj_hexas_around_edge (mesh, asso_eh, adj_chs);
					//		bool this_ok = true;
					//		foreach (auto &ch, adj_chs){
					//			if (contains (barrier_set, ch)){
					//				foreach (auto &ch, adj_chs){
					//					tested_chs.insert (ch);
					//				}
					//				this_ok = false;
					//				break;
					//			}
					//		} 
					//		if (!this_ok) continue; //foreach (auto &fh, hexa_set_bound_fhs){...
					//		foreach (auto &ch, adj_chs){
					//			if (!JC::contains (hs, ch)){
					//				hs.insert (ch);
					//				fUpdateBoundaryFhsLocally (ch);
					//			}
					//		}
					//		return true;
					//	}
					//	
					//}//end if (fAllowed (out_adj_ch)){...
				}//end if (out_adj_chs_num == 1){...
			}//end foreach (OvmEgH eh, hexa_set_bound_ehs){...
			return false;
		};

		auto fFixHardEdgeIssue = [&] (std::unordered_set<OvmCeH> &hs, std::vector<OvmCeH> &exp_chs)->bool{
			std::hash_map<OvmEgH, int> ehs_adj_fhs_count;
			foreach (auto &fh, hexa_set_bound_fhs){
				auto heh_vec = mesh->face (fh).halfedges ();
				foreach (auto &heh, heh_vec)
					ehs_adj_fhs_count[mesh->edge_handle (heh)]++;
			}

			foreach (auto &p, ehs_adj_fhs_count){
				if (p.second != 1) continue;
				auto acis_edge = get_associated_geometry_edge_of_boundary_eh (mesh, p.first, V_ENTITY_PTR);
				if (acis_edge == NULL) continue;
				std::unordered_set<OvmCeH> adj_chs;
				get_adj_hexas_around_edge (mesh, p.first, adj_chs);
				foreach (auto &ch, adj_chs)
					if (contains (barrier_set, ch)) return false;
				foreach (auto &ch, adj_chs){
					if (!JC::contains (hs, ch)){
						hs.insert (ch);
						fUpdateBoundaryFhsLocally (ch);
						exp_chs.push_back (ch);
					}
				}
			}
			return true;
		};

		std::vector<OvmCeH> exp_chs;
		//auto fxresult = fFixHardEdgeIssue (hexa_set, exp_chs);
		//assert (fxresult);
		//while (!exp_chs.empty ()){
		//	foreach (auto &ch, exp_chs)
		//		expanded_chs.push_back (ch);
		//	exp_chs.clear ();
		//	fxresult = fFixHardEdgeIssue (hexa_set, exp_chs);
		//	assert (fxresult);
		//}
		//exp_chs.clear ();
		auto has_expaned = fExpandOnce (hexa_set, exp_chs);
		while (has_expaned){
			foreach (auto &ch, exp_chs)
				expanded_chs.push_back (ch);
			exp_chs.clear ();
			has_expaned = fExpandOnce (hexa_set, exp_chs);
		}
	}

	void smooth_volume_mesh (VolumeMesh *mesh, BODY *body, int smooth_rounds, HoopsView *hoopsview)
	{
//#define MYDEBUG
#ifdef MYDEBUG
		QMessageBox::information (NULL, "pre", "enter");
#endif
		assert (mesh->vertex_property_exists<unsigned int> ("entityptr"));
		auto V_ENTITY_PTR = mesh->request_vertex_property<unsigned int>("entityptr");

		std::unordered_set<OvmVeH> fixed_vhs;
		if (mesh->vertex_property_exists<bool> ("vertexfixedforsmooth")){
			auto V_FIXED = mesh->request_vertex_property<bool> ("vertexfixedforsmooth");
			auto V_ENTITY_PTR = mesh->request_vertex_property<unsigned int>("entityptr");
			for (auto v_it = mesh->vertices_begin (); v_it != mesh->vertices_end (); ++v_it){
				if (V_FIXED[*v_it] == true) fixed_vhs.insert (*v_it);
			}
		}
		//对边的类型统计一下
		std::hash_map<VERTEX*, OvmVeH> vertices_mapping;
		std::hash_map<EDGE*, std::unordered_set<OvmVeH> > edges_vertices_mapping;
		std::hash_map<FACE*, std::unordered_set<OvmVeH> > faces_vertices_mapping;
		std::unordered_set<OvmVeH> volume_vhs, new_boundary_vhs;

		ENTITY_LIST vertices_list, edges_list, faces_list;
		api_get_vertices (body, vertices_list);
		api_get_edges (body, edges_list);
		api_get_faces (body, faces_list);

		auto visitedEdge = mesh->request_edge_property<bool> ("edgevisited", false);

		std::set<FACE*> all_faces;
		entity_list_to_set (faces_list, all_faces);
		std::vector<std::set<FACE*> > cylindrical_face_groups;
		std::hash_map<ENTITY*, int> edge_face_group_mapping;
		std::vector<std::unordered_set<OvmVeH> > determined_vhs_on_face_groups, real_vhs_on_face_groups;
		std::unordered_set<OvmEgH> all_ehs_on_geom_edges;
		std::unordered_set<OvmVeH> vhs_on_geom_edges;

		std::set<ENTITY*> cylinder_edges;
		//for (int i = 0; i != edges_list.count (); ++i){
		//	ENTITY_LIST adj_faces;
		//	api_get_faces (edges_list[i], adj_faces);
		//	bool is_cyl_f = true;
		//	for (int j = 0; j != adj_faces.count (); ++j){
		//		if (!is_cylindrical_face (adj_faces[j])){
		//			is_cyl_f = false; break;
		//		}
		//	}
		//	if (is_cyl_f && is_linear_edge (edges_list[i])){
		//		cylinder_edges.insert (edges_list[i]);
		//		std::set<FACE*> adj_faces_set;
		//		entity_list_to_set (adj_faces, adj_faces_set);
		//		if (contains (all_faces, adj_faces_set)){
		//			edge_face_group_mapping[edges_list[i]] = cylindrical_face_groups.size ();
		//			cylindrical_face_groups.push_back (adj_faces_set);
		//		}else{
		//			auto locate = std::find_if (cylindrical_face_groups.begin (), cylindrical_face_groups.end (), 
		//				[&](const std::set<FACE*> &one_group)->bool{
		//				if (intersects (one_group, adj_faces_set)) return true;
		//				else return false;
		//			});
		//			assert (locate != cylindrical_face_groups.end ());
		//			foreach (auto &f, adj_faces_set)
		//				locate->insert (f);
		//			edge_face_group_mapping[edges_list[i]] = locate - cylindrical_face_groups.begin ();
		//		}
		//		foreach (auto &f, adj_faces_set)
		//			all_faces.erase (f);
		//	}
		//}
#ifdef MYDEBUG
		QMessageBox::information (NULL, "pre", "step1");
#endif
		foreach (auto &f, all_faces){
			std::set<FACE*> one_group;
			one_group.insert (f);
			cylindrical_face_groups.push_back (one_group);
		}

		determined_vhs_on_face_groups.resize (cylindrical_face_groups.size ());

		for (auto v_it = mesh->vertices_begin (); v_it != mesh->vertices_end (); ++v_it)
		{
			if (!mesh->is_boundary (*v_it)){
				volume_vhs.insert (*v_it);
				continue;
			}
			auto entity_ptr_uint = V_ENTITY_PTR[*v_it];
			if (entity_ptr_uint == 0) new_boundary_vhs.insert (*v_it);
			else{
				ENTITY *entity = (ENTITY*)(entity_ptr_uint);
				if (is_VERTEX (entity)) vertices_mapping[(VERTEX*)entity] = *v_it;
				//else if (is_EDGE (entity)) edges_vertices_mapping[(EDGE*)entity].insert (*v_it);
				//else faces_vertices_mapping[(FACE*)entity].insert (*v_it);
				else if (is_EDGE (entity)){
					if (contains (cylinder_edges, entity)){
						int idx = edge_face_group_mapping[entity];
						determined_vhs_on_face_groups[idx].insert (*v_it);
					}else{
						edges_vertices_mapping[(EDGE*)entity].insert (*v_it);
					}
				}else{
					auto f = (FACE*)entity;
					auto locate = std::find_if (cylindrical_face_groups.begin (), cylindrical_face_groups.end (), 
						[&](const std::set<FACE*> &one_group)->bool{
							if (contains (one_group, f)) return true;
							else return false;
					});
					determined_vhs_on_face_groups[locate - cylindrical_face_groups.begin ()].insert (*v_it);
				}
			}
		}
		//if (!new_boundary_vhs.empty ())
		//	QMessageBox::information (NULL, "info", "new_boundary_vhs不为空");
#ifdef MYDEBUG
		QMessageBox::information (NULL, "pre", "step2");
#endif
		if (vertices_list.count () != vertices_mapping.size ()){
			if (hoopsview){
				//auto group = new VolumeMeshElementGroup (mesh, "si", "quad set for inflation");
				//foreach (auto &p, vertices_mapping){
				//	group->vhs.insert (p.second);
				//}
				//MeshRenderOptions render_options;
				//render_options.vertex_color = "red";
				//render_options.vertex_size = 2;
				//hoopsview->render_mesh_group (group, render_options);
				HC_Open_Segment_By_Key (hoopsview->GetHoopsView ()->GetModelKey ());{
					HC_Open_Segment ("geom vertices");{
						HC_Flush_Geometry ("...");
						HC_Set_Marker_Size (2);
						HC_Set_Color ("markers=blue");
						HC_Set_Visibility ("markers=on");
						for (int i = 0; i != vertices_list.count (); ++i){
							auto V = (VERTEX*)(vertices_list[i]);
							if (vertices_mapping.find (V) == vertices_mapping.end ()){
								SPAposition pos = V->geometry ()->coords ();
								HC_Insert_Marker (pos.x (), pos.y (), pos.z ());
							}
						}
					}HC_Close_Segment ();
				}HC_Close_Segment ();
				hoopsview->GetHoopsView ()->SetGeometryChanged ();
				hoopsview->GetHoopsView ()->Update ();
				hoopsview->show_mesh_faces (false);
				return;
			}
		}

		auto fDivideOneEdge = [&](EDGE *edge, int dis_num, std::vector<SPAposition> &pts){
			SPAinterval inter = edge->param_range ();
			double step = inter.length () / dis_num,
				start_param = inter.start_pt ();
			curve *crv = edge->geometry ()->trans_curve ();

			for (int i = 0; i <= dis_num; ++i){
				double param_val = start_param + step * i;
				SPAposition pos;
				crv->eval (param_val, pos);
				pts.push_back (pos);
			}
		};

		auto fSmoothAllEdges = [&](){
			for (int i = 0; i != edges_list.count (); ++i){
#ifdef MYDEBUG
				QMessageBox::information (NULL, "edge", QString("edge %1 begin").arg (i));
#endif
				ENTITY *entity = edges_list[i];
				if (contains (cylinder_edges, entity)) continue;

				EDGE *acis_edge = (EDGE*)entity;
				VERTEX *acis_v1 = acis_edge->start (),
					*acis_v2 = acis_edge->end ();
				OvmVeH mesh_vh1 = vertices_mapping[acis_v1],
					mesh_vh2 = vertices_mapping[acis_v2];
				auto &mesh_vhs_set = edges_vertices_mapping[acis_edge];
				mesh_vhs_set.insert (mesh_vh2);
				std::vector<OvmVeH> sorted_vhs;
				while (mesh_vh1 != mesh_vh2){
					sorted_vhs.push_back (mesh_vh1);
					OvmVeH next_vh = mesh->InvalidVertexHandle;
					//首先在mesh_vhs_set中找下一个顶点
					for (auto v_it = mesh_vhs_set.begin (); v_it != mesh_vhs_set.end (); ++v_it)
					{
						OvmHaEgH test_heh = mesh->halfedge (mesh_vh1, *v_it);
						if (test_heh != mesh->InvalidHalfEdgeHandle){
							next_vh = *v_it;
							mesh_vhs_set.erase (next_vh);
							break;
						}
					}
					//如果在mesh_vhs_set中找不到，那么在new_vhs中继续找
					if (next_vh == mesh->InvalidVertexHandle){
						double min_dist = std::numeric_limits<double>::max ();
						OvmVeH best_next_vh = OvmVeH (-1);
						foreach (auto &vh, new_boundary_vhs){
							OvmHaEgH test_heh = mesh->halfedge (mesh_vh1, vh);
							if (test_heh != mesh->InvalidHalfEdgeHandle){
								SPAposition closest_pt;
								double dist = 0.0f;
								OvmVec3d pos = mesh->vertex (vh);
								api_entity_point_distance (acis_edge, POS2SPA(pos), closest_pt, dist);
								if (dist < min_dist){
									best_next_vh = vh;
									min_dist = dist;
								}
							}
						}
						next_vh = best_next_vh;
						assert (next_vh != mesh->InvalidVertexHandle);
						new_boundary_vhs.erase (next_vh);
						V_ENTITY_PTR[next_vh] = (unsigned int) acis_edge;
					}
					mesh_vh1 = next_vh;
				}//end while (mesh_vh1 != mesh_vh2){...
				sorted_vhs.push_back (mesh_vh2);

#ifdef MYDEBUG
				QMessageBox::information (NULL, "edge", QString("edge %1 out while").arg (i));
#endif
				std::vector<SPAposition> pts;
				fDivideOneEdge (acis_edge, sorted_vhs.size () - 1, pts);

				//如果和边的离散顺序相反，则翻转一下
				if (!same_point (pts.front (), POS2SPA(mesh->vertex (sorted_vhs.front ())), SPAresabs * 1000))
					std::reverse (sorted_vhs.begin (), sorted_vhs.end ());
				for (int j = 0; j != sorted_vhs.size (); ++j)
					mesh->set_vertex (sorted_vhs[j], SPA2POS(pts[j]));
				if (sorted_vhs.size () > 2){
					for (int j = 1; j != sorted_vhs.size () - 1; ++j){
						V_ENTITY_PTR[sorted_vhs[j]] = (unsigned int)acis_edge;
					}
				}

				for (int j = 0; j != sorted_vhs.size () - 1; ++j){
					auto eh = mesh->edge_handle (mesh->halfedge (sorted_vhs[j], sorted_vhs[j + 1]));
					if (eh == mesh->InvalidEdgeHandle)
						QMessageBox::warning (NULL, "ERROR", "eh invalid!");
					visitedEdge[eh] = true;
					all_ehs_on_geom_edges.insert (eh);
				}
#ifdef MYDEBUG
				QMessageBox::information (NULL, "edge", QString("edge %1 finished").arg (i));
#endif
			}
		};//end auto fSmoothAllEdges = [&](){...

		auto fSmoothAllFaces = [&] (int times){
			for (int i = 0; i != cylindrical_face_groups.size (); ++i){
				//first we collect all the vertices handles on this face
				auto &vhs_test = determined_vhs_on_face_groups[i];
				std::unordered_set<OvmVeH> vhs_on_this_group;
				foreach (auto &vhs, real_vhs_on_face_groups){
					if (intersects (vhs, vhs_test)){
						vhs_on_this_group = vhs; break;
					}
				}
				if (vhs_on_this_group.empty ()){
					//QMessageBox::information (NULL, "face", QString("face %1 empty!").arg (i));
				}
				//then we try to smooth them using simplest laplacian smoothing
				int round = 0;
				ENTITY_LIST face_group_list;
				foreach (auto &f, cylindrical_face_groups[i])
					face_group_list.add (f);

#ifdef MYDEBUG
				QMessageBox::information (NULL, "face", QString("face %1 enter while").arg (i));
#endif

				while (round++ < times){
					std::vector<OvmVeH> vhs_vec;
					unordered_set_to_vector (vhs_on_this_group, vhs_vec);
					SPAposition closest_pos;
					double dist;
					for (int j = 0; j != vhs_vec.size (); ++j){
						auto Ni = vhs_vec[j];
						if (contains (fixed_vhs, Ni)) continue;

						//get all the adjacent vertices of this vertex on the face
						std::unordered_set<OvmVeH> adj_vhs;
						get_adj_boundary_vertices_around_vertex (mesh, Ni, adj_vhs);
						
						auto curr_pos = mesh->vertex (Ni);
						std::vector<OvmVec3d> Cj_vec;
						foreach (auto &adj_vh, adj_vhs){
							auto pos = mesh->vertex (adj_vh);
							auto Cj = pos - curr_pos;
								Cj_vec.push_back (Cj);
						}

						OvmVec3d upp (0, 0, 0);
						double den = 0;
						//cumulate the Vjs according to the formula
						foreach (auto &Cj, Cj_vec){
							//upp += (Cj.length () * Cj);
							//den += Cj.length ();
							upp += (1.0 * Cj);
							den += 1.0;
						}
						auto delta_i = upp / den;
						auto new_pos = curr_pos + delta_i;
				
						auto spa_pos = POS2SPA(new_pos);
						double curr_closest_dist = std::numeric_limits<double>::max ();
						ENTITY *curr_closest_f = NULL;
						SPAposition curr_closest_pos;
						for (int m = 0; m != face_group_list.count (); ++m){
							auto tmp_f = face_group_list[m];
							api_entity_point_distance (tmp_f, spa_pos, closest_pos, dist);
							if (dist < curr_closest_dist){
								curr_closest_dist = dist;
								curr_closest_pos = closest_pos;
								curr_closest_f = tmp_f;
							}
						}

						mesh->set_vertex (Ni, SPA2POS(curr_closest_pos));
						V_ENTITY_PTR[Ni] = (unsigned int)(curr_closest_f);
					}
					//QMessageBox::information (NULL, "face", QString ("round %1 finished").arg (round));
				}
#ifdef MYDEBUG
				QMessageBox::information (NULL, "face", QString("face %1 end").arg (i));
#endif
			}
		};//end auto fSmoothAllFaces = [&] (int times){..

		auto fSmoothVolumes = [&](int times){
			for (auto v_it = mesh->vertices_begin (); v_it != mesh->vertices_end (); ++v_it){
				auto entity_int = V_ENTITY_PTR[*v_it];
				if (entity_int == -1){
					volume_vhs.insert (*v_it);
					V_ENTITY_PTR[*v_it] = 0;
				}
			}
			int round = 0;
			while (round++ < times){
				foreach (auto &cur_vh, volume_vhs){
					//std::unordered_set<OvmVeH> adj_vhs;
					//get_adj_vertices_around_vertex (mesh, cur_vh, adj_vhs);
					std::unordered_set<OvmCeH> adj_chs;
					get_adj_hexas_around_vertex (mesh, cur_vh, adj_chs);

					OvmVec3d new_pos = OvmVec3d (0, 0, 0);
					foreach (auto &adj_ch, adj_chs)
						new_pos += mesh->barycenter (adj_ch);
					new_pos /= adj_chs.size ();
					mesh->set_vertex (cur_vh, new_pos);
				}
			}
		};//end auto fSmoothVolumes = [&](int times){...

		//先光顺边
		fSmoothAllEdges ();

#ifdef MYDEBUG
		QMessageBox::information (NULL, "info", "edge finished");
#endif
		//在光顺边的时候，会将一些新的未确定位置的点的位置确定，所以先要将剩下的点的位置确定，即确定它们具体在那个面上
		//首先根据boundEdge上的信息把体网格表面网格进行分块儿
		
		auto all_boundary_fhs = new std::unordered_set<OvmFaH>();
		for (auto bf_it = mesh->bf_iter (); bf_it; ++bf_it){
			all_boundary_fhs->insert (*bf_it);
		}
		std::vector<std::vector<OvmFaH> > quad_patches;
		while (!all_boundary_fhs->empty ()){
			OvmFaH first_fh = pop_begin_element (*all_boundary_fhs);
			std::queue<OvmFaH> spread_set;
			spread_set.push (first_fh);
			std::vector<OvmFaH> one_patch;
			one_patch.push_back (first_fh);
			while (!spread_set.empty ()){
				auto cur_fh = spread_set.front ();
				spread_set.pop ();
				auto heh_vec = mesh->face (cur_fh).halfedges ();
				foreach (auto heh, heh_vec){
					auto eh = mesh->edge_handle (heh);
					if (visitedEdge[eh]) continue;

					OvmFaH adj_fh = mesh->InvalidFaceHandle;
					for (auto hehf_it = mesh->hehf_iter (heh); hehf_it; ++hehf_it)
					{
						auto test_fh = mesh->face_handle (*hehf_it);
						if (test_fh == mesh->InvalidFaceHandle || test_fh == cur_fh)
							continue;
						if (all_boundary_fhs->find (test_fh) == all_boundary_fhs->end ())
							continue;
						adj_fh = test_fh;
						break;
					}
					one_patch.push_back (adj_fh);
					visitedEdge[eh] = true;
					spread_set.push (adj_fh);
				}
			}//end while (!spread_set.empty ())...
			quad_patches.push_back (one_patch);
			foreach (auto tmp_fh, one_patch)
				all_boundary_fhs->erase (tmp_fh);
#ifdef MYDEBUG
			QMessageBox::information (NULL, "faces", QString ("one patch %1 finished").arg (one_patch.size ()));
#endif
		}
		delete all_boundary_fhs;
#ifdef MYDEBUG
		QMessageBox::information (NULL, "faces", QString ("sep patches count %1").arg (quad_patches.size ()));
#endif
		
		foreach (auto &eh, all_ehs_on_geom_edges){
			auto eg = mesh->edge (eh);
			vhs_on_geom_edges.insert (eg.from_vertex ());
			vhs_on_geom_edges.insert (eg.to_vertex ());
		}

		foreach (auto &one_patch, quad_patches){
			std::unordered_set<OvmVeH> vhs_on_this_patch;
			foreach (auto &fh, one_patch){
				auto adj_vhs = get_adj_vertices_around_face (mesh, fh);
				foreach (auto &vh, adj_vhs){
					if (!contains (vhs_on_geom_edges, vh))
						vhs_on_this_patch.insert (vh);
				}
			}

			real_vhs_on_face_groups.push_back (vhs_on_this_patch);
		}
#ifdef MYDEBUG
		QMessageBox::information (NULL, "info", "sep patches finished");
#endif
		//char *colors[] = {"yellow", "pink", "blue", "green", "red", "purple"};
		//for (int i = 0; i != real_vhs_on_face_groups.size (); ++i){
		//	auto &vhs_patch = real_vhs_on_face_groups[i];
		//	auto group = new VolumeMeshElementGroup (mesh, "si", "ehs on geometric edges");
		//	group->vhs = vhs_patch;
		//	MeshRenderOptions render_options;
		//	render_options.vertex_color = colors[i%6];
		//	render_options.vertex_size = 1;
		//	hoopsview->render_mesh_group (group, render_options);
		//}
		//return;
		fSmoothAllFaces (smooth_rounds);
#ifdef MYDEBUG
		QMessageBox::information (NULL, "info", "smooth faces finished");
#endif
		fSmoothVolumes (smooth_rounds);
#ifdef MYDEBUG
		QMessageBox::information (NULL, "info", "smooth volume finished");
#endif
	}

//	void smooth_volume_mesh2 (VolumeMesh *mesh, BODY *body, int smooth_rounds, HoopsView *hoopsview)
//	{
//		//#define MYDEBUG
//#ifdef MYDEBUG
//		QMessageBox::information (NULL, "pre", "enter");
//#endif
//		assert (mesh->vertex_property_exists<unsigned int> ("entityptr"));
//		auto V_ENTITY_PTR = mesh->request_vertex_property<unsigned int>("entityptr");
//
//		std::unordered_set<OvmVeH> fixed_vhs;
//		if (mesh->vertex_property_exists<bool> ("vertexfixedforsmooth")){
//			auto V_FIXED = mesh->request_vertex_property<bool> ("vertexfixedforsmooth");
//			for (auto v_it = mesh->vertices_begin (); v_it != mesh->vertices_end (); ++v_it){
//				if (V_FIXED[*v_it] == true) fixed_vhs.insert (*v_it);
//			}
//		}
//		std::hash_map<VERTEX*, OvmVeH> vertices_mapping;
//		std::hash_map<EDGE*, std::unordered_set<OvmVeH> > edges_vertices_mapping;
//		std::hash_map<FACE*, std::unordered_set<OvmVeH> > faces_vertices_mapping;
//		std::unordered_set<OvmVeH> volume_vhs, new_boundary_vhs;
//
//		ENTITY_LIST vertices_list, edges_list, faces_list;
//		api_get_vertices (body, vertices_list);
//		api_get_edges (body, edges_list);
//		api_get_faces (body, faces_list);
//
//		auto visitedEdge = mesh->request_edge_property<bool> ("edgevisited", false);
//
//		std::set<FACE*> all_faces;
//		entity_list_to_set (faces_list, all_faces);
//		std::hash_map<ENTITY*, int> edge_face_group_mapping;
//		std::vector<std::unordered_set<OvmVeH> > determined_vhs_on_face_groups, real_vhs_on_face_groups;
//		std::unordered_set<OvmEgH> all_ehs_on_geom_edges;
//		std::unordered_set<OvmVeH> vhs_on_geom_edges;
//
//		for (auto v_it = mesh->vertices_begin (); v_it != mesh->vertices_end (); ++v_it)
//		{
//			auto entity_ptr_uint = V_ENTITY_PTR[*v_it];
//			if (entity_ptr_uint == 0) volume_vhs.insert (*v_it);
//			else if (entity_ptr_uint == -1){
//				//判断一下这个新点是否在体内，如果是的话，则将他的entity_ptr设置为0
//				if (!mesh->is_boundary (*v_it)){
//					V_ENTITY_PTR[*v_it] = 0;
//					volume_vhs.insert (*v_it);
//				}else
//					new_boundary_vhs.insert (*v_it);
//			}else{
//				ENTITY *entity = (ENTITY*)(entity_ptr_uint);
//				if (is_VERTEX (entity)) vertices_mapping[(VERTEX*)entity] = *v_it;
//				//else if (is_EDGE (entity)) edges_vertices_mapping[(EDGE*)entity].insert (*v_it);
//				//else faces_vertices_mapping[(FACE*)entity].insert (*v_it);
//				else if (is_EDGE (entity)){
//					edges_vertices_mapping[(EDGE*)entity].insert (*v_it);
//				}else{
//					auto f = (FACE*)entity;
//					faces_vertices_mapping[f].insert (*v_it);
//				}
//			}
//		}
//		//if (!new_boundary_vhs.empty ())
//		//	QMessageBox::information (NULL, "info", "new_boundary_vhs不为空");
//#ifdef MYDEBUG
//		QMessageBox::information (NULL, "pre", "step2");
//#endif
//		if (vertices_list.count () != vertices_mapping.size ()){
//			if (hoopsview){
//				//auto group = new VolumeMeshElementGroup (mesh, "si", "quad set for inflation");
//				//foreach (auto &p, vertices_mapping){
//				//	group->vhs.insert (p.second);
//				//}
//				//MeshRenderOptions render_options;
//				//render_options.vertex_color = "red";
//				//render_options.vertex_size = 2;
//				//hoopsview->render_mesh_group (group, render_options);
//				HC_Open_Segment_By_Key (hoopsview->GetHoopsView ()->GetModelKey ());{
//					HC_Open_Segment ("geom vertices");{
//						HC_Flush_Geometry ("...");
//						HC_Set_Marker_Size (2);
//						HC_Set_Color ("markers=blue");
//						HC_Set_Visibility ("markers=on");
//						for (int i = 0; i != vertices_list.count (); ++i){
//							auto V = (VERTEX*)(vertices_list[i]);
//							if (vertices_mapping.find (V) == vertices_mapping.end ()){
//								SPAposition pos = V->geometry ()->coords ();
//								HC_Insert_Marker (pos.x (), pos.y (), pos.z ());
//							}
//						}
//					}HC_Close_Segment ();
//				}HC_Close_Segment ();
//				hoopsview->GetHoopsView ()->SetGeometryChanged ();
//				hoopsview->GetHoopsView ()->Update ();
//				hoopsview->show_mesh_faces (false);
//				return;
//			}
//		}
//
//		auto fDivideOneEdge = [&](EDGE *edge, int dis_num, std::vector<SPAposition> &pts){
//			SPAinterval inter = edge->param_range ();
//			double step = inter.length () / dis_num,
//				start_param = inter.start_pt ();
//			curve *crv = edge->geometry ()->trans_curve ();
//
//			for (int i = 0; i <= dis_num; ++i){
//				double param_val = start_param + step * i;
//				SPAposition pos;
//				crv->eval (param_val, pos);
//				pts.push_back (pos);
//			}
//		};
//
//		auto fSmoothAllEdges = [&](){
//			for (int i = 0; i != edges_list.count (); ++i){
//#ifdef MYDEBUG
//				QMessageBox::information (NULL, "edge", QString("edge %1 begin").arg (i));
//#endif
//				ENTITY *entity = edges_list[i];
//
//				EDGE *acis_edge = (EDGE*)entity;
//				VERTEX *acis_v1 = acis_edge->start (),
//					*acis_v2 = acis_edge->end ();
//				OvmVeH mesh_vh1 = vertices_mapping[acis_v1],
//					mesh_vh2 = vertices_mapping[acis_v2];
//				auto &mesh_vhs_set = edges_vertices_mapping[acis_edge];
//				mesh_vhs_set.insert (mesh_vh2);
//				std::vector<OvmVeH> sorted_vhs;
//				while (mesh_vh1 != mesh_vh2){
//					sorted_vhs.push_back (mesh_vh1);
//					OvmVeH next_vh = mesh->InvalidVertexHandle;
//					//首先在mesh_vhs_set中找下一个顶点
//					for (auto v_it = mesh_vhs_set.begin (); v_it != mesh_vhs_set.end (); ++v_it)
//					{
//						OvmHaEgH test_heh = mesh->halfedge (mesh_vh1, *v_it);
//						if (test_heh != mesh->InvalidHalfEdgeHandle){
//							next_vh = *v_it;
//							mesh_vhs_set.erase (next_vh);
//							break;
//						}
//					}
//					//如果在mesh_vhs_set中找不到，那么在new_vhs中继续找
//					if (next_vh == mesh->InvalidVertexHandle){
//						double min_dist = std::numeric_limits<double>::max ();
//						OvmVeH best_next_vh = OvmVeH (-1);
//						foreach (auto &vh, new_boundary_vhs){
//							OvmHaEgH test_heh = mesh->halfedge (mesh_vh1, vh);
//							if (test_heh != mesh->InvalidHalfEdgeHandle){
//								SPAposition closest_pt;
//								double dist = 0.0f;
//								OvmVec3d pos = mesh->vertex (vh);
//								api_entity_point_distance (acis_edge, POS2SPA(pos), closest_pt, dist);
//								if (dist < min_dist){
//									best_next_vh = vh;
//									min_dist = dist;
//								}
//							}
//						}
//						next_vh = best_next_vh;
//						assert (next_vh != mesh->InvalidVertexHandle);
//						new_boundary_vhs.erase (next_vh);
//						V_ENTITY_PTR[next_vh] = (unsigned int) acis_edge;
//					}
//					mesh_vh1 = next_vh;
//				}//end while (mesh_vh1 != mesh_vh2){...
//				sorted_vhs.push_back (mesh_vh2);
//
//#ifdef MYDEBUG
//				QMessageBox::information (NULL, "edge", QString("edge %1 out while").arg (i));
//#endif
//				std::vector<SPAposition> pts;
//				fDivideOneEdge (acis_edge, sorted_vhs.size () - 1, pts);
//
//				//如果和边的离散顺序相反，则翻转一下
//				if (!same_point (pts.front (), POS2SPA(mesh->vertex (sorted_vhs.front ())), SPAresabs * 1000))
//					std::reverse (sorted_vhs.begin (), sorted_vhs.end ());
//				for (int j = 0; j != sorted_vhs.size (); ++j)
//					mesh->set_vertex (sorted_vhs[j], SPA2POS(pts[j]));
//				if (sorted_vhs.size () > 2){
//					for (int j = 1; j != sorted_vhs.size () - 1; ++j){
//						V_ENTITY_PTR[sorted_vhs[j]] = (unsigned int)acis_edge;
//					}
//				}
//
//				for (int j = 0; j != sorted_vhs.size () - 1; ++j){
//					auto eh = mesh->edge_handle (mesh->halfedge (sorted_vhs[j], sorted_vhs[j + 1]));
//					if (eh == mesh->InvalidEdgeHandle)
//						QMessageBox::warning (NULL, "ERROR", "eh invalid!");
//					visitedEdge[eh] = true;
//					all_ehs_on_geom_edges.insert (eh);
//				}
//#ifdef MYDEBUG
//				QMessageBox::information (NULL, "edge", QString("edge %1 finished").arg (i));
//#endif
//			}
//		};//end auto fSmoothAllEdges = [&](){...
//
//		auto fSmoothAllFaces = [&] (int times){
//			for (int i = 0; i != cylindrical_face_groups.size (); ++i){
//				//first we collect all the vertices handles on this face
//				auto &vhs_test = determined_vhs_on_face_groups[i];
//				std::unordered_set<OvmVeH> vhs_on_this_group;
//				foreach (auto &vhs, real_vhs_on_face_groups){
//					if (intersects (vhs, vhs_test)){
//						vhs_on_this_group = vhs; break;
//					}
//				}
//				if (vhs_on_this_group.empty ()){
//					//QMessageBox::information (NULL, "face", QString("face %1 empty!").arg (i));
//				}
//				//then we try to smooth them using simplest laplacian smoothing
//				int round = 0;
//				ENTITY_LIST face_group_list;
//				foreach (auto &f, cylindrical_face_groups[i])
//					face_group_list.add (f);
//
//#ifdef MYDEBUG
//				QMessageBox::information (NULL, "face", QString("face %1 enter while").arg (i));
//#endif
//
//				while (round++ < times){
//					std::vector<OvmVeH> vhs_vec;
//					unordered_set_to_vector (vhs_on_this_group, vhs_vec);
//					SPAposition closest_pos;
//					double dist;
//					for (int j = 0; j != vhs_vec.size (); ++j){
//						auto Ni = vhs_vec[j];
//						if (contains (fixed_vhs, Ni)) continue;
//
//						//get all the adjacent vertices of this vertex on the face
//						std::unordered_set<OvmVeH> adj_vhs;
//						get_adj_boundary_vertices_around_vertex (mesh, Ni, adj_vhs);
//
//						auto curr_pos = mesh->vertex (Ni);
//						std::vector<OvmVec3d> Cj_vec;
//						foreach (auto &adj_vh, adj_vhs){
//							auto pos = mesh->vertex (adj_vh);
//							auto Cj = pos - curr_pos;
//							Cj_vec.push_back (Cj);
//						}
//
//						OvmVec3d upp (0, 0, 0);
//						double den = 0;
//						//cumulate the Vjs according to the formula
//						foreach (auto &Cj, Cj_vec){
//							//upp += (Cj.length () * Cj);
//							//den += Cj.length ();
//							upp += (1.0 * Cj);
//							den += 1.0;
//						}
//						auto delta_i = upp / den;
//						auto new_pos = curr_pos + delta_i;
//
//						auto spa_pos = POS2SPA(new_pos);
//						double curr_closest_dist = std::numeric_limits<double>::max ();
//						ENTITY *curr_closest_f = NULL;
//						SPAposition curr_closest_pos;
//						for (int m = 0; m != face_group_list.count (); ++m){
//							auto tmp_f = face_group_list[m];
//							api_entity_point_distance (tmp_f, spa_pos, closest_pos, dist);
//							if (dist < curr_closest_dist){
//								curr_closest_dist = dist;
//								curr_closest_pos = closest_pos;
//								curr_closest_f = tmp_f;
//							}
//						}
//
//						mesh->set_vertex (Ni, SPA2POS(curr_closest_pos));
//						V_ENTITY_PTR[Ni] = (unsigned int)(curr_closest_f);
//					}
//					//QMessageBox::information (NULL, "face", QString ("round %1 finished").arg (round));
//				}
//#ifdef MYDEBUG
//				QMessageBox::information (NULL, "face", QString("face %1 end").arg (i));
//#endif
//			}
//		};//end auto fSmoothAllFaces = [&] (int times){..
//
//		auto fSmoothVolumes = [&](int times){
//			for (auto v_it = mesh->vertices_begin (); v_it != mesh->vertices_end (); ++v_it){
//				auto entity_int = V_ENTITY_PTR[*v_it];
//				if (entity_int == -1){
//					volume_vhs.insert (*v_it);
//					V_ENTITY_PTR[*v_it] = 0;
//				}
//			}
//			int round = 0;
//			while (round++ < times){
//				foreach (auto &cur_vh, volume_vhs){
//					//std::unordered_set<OvmVeH> adj_vhs;
//					//get_adj_vertices_around_vertex (mesh, cur_vh, adj_vhs);
//					std::unordered_set<OvmCeH> adj_chs;
//					get_adj_hexas_around_vertex (mesh, cur_vh, adj_chs);
//
//					OvmVec3d new_pos = OvmVec3d (0, 0, 0);
//					foreach (auto &adj_ch, adj_chs)
//						new_pos += mesh->barycenter (adj_ch);
//					new_pos /= adj_chs.size ();
//					mesh->set_vertex (cur_vh, new_pos);
//				}
//			}
//		};//end auto fSmoothVolumes = [&](int times){...
//
//		//先光顺边
//		fSmoothAllEdges ();
//
//#ifdef MYDEBUG
//		QMessageBox::information (NULL, "info", "edge finished");
//#endif
//		//在光顺边的时候，会将一些新的未确定位置的点的位置确定，所以先要将剩下的点的位置确定，即确定它们具体在那个面上
//		//首先根据boundEdge上的信息把体网格表面网格进行分块儿
//
//		auto all_boundary_fhs = new std::unordered_set<OvmFaH>();
//		for (auto bf_it = mesh->bf_iter (); bf_it; ++bf_it){
//			all_boundary_fhs->insert (*bf_it);
//		}
//		std::vector<std::vector<OvmFaH> > quad_patches;
//		while (!all_boundary_fhs->empty ()){
//			OvmFaH first_fh = pop_begin_element (*all_boundary_fhs);
//			std::queue<OvmFaH> spread_set;
//			spread_set.push (first_fh);
//			std::vector<OvmFaH> one_patch;
//			one_patch.push_back (first_fh);
//			while (!spread_set.empty ()){
//				auto cur_fh = spread_set.front ();
//				spread_set.pop ();
//				auto heh_vec = mesh->face (cur_fh).halfedges ();
//				foreach (auto heh, heh_vec){
//					auto eh = mesh->edge_handle (heh);
//					if (visitedEdge[eh]) continue;
//
//					OvmFaH adj_fh = mesh->InvalidFaceHandle;
//					for (auto hehf_it = mesh->hehf_iter (heh); hehf_it; ++hehf_it)
//					{
//						auto test_fh = mesh->face_handle (*hehf_it);
//						if (test_fh == mesh->InvalidFaceHandle || test_fh == cur_fh)
//							continue;
//						if (all_boundary_fhs->find (test_fh) == all_boundary_fhs->end ())
//							continue;
//						adj_fh = test_fh;
//						break;
//					}
//					one_patch.push_back (adj_fh);
//					visitedEdge[eh] = true;
//					spread_set.push (adj_fh);
//				}
//			}//end while (!spread_set.empty ())...
//			quad_patches.push_back (one_patch);
//			foreach (auto tmp_fh, one_patch)
//				all_boundary_fhs->erase (tmp_fh);
//#ifdef MYDEBUG
//			QMessageBox::information (NULL, "faces", QString ("one patch %1 finished").arg (one_patch.size ()));
//#endif
//		}
//		delete all_boundary_fhs;
//#ifdef MYDEBUG
//		QMessageBox::information (NULL, "faces", QString ("sep patches count %1").arg (quad_patches.size ()));
//#endif
//
//		foreach (auto &eh, all_ehs_on_geom_edges){
//			auto eg = mesh->edge (eh);
//			vhs_on_geom_edges.insert (eg.from_vertex ());
//			vhs_on_geom_edges.insert (eg.to_vertex ());
//		}
//
//		foreach (auto &one_patch, quad_patches){
//			std::unordered_set<OvmVeH> vhs_on_this_patch;
//			foreach (auto &fh, one_patch){
//				auto adj_vhs = get_adj_vertices_around_face (mesh, fh);
//				foreach (auto &vh, adj_vhs){
//					if (!contains (vhs_on_geom_edges, vh))
//						vhs_on_this_patch.insert (vh);
//				}
//			}
//
//			real_vhs_on_face_groups.push_back (vhs_on_this_patch);
//		}
//#ifdef MYDEBUG
//		QMessageBox::information (NULL, "info", "sep patches finished");
//#endif
//		//char *colors[] = {"yellow", "pink", "blue", "green", "red", "purple"};
//		//for (int i = 0; i != real_vhs_on_face_groups.size (); ++i){
//		//	auto &vhs_patch = real_vhs_on_face_groups[i];
//		//	auto group = new VolumeMeshElementGroup (mesh, "si", "ehs on geometric edges");
//		//	group->vhs = vhs_patch;
//		//	MeshRenderOptions render_options;
//		//	render_options.vertex_color = colors[i%6];
//		//	render_options.vertex_size = 1;
//		//	hoopsview->render_mesh_group (group, render_options);
//		//}
//		//return;
//		fSmoothAllFaces (smooth_rounds);
//#ifdef MYDEBUG
//		QMessageBox::information (NULL, "info", "smooth faces finished");
//#endif
//		fSmoothVolumes (smooth_rounds);
//#ifdef MYDEBUG
//		QMessageBox::information (NULL, "info", "smooth volume finished");
//#endif
//	}
	void smooth_acis_edges (VolumeMesh *mesh, std::vector<EDGE*> &acis_edges, int smooth_rounds, HoopsView *hoopsview)
	{
	}

	void smooth_acis_faces (VolumeMesh *mesh, std::vector<FACE*> &acis_faces, int smooth_rounds, HoopsView *hoopsview)
	{
		//获得acis_faces上的边和点
		std::unordered_set<VERTEX*> geom_vts_on_faces;
		std::unordered_set<EDGE*> geom_egs_on_faces, shared_egs_on_faces;
		foreach (auto f, acis_faces){
			ENTITY_LIST edge_list;
			api_get_edges (f, edge_list);
			for (int i = 0; i != edge_list.count (); ++i){
				auto eg = (EDGE*)(edge_list[i]);
				if (contains (geom_egs_on_faces, eg))
					shared_egs_on_faces.insert (eg);
				else
					geom_egs_on_faces.insert (eg);
			}
		}
		foreach (auto acis_edge, geom_egs_on_faces){
			VERTEX *acis_v1 = acis_edge->start (),
				*acis_v2 = acis_edge->end ();
			geom_vts_on_faces.insert (acis_v1);
			geom_vts_on_faces.insert (acis_v2);
		}

		assert (mesh->vertex_property_exists<unsigned int> ("entityptr"));
		auto V_ENTITY_PTR = mesh->request_vertex_property<unsigned int>("entityptr");
		auto V_FIXED = mesh->request_vertex_property<bool> ("vertexfixedforsmooth");


		std::set<FACE*> all_faces;
		vector_to_set (acis_faces, all_faces);
		std::vector<std::set<FACE*> > cylindrical_face_groups;
		std::hash_map<EDGE*, int> edge_face_group_mapping;
		auto determined_vhs_on_face_groups = new std::vector<std::unordered_set<OvmVeH> > (), 
			real_vhs_on_face_groups = new std::vector<std::unordered_set<OvmVeH> > ();
		auto all_ehs_on_geom_edges = new std::unordered_set<OvmEgH> ();

		std::unordered_set<OvmVeH> vhs_on_geom_edges;
		auto edges_vertices_mapping = new std::hash_map<EDGE*, std::unordered_set<OvmVeH> >();
		auto faces_vertices_mapping = new std::hash_map<FACE*, std::unordered_set<OvmVeH> > ();

		//找出哪些边是类似圆柱侧面上的母线那种可以忽略的边
		std::set<EDGE*> cylinder_edges;
		//foreach (auto acis_edge, shared_egs_on_faces){
		//	ENTITY_LIST adj_faces;
		//	api_get_faces (acis_edge, adj_faces);
		//	bool is_cyl_f = true;
		//	//这种边的特点是和它相邻的两个面都是圆柱面，而它本身又是直线
		//	//注意：这种判别方式并不严谨，但是大部分情况可以这么判断
		//	for (int j = 0; j != adj_faces.count (); ++j){
		//		if (!is_cylindrical_face (adj_faces[j])){
		//			is_cyl_f = false; break;
		//		}
		//	}
		//	if (is_cyl_f && is_linear_edge (acis_edge)){
		//		cylinder_edges.insert (acis_edge);
		//		std::set<FACE*> adj_faces_set;
		//		entity_list_to_set (adj_faces, adj_faces_set);
		//		if (contains (all_faces, adj_faces_set)){
		//			edge_face_group_mapping[acis_edge] = cylindrical_face_groups.size ();
		//			cylindrical_face_groups.push_back (adj_faces_set);
		//		}else{
		//			auto locate = std::find_if (cylindrical_face_groups.begin (), cylindrical_face_groups.end (), 
		//				[&](const std::set<FACE*> &one_group)->bool{
		//					if (intersects (one_group, adj_faces_set)) return true;
		//					else return false;
		//			});
		//			assert (locate != cylindrical_face_groups.end ());
		//			foreach (auto &f, adj_faces_set)
		//				locate->insert (f);
		//			edge_face_group_mapping[acis_edge] = locate - cylindrical_face_groups.begin ();
		//		}
		//		foreach (auto &f, adj_faces_set)
		//			all_faces.erase (f);
		//	}
		//}

		//对于all_face中不和其他面合并的面，让他们也单独成为一个group，便于后面统一处理
		foreach (auto &f, all_faces){
			std::set<FACE*> one_group;
			one_group.insert (f);
			cylindrical_face_groups.push_back (one_group);
		}

		determined_vhs_on_face_groups->resize (cylindrical_face_groups.size ());

		auto fixed_vhs = new std::unordered_set<OvmVeH> ();
		auto volume_vhs = new std::unordered_set<OvmVeH> ();
		auto vertices_mapping = new std::hash_map<VERTEX*, OvmVeH> ();
		auto new_boundary_vhs = new std::unordered_set<OvmVeH> ();

		for (auto v_it = mesh->vertices_begin (); v_it != mesh->vertices_end (); ++v_it){
			auto entity_ptr_uint = V_ENTITY_PTR[*v_it];
			if (!mesh->is_boundary (*v_it)){
				volume_vhs->insert (*v_it);
			}
			else if (V_FIXED[*v_it] == true) fixed_vhs->insert (*v_it);
			else{
				ENTITY *entity = (ENTITY*)(entity_ptr_uint);
				if (entity == NULL) new_boundary_vhs->insert (*v_it);
				else if (is_VERTEX (entity)){
					auto vt = (VERTEX*)entity;
					//只搜集当前需要处理的几何点
					if (contains (geom_vts_on_faces, vt))
						vertices_mapping->insert (std::make_pair(vt, *v_it));
				}else if (is_EDGE (entity)){
					auto eg = (EDGE*)entity;
					//只处理当前需要处理的几何边
					if (!contains (geom_egs_on_faces, eg)) continue;
					if (contains (cylinder_edges, eg)){
						int idx = edge_face_group_mapping[eg];
						(*determined_vhs_on_face_groups)[idx].insert (*v_it);
					}else{
						(*edges_vertices_mapping)[eg].insert (*v_it);
					}
				}else{
					auto f = (FACE*)entity;
					auto locate = std::find_if (cylindrical_face_groups.begin (), cylindrical_face_groups.end (), 
						[&](const std::set<FACE*> &one_group)->bool{
							if (contains (one_group, f)) return true;
							else return false;
					});
					//只处理当前需要处理的几何面
					if (locate != cylindrical_face_groups.end ())
						(*determined_vhs_on_face_groups)[locate - cylindrical_face_groups.begin ()].insert (*v_it);
				}
			}
		}

		auto visitedEdge = mesh->request_edge_property<bool> ("edgevisited", false);

		if (geom_vts_on_faces.size () != vertices_mapping->size ()){
			if (hoopsview){
				HC_Open_Segment_By_Key (hoopsview->GetHoopsView ()->GetModelKey ());{
					HC_Open_Segment ("geom vertices");{
						HC_Flush_Geometry ("...");
						HC_Set_Marker_Size (2);
						HC_Set_Color ("markers=blue");
						HC_Set_Visibility ("markers=on");
						foreach (auto V, geom_vts_on_faces){
							if (vertices_mapping->find (V) == vertices_mapping->end ()){
								SPAposition pos = V->geometry ()->coords ();
								HC_Insert_Marker (pos.x (), pos.y (), pos.z ());
							}
						}
					}HC_Close_Segment ();
				}HC_Close_Segment ();
				hoopsview->GetHoopsView ()->SetGeometryChanged ();
				hoopsview->GetHoopsView ()->Update ();
				hoopsview->show_mesh_faces (false);
				return;
			}
		}

		auto fDivideOneEdge = [&](EDGE *edge, int dis_num, std::vector<SPAposition> &pts){
			SPAinterval inter = edge->param_range ();
			double step = inter.length () / dis_num,
				start_param = inter.start_pt ();
			curve *crv = edge->geometry ()->trans_curve ();

			for (int i = 0; i <= dis_num; ++i){
				double param_val = start_param + step * i;
				SPAposition pos;
				crv->eval (param_val, pos);
				pts.push_back (pos);
			}
		};

		//////////////////////////////////////////////////////////////////////////
		//smooth edge
		//////////////////////////////////////////////////////////////////////////
		foreach (auto acis_edge, geom_egs_on_faces){
			if (contains (cylinder_edges, acis_edge)) continue;
			if (edges_vertices_mapping->find (acis_edge) == edges_vertices_mapping->end ()) continue;
			VERTEX *acis_v1 = acis_edge->start (),
				*acis_v2 = acis_edge->end ();
			OvmVeH mesh_vh1 = vertices_mapping->at (acis_v1),
				mesh_vh2 = vertices_mapping->at (acis_v2);
			auto &mesh_vhs_set = edges_vertices_mapping->at (acis_edge);
			mesh_vhs_set.insert (mesh_vh2);
			std::vector<OvmVeH> sorted_vhs;
			while (mesh_vh1 != mesh_vh2){
				sorted_vhs.push_back (mesh_vh1);
				OvmVeH next_vh = mesh->InvalidVertexHandle;
				//首先在mesh_vhs_set中找下一个顶点
				for (auto v_it = mesh_vhs_set.begin (); v_it != mesh_vhs_set.end (); ++v_it)
				{
					OvmHaEgH test_heh = mesh->halfedge (mesh_vh1, *v_it);
					if (test_heh != mesh->InvalidHalfEdgeHandle){
						next_vh = *v_it;
						mesh_vhs_set.erase (next_vh);
						break;
					}
				}
				//如果在mesh_vhs_set中找不到，那么在new_vhs中继续找
				if (next_vh == mesh->InvalidVertexHandle){
					double min_dist = std::numeric_limits<double>::max ();
					OvmVeH best_next_vh = OvmVeH (-1);
					foreach (auto &vh, *new_boundary_vhs){
						OvmHaEgH test_heh = mesh->halfedge (mesh_vh1, vh);
						if (test_heh != mesh->InvalidHalfEdgeHandle){
							SPAposition closest_pt;
							double dist = 0.0f;
							OvmVec3d pos = mesh->vertex (vh);
							api_entity_point_distance (acis_edge, POS2SPA(pos), closest_pt, dist);
							if (dist < min_dist){
								best_next_vh = vh;
								min_dist = dist;
							}
						}
					}
					next_vh = best_next_vh;
					assert (next_vh != mesh->InvalidVertexHandle);
					new_boundary_vhs->erase (next_vh);
					V_ENTITY_PTR[next_vh] = (unsigned int) acis_edge;
				}
				mesh_vh1 = next_vh;
			}//end while (mesh_vh1 != mesh_vh2){...
			sorted_vhs.push_back (mesh_vh2);

			std::vector<SPAposition> pts;
			fDivideOneEdge (acis_edge, sorted_vhs.size () - 1, pts);

			//如果和边的离散顺序相反，则翻转一下
			if (!same_point (pts.front (), POS2SPA(mesh->vertex (sorted_vhs.front ())), SPAresabs * 1000))
				std::reverse (sorted_vhs.begin (), sorted_vhs.end ());
			for (int j = 0; j != sorted_vhs.size (); ++j)
				mesh->set_vertex (sorted_vhs[j], SPA2POS(pts[j]));
			if (sorted_vhs.size () > 2){
				for (int j = 1; j != sorted_vhs.size () - 1; ++j){
					V_ENTITY_PTR[sorted_vhs[j]] = (unsigned int)acis_edge;
				}
			}

			for (int j = 0; j != sorted_vhs.size () - 1; ++j){
				auto eh = mesh->edge_handle (mesh->halfedge (sorted_vhs[j], sorted_vhs[j + 1]));
				if (eh == mesh->InvalidEdgeHandle)
					QMessageBox::warning (NULL, "ERROR", "eh invalid!");
				visitedEdge[eh] = true;
				all_ehs_on_geom_edges->insert (eh);
			}
		}

		//////////////////////////////////////////////////////////////////////////
		//smooth face
		//////////////////////////////////////////////////////////////////////////
		//首先根据前面对边的光顺，可以将网格表面的四边形面分成若干个集合，
		//然后对每个集合进行光顺
		auto all_boundary_fhs = new std::unordered_set<OvmFaH>();
		for (auto bf_it = mesh->bf_iter (); bf_it; ++bf_it){
			all_boundary_fhs->insert (*bf_it);
		}
		auto quad_patches = new std::vector<std::vector<OvmFaH> > ();
		while (!all_boundary_fhs->empty ()){
			OvmFaH first_fh = pop_begin_element (*all_boundary_fhs);
			std::queue<OvmFaH> spread_set;
			spread_set.push (first_fh);
			std::vector<OvmFaH> one_patch;
			one_patch.push_back (first_fh);
			while (!spread_set.empty ()){
				auto cur_fh = spread_set.front ();
				spread_set.pop ();
				auto heh_vec = mesh->face (cur_fh).halfedges ();
				foreach (auto heh, heh_vec){
					auto eh = mesh->edge_handle (heh);
					if (visitedEdge[eh]) continue;

					OvmFaH adj_fh = mesh->InvalidFaceHandle;
					for (auto hehf_it = mesh->hehf_iter (heh); hehf_it; ++hehf_it)
					{
						auto test_fh = mesh->face_handle (*hehf_it);
						if (test_fh == mesh->InvalidFaceHandle || test_fh == cur_fh)
							continue;
						if (all_boundary_fhs->find (test_fh) == all_boundary_fhs->end ())
							continue;
						adj_fh = test_fh;
						break;
					}
					one_patch.push_back (adj_fh);
					visitedEdge[eh] = true;
					spread_set.push (adj_fh);
				}
			}//end while (!spread_set.empty ())...
			quad_patches->push_back (one_patch);
			foreach (auto tmp_fh, one_patch)
				all_boundary_fhs->erase (tmp_fh);
		}
		delete all_boundary_fhs;

		foreach (auto &eh, *all_ehs_on_geom_edges){
			auto eg = mesh->edge (eh);
			vhs_on_geom_edges.insert (eg.from_vertex ());
			vhs_on_geom_edges.insert (eg.to_vertex ());
		}

		for (int i = 0; i != quad_patches->size (); ++i){
			std::unordered_set<OvmVeH> vhs_on_this_patch;
			foreach (auto &fh, quad_patches->at (i)){
				auto adj_vhs = get_adj_vertices_around_face (mesh, fh);
				foreach (auto &vh, adj_vhs){
					if (!contains (vhs_on_geom_edges, vh))
						vhs_on_this_patch.insert (vh);
				}
			}

			real_vhs_on_face_groups->push_back (vhs_on_this_patch);
		}

		for (int i = 0; i != cylindrical_face_groups.size (); ++i){
			//first we collect all the vertices handles on this face
			auto &vhs_test = determined_vhs_on_face_groups->at (i);
			std::unordered_set<OvmVeH> vhs_on_this_group;
			for (int m = 0; m != real_vhs_on_face_groups->size (); ++m){
				if (intersects (real_vhs_on_face_groups->at (m), vhs_test)){
					vhs_on_this_group = real_vhs_on_face_groups->at (m); break;
				}
			}
			if (vhs_on_this_group.empty ())
				QMessageBox::information (NULL, "face", QString("face %1 empty!").arg (i));
			//then we try to smooth them using simplest laplacian smoothing
			int round = 0;
			ENTITY_LIST face_group_list;
			foreach (auto &f, cylindrical_face_groups[i])
				face_group_list.add (f);

			while (round++ < smooth_rounds){
				std::vector<OvmVeH> vhs_vec;
				unordered_set_to_vector (vhs_on_this_group, vhs_vec);
				SPAposition closest_pos;
				double dist;
				for (int j = 0; j != vhs_vec.size (); ++j){
					auto Ni = vhs_vec[j];
					if (contains (fixed_vhs, Ni)) continue;

					//get all the adjacent vertices of this vertex on the face
					std::unordered_set<OvmVeH> adj_vhs;
					get_adj_boundary_vertices_around_vertex (mesh, Ni, adj_vhs);

					OvmVec3d new_pos (0, 0, 0);
					foreach (auto &adj_vh, adj_vhs){
						auto pos = mesh->vertex (adj_vh);
						new_pos += pos;
					}
					new_pos /= adj_vhs.size ();

					auto spa_pos = POS2SPA(new_pos);
					double curr_closest_dist = std::numeric_limits<double>::max ();
					ENTITY *curr_closest_f = NULL;
					SPAposition curr_closest_pos;
					for (int m = 0; m != face_group_list.count (); ++m){
						auto tmp_f = face_group_list[m];
						api_entity_point_distance (tmp_f, spa_pos, closest_pos, dist);
						if (dist < curr_closest_dist){
							curr_closest_dist = dist;
							curr_closest_pos = closest_pos;
							curr_closest_f = tmp_f;
						}
					}

					mesh->set_vertex (Ni, SPA2POS(curr_closest_pos));
					V_ENTITY_PTR[Ni] = (unsigned int)(curr_closest_f);
				}
				//QMessageBox::information (NULL, "face", QString ("round %1 finished").arg (round));
			}

		}

		//////////////////////////////////////////////////////////////////////////
		//smooth volume
		//////////////////////////////////////////////////////////////////////////
		for (auto it = new_boundary_vhs->begin (); it != new_boundary_vhs->end (); ++it){
			auto vh = *it;
			auto entity_int = V_ENTITY_PTR[vh];
			if (entity_int == -1){
				volume_vhs->insert (vh);
				V_ENTITY_PTR[vh] = 0;
			}
		}

		int round = 0;
		while (round++ < smooth_rounds){
			for (auto it = volume_vhs->begin (); it != volume_vhs->end (); ++it){
				auto cur_vh = *it;
				//std::unordered_set<OvmVeH> adj_vhs;
				//get_adj_vertices_around_vertex (mesh, cur_vh, adj_vhs);
				std::unordered_set<OvmCeH> adj_chs;
				get_adj_hexas_around_vertex (mesh, cur_vh, adj_chs);

				OvmVec3d new_pos = OvmVec3d (0, 0, 0);
				foreach (auto &adj_ch, adj_chs)
					new_pos += mesh->barycenter (adj_ch);
				new_pos /= adj_chs.size ();
				mesh->set_vertex (cur_vh, new_pos);
			}
		}

		//////////////////////////////////////////////////////////////////////////
		//clean up
		//////////////////////////////////////////////////////////////////////////
		delete determined_vhs_on_face_groups;
		delete real_vhs_on_face_groups;
		delete all_ehs_on_geom_edges;
		delete edges_vertices_mapping;
		delete faces_vertices_mapping;
		delete fixed_vhs;
		delete volume_vhs;
		delete vertices_mapping;
		delete new_boundary_vhs;
		delete quad_patches;
	}
}
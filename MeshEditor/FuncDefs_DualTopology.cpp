#include "stdafx.h"
#include "FuncDefs.h"
#include <OpenVolumeMesh/Attribs/StatusAttrib.hh>

namespace JC{
	void get_cell_groups_around_vertex (VolumeMesh *mesh, OvmVeH vh, std::unordered_set<OvmFaH> &fhs,
		std::vector<std::unordered_set<OvmCeH> > &cell_groups)
	{
		std::unordered_set<OvmCeH> all_cells;
		get_adj_hexas_around_vertex (mesh, vh, all_cells);

		while (!all_cells.empty ()) {
			auto ch = *(all_cells.begin ());
			std::unordered_set<OvmCeH> one_group;
			one_group.insert (ch);
			std::queue<OvmCeH> spread_set;
			spread_set.push (ch);
			while (!spread_set.empty ()){
				ch = spread_set.front ();
				spread_set.pop ();

				auto hfh_vec = mesh->cell (ch).halffaces ();
				foreach (auto &hfh, hfh_vec){
					auto fh = mesh->face_handle (hfh);
					if (contains (fhs, fh)) continue;
					auto oppo_ch = mesh->incident_cell (mesh->opposite_halfface_handle (hfh));
					if (oppo_ch == mesh->InvalidCellHandle) continue;
					if (!contains (all_cells, oppo_ch)) continue;
					if (contains (one_group, oppo_ch)) continue;

					spread_set.push (oppo_ch);
					one_group.insert (oppo_ch);
				}
			}//end while (!spread_set.empty ())...
			cell_groups.push_back (one_group);
			foreach (auto &ch, one_group)
				all_cells.erase (ch);
		}
	}

	//void get_hfhs_groups_around_vertex (VolumeMesh *mesh, OvmVeH vh, std::unordered_set<OvmFaH> &fhs,
	//	std::vector<std::unordered_set<OvmHaFaH> > &hfh_groups)
	//{
	//	std::unordered_set<OvmFaH> adj_fhs;
	//	get_adj_faces_around_vertex (mesh, vh, adj_fhs);
	//	std::unordered_set<OvmHaFaH> all_adj_hfhs;
	//	foreach (auto &)
	//}

	std::vector<DualSheet *> one_simple_sheet_inflation (VolumeMesh *mesh, std::unordered_set<OvmFaH> &inflation_quad_set, 
		std::unordered_set<OvmCeH> &shrink_set, std::unordered_set<OvmEgH> &int_ehs, std::hash_map<OvmVeH, OvmVeH> &old_new_vhs_mapping)
	{
		assert (mesh->vertex_property_exists<unsigned long> ("entityptr"));
		auto V_ENTITY_PTR = mesh->request_vertex_property<unsigned long>("entityptr");
		auto V_PREV_HANDLE = mesh->request_vertex_property<OvmVeH> ("prevhandle", mesh->InvalidVertexHandle);
		for (auto v_it = mesh->vertices_begin (); v_it != mesh->vertices_end (); ++v_it)
			V_PREV_HANDLE[*v_it] = *v_it;

		auto E_SHEET_PTR = mesh->request_edge_property<unsigned long> ("sheetptr");
		auto E_CHORD_PTR = mesh->request_edge_property<unsigned long> ("chordptr");

		//���fhs�ϵ����ж���
		std::unordered_set<OvmVeH> all_vhs_on_fhs;
		foreach (auto &fh, inflation_quad_set){
			auto hehs = mesh->face (fh).halfedges ();
			foreach (auto &heh, hehs)
				all_vhs_on_fhs.insert (mesh->halfedge (heh).to_vertex ());
		}

		//��žɵĵ���µĵ�֮���ӳ���ϵ
		//����һ��������ɵĵ�һ��Ϊ�������ڽ��洦�ɵĵ�һ��Ϊ�ģ����Ծɵ���µ��м������һ�Զ�Ķ�Ӧ��ϵ��
		//Ϊ������ɵ㵽�׶�Ӧ���ĸ��µ㣬��Ҫһ�������弯�������жϣ��������弯�Ͼ��Ǿɵ���ϵ�һ��shrink_set
		auto newly_created_vertices_cells_mapping = new std::map<OvmVeH, std::map<OvmVeH, std::unordered_set<OvmCeH> > > ();
		//new_original_vhs_mapping�洢�����ɵĵ��ԭʼ��֮��Ķ�Ӧ��ϵ����һ�ֶ��һ�Ĺ�ϵ
		std::hash_map<OvmVeH, OvmVeH> new_original_vhs_mapping;
		//lambda function to get the corresponding new vertex according to the original vertex and the cell
		auto fGetNewVeHOnCeH = [&] (OvmVeH vh, OvmCeH ch)->OvmVeH{
			auto locate = newly_created_vertices_cells_mapping->find (vh);
			//if not found, return invalid handle
			if (locate == newly_created_vertices_cells_mapping->end ())
				return mesh->InvalidVertexHandle;
			auto &mapping = locate->second;
			foreach (auto &p, mapping) {
				if (contains (p.second, ch))
					return p.first;
				//if (p.second.empty () && ch == mesh->InvalidCellHandle)
				//	return p.first;
			}
			return mesh->InvalidVertexHandle;
		};

		//stores all the cells adjacent to faces in fhs
		std::unordered_set<OvmCeH> all_adj_cells;
		std::unordered_set<OvmVeH> new_vertices;
		foreach (auto &vh, all_vhs_on_fhs) {
			std::vector<std::unordered_set<OvmCeH> > cell_groups;
			std::map<OvmVeH, std::unordered_set<OvmCeH> > newly_vertices_distribution;
			get_cell_groups_around_vertex (mesh, vh, inflation_quad_set, cell_groups);
			//���fhs�������������������ʱ��cell_groupsֻ����һ�����ϣ����ʱ��Ҫ�ٲ���һ���ռ���
			if (cell_groups.size () == 1){
				std::unordered_set<OvmCeH> tmp;
				tmp.insert (mesh->InvalidCellHandle);
				cell_groups.push_back (tmp);
			}else if (cell_groups.size () == 2){
				std::unordered_set<OvmFaH> adj_fhs, adj_fhs_on_inflation_fhs;
				get_adj_faces_around_vertex (mesh, vh, adj_fhs);
				foreach (auto &fh, adj_fhs){
					if (contains (inflation_quad_set, fh))
						adj_fhs_on_inflation_fhs.insert (fh);
				}
				foreach (auto &fh, adj_fhs_on_inflation_fhs){
					auto hfh = mesh->halfface_handle (fh, 0);
					auto ch1 = mesh->incident_cell (hfh);
					hfh = mesh->opposite_halfface_handle (hfh);
					auto ch2 = mesh->incident_cell (hfh);
					if (ch1 != mesh->InvalidCellHandle && ch2 != mesh->InvalidCellHandle)
						continue;
					if (ch1 == mesh->InvalidCellHandle){
						if (contains (cell_groups.front (), ch2)){
							cell_groups[1].insert (ch1);
						}else{
							cell_groups[0].insert (ch1);
						}
					}else{
						if (contains (cell_groups.front (), ch1)){
							cell_groups[1].insert (ch2);
						}else{
							cell_groups[0].insert (ch2);
						}
					}
				}
			}
			foreach (auto &one_chs_group, cell_groups){				
				//����Ҫ�ж���one_chs_group�Ƿ���shrink_set�С�����ڵĻ�����ô����µĵ����²����ģ���������entity_ptrΪ��
				//�������shrink_set�У���ô��������ԭ���ľɵ㣨��Ϊ�����޸ĵ���Ҫ�����½�������
				OvmVeH new_vertex = OvmVeH (-1);
				OvmVec3d reasonable_pos (0, 0, 0);
				reasonable_pos = mesh->vertex (vh);
				new_vertex = mesh->add_vertex (reasonable_pos);
				if (V_ENTITY_PTR[vh] == 0){
					V_ENTITY_PTR[new_vertex] = 0;
				}else{
					if (intersects (one_chs_group, shrink_set)){
						V_ENTITY_PTR[new_vertex] = 0;
						//V_ENTITY_PTR[new_vertex] = V_ENTITY_PTR[vh];
					}else{
						V_ENTITY_PTR[new_vertex] = V_ENTITY_PTR[vh];
					}
				}
				new_original_vhs_mapping.insert (std::make_pair (new_vertex, vh));
				new_vertices.insert (new_vertex);
				newly_vertices_distribution.insert (std::make_pair (new_vertex, one_chs_group));
				foreach (auto &ch, one_chs_group){
					if (ch != mesh->InvalidCellHandle)
						all_adj_cells.insert (ch);
				}
			}
			newly_created_vertices_cells_mapping->insert (std::make_pair(vh, newly_vertices_distribution));
		}//end foreach (auto &vh, all_vhs) {...


		//���ں������garbage_collection��ʱ���ɾ����һЩ�㣬Ȼ��OVM�ڲ������¼��㶥��ľ����ֵ
		//������ҪԤ�ȱ���һ�¾ɵĶ����Ӧ�ļ���λ���Լ�ENTITY_PTR��ֵ
		struct PointInfo{
			PointInfo (){
				pos = OvmVec3d (0, 0, 0);
				entity_ptr = -1;
			}
			OvmVec3d pos;
			unsigned long entity_ptr;
		};

		std::hash_map<OvmVeH, PointInfo> vh_info_for_readd;
		foreach (auto &ch, all_adj_cells){
			for (auto v_it = mesh->cv_iter (ch); v_it; ++v_it){
				auto vh = *v_it;
				if (!contains (all_vhs_on_fhs, vh)){
					PointInfo point_info;
					point_info.pos = mesh->vertex (vh);
					point_info.entity_ptr = V_ENTITY_PTR[vh];
					vh_info_for_readd.insert (std::make_pair (vh, point_info));
				}
			}
		}
		foreach (auto &vh, new_vertices){
			PointInfo point_info;
			point_info.pos = mesh->vertex (vh);
			point_info.entity_ptr = V_ENTITY_PTR[vh];
			vh_info_for_readd.insert (std::make_pair (vh, point_info));
		}

		auto old_mesh_sheet_ptr = new std::map<std::set<OvmVeH>, unsigned long> ();
		auto old_mesh_chord_ptr = new std::map<std::set<OvmVeH>, unsigned long> ();

		//�Ѽ���Щ��Ҫ�ؽ���������İ˸�����
		std::hash_map<OvmCeH, std::vector<OvmVeH> > cells_rebuilding_recipes;
		foreach (auto &ch, all_adj_cells) {
			std::vector<OvmVeH> ch_vhs;
			std::hash_map<OvmVeH, OvmVeH> curr_old_new_vhs_mapping;//�洢��ǰ���������¾ɵ��ӳ���ϵ
			for (auto hv_it = mesh->hv_iter (ch); hv_it; ++hv_it) {
				auto newly_vh = fGetNewVeHOnCeH (*hv_it, ch);
				if (newly_vh == mesh->InvalidVertexHandle){
					ch_vhs.push_back (*hv_it);
					curr_old_new_vhs_mapping[*hv_it] = *hv_it;
				}else{
					ch_vhs.push_back (newly_vh);
					curr_old_new_vhs_mapping[*hv_it] = newly_vh;
				}
			}
			cells_rebuilding_recipes.insert (std::make_pair (ch, ch_vhs));
			std::unordered_set<OvmEgH> ehs_on_ch;
			get_adj_edges_around_cell (mesh, ch, ehs_on_ch);

			foreach (auto &eh, ehs_on_ch){
				auto vh1 = curr_old_new_vhs_mapping[mesh->edge (eh).to_vertex ()],
					vh2 = curr_old_new_vhs_mapping[mesh->edge (eh).from_vertex ()];
				std::set<OvmVeH> vhs_set;
				vhs_set.insert (vh1); vhs_set.insert (vh2);
				(*old_mesh_sheet_ptr)[vhs_set] = E_SHEET_PTR[eh];
				(*old_mesh_chord_ptr)[vhs_set] = E_CHORD_PTR[eh];
			}
		}//end foreach (auto &ch, all_adj_cells) {...

		//�Ѽ���Щ��ͨ���򣨼������ڽ��������ϵ��µ�������İ˸�����
		std::vector<std::vector<OvmVeH> > ord_newly_created_cells_recipes;
		foreach (auto &fh, inflation_quad_set) {
			auto hfh1 = mesh->halfface_handle (fh, 0), hfh2 = mesh->halfface_handle (fh, 1);
			auto ch1 = mesh->incident_cell (hfh1), ch2 = mesh->incident_cell (hfh2);
			std::vector<OvmVeH> ch_vhs;
			for (auto hfv_it = mesh->hfv_iter (hfh1); hfv_it; ++hfv_it){
				auto newly_vh = fGetNewVeHOnCeH (*hfv_it, ch1);
				assert (newly_vh != mesh->InvalidCellHandle);
				ch_vhs.push_back (newly_vh);
			}

			for (auto hfv_it = mesh->hfv_iter (hfh2); hfv_it; ++hfv_it){
				auto newly_vh = fGetNewVeHOnCeH (*hfv_it, ch2);
				assert (newly_vh != mesh->InvalidCellHandle);
				ch_vhs.push_back (newly_vh);
			}
			assert (ch_vhs.size () == 8);
			ord_newly_created_cells_recipes.push_back (ch_vhs);
		}

		//�Ѽ����������µ�������İ˸�����
		std::vector<std::vector<OvmVeH> > int_newly_created_cells_recipes;
		foreach (auto &eh, int_ehs) {
			auto heh = mesh->halfedge_handle (eh, 0);
			std::vector<OvmVeH> ch_vhs_up, ch_vhs_down;
			auto vh_up_origin = mesh->halfedge (heh).from_vertex (),
				vh_down_origin = mesh->halfedge (heh).to_vertex ();
			//use the intersecting halfedges to find the adjacent cell groups,
			//one cell group indicates one newly created vertex
			for (auto hehf_it = mesh->hehf_iter (heh); hehf_it; ++hehf_it){
				auto fh = mesh->face_handle (*hehf_it);
				//if fh is in fhs, one cell group is found
				if (inflation_quad_set.find (fh) != inflation_quad_set.end ()) {
					//get the 1st newly created vertex
					auto test_ch = mesh->incident_cell (*hehf_it);
					auto newly_vh = fGetNewVeHOnCeH (vh_up_origin, test_ch);
					assert (newly_vh != mesh->InvalidVertexHandle);
					ch_vhs_up.push_back (newly_vh);

					//get the 2nd newly created vertex
					newly_vh = fGetNewVeHOnCeH (vh_down_origin, test_ch);
					assert (newly_vh != mesh->InvalidVertexHandle);
					ch_vhs_down.push_back (newly_vh);
				}
			}//end for (auto hehf_it = mesh->hehf_iter (heh); hehf_it; ++hehf_it)...
			assert (ch_vhs_up.size () == 4 && ch_vhs_down.size () == 4);
			qSwap (ch_vhs_up[1], ch_vhs_up[3]);
			ch_vhs_up.insert (ch_vhs_up.end (), ch_vhs_down.begin (), ch_vhs_down.end ());
			int_newly_created_cells_recipes.push_back (ch_vhs_up);
		}


		OpenVolumeMesh::StatusAttrib status_attrib (*mesh);
		//����ɾ���ɵ���Ҫɾ���Ķ��㣬����ɾ����������ڵ�������
		foreach (auto &ch, all_adj_cells)
			status_attrib[ch].set_deleted (true);
	
		status_attrib.garbage_collection (true);


		//old_new_vhs_mapping�洢�ɵĵ㣨����ǰ����Ѻ�ĵ㣬������original�ĵ㣩���µĵ�֮��Ķ�Ӧ��ϵ
		//�����Ƿ��Ѻ�ĵ㣬��������Ӧ��ϵ��һ��һӳ��
		//std::hash_map<OvmVeH, OvmVeH> old_new_vhs_mapping;
		for (auto v_it = mesh->vertices_begin (); v_it != mesh->vertices_end (); ++v_it){
			auto new_vh = *v_it;
			auto ori_vh = V_PREV_HANDLE[new_vh];
			if (ori_vh != mesh->InvalidVertexHandle)
				old_new_vhs_mapping.insert (std::make_pair (ori_vh, new_vh));
		}

		//vhs��֮ǰ����ĵ����У�fUpdateVhs���ڸ�����������У�
		//���vhs�еĵ���old_new_vhs_mapping���ܹ��ҵ���˵��garbage_collection�����в�û�н���ɾ����
		//���OpenVolumeMesh�Զ�������V_PREV_HANDLE�е����ԣ�
		//���vhs�еĵ㲻�ܹ���old_new_vhs_mapping���ҵ���˵����garbage_collection��ɾ���ˣ�
		//�����Ҫ����֮ǰ������vh_info_db�иõ�ļ�������λ�ü�������Ϣ�����ؽ���ͬʱ����V_PREV_HANDLE�õ�洢����Ϣ
		auto fUpdateVhs = [&] (std::vector<OvmVeH> &vhs){
			for (int i = 0; i != vhs.size (); ++i) {
				auto old_vh = vhs[i];
				auto locate = old_new_vhs_mapping.find (old_vh);
				if (locate != old_new_vhs_mapping.end ())
					vhs[i] = locate->second;
				else{
					auto locate = vh_info_for_readd.find (old_vh);
					assert (locate != vh_info_for_readd.end ());
					auto new_vh = mesh->add_vertex (locate->second.pos);
					V_PREV_HANDLE[new_vh] = old_vh;
					V_ENTITY_PTR[new_vh] = locate->second.entity_ptr;
					vhs[i] = new_vh;
					old_new_vhs_mapping.insert (std::make_pair (old_vh, new_vh));
				}
			}
		};

		for (auto it = cells_rebuilding_recipes.begin (); it != cells_rebuilding_recipes.end (); ++it){
			fUpdateVhs (it->second);
			auto ch = mesh->add_cell (it->second);
			assert (ch != mesh->InvalidCellHandle);
		}

		std::unordered_set<OvmCeH> new_chs;

		for (int i = 0; i != ord_newly_created_cells_recipes.size (); ++i){
			auto &vhs = ord_newly_created_cells_recipes[i];
			fUpdateVhs (vhs);
			auto ch = mesh->add_cell (vhs);
			assert (ch != mesh->InvalidCellHandle);
			new_chs.insert (ch);
		}

		for (int i = 0; i != int_newly_created_cells_recipes.size (); ++i){
			auto &vhs = int_newly_created_cells_recipes[i];
			fUpdateVhs (vhs);
			auto ch = mesh->add_cell (vhs);
			assert (ch != mesh->InvalidCellHandle);
			new_chs.insert (ch);
		}

		std::vector<DualSheet *> new_sheets;
		auto fIsNewSheet = [&] (std::unordered_set<OvmCeH> &chs)->bool{
			foreach (auto &ch, chs){
				if (!contains (new_chs, ch)) return false;
			}
			return true;
		};

		while (!new_chs.empty ()){
			DualSheet *new_sheet = new DualSheet (mesh);

			std::unordered_set<OvmEgH> all_ehs_of_first_ch;
			OvmCeH first_ch = *(new_chs.begin ());
			auto hfh_vec = mesh->cell (first_ch).halffaces ();
			foreach (auto &hfh, hfh_vec){
				auto heh_vec = mesh->halfface (hfh).halfedges ();
				foreach (auto &heh, heh_vec)
					all_ehs_of_first_ch.insert (mesh->edge_handle (heh));
			}

			forever{
				assert (!all_ehs_of_first_ch.empty ());
				auto test_eh = *(all_ehs_of_first_ch.begin ());
				std::unordered_set<OvmEgH> sheet_ehs;
				std::unordered_set<OvmCeH> sheet_chs;
				retrieve_one_sheet (mesh, test_eh, sheet_ehs, sheet_chs);
				if (fIsNewSheet (sheet_chs)){
					new_sheet->ehs = sheet_ehs;
					new_sheet->chs = sheet_chs;
					break;
				}else{
					foreach (auto &eh, sheet_ehs)
						all_ehs_of_first_ch.erase (eh);
				}
			}
			foreach (auto &eh, new_sheet->ehs)
				E_SHEET_PTR[eh] = (unsigned long)new_sheet;

			foreach (auto &ch, new_sheet->chs)
				new_chs.erase (ch);
			new_sheets.push_back (new_sheet);
		}

		for (auto e_it = mesh->edges_begin (); e_it != mesh->edges_end (); ++e_it){
			if (E_SHEET_PTR[*e_it]) continue;
			auto vh1 = mesh->edge (*e_it).to_vertex (), 
				vh2 = mesh->edge (*e_it).from_vertex ();
			auto old_vh1 = V_PREV_HANDLE[vh1], old_vh2 = V_PREV_HANDLE[vh2];
			std::set<OvmVeH> vhs_set;
			vhs_set.insert (old_vh1); vhs_set.insert (old_vh2);
			auto locate = old_mesh_sheet_ptr->find (vhs_set);
			assert (locate != old_mesh_sheet_ptr->end ());
			E_SHEET_PTR[*e_it] = locate->second;
		}

		for (auto e_it = mesh->edges_begin (); e_it != mesh->edges_end (); ++e_it){
			if (E_CHORD_PTR[*e_it]) continue;
			auto vh1 = mesh->edge (*e_it).to_vertex (), 
				vh2 = mesh->edge (*e_it).from_vertex ();
			auto old_vh1 = V_PREV_HANDLE[vh1], old_vh2 = V_PREV_HANDLE[vh2];
			std::set<OvmVeH> vhs_set;
			vhs_set.insert (old_vh1); vhs_set.insert (old_vh2);
			auto locate = old_mesh_chord_ptr->find (vhs_set);
			if (locate != old_mesh_chord_ptr->end ())
				E_CHORD_PTR[*e_it] = locate->second;
		}

		//clear
		delete newly_created_vertices_cells_mapping;
		delete old_mesh_chord_ptr;
		delete old_mesh_sheet_ptr;
		return new_sheets;
	}


	std::vector<DualSheet * >  one_simple_sheet_inflation (VolumeMesh *mesh, std::unordered_set<OvmFaH> &fhs, 
		std::unordered_set<OvmCeH> &shrink_set, std::hash_map<OvmVeH, OvmVeH> &old_new_vhs_mapping)
	{
		std::unordered_set<OvmEgH> int_ehs;
		return one_simple_sheet_inflation (mesh, fhs, shrink_set, int_ehs, old_new_vhs_mapping);
	}

	void retrieve_chords (VolumeMesh *mesh, std::unordered_set<OvmFaH> &fhs, ChordSet &chord_set)
	{
		chord_set.clear ();
		if (!mesh->edge_property_exists<unsigned long> ("chordptr")){
			auto prop = mesh->request_edge_property<unsigned long> ("chordptr", 0);
			mesh->set_persistent (prop);
		}
		auto E_CHORD_PTR = mesh->request_edge_property<unsigned long> ("chordptr");
		if (!mesh->edge_property_exists<unsigned long> ("sheetptr")){
			retrieve_sheets (mesh, SheetSet ());
		}
		auto E_SHEET_PTR = mesh->request_edge_property<unsigned long> ("sheetptr");

		std::unordered_set<OvmEgH> ehs_of_fhs;
		foreach (auto &fh, fhs){
			auto heh_vec = mesh->face (fh).halfedges ();
			foreach (auto &heh, heh_vec)
				ehs_of_fhs.insert (mesh->edge_handle (heh));
		}

		//�ɵ�ǰ��seed_eh��fh��һֱ�ݹ���ҶԱߣ�ֱ���ҵ��߽���߻ص�seed_eh������һ���ڲ���·ʱ��
		auto fTraceForward = [&] (OvmEgH seed_eh, OvmFaH fh, 
			std::pair<std::vector<OvmEgH>, std::vector<OvmFaH> > &traced_ehs_fhs)->bool{
				auto oppo_eh = seed_eh;
				oppo_eh = get_opposite_edge_on_face (mesh, fh, oppo_eh);
				traced_ehs_fhs.first.push_back (oppo_eh);
				traced_ehs_fhs.second.push_back (fh);
				while (oppo_eh != seed_eh){
					auto adj_fhs = get_adj_faces_around_edge (mesh, oppo_eh, true);
					assert (adj_fhs.size () == 2 && contains (adj_fhs, fh));
					fh = (fh == adj_fhs.front ())? adj_fhs.back () : adj_fhs.front ();
					if (!contains (fhs, fh)) break;
					oppo_eh = get_opposite_edge_on_face (mesh, fh, oppo_eh);
					traced_ehs_fhs.first.push_back (oppo_eh);
					traced_ehs_fhs.second.push_back (fh);
				}
				if (oppo_eh == seed_eh) return true;
				else return false;
		};

		int chord_count = 0;
		while (!ehs_of_fhs.empty ()){
			OvmEgH seed_eh = *(ehs_of_fhs.begin ());

			//�������ֻ�ȡ��Ȼ����װ������ע��������trace�ıߵ�����˳�����෴��
			auto adj_fhs = get_adj_faces_around_edge (mesh, seed_eh, true);
			assert (adj_fhs.size () == 2);
			std::vector<OvmEgH> chord_ehs;
			std::vector<OvmFaH> chord_fhs;
			chord_ehs.push_back (seed_eh);
			bool is_closed = false;
			if (contains (fhs, adj_fhs.front ())){
				std::pair<std::vector<OvmEgH>, std::vector<OvmFaH> > traced_ehs_fhs;
				is_closed = fTraceForward (seed_eh, adj_fhs.front (), traced_ehs_fhs);
				foreach (auto &eh, traced_ehs_fhs.first) chord_ehs.push_back (eh);
				foreach (auto &fh, traced_ehs_fhs.second) chord_fhs.push_back (fh);
			}
			if (!is_closed && contains (fhs, adj_fhs.back ())){
				std::pair<std::vector<OvmEgH>, std::vector<OvmFaH> > traced_ehs_fhs;
				fTraceForward (seed_eh, adj_fhs.back (), traced_ehs_fhs);
				foreach (auto &eh, traced_ehs_fhs.first) chord_ehs.insert (chord_ehs.begin (), eh);
				foreach (auto &fh, traced_ehs_fhs.second) chord_fhs.insert (chord_fhs.begin (), fh);
			}

			//����������sheet
			DualChord *chord = NULL;
			QString str;
			foreach (auto &eh, chord_ehs){
				if (E_CHORD_PTR[eh] != 0){
					if (chord == NULL)
						chord = (DualChord *)(E_CHORD_PTR[eh]);
					else
						assert (chord == (DualChord *)(E_CHORD_PTR[eh]));
				}
			}
			if (chord == NULL){
				chord = new DualChord (mesh);
				chord->is_closed = is_closed;
			}else{
				auto sheet = (DualSheet *)(E_SHEET_PTR[seed_eh]);
				chord->chord_sheet = sheet;
			}

			chord->ordered_ehs = chord_ehs; chord->ordered_fhs = chord_fhs;

			foreach (auto &eh, chord_ehs){
				E_CHORD_PTR[eh] = (unsigned long)chord;
				ehs_of_fhs.erase (eh);
			}
			chord_set.insert (chord);
		}
	}


	void retrieve_one_sheet (VolumeMesh *mesh, OvmEgH start_eh, OpenVolumeMesh::EdgePropertyT<bool> &E_VISITED,
		std::unordered_set<OvmEgH> &sheet_ehs, std::unordered_set<OvmCeH> &sheet_chs)
	{
		std::queue<OvmHaEgH> spread_set;
		OvmHaEgH start_heh = mesh->halfedge_handle (start_eh, 0);
		sheet_ehs.insert (start_eh);
		E_VISITED[start_eh] = true;
		spread_set.push (start_heh);
		while (!spread_set.empty ()){
			start_heh = spread_set.front ();
			spread_set.pop ();
			for (auto hehf_iter = mesh->hehf_iter (start_heh); hehf_iter; ++hehf_iter){
				OvmHaFaH hfh = *hehf_iter;
				OvmHaEgH oppo_heh = mesh->prev_halfedge_in_halfface (
					mesh->prev_halfedge_in_halfface (start_heh, hfh), hfh);
				OvmEgH eh = mesh->edge_handle (oppo_heh);
				if (eh != mesh->InvalidEdgeHandle)
					sheet_ehs.insert (eh);
				OvmCeH ch = mesh->incident_cell (hfh);
				if (ch != mesh->InvalidCellHandle)
					sheet_chs.insert (mesh->incident_cell (hfh));
				if (!E_VISITED[eh]){
					E_VISITED[eh] = true;
					spread_set.push (oppo_heh);
				}
			}//end for (auto hehf_iter = mesh->hehf_iter (start_heh); hehf_iter; ++hehf_iter){...
		}//end while (!spread_set.empty ()){...
	}

	void retrieve_one_sheet (VolumeMesh *mesh, OvmEgH start_eh, std::unordered_set<OvmEgH> &sheet_ehs, std::unordered_set<OvmCeH> &sheet_chs)
	{
		auto E_VISITED = mesh->request_edge_property<bool> ("visied", false);
		std::queue<OvmHaEgH> spread_set;
		OvmHaEgH start_heh = mesh->halfedge_handle (start_eh, 0);
		sheet_ehs.insert (start_eh);
		E_VISITED[start_eh] = true;
		spread_set.push (start_heh);
		while (!spread_set.empty ()){
			start_heh = spread_set.front ();
			spread_set.pop ();
			for (auto hehf_iter = mesh->hehf_iter (start_heh); hehf_iter; ++hehf_iter){
				OvmHaFaH hfh = *hehf_iter;
				OvmHaEgH oppo_heh = mesh->prev_halfedge_in_halfface (
					mesh->prev_halfedge_in_halfface (start_heh, hfh), hfh);
				OvmEgH eh = mesh->edge_handle (oppo_heh);
				if (eh != mesh->InvalidEdgeHandle)
					sheet_ehs.insert (eh);
				OvmCeH ch = mesh->incident_cell (hfh);
				if (ch != mesh->InvalidCellHandle)
					sheet_chs.insert (mesh->incident_cell (hfh));
				if (!E_VISITED[eh]){
					E_VISITED[eh] = true;
					spread_set.push (oppo_heh);
				}
			}//end for (auto hehf_iter = mesh->hehf_iter (start_heh); hehf_iter; ++hehf_iter){...
		}//end while (!spread_set.empty ()){...
	}

	void retrieve_sheets (VolumeMesh *mesh, SheetSet &sheet_set)
	{
		sheet_set.clear ();
		if (!mesh->edge_property_exists<unsigned long> ("sheetptr")){
			auto prop = mesh->request_edge_property<unsigned long> ("sheetptr", 0);
			mesh->set_persistent (prop);
		}
		auto E_SHEET_PTR = mesh->request_edge_property<unsigned long> ("sheetptr");
		auto E_VISITED = mesh->request_edge_property<bool> ("visied", false);

		int sheet_count = 0;
		std::unordered_set<OvmEgH> all_ehs;
		for (auto e_it = mesh->edges_begin (); e_it != mesh->edges_end (); ++e_it)
			all_ehs.insert (*e_it);

		while (!all_ehs.empty ()){
			auto seed_eh = *(all_ehs.begin ());
			std::unordered_set<OvmEgH> sheet_ehs;
			std::unordered_set<OvmCeH> sheet_chs;
			retrieve_one_sheet (mesh, seed_eh, E_VISITED, sheet_ehs, sheet_chs);
			DualSheet *sheet = NULL;
			foreach (auto &eh, sheet_ehs){
				auto tmp_sheet = (DualSheet *)(E_SHEET_PTR[seed_eh]);
				if (tmp_sheet && !contains (sheet_set, tmp_sheet)){
					sheet = tmp_sheet; break;
				}
			}
			if (sheet == NULL)
				sheet = new DualSheet (mesh);

			sheet->ehs = sheet_ehs; sheet->chs = sheet_chs; 
			foreach (auto &eh, sheet_ehs){
				E_SHEET_PTR[eh] = (unsigned long)sheet;
				all_ehs.erase (eh);
			}

			sheet_set.insert (sheet);
		}
	}

	DualColumn *retrieve_column (VolumeMesh *mesh, OvmFaH seed_fh)
	{
		auto fTraceThrough = [&] (OvmHaFaH start_hfh, OvmCeH ch, std::vector<OvmCeH> &chs, std::vector<OvmFaH> &fhs)->bool{
			chs.push_back (ch);
			OvmFaH start_fh = mesh->face_handle (start_hfh);
			OvmHaFaH oppo_hfh = mesh->opposite_halfface_handle_in_cell (start_hfh, ch);
			OvmFaH fh = mesh->face_handle (oppo_hfh);
			forever{
				if (fh == start_fh) return true;
				fhs.push_back (fh);
				if (mesh->is_boundary (fh)) return false;
				
				oppo_hfh = mesh->opposite_halfface_handle (oppo_hfh);
				ch = mesh->incident_cell (oppo_hfh);
				oppo_hfh = mesh->opposite_halfface_handle_in_cell (oppo_hfh, ch);
				fh = mesh->face_handle (oppo_hfh);
				chs.push_back (ch);
			}
		};

		std::vector<OvmCeH> part_column_chs[2], column_chs;
		std::vector<OvmFaH> part_column_fhs[2], column_fhs;

		bool is_circle = false;
		for (int i = 0; i != 2; ++i){
			OvmHaFaH hfh = mesh->halfface_handle (seed_fh, i);
			OvmCeH adj_ch = mesh->incident_cell (hfh);

			if (adj_ch != mesh->InvalidCellHandle){
				is_circle = fTraceThrough (hfh, adj_ch, part_column_chs[i], part_column_fhs[i]);
				if (is_circle) break;
			}
		}
		column_fhs.push_back (seed_fh);
		foreach (auto &fh, part_column_fhs[0])
			column_fhs.push_back (fh);
			
		foreach (auto &ch, part_column_chs[0])
			column_chs.push_back (ch);

		for (int i = 0; i != part_column_chs[1].size (); ++i){
			column_fhs.insert (column_fhs.begin (), part_column_fhs[1][i]);
			column_chs.insert (column_chs.begin (), part_column_chs[1][i]);
		}
		DualColumn *new_column = new DualColumn (mesh);
		new_column->ordered_fhs = column_fhs; new_column->ordered_chs = column_chs;
		new_column->is_closed = is_circle;
		return new_column;
	}

	std::set<DualColumn *> retrieve_columns (VolumeMesh *mesh, DualSheet *self_int_sheet)
	{
		std::set<DualColumn *> columns;
		return columns;
	}

	std::set<DualColumn *> retrieve_columns (VolumeMesh *mesh, SheetSet &sheet_set)
	{
		std::set<DualColumn *> columns;
		std::unordered_set<OvmFaH> all_fhs_on_sheet;
		std::unordered_set<OvmEgH> all_ehs_on_sheet;

		foreach (auto &sheet, sheet_set){
			foreach (auto &ch, sheet->chs){
				auto hfh_vec = mesh->cell (ch).halffaces ();
				foreach (auto &hfh, hfh_vec){
					auto fh = mesh->face_handle (hfh);
					all_fhs_on_sheet.insert (fh);
				}
			}
			foreach (auto &eh, sheet->ehs)
				all_ehs_on_sheet.insert (eh);
		}

		auto fFhOnInt = [&] (OvmFaH fh)->bool{
			auto heh_vec = mesh->face (fh).halfedges ();
			foreach (auto &heh, heh_vec){
				auto eh = mesh->edge_handle (heh);
				if (!contains (all_ehs_on_sheet, eh))
					return false;
			}
			return true;
		};
		std::unordered_set<OvmFaH> all_int_fhs;
		foreach (auto &fh, all_fhs_on_sheet){
			if (fFhOnInt (fh))
				all_int_fhs.insert (fh);
		}

		while (!all_int_fhs.empty ()){
			auto seed_fh = *(all_int_fhs.begin ());
			auto column = retrieve_column (mesh, seed_fh);
			foreach (auto &fh, column->ordered_fhs)
				all_int_fhs.erase (fh);
			columns.insert (column);
		}
		return columns;
	}

	bool get_sweeping_quad_set (VolumeMesh *mesh, std::unordered_set<OvmEgH> ehs, OvmFaH direction_fh, 
		std::unordered_set<OvmFaH> &sweeping_quad_set)
	{
		auto fInNextLayer = [&] (std::unordered_set<OvmEgH> front_ehs, OvmFaH fh)->std::vector<OvmEgH>{
			std::unordered_set<OvmEgH> adj_ehs;
			std::vector<OvmEgH> captured_front_ehs;
			JC::get_adj_edges_around_face (mesh, fh, adj_ehs);
			foreach (auto eh, adj_ehs){
				if (JC::contains (front_ehs, eh))
					captured_front_ehs.push_back (eh);
			}
			return captured_front_ehs;
		};

		auto cur_front_ehs = ehs;
		while (true){
			sweeping_quad_set.insert (direction_fh);
			std::unordered_set<OvmEgH> adj_ehs;
			JC::get_adj_edges_around_face (mesh, direction_fh, adj_ehs);
			foreach (auto eh, adj_ehs){
				cur_front_ehs.erase (eh);
			}

			while (!cur_front_ehs.empty ()){
				std::unordered_set<OvmFaH> adj_fhs;
				JC::get_adj_faces_around_face_edge (mesh, direction_fh, adj_fhs);
				OvmFaH next_dire_fh = mesh->InvalidFaceHandle;
				foreach (auto adj_fh, adj_fhs){
					if (JC::contains (sweeping_quad_set, adj_fh)) continue;
					auto captured_front_ehs = fInNextLayer (cur_front_ehs, adj_fh);
					if (captured_front_ehs.size () == 1){
						sweeping_quad_set.insert (adj_fh);
						next_dire_fh = adj_fh;
						cur_front_ehs.erase (captured_front_ehs.front ());
						break;
					}else if (captured_front_ehs.size () > 1) {
						return false;
					}
				}
				if (next_dire_fh == mesh->InvalidFaceHandle){
					return false;
				}
				direction_fh = next_dire_fh;
			}//end while (!cur_front_ehs.empty ()){...

			std::unordered_set<OvmEgH> tmp_bnd_ehs;
			JC::collect_boundary_element (mesh, sweeping_quad_set, NULL, &tmp_bnd_ehs);
			foreach (auto eh, tmp_bnd_ehs){
				if (!mesh->is_boundary (eh))
					cur_front_ehs.insert (eh);
			}

			if (cur_front_ehs.empty ()) break;

			auto seed_eh = *(cur_front_ehs.begin ());
			std::unordered_set<OvmFaH> adj_fhs_to_seed_eh;
			JC::get_adj_faces_around_edge (mesh, seed_eh, adj_fhs_to_seed_eh);
			if (adj_fhs_to_seed_eh.size () != 4){
				return false;
			}
			OvmFaH fh_on_this_layer;
			foreach (auto fh, adj_fhs_to_seed_eh){
				if (JC::contains (sweeping_quad_set, fh)){
					fh_on_this_layer = fh;
					adj_fhs_to_seed_eh.erase (fh);
					break;
				}
			}

			std::unordered_set<OvmCeH> adj_chs;
			JC::get_adj_hexas_incident_face (mesh, fh_on_this_layer, adj_chs);
			foreach (auto fh, adj_fhs_to_seed_eh){
				std::unordered_set<OvmCeH> tmp_adj_chs;
				JC::get_adj_hexas_incident_face (mesh, fh, tmp_adj_chs);
				if (!JC::intersects (tmp_adj_chs, adj_chs)){
					direction_fh = fh;
					break;
				}
			}
		}//end while (true){...
		return true;
	}

	bool one_simple_sheet_extraction (VolumeMesh *mesh, DualSheet *sheet, std::unordered_set<OvmFaH> &result_fhs)
	{
		assert (mesh->vertex_property_exists<unsigned long> ("entityptr"));
		auto V_ENTITY_PTR = mesh->request_vertex_property<unsigned long>("entityptr");
		auto V_PREV_HANDLE = mesh->request_vertex_property<OvmVeH> ("prevhandle");
		for (auto v_it = mesh->vertices_begin (); v_it != mesh->vertices_end (); ++v_it)
			V_PREV_HANDLE[*v_it] = *v_it;

		auto E_SHEET_PTR = mesh->request_edge_property<unsigned long> ("sheetptr");
		auto E_CHORD_PTR = mesh->request_edge_property<unsigned long> ("chordptr");


		//�ж�һ����Щ������޷�����ģ����Ե��жϣ���Щ���˵�������޷��жϵģ���
		foreach (auto &eh, sheet->ehs){
			auto vh1 = mesh->edge (eh).from_vertex (),
				vh2 = mesh->edge (eh).to_vertex ();
			auto entity1 = (ENTITY*)(V_ENTITY_PTR[vh1]),
				entity2 = (ENTITY*)(V_ENTITY_PTR[vh2]);
			//����������㶼�ڼ��ε����ǲ��е�
			if (is_VERTEX (entity1) && is_VERTEX (entity2))
				return false;
			////����������㶼�ڼ��α��ϣ��������ڼ��α��ֲ�ͬ��Ҳ�ǲ��е�
			//else if (is_EDGE (entity1) && is_EDGE (entity2) && entity1 != entity2)
			//	return false;
			////����������㶼�ڼ������ϣ��������ڼ������ֲ�ͬ��Ҳ�ǲ��е�
			//else if (is_FACE (entity1) && is_FACE (entity2) && entity1 != entity2)
			//	return false;
		}

		//��ȡsheet�����еĶ���
		std::unordered_set<OvmVeH> vhs_on_sheet;
		foreach (auto &eh, sheet->ehs){
			auto vh1 = mesh->edge (eh).from_vertex (),
				vh2 = mesh->edge (eh).to_vertex ();
			vhs_on_sheet.insert (vh1); vhs_on_sheet.insert (vh2);
		}
		//��ú�sheet�ϵ����������ڵ������壬��Щ�����嶼��Ҫ�����ؽ�������ɾ����Ȼ��������ǵİ˸�����ľ����Ȼ�����ؽ���
		std::unordered_set<OvmCeH> adj_chs_need_rebuilding;
		foreach (auto &vh, vhs_on_sheet){
			std::unordered_set<OvmCeH> adj_chs;
			get_adj_hexas_around_vertex (mesh, vh, adj_chs);
			foreach (auto &ch, adj_chs){
				if (!contains (sheet->chs, ch))
					adj_chs_need_rebuilding.insert (ch);
			}
		}

		//��sheet�����б߽��з��飬ÿ���ڵı߾������ӹ�ϵ
		std::unordered_set<OvmEgH> all_ehs_in_sheet(sheet->ehs);
		auto fTraceThrough = [&] (OvmEgH start_eh, OvmVeH start_vh, std::unordered_set<OvmEgH> &ehs)->bool{
			ehs.insert (start_eh);
			std::unordered_set<OvmEgH> adj_edges;
			get_adj_edges_around_vertex (mesh, start_vh, adj_edges);
			OvmEgH next_eh = mesh->InvalidEdgeHandle;
			foreach (auto &eh, adj_edges){
				if (eh == start_eh) continue;
				if (contains (all_ehs_in_sheet, eh)){
					next_eh = eh; break;
				}
			}
			if (next_eh == mesh->InvalidEdgeHandle)
				return false;

			OvmEgH curr_eh = next_eh;
			OvmVeH curr_vh = get_other_vertex_on_edge (mesh, curr_eh, start_vh);
			while (curr_eh != mesh->InvalidEdgeHandle){
				ehs.insert (curr_eh);
				adj_edges.clear ();
				get_adj_edges_around_vertex (mesh, curr_vh, adj_edges);

				next_eh = mesh->InvalidEdgeHandle;
				foreach (auto &eh, adj_edges){
					if (eh == curr_eh) continue;
					if (contains (all_ehs_in_sheet, eh)){
						next_eh = eh; break;
					}
				}
				curr_eh = next_eh;
				curr_vh = get_other_vertex_on_edge (mesh, curr_eh, curr_vh);
				if (curr_eh == start_eh) return true;
			}//end while (curr_eh != mesh->InvalidEdgeHandle){...

			return false;
		};

		std::set<std::unordered_set<OvmEgH>> connected_edge_sets;
		while (!all_ehs_in_sheet.empty()){
			auto eh = *(all_ehs_in_sheet.begin());
			std::unordered_set<OvmEgH> one_edge_set;
			OvmVeH vh1 = mesh->edge (eh).to_vertex (), vh2 = mesh->edge (eh).from_vertex ();
			if (!fTraceThrough (eh, vh1, one_edge_set)){
				fTraceThrough (eh, vh2, one_edge_set);
			}

			connected_edge_sets.insert(one_edge_set);
			foreach (auto &eh, one_edge_set)
				all_ehs_in_sheet.erase (eh);
		}

		struct PointInfo{
			PointInfo (){
				pos = OvmVec3d (0, 0, 0);
				entity_ptr = -1;
			}
			OvmVec3d pos;
			unsigned long entity_ptr;
		};

		//���ں����ɾ���������ܻὫһЩ����ĵ�һ��ɾ����������Ҫ�Ƚ����п��ܱ�ɾ���ĵ����Ϣ��������

		std::map<OvmVeH, PointInfo> vhs_info_for_readd;
		std::unordered_set<OvmVeH> new_vhs;
		std::hash_map<OvmVeH, OvmVeH> old_new_vhs_mapping;
		foreach(auto &one_edge_set,connected_edge_sets){
			std::unordered_set<OvmVeH> vhs;
			foreach (auto &eh, one_edge_set){
				auto vh1 = mesh->edge (eh).from_vertex (),
					vh2 = mesh->edge (eh).to_vertex ();
				vhs.insert (vh1); vhs.insert (vh2);
			}
			//�ںϲ���ʱ����Ҫ������Щ�ϲ��ĵ������
			//���磬��һ��λ�ڶ����ϵĵ��һ�����ڶ����ϵĵ�Ҫ�ϲ�ʱ������Ҫ���µĵ�Ҳλ�ڶ�����
			OvmVeH ref_vh = mesh->InvalidVertexHandle;
			foreach (auto &vh, vhs){
				ENTITY *entity = (ENTITY*)(V_ENTITY_PTR[vh]);
				if (is_VERTEX (entity)){
					ref_vh = vh; break;
				}
			}
			if (ref_vh == mesh->InvalidVertexHandle){
				foreach (auto &vh, vhs){
					ENTITY *entity = (ENTITY*)(V_ENTITY_PTR[vh]);
					if (is_EDGE (entity)){
						ref_vh = vh; break;
					}
				}
			}

			if (ref_vh == mesh->InvalidVertexHandle){
				foreach (auto &vh, vhs){
					ENTITY *entity = (ENTITY*)(V_ENTITY_PTR[vh]);
					if (is_FACE (entity)){
						ref_vh = vh; break;
					}
				}
			}

			PointInfo new_pt_info;

			OvmVec3d new_pos (0, 0, 0);

			if (ref_vh != mesh->InvalidVertexHandle){
				new_pt_info.entity_ptr = V_ENTITY_PTR[ref_vh];
				new_pos = mesh->vertex (ref_vh);
			}else{
				new_pt_info.entity_ptr = 0;
				foreach (auto &vh, vhs)
					new_pos += mesh->vertex (vh);
				new_pos /= vhs.size ();
			}
			
			new_pt_info.pos = new_pos;
			auto new_vh = mesh->add_vertex (new_pt_info.pos);
			vhs_info_for_readd.insert (std::make_pair (new_vh, new_pt_info));
			new_vhs.insert (new_vh);

			foreach (auto &vh, vhs)
				old_new_vhs_mapping.insert (std::make_pair (vh, new_vh));
		}

		//���adj_chs_need_rebuilding�ϵ�������İ˸�����
		std::map<std::set<OvmVeH>, unsigned long> old_mesh_sheet_ptr, old_mesh_chord_ptr;
		std::hash_map<OvmCeH, std::vector<OvmVeH> > adj_chs_rebuild_recipe;
		std::unordered_set<OvmVeH> vhs_not_replaced;
		foreach (auto &ch, adj_chs_need_rebuilding){
			std::vector<OvmVeH> one_chs_recipe;
			std::hash_map<OvmVeH, OvmVeH> curr_old_new_vhs_mapping;//�洢��ǰ���������¾ɵ��ӳ���ϵ
			for (auto hv_it = mesh->hv_iter (ch); hv_it; ++hv_it){
				auto vh = *hv_it;
				auto locate = old_new_vhs_mapping.find (vh);
				if (locate != old_new_vhs_mapping.end ()){
					one_chs_recipe.push_back (locate->second);
					curr_old_new_vhs_mapping[vh] = locate->second;
				}else{
					one_chs_recipe.push_back (vh);
					vhs_not_replaced.insert (vh);
					curr_old_new_vhs_mapping[vh] = vh;
				}
			}

			adj_chs_rebuild_recipe.insert (std::make_pair (ch, one_chs_recipe));

			std::unordered_set<OvmEgH> ehs_on_ch;
			get_adj_edges_around_cell (mesh, ch, ehs_on_ch);

			foreach (auto &eh, ehs_on_ch){
				auto vh1 = curr_old_new_vhs_mapping[mesh->edge (eh).to_vertex ()],
					vh2 = curr_old_new_vhs_mapping[mesh->edge (eh).from_vertex ()];
				std::set<OvmVeH> vhs_set;
				vhs_set.insert (vh1); vhs_set.insert (vh2);
				old_mesh_sheet_ptr[vhs_set] = E_SHEET_PTR[eh];
				old_mesh_chord_ptr[vhs_set] = E_CHORD_PTR[eh];
			}

		}

		//��adj_chs_rebuild_recipe����Щ��������Ҳ������Ϣ����vhs_info_for_readd�У���������˵�������ڱ�ɾ������������
		foreach (auto &vh, vhs_not_replaced){
			PointInfo new_pt_info;
			new_pt_info.entity_ptr = V_ENTITY_PTR[vh];
			new_pt_info.pos = mesh->vertex (vh);
			vhs_info_for_readd.insert (std::make_pair (vh, new_pt_info));
		}

		OpenVolumeMesh::StatusAttrib status_attrib (*mesh);
		//����ɾ��sheet�е�������
		foreach (auto &ch, sheet->chs)
			status_attrib[ch].set_deleted (true);
		//Ȼ��ɾ����sheet�е����������ڵ�������
		foreach (auto &ch, adj_chs_need_rebuilding)
			status_attrib[ch].set_deleted (true);

		status_attrib.garbage_collection (true);
		
		std::hash_map<OvmVeH, OvmVeH> old_new_vhs_mapping2;
		for (auto v_it = mesh->vertices_begin (); v_it != mesh->vertices_end (); ++v_it)
			old_new_vhs_mapping2.insert (std::make_pair (V_PREV_HANDLE[*v_it], *v_it));


		//����adj_chs_rebuild_recipe�еľɵĶ�����Ϊ�µĶ�����
		foreach (auto &p, adj_chs_rebuild_recipe){
			std::vector<OvmVeH> one_recipe = p.second;
			for (int i = 0; i != one_recipe.size (); ++i){
				auto locate = old_new_vhs_mapping2.find (one_recipe[i]);
				if (locate != old_new_vhs_mapping2.end ())
					one_recipe[i] = locate->second;
				else{
					auto locate2 = vhs_info_for_readd.find (one_recipe[i]);
					assert (locate2 != vhs_info_for_readd.end ());
					PointInfo pt_info = locate2->second;
					auto new_vh = mesh->add_vertex (pt_info.pos);
					V_PREV_HANDLE[new_vh] = one_recipe[i];
					V_ENTITY_PTR[new_vh] = pt_info.entity_ptr;
					old_new_vhs_mapping2.insert (std::make_pair (one_recipe[i], new_vh));
					one_recipe[i] = new_vh;
				}
			}
			auto rebuilt_ch = mesh->add_cell (one_recipe);
			assert (rebuilt_ch != mesh->InvalidCellHandle);
		}

		for (auto e_it = mesh->edges_begin (); e_it != mesh->edges_end (); ++e_it){
			if (E_SHEET_PTR[*e_it]) continue;
			auto vh1 = mesh->edge (*e_it).to_vertex (), 
				vh2 = mesh->edge (*e_it).from_vertex ();
			auto old_vh1 = V_PREV_HANDLE[vh1], old_vh2 = V_PREV_HANDLE[vh2];
			std::set<OvmVeH> vhs_set;
			vhs_set.insert (old_vh1); vhs_set.insert (old_vh2);
			auto locate = old_mesh_sheet_ptr.find (vhs_set);
			assert (locate != old_mesh_sheet_ptr.end ());
			E_SHEET_PTR[*e_it] = locate->second;
		}

		for (auto e_it = mesh->edges_begin (); e_it != mesh->edges_end (); ++e_it){
			if (E_CHORD_PTR[*e_it]) continue;
			auto vh1 = mesh->edge (*e_it).to_vertex (), 
				vh2 = mesh->edge (*e_it).from_vertex ();
			auto old_vh1 = V_PREV_HANDLE[vh1], old_vh2 = V_PREV_HANDLE[vh2];
			std::set<OvmVeH> vhs_set;
			vhs_set.insert (old_vh1); vhs_set.insert (old_vh2);
			auto locate = old_mesh_chord_ptr.find (vhs_set);
			if (locate != old_mesh_chord_ptr.end ())
				E_CHORD_PTR[*e_it] = locate->second;
		}


		//�����Ѽ���sheet��ȡ�����ɵ��漯����Щ����ĸ��˵㶼��new_vhs�еĵ�
		std::unordered_set<OvmVeH> updated_new_vhs;
		foreach (auto &vh, new_vhs)
			updated_new_vhs.insert (old_new_vhs_mapping2[vh]);

		foreach (auto &vh, updated_new_vhs){
			std::unordered_set<OvmFaH> adj_fhs;
			get_adj_faces_around_vertex (mesh, vh, adj_fhs);
			foreach (auto &fh, adj_fhs){
				auto adj_vhs = get_adj_vertices_around_face (mesh, fh);
				bool is_ok = true;
				foreach (auto &adj_vh, adj_vhs){
					if (!contains (updated_new_vhs, adj_vh)){
						is_ok = false; break;
					}
				}
				if (is_ok)
					result_fhs.insert (fh);
			}
		}

		return true;
	}

	void column_collapse (VolumeMesh *mesh, DualColumn *column, std::pair<OvmVeH, OvmVeH> &collapse_vhs_pair, std::hash_map<OvmVeH, OvmVeH> &old_new_vhs_mapping)
	{
		assert (mesh->vertex_property_exists<unsigned long> ("entityptr"));
		auto V_ENTITY_PTR = mesh->request_vertex_property<unsigned long>("entityptr");

		auto V_PREV_HANDLE = mesh->request_vertex_property<OvmVeH> ("prevhandle", mesh->InvalidVertexHandle);
		for (auto v_it = mesh->vertices_begin (); v_it != mesh->vertices_end (); ++v_it)
			V_PREV_HANDLE[*v_it] = *v_it;

		//���Ȼ�����еĺϲ����
		std::vector<std::pair<OvmVeH, OvmVeH> > all_collapse_vhs_pairs;
		auto fhs_array = column->ordered_fhs;
		auto adj_vhs = get_adj_vertices_around_face (mesh, fhs_array.front ());
		if (!contains (adj_vhs, collapse_vhs_pair.first)){
			std::reverse (fhs_array.begin (), fhs_array.end ());
			adj_vhs = get_adj_vertices_around_face (mesh, fhs_array.front ());
			assert (contains (adj_vhs, collapse_vhs_pair.first));
		}

		all_collapse_vhs_pairs.push_back (collapse_vhs_pair);
		auto prev_vhs_pair = collapse_vhs_pair;
		for (int i = 1; i != fhs_array.size (); ++i){
			adj_vhs = get_adj_vertices_around_face (mesh, fhs_array[i]);
			std::pair<OvmVeH, OvmVeH> next_vhs_pair;
			next_vhs_pair.first = next_vhs_pair.second = mesh->InvalidVertexHandle;
			foreach (auto &vh, adj_vhs){
				auto heh = mesh->halfedge (prev_vhs_pair.first, vh);
				if (heh != mesh->InvalidHalfEdgeHandle){
					assert (next_vhs_pair.first == mesh->InvalidVertexHandle);
					next_vhs_pair.first = vh;
					continue;
				}
				heh = mesh->halfedge (prev_vhs_pair.second, vh);
				if (heh != mesh->InvalidHalfEdgeHandle){
					assert (next_vhs_pair.second == mesh->InvalidVertexHandle);
					next_vhs_pair.second = vh;
				}
			}//end foreach (auto &vh, adj_vhs){...
			assert (next_vhs_pair.first != mesh->InvalidVertexHandle &&
				next_vhs_pair.second != mesh->InvalidVertexHandle);

			assert (V_ENTITY_PTR[next_vhs_pair.first] == V_ENTITY_PTR[next_vhs_pair.second]);

			all_collapse_vhs_pairs.push_back (next_vhs_pair);
			prev_vhs_pair = next_vhs_pair;
		}//end for (int i = 1; i != fhs_array.size (); ++i){...

		//������к�first�����second�������ڵ������壬�Ա��ں�������ؽ�
		std::unordered_set<OvmCeH> all_adj_cells;
		std::hash_map<OvmVeH, OvmVeH> collapse_vhs_new_vh_mapping;

		foreach (auto &p, all_collapse_vhs_pairs){
			std::unordered_set<OvmCeH> adj_chs;
			get_adj_hexas_around_vertex (mesh, p.first, adj_chs);
			foreach (auto &ch, adj_chs){
				if (!contains (column->ordered_chs, ch)) all_adj_cells.insert (ch);
			}
			adj_chs.clear ();
			get_adj_hexas_around_vertex (mesh, p.second, adj_chs);
			foreach (auto &ch, adj_chs){
				if (!contains (column->ordered_chs, ch)) all_adj_cells.insert (ch);
			}

			auto new_vh = mesh->add_vertex ((mesh->vertex (p.first) + mesh->vertex (p.second)) / 2);
			V_ENTITY_PTR[new_vh] = V_ENTITY_PTR[p.first];

			collapse_vhs_new_vh_mapping.insert (std::make_pair (p.first, new_vh));
			collapse_vhs_new_vh_mapping.insert (std::make_pair (p.second, new_vh));
		}

		std::vector<std::vector<OvmVeH> > chs_rebuild_recipe;
		std::unordered_set<OvmVeH> all_vhs_in_recipe;
		foreach (auto &ch, all_adj_cells){
			std::vector<OvmVeH> one_recipe;
			for (auto hv_it = mesh->hv_iter (ch); hv_it; ++hv_it){
				auto vh = *hv_it;
				auto locate = collapse_vhs_new_vh_mapping.find (vh);
				if (locate != collapse_vhs_new_vh_mapping.end ())
					vh = locate->second;
				one_recipe.push_back (vh);
				all_vhs_in_recipe.insert (vh);
			}
			chs_rebuild_recipe.push_back (one_recipe);
		}

		std::hash_map<OvmVeH, std::pair<OvmVec3d, unsigned long> > vh_info_for_readd;
		foreach (auto &vh, all_vhs_in_recipe){
			vh_info_for_readd.insert (std::make_pair (vh, std::make_pair (mesh->vertex (vh), V_ENTITY_PTR[vh])));
		}

		OpenVolumeMesh::StatusAttrib status_attrib (*mesh);
		//����ɾ���ɵ���Ҫɾ���Ķ��㣬����ɾ����������ڵ�������
		foreach (auto &ch, column->ordered_chs)
			status_attrib[ch].set_deleted (true);

		foreach (auto &ch, all_adj_cells)
			status_attrib[ch].set_deleted (true);

		status_attrib.garbage_collection (true);

		//std::hash_map<OvmVeH, OvmVeH> old_new_vhs_mapping;
		for (auto v_it = mesh->vertices_begin (); v_it != mesh->vertices_end (); ++v_it){
			auto new_vh = *v_it;
			auto ori_vh = V_PREV_HANDLE[new_vh];
			if (ori_vh != mesh->InvalidVertexHandle)
				old_new_vhs_mapping.insert (std::make_pair (ori_vh, new_vh));
		}

		//vhs��֮ǰ����ĵ����У�fUpdateVhs���ڸ�����������У�
		//���vhs�еĵ���old_new_vhs_mapping���ܹ��ҵ���˵��garbage_collection�����в�û�н���ɾ����
		//���OpenVolumeMesh�Զ�������V_PREV_HANDLE�е����ԣ�
		//���vhs�еĵ㲻�ܹ���old_new_vhs_mapping���ҵ���˵����garbage_collection��ɾ���ˣ�
		//�����Ҫ����֮ǰ������vh_info_db�иõ�ļ�������λ�ü�������Ϣ�����ؽ���ͬʱ����V_PREV_HANDLE�õ�洢����Ϣ
		auto fUpdateVhs = [&] (std::vector<OvmVeH> &vhs){
			for (int i = 0; i != vhs.size (); ++i) {
				auto old_vh = vhs[i];
				auto locate = old_new_vhs_mapping.find (old_vh);
				if (locate != old_new_vhs_mapping.end ())
					vhs[i] = locate->second;
				else{
					auto locate = vh_info_for_readd.find (old_vh);
					assert (locate != vh_info_for_readd.end ());
					auto pos = locate->second.first;
					auto new_vh = mesh->add_vertex (pos);
					V_ENTITY_PTR[new_vh] = locate->second.second;
					vhs[i] = new_vh;
					old_new_vhs_mapping.insert (std::make_pair (old_vh, new_vh));
				}
			}
		};

		foreach (auto one_recipe, chs_rebuild_recipe){
			fUpdateVhs (one_recipe);
			mesh->add_cell (one_recipe);
		}
	}

	void columns_collapse (VolumeMesh *mesh, std::map<DualColumn *, std::pair<OvmVeH, OvmVeH> > &collapse_vhs_pairs,
		std::hash_map<OvmVeH, OvmVeH> &old_new_vhs_mapping)
	{
		assert (mesh->vertex_property_exists<unsigned long> ("entityptr"));
		auto V_ENTITY_PTR = mesh->request_vertex_property<unsigned long>("entityptr");

		auto V_PREV_HANDLE = mesh->request_vertex_property<OvmVeH> ("prevhandle", mesh->InvalidVertexHandle);
		for (auto v_it = mesh->vertices_begin (); v_it != mesh->vertices_end (); ++v_it)
			V_PREV_HANDLE[*v_it] = *v_it;

		//���Ȼ�����еĺϲ����
		std::vector<std::pair<OvmVeH, OvmVeH> > all_collapse_vhs_pairs;
		foreach (auto &p, collapse_vhs_pairs){
			auto fhs_array = p.first->ordered_fhs;
			auto adj_vhs = get_adj_vertices_around_face (mesh, fhs_array.front ());
			if (!contains (adj_vhs, p.second.first)){
				std::reverse (fhs_array.begin (), fhs_array.end ());
				adj_vhs = get_adj_vertices_around_face (mesh, fhs_array.front ());
				assert (contains (adj_vhs, p.second.first));
			}

			all_collapse_vhs_pairs.push_back (p.second);
			auto prev_vhs_pair = p.second;
			for (int i = 1; i != fhs_array.size (); ++i){
				adj_vhs = get_adj_vertices_around_face (mesh, fhs_array[i]);
				std::pair<OvmVeH, OvmVeH> next_vhs_pair;
				next_vhs_pair.first = next_vhs_pair.second = mesh->InvalidVertexHandle;
				foreach (auto &vh, adj_vhs){
					auto heh = mesh->halfedge (prev_vhs_pair.first, vh);
					if (heh != mesh->InvalidHalfEdgeHandle){
						assert (next_vhs_pair.first == mesh->InvalidVertexHandle);
						next_vhs_pair.first = vh;
						continue;
					}
					heh = mesh->halfedge (prev_vhs_pair.second, vh);
					if (heh != mesh->InvalidHalfEdgeHandle){
						assert (next_vhs_pair.second == mesh->InvalidVertexHandle);
						next_vhs_pair.second = vh;
					}
				}//end foreach (auto &vh, adj_vhs){...
				assert (next_vhs_pair.first != mesh->InvalidVertexHandle &&
					next_vhs_pair.second != mesh->InvalidVertexHandle);

				//assert (V_ENTITY_PTR[next_vhs_pair.first] == V_ENTITY_PTR[next_vhs_pair.second]);

				all_collapse_vhs_pairs.push_back (next_vhs_pair);
				prev_vhs_pair = next_vhs_pair;
			}//end for (int i = 1; i != fhs_array.size (); ++i){...
		}
		
		//������к�first�����second�������ڵ������壬�Ա��ں�������ؽ�
		std::unordered_set<OvmCeH> all_adj_cells;
		std::hash_map<OvmVeH, OvmVeH> collapse_vhs_new_vh_mapping;

		//���������columns�ϵ�������
		std::unordered_set<OvmCeH> all_chs_on_columns;
		foreach (auto &p, collapse_vhs_pairs){
			foreach (auto &ch, p.first->ordered_chs)
				all_chs_on_columns.insert (ch);
		}

		foreach (auto &p, all_collapse_vhs_pairs){
			std::unordered_set<OvmCeH> adj_chs;
			get_adj_hexas_around_vertex (mesh, p.first, adj_chs);
			foreach (auto &ch, adj_chs){
				if (!contains (all_chs_on_columns, ch)) all_adj_cells.insert (ch);
			}
			adj_chs.clear ();
			get_adj_hexas_around_vertex (mesh, p.second, adj_chs);
			foreach (auto &ch, adj_chs){
				if (!contains (all_chs_on_columns, ch)) all_adj_cells.insert (ch);
			}

			auto new_vh = mesh->add_vertex ((mesh->vertex (p.first) + mesh->vertex (p.second)) / 2);
			V_ENTITY_PTR[new_vh] = V_ENTITY_PTR[p.first];

			collapse_vhs_new_vh_mapping.insert (std::make_pair (p.first, new_vh));
			collapse_vhs_new_vh_mapping.insert (std::make_pair (p.second, new_vh));
		}

		std::vector<std::vector<OvmVeH> > chs_rebuild_recipe;
		std::unordered_set<OvmVeH> all_vhs_in_recipe;
		foreach (auto &ch, all_adj_cells){
			std::vector<OvmVeH> one_recipe;
			for (auto hv_it = mesh->hv_iter (ch); hv_it; ++hv_it){
				auto vh = *hv_it;
				auto locate = collapse_vhs_new_vh_mapping.find (vh);
				if (locate != collapse_vhs_new_vh_mapping.end ())
					vh = locate->second;
				one_recipe.push_back (vh);
				all_vhs_in_recipe.insert (vh);
			}
			chs_rebuild_recipe.push_back (one_recipe);
		}

		std::hash_map<OvmVeH, std::pair<OvmVec3d, unsigned long> > vh_info_for_readd;
		foreach (auto &vh, all_vhs_in_recipe){
			vh_info_for_readd.insert (std::make_pair (vh, std::make_pair (mesh->vertex (vh), V_ENTITY_PTR[vh])));
		}

		OpenVolumeMesh::StatusAttrib status_attrib (*mesh);
		//����ɾ���ɵ���Ҫɾ���Ķ��㣬����ɾ����������ڵ�������
		foreach (auto &p, collapse_vhs_pairs){
			auto column = p.first;
			foreach (auto &ch, column->ordered_chs)
				status_attrib[ch].set_deleted (true);
		}

		foreach (auto &ch, all_adj_cells)
			status_attrib[ch].set_deleted (true);

		status_attrib.garbage_collection (true);

		//std::hash_map<OvmVeH, OvmVeH> old_new_vhs_mapping;
		for (auto v_it = mesh->vertices_begin (); v_it != mesh->vertices_end (); ++v_it){
			auto new_vh = *v_it;
			auto ori_vh = V_PREV_HANDLE[new_vh];
			if (ori_vh != mesh->InvalidVertexHandle)
				old_new_vhs_mapping.insert (std::make_pair (ori_vh, new_vh));
		}

		//vhs��֮ǰ����ĵ����У�fUpdateVhs���ڸ�����������У�
		//���vhs�еĵ���old_new_vhs_mapping���ܹ��ҵ���˵��garbage_collection�����в�û�н���ɾ����
		//���OpenVolumeMesh�Զ�������V_PREV_HANDLE�е����ԣ�
		//���vhs�еĵ㲻�ܹ���old_new_vhs_mapping���ҵ���˵����garbage_collection��ɾ���ˣ�
		//�����Ҫ����֮ǰ������vh_info_db�иõ�ļ�������λ�ü�������Ϣ�����ؽ���ͬʱ����V_PREV_HANDLE�õ�洢����Ϣ
		auto fUpdateVhs = [&] (std::vector<OvmVeH> &vhs){
			for (int i = 0; i != vhs.size (); ++i) {
				auto old_vh = vhs[i];
				auto locate = old_new_vhs_mapping.find (old_vh);
				if (locate != old_new_vhs_mapping.end ())
					vhs[i] = locate->second;
				else{
					auto locate = vh_info_for_readd.find (old_vh);
					assert (locate != vh_info_for_readd.end ());
					auto pos = locate->second.first;
					auto new_vh = mesh->add_vertex (pos);
					V_ENTITY_PTR[new_vh] = locate->second.second;
					vhs[i] = new_vh;
					old_new_vhs_mapping.insert (std::make_pair (old_vh, new_vh));
				}
			}
		};

		foreach (auto one_recipe, chs_rebuild_recipe){
			fUpdateVhs (one_recipe);
			mesh->add_cell (one_recipe);
		}
	}
}


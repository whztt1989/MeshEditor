#include "stdafx.h"
#include "MeshMatchingHandler.h"
#include <limits>
#include "polysim.h"

namespace MM{
	void adjust_interface_bondary (VolumeMesh *mesh, std::unordered_set<OvmFaH> &inter_fhs, std::set<FACE *> interfaces)
	{
		std::unordered_set<OvmVeH> bound_vhs;
		std::unordered_set<OvmEgH> bound_ehs;

		JC::get_boundary_elements_of_faces_patch (mesh, inter_fhs, bound_vhs, bound_ehs);

		//首先调整点，把一些网格点移到集合点上
		std::set<ENTITY*> vertices_list, edges_list_used;
		foreach (auto f, interfaces){
			ENTITY_LIST tmp_edges_list;
			api_get_edges (f, tmp_edges_list);
			for (int i = 0; i != tmp_edges_list.count (); ++i){
				if (edges_list_used.find (tmp_edges_list[i]) == edges_list_used.end ())
					edges_list_used.insert (tmp_edges_list[i]);
				else
					edges_list_used.erase (tmp_edges_list[i]);
			}
		}

		foreach (auto e, edges_list_used){
			ENTITY_LIST tmp_vertices_list;
			api_get_vertices (e, tmp_vertices_list);
			for (int i = 0; i != tmp_vertices_list.count (); ++i)
				vertices_list.insert (tmp_vertices_list[i]);
		}
		auto V_ENT_PTR = mesh->request_vertex_property<unsigned long> ("entityptr");

		std::unordered_set<OvmVeH> vhs_on_geom_vertices;
		foreach (auto entity_ptr, vertices_list){
			VERTEX *v = (VERTEX*)(entity_ptr);
			auto coords = v->geometry ()->coords ();
			auto pt = SPA2POS(coords);
			double min_dist = std::numeric_limits<double>::max ();
			OvmVeH closest_vh = mesh->InvalidVertexHandle;
			foreach (auto &vh, bound_vhs){
				double dist = (pt - mesh->vertex (vh)).length ();
				if (dist < min_dist){
					min_dist = dist; closest_vh = vh;
				}
			}
			mesh->set_vertex (closest_vh, pt);
			bound_vhs.erase (closest_vh);
			V_ENT_PTR[closest_vh] = (unsigned long)v;
			vhs_on_geom_vertices.insert (closest_vh);
		}

		auto fGetGeoEgBtw2Vs = [&] (OvmVeH vh1, OvmVeH vh2)->std::vector<ENTITY*>{
			VERTEX *v1, *v2;
			v1 = (VERTEX *)(V_ENT_PTR[vh1]);
			v2 = (VERTEX *)(V_ENT_PTR[vh2]);
			ENTITY_LIST el1, el2;
			std::vector<ENTITY*> shared_geom_egs;
			api_get_edges (v1, el1); api_get_edges (v2, el2);
			for (int i = 0; i != el1.count (); ++i){
				for (int j = 0; j != el2.count (); ++j){
					if (el1[i] == el2[j]) shared_geom_egs.push_back (el1[i]);
				}
			}
			return shared_geom_egs;
		};

		//std::vector<std::unordered_set<OvmVeH> > bound_vhs_groups;
		while (!bound_ehs.empty ()){
			OvmEgH seed_eh; OvmVeH seed_vh;
			foreach (auto eh, bound_ehs){
				OvmVeH vh1 = mesh->edge (eh).from_vertex (), vh2 = mesh->edge (eh).to_vertex ();
				if (JC::contains (vhs_on_geom_vertices, vh1)){
					seed_eh = eh; seed_vh = vh1;
					break;
				}
				if (JC::contains (vhs_on_geom_vertices, vh2)){
					seed_eh = eh; seed_vh = vh2;
					break;
				}
			}
			OvmVeH next_vh = mesh->edge (seed_eh).from_vertex () == seed_vh?
				mesh->edge (seed_eh).to_vertex () : mesh->edge (seed_eh).from_vertex ();

			std::unordered_set<OvmVeH> one_vhs_group;
			one_vhs_group.insert (seed_vh); one_vhs_group.insert (next_vh);
			bound_ehs.erase (seed_eh);

			while (!JC::contains (vhs_on_geom_vertices, next_vh)){
				std::unordered_set<OvmEgH> adj_bnd_ehs;
				JC::get_adj_boundary_edges_around_vertex (mesh, next_vh, adj_bnd_ehs);
				OvmEgH next_eh;
				foreach (auto eh, adj_bnd_ehs){
					if (!JC::contains (bound_ehs, eh)) continue;
					OvmVeH vh1 = mesh->edge (eh).from_vertex (), vh2 = mesh->edge (eh).to_vertex ();
					if (vh1 == next_vh){
						next_vh = vh2; 
						next_eh = eh;
						break;
					}else if (vh2 == next_vh){
						next_vh = vh1; 
						next_eh = eh;
						break;
					}
				}
				one_vhs_group.insert (next_vh);
				bound_ehs.erase (next_eh);
			}
			
			auto shared_egs = fGetGeoEgBtw2Vs (seed_vh, next_vh);
			ENTITY *on_which_edge = NULL;
			if (shared_egs.size () == 1){
				on_which_edge = shared_egs.front ();
			}else{
				double closest_avg_dist = std::numeric_limits<double>::max ();
				ENTITY *closest_edge = NULL;
				foreach (auto eg, shared_egs){
					double avg_dist = 0.0f, dist = 0.0f;
					SPAposition pos;
					foreach (auto vh, one_vhs_group){
						auto pt = mesh->vertex (vh);
						api_entity_point_distance (eg, POS2SPA(pt), pos, dist);
						avg_dist += dist;
					}
					if (avg_dist < closest_avg_dist){
						closest_avg_dist = avg_dist;
						closest_edge = eg;
					}
				}
				on_which_edge = closest_edge;
			}
			foreach (auto vh, one_vhs_group){
				double dist;
				SPAposition pos;
				if (!JC::contains (vhs_on_geom_vertices, vh)){
					V_ENT_PTR[vh] = (unsigned long) (on_which_edge);
					auto pt = mesh->vertex (vh);
					api_entity_point_distance (on_which_edge, POS2SPA(pt), pos, dist);
					mesh->set_vertex (vh, SPA2POS(pos));
				}
			}
		}//end while (!bound_ehs.empty ()){...
	}

	void get_hexas_within_depth (VolumeMesh *mesh, int depth_num, std::unordered_set<OvmFaH> &inter_fhs,
		std::unordered_set<OvmCeH> &hexas)
	{
		std::unordered_set<OvmVeH> front_vhs, all_vhs;
		foreach (auto &fh, inter_fhs){
			auto adj_vhs = JC::get_adj_vertices_around_face (mesh, fh);
			foreach (auto &vh, adj_vhs){
				front_vhs.insert (vh);
				all_vhs.insert (vh);
			}
		}

		int count = 1;
		while (count++ < depth_num){
			std::unordered_set<OvmVeH> new_front_vhs;
			foreach (auto &vh, front_vhs){
				std::unordered_set<OvmVeH> adj_vhs;
				JC::get_adj_vertices_around_vertex (mesh, vh, adj_vhs);
				foreach (auto &adj_vh, adj_vhs){
					if (JC::contains (all_vhs, adj_vh)) continue;
					all_vhs.insert (adj_vh);
					new_front_vhs.insert (adj_vh);
				}
			}
			front_vhs = new_front_vhs;
		}

		foreach (auto &vh, all_vhs){
			std::unordered_set<OvmCeH> adj_chs;
			JC::get_adj_hexas_around_vertex (mesh, vh, adj_chs);
			foreach (auto &ch, adj_chs)
				hexas.insert (ch);
		}
	}

	void get_diff_fhs_patch (VolumeMesh *mesh, std::unordered_set<OvmCeH> &hexas, std::unordered_set<OvmFaH> &inter_fhs,
		std::unordered_set<OvmFaH> &fixed_patch, std::unordered_set<OvmFaH> &free_patch)
	{
		std::unordered_set<OvmFaH> bound_fhs;
		JC::collect_boundary_element (mesh, hexas, NULL, NULL, &bound_fhs);
		foreach (auto &fh, bound_fhs){
			if (mesh->is_boundary (fh)){
				if (!JC::contains (inter_fhs, fh))
					fixed_patch.insert (fh);
			}else{
				free_patch.insert (fh);
			}
		}
	}

	void create_mesh_data_for_match (VolumeMesh *old_mesh, std::unordered_set<OvmCeH> &old_hexas, 
		std::unordered_set<OvmFaH> &old_inter_fhs, std::unordered_set<OvmFaH> &old_fixed_patch, 
		std::unordered_set<OvmFaH> &old_free_patch,
		MMData &mm_data)
	{
		std::vector<std::vector<OvmVeH> > recipe_for_chs;
		std::hash_map<OvmVeH, OvmVeH> vhs_mapping; //old vertex handle --- new vertex handle
		std::unordered_set<OvmVeH> all_vhs;
		mm_data.mesh = new VolumeMesh ();
		foreach (auto &ch, old_hexas){
			auto adj_vhs = JC::get_adj_vertices_around_hexa (old_mesh, ch);
			foreach (auto &vh, adj_vhs)
				all_vhs.insert (vh);
			recipe_for_chs.push_back (adj_vhs);
		}
		foreach (auto &vh, all_vhs){
			auto pos = old_mesh->vertex (vh);
			OvmVeH new_vh = mm_data.mesh->add_vertex (pos);
			vhs_mapping.insert (std::make_pair (vh, new_vh));
		}
		for (int i = 0; i != recipe_for_chs.size (); ++i){
			auto &one_recipe = recipe_for_chs[i];
			for (int j = 0; j != one_recipe.size (); ++j){
				one_recipe[j] = vhs_mapping[one_recipe[j]];
			}
			mm_data.mesh->add_cell (one_recipe);
		}

		std::map<std::set<OvmVeH>, OvmFaH> footprints_of_bound_fhs;
		for (auto bf_it = mm_data.mesh->bf_iter (); bf_it; ++bf_it){
			auto adj_vhs = JC::get_adj_vertices_around_face (mm_data.mesh, *bf_it);
			std::set<OvmVeH> footprint_of_fh;
			JC::vector_to_set (adj_vhs, footprint_of_fh);
			footprints_of_bound_fhs.insert (std::make_pair (footprint_of_fh, *bf_it));
		}

		auto fGetNewPatch = [&] (std::unordered_set<OvmFaH> &old_patch, std::unordered_set<OvmFaH> &new_patch){
			foreach (auto &fh, old_patch){
				auto adj_vhs = JC::get_adj_vertices_around_face (old_mesh, fh);
				for (int i = 0; i != adj_vhs.size (); ++i)
					adj_vhs[i] = vhs_mapping[adj_vhs[i]];
				std::set<OvmVeH> footprint_of_fh;
				JC::vector_to_set (adj_vhs, footprint_of_fh);
				OvmFaH new_fh = footprints_of_bound_fhs[footprint_of_fh];
				new_patch.insert (new_fh);
			}
		};
		
		fGetNewPatch (old_inter_fhs, mm_data.inter_patch);
		fGetNewPatch (old_fixed_patch, mm_data.fixed_patch);
		fGetNewPatch (old_free_patch, mm_data.free_patch);
	}

	void asso_mesh_with_interface (MMData *mm_data, FACE *int_face)
	{
		VolumeMesh *mesh = mm_data->mesh;
		auto &inter_fhs = mm_data->inter_patch;
		auto V_ENT_PTR = mesh->request_vertex_property<unsigned long> ("entityptr", 0);
		mesh->set_persistent (V_ENT_PTR);

		ENTITY_LIST vertices_list, edges_list;
		api_get_vertices (int_face, vertices_list);
		api_get_edges (int_face, edges_list);

		SPAposition closest_pos;
		double dist = 0.0f;
		double myresabs = SPAresabs * 1000;
		auto fOnWhichEntity = [&myresabs, &closest_pos, &dist](ENTITY_LIST &entity_list, SPAposition &pos)->ENTITY*{
			for (int i = 0; i != entity_list.count (); ++i){
				api_entity_point_distance (entity_list[i], pos, closest_pos, dist);
				if (dist < myresabs)
					return entity_list[i];
			}
			return NULL;
		};

		std::unordered_set<OvmVeH> vhs_on_fhs;
		foreach (auto &fh, inter_fhs){
			auto adj_vhs = JC::get_adj_vertices_around_face (mesh, fh);
			foreach (auto &vh, adj_vhs)
				vhs_on_fhs.insert (vh);
		}

		foreach (auto &vh, vhs_on_fhs){
			SPAposition spa_pos = POS2SPA(mesh->vertex (vh));
			ENTITY* on_this_vertex = fOnWhichEntity (vertices_list, spa_pos);
			if (on_this_vertex)
				V_ENT_PTR[vh] = (unsigned long)on_this_vertex;
			else{
				ENTITY* on_this_edge = fOnWhichEntity (edges_list, spa_pos);
				if (on_this_edge)
					V_ENT_PTR[vh] = (unsigned long)on_this_edge;
			}//end if (on_this_vertex)
		}
	}
}

MeshMatchingHandler::MeshMatchingHandler (MMData *_data1, MMData *_data2, std::set<FACE *> _interfaces)
	: mm_data1 (_data1), mm_data2 (_data2), interfaces (_interfaces)
{
	JC::retrieve_chords (mm_data1->mesh, mm_data1->inter_patch, chord_set1);
	JC::retrieve_chords (mm_data2->mesh, mm_data2->inter_patch, chord_set2);
	myresabs = SPAresabs * 10000;

	get_all_ehs_on_interface (mm_data1);get_all_ehs_on_interface (mm_data2);
	attach_interface_entity_to_mesh (mm_data1);
	attach_interface_entity_to_mesh (mm_data2);
}

bool MeshMatchingHandler::check_match ()
{
	all_matched_chords.clear ();
	foreach (auto &p, matched_chord_pairs){
		all_matched_chords.insert (p.first);
		all_matched_chords.insert (p.second);
	}
	foreach (auto &p, matched_chord_pairs){
		if (!can_two_chords_match (p.first, p.second)){
			can_two_chords_match (p.first, p.second);
			return false;
		}
	}
	return true;
}

void MeshMatchingHandler::get_all_ehs_on_interface (MMData *mm_data)
{
	mm_data->ehs_on_interface.clear ();
	foreach (auto &fh, mm_data->inter_patch){
		auto heh_vec = mm_data->mesh->face (fh).halfedges ();
		foreach (auto &heh, heh_vec){
			auto eh = mm_data->mesh->edge_handle (heh);
			mm_data->ehs_on_interface.insert (eh);
		}
	}
}

void MeshMatchingHandler::attach_interface_entity_to_mesh (MMData *mm_data)
{
	//获得那些位于边界上的几何边（因为interface可能由多个相邻的几何面组成）
	std::set<ENTITY*> perip_edges_buff;
	foreach (auto f, interfaces){
		ENTITY_LIST tmp_edge_list;
		api_get_edges (f, tmp_edge_list);
		for (int i = 0; i != tmp_edge_list.count (); ++i){
			auto eg = tmp_edge_list[i];
			if (perip_edges_buff.find (eg) == perip_edges_buff.end ())
				perip_edges_buff.insert (eg);
			else
				perip_edges_buff.erase (eg);
		}
	}

	std::vector<ENTITY*> perip_edges_list;
	JC::set_to_vector (perip_edges_buff, perip_edges_list);
	
	VolumeMesh *mesh = mm_data->mesh;
	std::map<EDGE*, std::unordered_set<OvmEgH> > ehs_on_edges;
	if (!mesh->edge_property_exists<unsigned long> ("intedgeptr")){
		auto tmp = mesh->request_edge_property<unsigned long> ("intedgeptr", 0);
		mesh->set_persistent (tmp);
	}
	auto E_INT_EG_PTR = mesh->request_edge_property<unsigned long> ("intedgeptr");

	foreach (auto &eh, mm_data->ehs_on_interface){
		EDGE *on_which_eg = NULL;
		OvmVec3d vh1 = mesh->vertex(mesh->edge(eh).to_vertex()), 
			vh2 = mesh->vertex(mesh->edge(eh).from_vertex());
		SPAposition closest_pos;
		double dist1 = 0.0f, dist2 = 0.0f;
		for (int i = 0; i != perip_edges_list.size (); ++i){
			api_entity_point_distance(perip_edges_list[i], POS2SPA(vh1), closest_pos, dist1);
			api_entity_point_distance(perip_edges_list[i], POS2SPA(vh2), closest_pos, dist2);
			if (dist1 < myresabs && dist2 < myresabs){
				on_which_eg = (EDGE *)(perip_edges_list[i]);
				E_INT_EG_PTR[eh] = (unsigned long)on_which_eg;
				break;
			}
		}
		if (on_which_eg){
			ehs_on_edges[on_which_eg].insert (eh);
		}
	}
	for (int i = 0; i != perip_edges_list.size (); ++i){
		EDGE *eg = (EDGE*)(perip_edges_list[i]);
		auto &ordered_ehs = mm_data->ordered_ehs_on_edges[eg];
		auto ssss = ehs_on_edges[eg];
		get_ordered_ehs_on_edge (mesh, ehs_on_edges[eg], eg, ordered_ehs);
		ordered_ehs = ordered_ehs;
	}
}

void MeshMatchingHandler::init_match ()
{
	struct ChordPairCandidate{
		DualChord *chord1, *chord2;
		double dist;

		//重载<运算符，使之能够在STL set容器中自动排序
		bool operator < (const ChordPairCandidate &rhs_) const
		{
			return dist < rhs_.dist;
		}
	};
	//根据两个chord的起始边、围成的多边形来初步判断他们能否匹配
	//将能匹配的放入candidates数组
	std::set<ChordPairCandidate> cpcs;
	foreach (auto &chord1, chord_set1){
		foreach (auto &chord2, chord_set2){
			if (can_two_chords_match (chord1, chord2)){
				auto polyline1 = get_chord_spa_polyline (chord1),
					polyline2 = get_chord_spa_polyline (chord2);
				auto dist = maximum_distance (polyline1, polyline2);
				ChordPairCandidate cpc;
				cpc.chord1 =chord1; cpc.chord2 =chord2;	cpc.dist = dist;
				cpcs.insert (cpc);
			}
		}
	}

	//下面从cpcs中每次取出dist最小的candidate，加入到matched_pairs中
	//后续加入的需要将之前匹配的chord对考虑在内
	while (!cpcs.empty ()){
		auto candidiate = JC::pop_begin_element (cpcs);
		if (can_two_chords_match (candidiate.chord1, candidiate.chord2)){
			matched_chord_pairs.insert (std::make_pair (candidiate.chord1, candidiate.chord2));
			all_matched_chords.insert (candidiate.chord1);
			all_matched_chords.insert (candidiate.chord2);

			for (auto it = cpcs.begin (); it != cpcs.end ();){
				if (it->chord1 == candidiate.chord1 || it->chord2 == candidiate.chord2)
					it = cpcs.erase (it);
				else it++;
			}
		}else{
			can_two_chords_match (candidiate.chord1, candidiate.chord2);
			int i = 0;
			i = i;
			
		}
	}
}

bool MeshMatchingHandler::can_two_chords_match (DualChord *chord1, DualChord *chord2)
{
	//两个chord的类型必须一致
	if (chord1->is_closed != chord2->is_closed)
		return false;

	//获得chord位置描述子
	auto cpd1 = get_chord_pos_desc (mm_data1, chord1),
		cpd2 = get_chord_pos_desc (mm_data2, chord2);

	//如果相交序列长度不一样，那肯定不能匹配
	if (cpd1.inter_graph.size () != cpd2.inter_graph.size ())
		return false;

	int graph_size = cpd1.inter_graph.size ();

	//下面逐个判断相交序列中的chord是否匹配

	auto fSameIntGraph = [&] (std::vector<DualChord*> &int_graph1, std::vector<DualChord*> &int_graph2)->bool{
		for (int i = 0; i != graph_size; ++i){
			auto int_chord1 = int_graph1[i],
				int_chord2 = int_graph2[i];
			if (int_chord1 == chord1 || int_chord2 == chord2){
				if (!(int_chord1 == chord1 && int_chord2 == chord2))
					return false;
			}
			bool has_found = false;
			foreach (auto &p, matched_chord_pairs){
				if (p.first == int_chord1 && p.second == int_chord2){
					has_found = true; break;
				}
			}
			if (!has_found) return false;
		}
		return true;
	};
	if (chord1->is_closed){
		if (graph_size == 0) return true;
		bool same_circle = false;
		for (int i = 0; i != graph_size; ++i){
			if (fSameIntGraph (cpd1.inter_graph, cpd2.inter_graph)){
				same_circle = true;
				break;
			}
			auto front_chord2 = JC::pop_begin_element (cpd2.inter_graph);
			cpd2.inter_graph.push_back (front_chord2);
		}
		if (!same_circle){
			std::reverse (cpd2.inter_graph.begin (), cpd2.inter_graph.end ());
			for (int i = 0; i != graph_size; ++i){
				if (fSameIntGraph (cpd1.inter_graph, cpd2.inter_graph)){
					same_circle = true;
					break;
				}
				auto front_chord2 = JC::pop_begin_element (cpd2.inter_graph);
				cpd2.inter_graph.push_back (front_chord2);
			}
		}
		if (!same_circle) return false;
	}else{
		if (!(cpd1.start_edge == cpd2.start_edge && cpd1.end_edge == cpd2.end_edge))
			return false;
		//下面判断起始和终止边上的位置是否一样
		if (cpd1.start_idx != cpd2.start_idx || cpd1.end_idx != cpd2.end_idx)
			return false;
		
		if (!fSameIntGraph (cpd1.inter_graph, cpd2.inter_graph))
			return false;
	}

	//下面还需要考虑两个chord的polyline围成的区域内部是否包含内环
	//暂时这部分未实现
	//...

	return true;
}

ChordPosDesc MeshMatchingHandler::get_chord_pos_desc (MMData *mm_data, DualChord *chord)
{
	ChordPosDesc cpd;
	cpd.chord = chord;
	VolumeMesh *mesh = mm_data->mesh;
	get_inter_graph (cpd, chord);
	if (!chord->is_closed){
		get_edge_graph (cpd, mm_data, chord);
	}
	return cpd;
}

void MeshMatchingHandler::get_inter_graph (ChordPosDesc &cpd, DualChord *chord)
{
	VolumeMesh *mesh = chord->mesh;
	auto E_INT_EG_PTR = mesh->request_edge_property<unsigned long> ("intedgeptr");

	//auto start_asso_eg = JC::get_associated_geometry_edge_of_boundary_eh (mesh, chord->ordered_ehs.front ());
	//auto end_asso_eg = JC::get_associated_geometry_edge_of_boundary_eh (mesh, chord->ordered_ehs.back ());
	auto start_asso_eg = (EDGE*)(E_INT_EG_PTR[chord->ordered_ehs.front ()]);
	auto end_asso_eg = (EDGE*)(E_INT_EG_PTR[chord->ordered_ehs.back ()]);

	//调整start edge和end edge，是的在cpd中，start edge的值比end edge的值要小，这样便于后面的一些判断操作
	cpd.start_edge = start_asso_eg < end_asso_eg? start_asso_eg : end_asso_eg;
	cpd.end_edge = start_asso_eg < end_asso_eg? end_asso_eg : start_asso_eg;

	//如果chord中的ehs的顺序和cpd的start和end顺序相反，并且start和end edge不是同一条几何边
	//那么就将ehs的顺序反向
	if (start_asso_eg != end_asso_eg && start_asso_eg == cpd.end_edge){
		std::reverse (chord->ordered_ehs.begin (), chord->ordered_ehs.end ());
		std::reverse (chord->ordered_fhs.begin (), chord->ordered_fhs.end ());
	}
	//如果start和end edge是同一条几何边，那么ehs的顺序在后面的get_edge_graph中进行调整

	auto E_CHORD_PTR = mesh->request_edge_property<unsigned long> ("chordptr");
	auto ordered_fhs = chord->ordered_fhs;
	auto &int_graph = cpd.inter_graph;
	auto &int_ordered_ehs = cpd.inter_ordered_ehs;
	foreach (auto &fh, ordered_fhs){
		auto heh_vec = mesh->face (fh).halfedges ();
		OvmEgH inter_chord_eh = mesh->InvalidEdgeHandle;
		foreach (auto &heh, heh_vec){
			auto eh = mesh->edge_handle (heh);
			if (!JC::contains (chord->ordered_ehs, eh)){
				DualChord *int_chord = (DualChord*)(E_CHORD_PTR[eh]);
				if (JC::contains (all_matched_chords, int_chord)){
					int_graph.push_back (int_chord);
					int_ordered_ehs.push_back (eh);
					//为了精确标注位置，还需要获得在int_chord的相交序列中，该chord位于什么位置

				}
				break;
			}
		}
	}
}

void MeshMatchingHandler::get_edge_graph (ChordPosDesc &cpd, MMData *mm_data, DualChord *chord)
{
	VolumeMesh *mesh = mm_data->mesh;

	auto E_CHORD_PTR = mesh->request_edge_property<unsigned long> ("chordptr");
	auto fGetOrderedPos = [&] (std::vector<OvmEgH> &ordered_ehs, std::vector<DualChord*> &ordered_chords, EDGE *eg){
		std::vector<OvmEgH> &all_ordered_ehs = mm_data->ordered_ehs_on_edges[eg];
		foreach (auto &eh, all_ordered_ehs){
			DualChord *chord_on_eh = (DualChord*)(E_CHORD_PTR[eh]);
			if (JC::contains (all_matched_chords, chord_on_eh) || chord_on_eh == chord){
				ordered_chords.push_back (chord_on_eh);
				ordered_ehs.push_back (eh);
			}
		}
	};
	auto fGetIndex = [&] (std::vector<OvmEgH> &ordered_ehs, OvmEgH eh)->int{
		for (int i = 0; i != ordered_ehs.size (); ++i){
			if (ordered_ehs[i] == eh) return i;
		}
		return -1;
	};

	//如果起始边和终止边在不同的几何边上，则需要获得两个序列
	if (cpd.start_edge != cpd.end_edge){
		fGetOrderedPos (cpd.start_edge_ordered_ehs, cpd.start_edge_graph, cpd.start_edge);
		fGetOrderedPos (cpd.end_edge_ordered_ehs, cpd.end_edge_graph, cpd.end_edge);
		cpd.start_idx = fGetIndex (cpd.start_edge_ordered_ehs, chord->ordered_ehs.front ());
		assert (cpd.start_idx != -1);
		cpd.end_idx = fGetIndex (cpd.end_edge_ordered_ehs, chord->ordered_ehs.back ());
		assert (cpd.end_idx != -1);
	}
	//如果起始边和终止边相同，只需要获得一个序列
	else{
		fGetOrderedPos (cpd.start_edge_ordered_ehs, cpd.start_edge_graph, cpd.start_edge);
		int start_idx = fGetIndex (cpd.start_edge_ordered_ehs, chord->ordered_ehs.front ()),
			end_idx = fGetIndex (cpd.start_edge_ordered_ehs, chord->ordered_ehs.back ());
		//如果起始边在该边上的序号比终止边大，那么就反转该chord的ehs
		if (start_idx > end_idx){
			std::reverse (chord->ordered_ehs.begin (), chord->ordered_ehs.end ());
			std::reverse (chord->ordered_fhs.begin (), chord->ordered_fhs.end ());
			std::reverse (cpd.inter_graph.begin (), cpd.inter_graph.end ());
			qSwap (start_idx, end_idx);
		}
		cpd.start_idx = start_idx; cpd.end_idx = end_idx;
	}
}

void MeshMatchingHandler::get_ordered_ehs_on_edge (VolumeMesh *mesh, std::unordered_set<OvmEgH> &ehs_on_eg, 
	EDGE *eg, std::vector<OvmEgH> &ordered_ehs)
{
	VERTEX *start_vertex = eg->start ();
	SPAposition start_pos = start_vertex->geometry ()->coords ();
	OvmEgH start_eh = mesh->InvalidEdgeHandle;
	OvmVeH joint_vh = mesh->InvalidVertexHandle;
	foreach (auto &eh, ehs_on_eg){
		auto vh1 = mesh->edge (eh).to_vertex (),
			vh2 = mesh->edge (eh).from_vertex ();
		auto pos1 = POS2SPA(mesh->vertex(vh1)),
			pos2 = POS2SPA(mesh->vertex(vh2));
		if (same_point(pos1, start_pos, myresabs)){
			joint_vh = vh2;
			start_eh = eh;
		}
		else if (same_point(pos2, start_pos, myresabs)){
			joint_vh = vh1;
			start_eh = eh;
		}
	}

	ordered_ehs.push_back (start_eh);
	ehs_on_eg.erase (start_eh);
	while (!ehs_on_eg.empty ()){
		foreach (auto &eh, ehs_on_eg){
			OvmEgH next_eh = mesh->InvalidEdgeHandle;
			auto vh1 = mesh->edge (eh).to_vertex (),
				vh2 = mesh->edge (eh).from_vertex ();
			if (vh1 == joint_vh){
				joint_vh = vh2;
				next_eh = eh;
			}else if (vh2 == joint_vh){
				joint_vh = vh1;
				next_eh = eh;
			}
			if (next_eh != mesh->InvalidEdgeHandle){
				ordered_ehs.push_back (eh);
				ehs_on_eg.erase (eh);
				break;
			}
		}
	}
}

void MeshMatchingHandler::get_all_vhs_on_interface (std::unordered_set<OvmVeH> &all_vhs)
{

}

void MeshMatchingHandler::translate_chord_pos_desc (ChordPosDesc &in_cpd, ChordPosDesc &out_cpd)
{
	VolumeMesh *in_mesh = in_cpd.chord->mesh;
	out_cpd.chord = NULL;
	out_cpd.start_edge = in_cpd.start_edge; out_cpd.end_edge = in_cpd.end_edge;
	out_cpd.start_idx = in_cpd.start_idx; out_cpd.end_idx = in_cpd.end_idx;

	auto fTranslate = [&] (std::vector<DualChord*> &graph)->std::vector<DualChord*>{
		std::vector<DualChord*> translated_graph;
		foreach (auto &chord, graph){
			if (chord == in_cpd.chord){
				translated_graph.push_back (NULL);
			}else{
				auto matched_chord = get_matched_chord (chord);
				assert (matched_chord);
				translated_graph.push_back (matched_chord);
			}
		}
		return translated_graph;
	};

	out_cpd.inter_graph = fTranslate (in_cpd.inter_graph);
	out_cpd.start_edge_graph = fTranslate (in_cpd.start_edge_graph);
	out_cpd.end_edge_graph = fTranslate (out_cpd.end_edge_graph);
}

DualChord *MeshMatchingHandler::get_matched_chord (DualChord *chord)
{
	foreach (auto &p, matched_chord_pairs){
		if (p.first == chord) return p.second;
		else if (p.second == chord) return p.first;
	}
	return NULL;
}

//std::vector<OvmEgH> MeshMatchingHandler::find_inflation_polyline (ChordPosDesc &in_cpd, ChordPosDesc &trans_cpd)
//{
//	DualChord *chord = in_cpd.chord;
//	VolumeMesh *chord_mesh = chord->mesh;
//	VolumeMesh *other_mesh = chord_mesh == mm_data1->mesh? mm_data2->mesh : mm_data1->mesh;
//	auto E_CHORD_PTR = other_mesh->request_edge_property<unsigned long> ("chordptr");
//	MMData *chord_mm_data = chord_mesh == mm_data1->mesh? mm_data1 : mm_data2;
//	MMData *other_mm_data = chord_mesh == mm_data1->mesh? mm_data2 : mm_data1;
//
//	auto fGetSuitableVhsOnEdge = [&](VolumeMesh *mesh, std::vector<OvmEgH> ordered_ehs, int idx)->
//		std::unordered_set<OvmVeH> {
//			std::unordered_set<OvmEgH> suitable_ehs;
//			int count = 0;
//			std::unordered_set<OvmVeH> suitable_vhs;
//			foreach (auto &eh, ordered_ehs){
//				if (count == idx) suitable_ehs.insert (eh);
//				DualChord *cur_chord = (DualChord*)(E_CHORD_PTR[eh]);
//				if (JC::contains (all_matched_chords, cur_chord))
//					count++;
//				if (count > idx) break;
//			}
//			foreach (auto &eh, suitable_ehs){
//				auto vh1 = mesh->edge (eh).to_vertex (),
//					vh2 = mesh->edge (eh).from_vertex ();
//				suitable_vhs.insert (vh1); suitable_vhs.insert (vh2);
//			}
//			return suitable_vhs;
//	};
//
//	//当chord为开放chord时
//	auto fFindOpenPolyline = [&]()->std::vector<OvmEgH>{
//		//找到起始点集合
//		std::vector<OvmEgH> ordered_ehs;
//		get_ordered_ehs_on_edge (other_mesh, other_mm_data->inter_patch, trans_cpd.start_edge, ordered_ehs);
//		auto suitable_start_vhs = fGetSuitableVhsOnEdge (other_mesh, ordered_ehs, trans_cpd.start_idx);
//
//	};
//
//	if (!chord->is_closed){
//
//	}
//}
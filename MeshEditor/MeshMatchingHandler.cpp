#include "stdafx.h"
#include "MeshMatchingHandler.h"
#include <limits>
#include "polysim.h"
#include <QFile>
#include <QDataStream>
#include <QObject>
#include <QMessageBox>
#include "bool_api_options.hxx"
#include "ChordPolylineDijkstra.h"

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
		auto V_ENT_PTR = mesh->request_vertex_property<unsigned int> ("entityptr");

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
			V_ENT_PTR[closest_vh] = (unsigned int)v;
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
					V_ENT_PTR[vh] = (unsigned int) (on_which_edge);
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
		auto V_ENT_PTR = mesh->request_vertex_property<unsigned int> ("entityptr", 0);
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
				V_ENT_PTR[vh] = (unsigned int)on_this_vertex;
			else{
				ENTITY* on_this_edge = fOnWhichEntity (edges_list, spa_pos);
				if (on_this_edge)
					V_ENT_PTR[vh] = (unsigned int)on_this_edge;
			}//end if (on_this_vertex)
		}
	}
}


MeshMatchingHandler::MeshMatchingHandler (QWidget *parent)
{
	this->parent = parent;
	mm_data1 = NULL;
	mm_data2 = NULL;
	myresabs = SPAresabs * 1000;
}

MeshMatchingHandler::~MeshMatchingHandler ()
{
	if (mm_data1)
		delete mm_data1;
	if (mm_data2)
		delete mm_data2;
	foreach (auto f, interfaces)
		api_del_entity (f);
}

bool MeshMatchingHandler::load_mesh_matching_data (QString dir_path)
{
	QString part1mesh = dir_path + "\\part1.ovm",
		part1body = dir_path + "\\part1.sat",
		part2mesh = dir_path + "\\part2.ovm",
		part2body = dir_path + "\\part2.sat",
		chord_pairs_path = dir_path + "\\matched_chord_pairs.dat";

	QFileInfo fi1 (part1mesh), fi2 (part1body), fi3 (part2mesh), fi4 (part2body);
	if (!fi1.exists () || !fi2.exists () || !fi3.exists () || !fi4.exists ()){
		warning (QObject::tr("打不开文件！"));
		return false;
	}

	if (mm_data1) delete mm_data1;
	if (mm_data2) delete mm_data2;

	mm_data1 = new MMData; mm_data2 = new MMData;

	//加载六面体网格和对应的sat模型
	mm_data1->mesh = JC::load_volume_mesh (part1mesh.toAscii ().data ());
	mm_data1->body = JC::load_acis_model (part1body);
	mm_data2->mesh = JC::load_volume_mesh (part2mesh.toAscii ().data ());
	mm_data2->body = JC::load_acis_model (part2body);

	if (!locate_interfaces ()){
		warning (QObject::tr("查找贴合面失败！"));
		return false;
	}

	JC::init_volume_mesh (mesh1(), body1 (), myresabs);
	JC::init_volume_mesh (mesh2(), body2 (), myresabs);
	//构建或者从六面体网格属性中恢复对偶结构，同时搜集interfaces上的网格边、面
	construct_dual_structure_and_gather_interface_elements (mm_data1);
	construct_dual_structure_and_gather_interface_elements (mm_data2);

	//将interface上的面边结构和网格相关联
	attach_interface_entity_to_mesh (mm_data1);
	attach_interface_entity_to_mesh (mm_data2);

	//构建chord匹配关系
	matched_chord_pairs.clear ();
	QFileInfo matched_chord_pairs_file (chord_pairs_path);
	if (matched_chord_pairs_file.exists ()){
		if (!load_matched_chords (chord_pairs_path)){
			warning (QObject::tr("chord匹配文件存在但是读取错误！"));
			return false;
		}
	}
	return true;
}

bool MeshMatchingHandler::save_mesh_matching_data (QString dir_path)
{
	auto fSaving = [&] (MMData *mm_data, QString which_part){
		auto mesh = mm_data->mesh;
		auto body = mm_data->body;
		auto E_CHORD_ORDER = mesh->request_edge_property<int> ("chordorder", 0);
		foreach (auto c, mm_data->unmatched_chords){
			for (int i = 0; i != c->ordered_ehs.size (); ++i)
				E_CHORD_ORDER[c->ordered_ehs[i]] = i;
			if (c->is_closed){
				E_CHORD_ORDER[c->ordered_ehs.front ()] = -1;
			}
		}

		foreach (auto c, mm_data->matched_chords){
			for (int i = 0; i != c->ordered_ehs.size (); ++i)
				E_CHORD_ORDER[c->ordered_ehs[i]] = i;
			if (c->is_closed){
				E_CHORD_ORDER[c->ordered_ehs.front ()] = -1;
			}
		}

		mesh->set_persistent (E_CHORD_ORDER);
		auto M_DUAL = mesh->request_mesh_property<bool> ("dual", true);
		mesh->set_persistent (M_DUAL);
		JC::save_volume_mesh (mesh, dir_path + "/" + which_part +".ovm");
		JC::save_acis_entity (body, dir_path + "/" + which_part +".sat");
	};
	
	fSaving (mm_data1, "part1");
	fSaving (mm_data2, "part2");

	return save_matched_chords (dir_path + "/matched_chord_pairs.dat");
}

void MeshMatchingHandler::get_quads_on_interfaces (MMData *mm_data)
{
	JC::get_fhs_on_acis_faces (mm_data->mesh, mm_data->interfaces, mm_data->inter_patch);
}

void MeshMatchingHandler::construct_dual_structure_and_gather_interface_elements (MMData *mm_data)
{
	auto mesh = mm_data->mesh;
	auto &sheet_id_mapping = mm_data->sheet_id_mapping;
	auto &chord_id_mapping = mm_data->chord_id_mapping;
	//cleaning
	mm_data->sheets.clear ();
	mm_data->sheet_id_mapping.clear ();
	mm_data->matched_chords.clear ();
	mm_data->unmatched_chords.clear ();
	mm_data->chord_id_mapping.clear ();
	mm_data->inter_patch.clear ();

	//如果六面体网格中已经存了对偶结构信息，则直接利用该信息重构对偶结构
	if (mesh->mesh_property_exists<bool> ("dual")){
		//创建必要的对偶结构属性
		if (!mesh->edge_property_exists<unsigned int> ("sheetptr")){
			warning (QObject::tr("Sheet属性不存在！"));
			return;
		}
		if (!mesh->edge_property_exists<unsigned int> ("chordptr")){
			warning (QObject::tr("Chord属性不存在！"));
			return;
		}
		auto E_SHEET_PTR = mesh->request_edge_property<unsigned int> ("sheetptr");
		auto E_CHORD_PTR = mesh->request_edge_property<unsigned int> ("chordptr");

		//首先构建sheet
		std::vector<OvmEgH> bnd_ehs;
		for (auto e_it = mesh->edges_begin (); e_it != mesh->edges_end (); ++e_it){
			auto eh = *e_it;
			//收集表面网格边，用于后面的chord重建
			if (mesh->is_boundary (eh)) bnd_ehs.push_back (eh);

			auto old_sheet_ptr = E_SHEET_PTR[eh];
			DualSheet *sheet_of_this_eh = NULL;
			auto locate = sheet_id_mapping.find (old_sheet_ptr);
			if (locate != sheet_id_mapping.end ()){
				sheet_of_this_eh = locate->second;
				sheet_of_this_eh->ehs.insert (eh);
				//更新E_SHEET_PTR中该边存放的sheet指针
				E_SHEET_PTR[eh] = (unsigned int)(sheet_of_this_eh);

			}else{
				auto new_sheet = new DualSheet (mesh);
				new_sheet->ehs.insert (eh);
				sheet_id_mapping.insert (std::make_pair (old_sheet_ptr, new_sheet));
				mm_data->sheets.insert (new_sheet);
				//更新E_SHEET_PTR中该边存放的sheet指针
				E_SHEET_PTR[eh] = (unsigned int)(new_sheet);
				sheet_of_this_eh = new_sheet;
			}
			//将eh周围相邻的六面体也放入该sheet中
			JC::get_adj_hexas_around_edge (mesh, eh, sheet_of_this_eh->chs);
		}

		//然后构建chord
		std::vector<OvmFaH> bnd_fhs;
		auto E_CHORD_ORDER = mesh->request_edge_property<int> ("chordorder");
		auto element_orders_on_chords = new std::map<unsigned int, std::map<int, OvmEgH> > ();

		foreach (auto eh, bnd_ehs){
			auto old_chord_ptr = E_CHORD_PTR[eh];
			//如果old_chord_ptr为0，则表示这条不在interface上
			if (old_chord_ptr == 0) continue;
			//如果old_chord_ptr不为0，则说明这条网格边在interfaces上，需要搜集起来
			mm_data->ehs_on_interface.insert (eh);
			(*element_orders_on_chords)[old_chord_ptr].insert (std::make_pair (E_CHORD_ORDER[eh], eh));
		}

		for (auto it = element_orders_on_chords->begin (); it != element_orders_on_chords->end (); ++it){
			auto new_chord = new DualChord (mesh);
			foreach (auto &p, it->second){
				new_chord->ordered_ehs.push_back (p.second);
				E_CHORD_PTR[p.second] = (unsigned int)new_chord;
			}
			//如果it->second的第一个chord order为-1的话，则表示该chord为环状chord，我们需要在它的ordered_ehs末尾重复添加第一条边
			if (it->second.begin ()->first == -1){
				new_chord->ordered_ehs.push_back (new_chord->ordered_ehs.front ());
				new_chord->is_closed = true;
			}
			//构建chord上的四边形面集以及顺序
			new_chord->ordered_fhs.clear ();
			for (int i = 0; i != new_chord->ordered_ehs.size () - 1; ++i){
				auto fh = JC::get_common_face_handle (mesh, new_chord->ordered_ehs[i], new_chord->ordered_ehs[i + 1]);
				if (JC::contains (new_chord->ordered_fhs, fh))
					new_chord->is_self_int = true;
				new_chord->ordered_fhs.push_back (fh);
				mm_data->inter_patch.insert (fh);
			}

			chord_id_mapping[it->first] = new_chord;
			mm_data->unmatched_chords.insert (new_chord);
		}

		delete element_orders_on_chords;
		mesh->set_persistent (E_CHORD_ORDER, false);
	}//end if (mesh->mesh_property_exists<bool> ("dual")){...
	//如果六面体网格中没有对偶结构，则需要重新构建
	else{
		get_quads_on_interfaces (mm_data);
		get_all_ehs_on_interface (mm_data);
		JC::retrieve_sheets (mesh, mm_data->sheets);
		JC::retrieve_chords (mesh, mm_data->inter_patch, mm_data->unmatched_chords);
	}
}

bool MeshMatchingHandler::locate_interfaces ()
{
	logical is_touch;
	api_entity_entity_touch (body1 (), body2 (), is_touch);
	if (is_touch == FALSE){
		warning (QObject::tr("两个模型没有接触面！"));
		return false;
	}
	BoolOptions boolopts;
	boolopts.set_near_coincidence_fuzz (SPAresabs * 1000);
	api_initialize_booleans ();
	ENTITY_LIST faces_list1, faces_list2;
	std::vector<FACE*> faces1, faces2;
	outcome o = api_imprint (body2 (), body1 (), &boolopts);
	if (!o.ok ()){
		auto str = o.get_error_info ()->error_message ();
		warning (str);
	}
	api_terminate_booleans ();

	//定义函数两个函数用于获得两个body之间的共享几何面
	auto face_coincident = [&](FACE *f1, FACE *f2, double myresabs)->bool{
		ENTITY_LIST vertices1, vertices2;
		api_get_vertices (f1, vertices1); api_get_vertices (f2, vertices2);
		std::vector<VERTEX*> std_vertices1, std_vertices2;
		JC::entity_list_to_vector (vertices1, std_vertices1);
		JC::entity_list_to_vector (vertices2, std_vertices2);

		//check vertices
		foreach (VERTEX *v1, std_vertices1){
			auto pFuncFindCoincidentVertex = [&v1, &myresabs](const VERTEX *v2)-> bool{
				SPAposition pos1 = v1->geometry ()->coords (),
					pos2 = v2->geometry ()->coords();
				return (same_point (pos1, pos2, myresabs));
			};
			auto locate = std::find_if (std_vertices2.begin (), std_vertices2.end (), pFuncFindCoincidentVertex);
			if (locate != std_vertices2.end ())
				std_vertices2.erase (locate);
			else return false;
		}

		//check edges
		ENTITY_LIST edges1, edges2;
		api_get_edges (f1, edges1); api_get_edges (f2,edges2);
		std::vector<SPAposition> mid_positions1, mid_positions2;
		std::vector<EDGE*> std_edges1, std_edges2;
		JC::entity_list_to_vector (edges1, std_edges1);
		JC::entity_list_to_vector (edges2, std_edges2);
		std::for_each (std_edges1.begin (), std_edges1.end (), [&mid_positions1](EDGE*e){mid_positions1.push_back (e->mid_pos ());});
		std::for_each (std_edges2.begin (), std_edges2.end (), [&mid_positions2](EDGE*e){mid_positions2.push_back (e->mid_pos ());});
		foreach (SPAposition p1, mid_positions1){
			auto pFuncFindCoincidentPoint = [&p1, &myresabs](SPAposition p2)-> bool{
				return (same_point (p1, p2, myresabs));
			};
			auto locate = std::find_if (mid_positions2.begin (), mid_positions2.end (), pFuncFindCoincidentPoint);
			if (locate != mid_positions2.end ())
				mid_positions2.erase (locate);
			else
				return false;
		}
		return true;
	};
	auto find_interface = [&](BODY *body1, BODY *body2, double myresabs)->std::vector<std::pair<FACE*, FACE*> >{
		std::pair<FACE*, FACE*> intf_pair;
		intf_pair.first = intf_pair.second = NULL;
		std::vector<std::pair<FACE*, FACE*> > pairs;
		ENTITY_LIST face_list1, face_list2;
		api_get_faces (body1, face_list1); api_get_faces (body2, face_list2);
		for (int i = 0; i != face_list1.count (); ++i){
			FACE *f1 = (FACE*)face_list1[i];
			for (int j = 0; j != face_list2.count (); ++j){
				FACE *f2 = (FACE*)face_list2[j];
				if (face_coincident (f1, f2, myresabs)){
					intf_pair.first = f1; intf_pair.second = f2;
					pairs.push_back (intf_pair);
				}
			}
		}
		return pairs;
	};
	auto interface_pairs = find_interface (body1(), body2(), myresabs);

	mm_data1->interfaces.clear (); mm_data2->interfaces.clear ();
	interfaces.clear ();
	foreach (auto &p, interface_pairs){
		mm_data1->interfaces.insert (p.first);
		mm_data2->interfaces.insert (p.second);
		//ENTITY *com_face = NULL;
		//api_deep_down_copy_entity (p.first, com_face);
		interfaces.insert (p.first);
		//HC_Open_Segment ("/match models");{
		//	HC_Open_Segment ("interfaces");{
		//		HA_Set_Rendering_Options ("preserve color=off,merge faces=off");
		//		HA_Render_Entity (com_face, ".");
		//	}HC_Close_Segment ();
		//}HC_Close_Segment ();
	}

	//定义一个函数用于获得边界几何边以及内部几何边
	auto fGetDiffGeomEdges = [&] (std::set<FACE*> faces, std::set<EDGE*>&bnd_geom_egs, std::set<EDGE*>&inn_geom_egs){
		bnd_geom_egs.clear (); inn_geom_egs.clear ();
		
		foreach (auto f, faces){
			ENTITY_LIST edge_list;
			api_get_edges (f, edge_list);
			for (int i = 0; i != edge_list.count (); ++i){
				auto eg = (EDGE*)edge_list[i];
				if (JC::contains (bnd_geom_egs, eg)){
					inn_geom_egs.insert (eg);
					bnd_geom_egs.erase (eg);
				}else
					bnd_geom_egs.insert (eg);
			}
		}
	};

	fGetDiffGeomEdges (interfaces, bound_geom_edges, inner_geom_edges);
	return true;
}

bool MeshMatchingHandler::add_two_matched_chords (DualChord *chord1, DualChord *chord2)
{
	if (!can_two_chords_match (chord1, chord2))
		return false;
	matched_chord_pairs.insert (std::make_pair (chord1, chord2));
	mm_data1->add_matched_chord (chord1);
	mm_data2->add_matched_chord (chord2);
	adjust_matched_chords_directions ();
	return true;
}

//bool MeshMatchingHandler::check_match ()
//{
//	hoopsview1->derender_mesh_groups ("mesh matching", "cannot matched chords", true);
//	hoopsview2->derender_mesh_groups ("mesh matching", "cannot matched chords", true);
//
//	foreach (auto &p, matched_chord_pairs){
//		if (!can_two_chords_match (p.first, p.second)){
//			can_two_chords_match (p.first, p.second);
//			MeshRenderOptions render_opts;
//			auto group = new VolumeMeshElementGroup (p.first->mesh, "mesh matching", "cannot matched chords");
//			JC::vector_to_unordered_set (p.first->ordered_ehs, group->ehs);
//			render_opts.edge_color = "red";
//			render_opts.edge_width = 12;
//			hoopsview1->render_mesh_group (group, render_opts);
//
//			group = new VolumeMeshElementGroup (p.second->mesh, "mesh matching", "cannot matched chords");
//			JC::vector_to_unordered_set (p.second->ordered_ehs, group->ehs);
//			render_opts.edge_color = "red";
//			render_opts.edge_width = 12;
//			hoopsview2->render_mesh_group (group, render_opts);
//			return false;
//		}
//	}
//	return true;
//}


//检查能否匹配分两步：
//1：检查interfaces的边界环上是否匹配
//2：检查内部被已匹配的chord划分出来的区域是否能匹配
bool MeshMatchingHandler::check_match ()
{
	//首先，做一些搜集工作
	//收集所有已匹配的chord
	std::set<DualChord*> all_matched_chords;
	foreach (auto p, matched_chord_pairs){
		all_matched_chords.insert (p.first);
		all_matched_chords.insert (p.second);
	}
	//然后搜集所有被两条chord相交的四边形面
	std::map<OvmFaH, std::set<DualChord*> > chord_fhs_statistics, int_fhs_mapping;
	foreach (auto c, all_matched_chords){
		foreach (auto quad, c->ordered_fhs)
			chord_fhs_statistics[quad].insert (c);
	}
	//找出其中被两条chord共享的四边形（即相交四边形）
	foreach (auto p, chord_fhs_statistics){
		if (p.second.size () == 2)
			int_fhs_mapping.insert (p);
	}

	//检查interfaces的边界环上是否匹配
	auto E_CHORD_PTR1 = get_E_CHORD_PTR (mm_data1),
		E_CHORD_PTR2 = get_E_CHORD_PTR (mm_data2);
	std::map<OvmEgH, OvmEgH> matched_ehs_pairs;
	std::map<OvmFaH, OvmFaH> matched_fhs_pairs;
	foreach (const auto &p, mm_data1->ordered_ehs_on_edges){
		auto geom_eg = p.first;
		auto ordered_ehs1 = p.second;
		auto ordered_ehs2 = mm_data2->ordered_ehs_on_edges[geom_eg];
		std::vector<DualChord*> matched_chords_seq1, matched_chords_seq2;
		std::vector<OvmEgH> matched_ehs_seq1, matched_ehs_seq2;
		foreach(auto eh, ordered_ehs1){
			auto chord = (DualChord*)(E_CHORD_PTR1[eh]);
			if (JC::contains (all_matched_chords, chord)){
				matched_chords_seq1.push_back (chord);
				matched_ehs_seq1.push_back (eh);
			}
		}
		foreach(auto eh, ordered_ehs2){
			auto chord = (DualChord*)(E_CHORD_PTR2[eh]);
			if (JC::contains (all_matched_chords, chord)){
				matched_chords_seq2.push_back (chord);
				matched_ehs_seq2.push_back (eh);
			}
		}
		//检查matched_chords_seq1中的chord是否和matched_chords_seq2中的chord按照顺序一一匹配
		if (matched_ehs_seq1.size () != matched_ehs_seq2.size ())
			return false;
		for (int i = 0; i != matched_chords_seq1.size (); ++i){
			auto chord1 = matched_chords_seq1[i], chord2 = matched_chords_seq2[i];
			if (get_matched_chord (chord1) != chord2)
				return false;
			auto bnd_eh1 = matched_ehs_seq1[i], bnd_eh2 = matched_ehs_seq2[i];
			//同时调整这两条chord上的边的序列，使之能够一致
			if (((chord1->ordered_ehs.front () == bnd_eh1) && (chord2->ordered_ehs.back () == bnd_eh2)) ||
				((chord1->ordered_ehs.back () == bnd_eh1) && (chord2->ordered_ehs.front () == bnd_eh2)))
				chord1->reverse ();
			else
				continue;
		}
	}

	//经过上面检查，边界上的chord序列是匹配的，下面检查内部的chord相交关系
	//首先定义一个函数，用于检查相交序列是否可以相匹配
	auto fCheckIntSeq = [&] (const std::vector<std::pair<DualChord*, OvmFaH> > &int_seq1, 
		const std::vector<std::pair<DualChord*, OvmFaH> > &int_seq2)->bool{
		//如果两个相交序列的长度不一样，则一定不能够匹配
		if (int_seq1.size () != int_seq2.size ())
			return false;
		for (int i = 0; i != int_seq1.size (); ++i){
			auto int_chord1 = int_seq1[i].first, int_chord2 = int_seq2[i].first;
			if ((int_chord1 == NULL && int_chord2 != NULL) || (int_chord1 != NULL && int_chord2 == NULL))
				return false;
			if (get_matched_chord (int_chord1) != int_chord2)
				return false;
			auto locate = matched_fhs_pairs.find (int_seq1[i].second);
			if (locate != matched_fhs_pairs.end ()){
				if (locate->second != int_seq2[i].second)
					return false;
			}
		}
		return true;
	};
	auto fUpdateMatchedFhsPairs = [&] (const std::vector<std::pair<DualChord*, OvmFaH> > &int_seq1, 
		const std::vector<std::pair<DualChord*, OvmFaH> > &int_seq2){
		for (int i = 0; i != int_seq1.size (); ++i){
			matched_fhs_pairs.insert (std::make_pair (int_seq1[i].second, int_seq2[i].second));
		}
	};

	//分两轮进行判断，首先进行open chord之间的匹配检测 
	foreach (auto p, matched_chord_pairs){
		auto chord1 = p.first;
		auto chord2 = p.second;
		//如果匹配的两条chord的类型不一样，则一定不能匹配
		if (chord1->is_closed != chord2->is_closed)
			return false;
		//这一轮先不处理封闭chord
		if (chord1->is_closed) continue;
		bool is_self_int1 = false, is_self_int2 = false;
		auto int_seq1 = get_matched_int_chord_seq (chord1, is_self_int1),
			int_seq2 = get_matched_int_chord_seq (chord2, is_self_int2);
		//判断两个chord是否都是自相交
		if (is_self_int1 != is_self_int2)
			return false;
		if (!fCheckIntSeq (int_seq1, int_seq2))
			return false;
		fUpdateMatchedFhsPairs (int_seq1, int_seq2);
	}
	//然后分析封闭chord
	foreach (auto p, matched_chord_pairs){
		auto chord1 = p.first;
		auto chord2 = p.second;
		if (!chord1->is_closed) continue;
		//首先调整chord上的边、面顺序，使得两个chord方向一致
		//首先在chord上找到matched_fhs_pairs中存在的面
		std::vector<OvmFaH> matched_fhs_on_chord1;

		auto first_matched_fh_on_chord1 = InvalidFaceHandle ();
		for (int i = 0; i != chord1->length (); ++i){
			if (JC::contains (matched_fhs_pairs, chord1->ordered_fhs.front ())){
				first_matched_fh_on_chord1 = chord1->ordered_fhs.front ();
				break;
			}else{
				chord1->cycle ();
			}
		}

		if (first_matched_fh_on_chord1 != InvalidFaceHandle ()){
			auto first_matched_fh_on_chord2 = matched_fhs_pairs[first_matched_fh_on_chord1];
			bool has_found_matched_fh = false;
			for (int i = 0; i != chord2->length (); ++i){
				if (chord2->ordered_fhs.front () == first_matched_fh_on_chord2){
					has_found_matched_fh = true;
					break;
				}else
					chord2->cycle ();
			}
			//如果找不到匹配的第一个四边形面，则这两个chord一定不能够匹配
			if (!has_found_matched_fh)
				return false;
		}

		//下面检查相交序列
		bool is_self_int1 = false, is_self_int2 = false;
		auto int_seq1 = get_matched_int_chord_seq (chord1, is_self_int1),
			int_seq2 = get_matched_int_chord_seq (chord2, is_self_int2);
		//判断两个chord是否都是自相交
		if (is_self_int1 != is_self_int2)
			return false;

		//首先以当前的chord2的边面顺序进行判断一次
		if (!fCheckIntSeq (int_seq1, int_seq2)){
			//如果不能够匹配，还需要将chord2的边面顺序反向一下，再比较一次才能够最终确定是否能够匹配
			chord2->cycle ();
			chord2->reverse ();
			int_seq2 = get_matched_int_chord_seq (chord2, is_self_int2);
			if (!fCheckIntSeq (int_seq1, int_seq2))
				return false;
		}
		fUpdateMatchedFhsPairs (int_seq1, int_seq2);
	}
	return true;
}

bool MeshMatchingHandler::save_matched_chords (QString file_path)
{
	QFile file (file_path);
	if (!file.open (QIODevice::WriteOnly)){
		warning (QObject::tr("打开文件错误！"));
		return false;
	}
	QDataStream out (&file);
	QVector<QPair<unsigned int, unsigned int> > converted_matched_chords_data;
	foreach (auto p, matched_chord_pairs){
		QPair<unsigned int, unsigned int> one_pair_data;
		one_pair_data.first = (unsigned int)(p.first);
		one_pair_data.second = (unsigned int)(p.second);

		converted_matched_chords_data.push_back (one_pair_data);
	}
	out<<converted_matched_chords_data;
	file.close ();

	return true;
}

//从文件中加载chord匹配关系
//匹配关系是有一对对chord序号组成，chord序号即为chord->idx的值
bool MeshMatchingHandler::load_matched_chords (QString file_path)
{
	QFile file (file_path);
	if (!file.open (QIODevice::ReadOnly)){
		warning (QObject::tr("打开文件错误！"));
		return false;
	}
	
	QDataStream in (&file);
	QVector<QPair<unsigned int, unsigned int> > converted_matched_chords_data;

	in>>converted_matched_chords_data;
	file.close ();

	ChordPairs loaded_chord_pairs;
	foreach (auto p, converted_matched_chords_data){
		if (mm_data1->chord_id_mapping.find (p.first) == mm_data1->chord_id_mapping.end ()){
			warning (QObject::tr("该chord序号在data1的chord_id_mapping中不存在！"));
			return false;
		}
		if (mm_data2->chord_id_mapping.find (p.second) == mm_data2->chord_id_mapping.end ()){
			warning (QObject::tr("该chord序号在data2的chord_id_mapping中不存在！"));
			return false;
		}
		DualChord *chord1 = mm_data1->chord_id_mapping[p.first], 
			*chord2 = mm_data2->chord_id_mapping[p.second];

		mm_data1->add_matched_chord (chord1);
		mm_data2->add_matched_chord (chord2);
		loaded_chord_pairs.insert (std::make_pair (chord1, chord2));
	}
	matched_chord_pairs.clear ();
	matched_chord_pairs = loaded_chord_pairs;
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
			perip_edges_buff.insert (eg);
		}
	}

	std::vector<ENTITY*> perip_edges_list;
	JC::set_to_vector (perip_edges_buff, perip_edges_list);
	
	VolumeMesh *mesh = mm_data->mesh;
	std::map<EDGE*, std::unordered_set<OvmEgH> > ehs_on_edges;
	if (!mesh->edge_property_exists<unsigned int> ("intedgeptr")){
		auto tmp = mesh->request_edge_property<unsigned int> ("intedgeptr", 0);
		mesh->set_persistent (tmp);
	}
	auto E_INT_EG_PTR = mesh->request_edge_property<unsigned int> ("intedgeptr");

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
				E_INT_EG_PTR[eh] = (unsigned int)on_which_eg;
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
	}
}

void MeshMatchingHandler::init_match ()
{
	//hoopsview1->derender_mesh_groups ("tmp", "ordered ehs");
	//foreach (auto p, mm_data1->ordered_ehs_on_edges){
	//	
	//	auto group = new VolumeMeshElementGroup (mesh1(), "tmp", "ordered ehs");
	//	group->ehs.insert (p.second.front ());

	//	MeshRenderOptions render_options;
	//	render_options.edge_color = "red";
	//	render_options.edge_width = 12;
	//	hoopsview1->render_mesh_group (group, render_options);

	//	group = new VolumeMeshElementGroup (mesh1(), "tmp", "ordered ehs");
	//	group->ehs.insert (p.second[1]);

	//	render_options.edge_color = "blue";
	//	hoopsview1->render_mesh_group (group, render_options);
	//}
	//hoopsview2->derender_mesh_groups ("tmp", "ordered ehs");
	//foreach (auto p, mm_data2->ordered_ehs_on_edges){
	//	
	//	auto group = new VolumeMeshElementGroup (mesh2(), "tmp", "ordered ehs");
	//	group->ehs.insert (p.second.front ());

	//	MeshRenderOptions render_options;
	//	render_options.edge_color = "red";
	//	render_options.edge_width = 12;
	//	hoopsview2->render_mesh_group (group, render_options);

	//	group = new VolumeMeshElementGroup (mesh2(), "tmp", "ordered ehs");
	//	group->ehs.insert (p.second[1]);

	//	render_options.edge_color = "blue";
	//	hoopsview2->render_mesh_group (group, render_options);
	//}
	//return;

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
	foreach (auto &chord1, mm_data1->unmatched_chords){
		foreach (auto &chord2, mm_data2->unmatched_chords){
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

	//QString msg;
	//DualChord *my_tmp_chord = NULL;
	//foreach (auto c, chord_set1){
	//	if (c->idx == 15){
	//		my_tmp_chord = c;
	//		break;
	//	}
	//}
	//if (my_tmp_chord == NULL) return;
	//foreach (auto &cpc, cpcs){
	//	if (cpc.chord1 == my_tmp_chord){
	//		msg += QString ("c%1: %2\n").arg (cpc.chord2->idx).arg (cpc.dist);
	//	}
	//}
	//QMessageBox::information (NULL, "init match", msg);

	//下面从cpcs中每次取出dist最小的candidate，加入到matched_pairs中
	//后续加入的需要将之前匹配的chord对考虑在内
	while (!cpcs.empty ()){
		auto candidiate = JC::pop_begin_element (cpcs);
		auto test_chord_pair = std::make_pair (candidiate.chord1, candidiate.chord2);
		matched_chord_pairs.insert (test_chord_pair);
		if (check_match ()){
			for (auto it = cpcs.begin (); it != cpcs.end ();){
				if (it->chord1 == candidiate.chord1 || it->chord2 == candidiate.chord2)
					it = cpcs.erase (it);
				else it++;
			}
			mm_data1->add_matched_chord (test_chord_pair.first);
			mm_data2->add_matched_chord (test_chord_pair.second);
		}else{
			matched_chord_pairs.erase (test_chord_pair);
		}
	}

	//adjust_matched_chords_directions ();
}

void MeshMatchingHandler::adjust_matched_chords_directions ()
{
	auto fAdjustChord = [&] (DualChord *chord){
		chord->ordered_ehs.erase (chord->ordered_ehs.begin ());
		chord->ordered_ehs.push_back (chord->ordered_ehs.front ());
		chord->ordered_fhs.push_back (chord->ordered_fhs.front ());
		chord->ordered_fhs.erase (chord->ordered_fhs.begin ());
	};
	foreach (auto p, matched_chord_pairs){
		auto chord1 = p.first;
		auto chord2 = p.second;
		auto cpd1 = get_chord_pos_desc (chord1),
			cpd2 = get_chord_pos_desc (chord2);

		if (chord1->is_closed){
			bool has_adjusted = false;
			for (int i = 0; i != chord2->ordered_ehs.size (); ++i){
				if (!can_two_int_graphs_match (chord1, cpd1.inter_graph, chord2, cpd2.inter_graph)){
					fAdjustChord (chord2);
					get_inter_graph (cpd2, chord2);
				}else{
					has_adjusted = true;
					break;
				}
			}
			if (!has_adjusted){
				std::reverse (chord2->ordered_ehs.begin () + 1, chord2->ordered_ehs.end () - 1);
				std::reverse (chord2->ordered_fhs.begin (), chord2->ordered_fhs.end ());
				get_inter_graph (cpd2, chord2);
				for (int i = 0; i != chord2->ordered_ehs.size (); ++i){
					if (!can_two_int_graphs_match (chord1, cpd1.inter_graph, chord2, cpd2.inter_graph)){
						fAdjustChord (chord2);
						get_inter_graph (cpd2, chord2);
					}else{
						has_adjusted = true;
						break;
					}
				}
			}//end if (!has_adjusted){...
			if (!has_adjusted){
				QMessageBox::warning (NULL, "ERROR", QString ("Cannot be adjusted!c%1 <--> c%2").arg (chord1->idx).arg (chord2->idx));
			}
		}//end if (!p.first->is_closed){...
	}
}

std::vector<std::pair<DualChord*,OvmFaH> > MeshMatchingHandler::get_matched_int_chord_seq (DualChord *chord, bool &self_int)
{
	std::vector<std::pair<DualChord*,OvmFaH> > int_seq;

	foreach (auto fh, chord->ordered_fhs){
		auto int_chord = get_int_chord_at_this_fh (chord, fh);
		//如果int_chord为NULL，说明chord为自相交chord
		if (int_chord == NULL){
			self_int = true;
			int_seq.push_back (std::make_pair (int_chord, fh));
		}else if (has_been_matched (int_chord)){
			int_seq.push_back (std::make_pair (int_chord, fh));
		}
	}
	return int_seq;
}

std::pair<std::set<OvmVeH>, std::set<OvmVeH> > MeshMatchingHandler::get_candidate_end_geom_vertices (ChordPosDesc &translated_cpd)
{
	auto mesh = translated_cpd.mesh;
	std::pair<std::set<OvmVeH>, std::set<OvmVeH> > candidate_vertices;
	auto mm_data = get_mesh_matching_data (mesh);
	auto ordered_ehs_on_start_geom_eg = mm_data->ordered_ehs_on_edges[translated_cpd.start_edge],
		ordered_ehs_on_end_geom_eg = mm_data->ordered_ehs_on_edges[translated_cpd.end_edge];

	auto E_CHORD_PTR = mesh->request_edge_property<unsigned int> ("chordptr");

	auto fGetCandidateVertices = [&] (std::vector<OvmEgH> &ordered_ehs_on_geom_eg, int interval_idx_want)->std::set<OvmVeH>{
		std::set<OvmVeH> candi_vers;
		std::vector<OvmHaEgH> directed_hehs;
		JC::get_piecewise_halfedges_from_edges (mesh, ordered_ehs_on_geom_eg, true, directed_hehs);
		if (interval_idx_want == 0)
			candi_vers.insert (mesh->halfedge (directed_hehs.front ()).from_vertex ());
		int interval_idx = 0;

		foreach (auto heh, directed_hehs){
			auto eh = mesh->edge_handle (heh);
			DualChord *eh_chord = (DualChord*)(E_CHORD_PTR[eh]);
			if (has_been_matched (eh_chord))
				interval_idx++;
			if (interval_idx == interval_idx_want)
				candi_vers.insert (mesh->halfedge (heh).to_vertex ());
			if (interval_idx > interval_idx_want)
				break;
		}
		return candi_vers;
	};

	candidate_vertices.first = fGetCandidateVertices (ordered_ehs_on_start_geom_eg, translated_cpd.start_idx);
	candidate_vertices.second = fGetCandidateVertices (ordered_ehs_on_end_geom_eg, translated_cpd.end_idx);
	return candidate_vertices;
}

std::vector<std::set<OvmEgH> > MeshMatchingHandler::get_candidate_interval_ehs (ChordPosDesc &translated_cpd)
{
	std::vector<std::set<OvmEgH> > candidate_interval_ehs;
	for (int i = 0; i != translated_cpd.inter_graph.size (); ++i){
		auto int_chord = translated_cpd.inter_graph[i];
		std::set<OvmEgH> cur_candi_int_ehs;
		//如果int_chord是NULL，则chord为自相交chord，此处该相交chord上的可选边为空
		if (int_chord != NULL){
			auto interval_idx_want = translated_cpd.inter_graph_indices[i];
			int cur_interval_idx = 0;

			if (interval_idx_want == 0)
				cur_candi_int_ehs.insert (int_chord->ordered_ehs.front ());

			for (int i = 1; i != int_chord->ordered_ehs.size (); ++i){
				auto fh_on_int_chord = int_chord->ordered_fhs[i - 1];
				auto int_chord_on_int_chord = get_int_chord_at_this_fh (int_chord, fh_on_int_chord);
				if (has_been_matched (int_chord_on_int_chord))
					cur_interval_idx++;
				if (cur_interval_idx == interval_idx_want)
					cur_candi_int_ehs.insert (int_chord->ordered_ehs[i]);
				if (cur_interval_idx > interval_idx_want)
					break;
			}
		}

		candidate_interval_ehs.push_back (cur_candi_int_ehs);
	}

	return candidate_interval_ehs;
}

CandiPolyline MeshMatchingHandler::get_best_polyline (VolumeMesh *mesh, OvmVeH start_vh, OvmVeH end_vh,
	std::vector<std::set<OvmEgH> > candi_interval_ehs, std::unordered_set<OvmEgH> rest_ehs)
{
	CandiPolyline candi_polyline;
	candi_polyline.start_vh = start_vh; candi_polyline.end_vh = end_vh;
	candi_polyline.dist = 0;
	while (!candi_interval_ehs.empty ()){
		auto cur_candi_interval_ehs = JC::pop_begin_element (candi_interval_ehs);
		//如果cur_candi_interval_ehs为空，则表示是自相交处的占位单元，直接略过即可 
		if (cur_candi_interval_ehs.empty ()) continue;

		//获得cur_candi_interval_ehs上面所有的网格点作为搜索的目标点集合
		std::unordered_set<OvmVeH> next_candi_vhs;
		std::map<OvmVeH, OvmEgH> vh_eh_mapping;
		foreach (auto eh, cur_candi_interval_ehs){
			auto vh1 = mesh->edge (eh).from_vertex (), vh2 = mesh->edge (eh).to_vertex ();
			next_candi_vhs.insert (vh1);
			next_candi_vhs.insert (vh2);
			vh_eh_mapping.insert (std::make_pair (vh1, eh));
			vh_eh_mapping.insert (std::make_pair (vh2, eh));
		}
		//如果start_vh已经包含在next_candi_vhs中时，直接从vh_eh_mapping取出那条边即可
		if (JC::contains (next_candi_vhs, start_vh)){
			auto adj_eh = vh_eh_mapping[start_vh];
			candi_polyline.polyline.push_back (adj_eh);
			start_vh = mesh->edge (adj_eh).to_vertex () == start_vh? mesh->edge (adj_eh).from_vertex ():
				mesh->edge (adj_eh).to_vertex ();
		}
		//否则就要用Dijkstra路径搜索找出最优路径
		else{
			MeshDijkstra md (mesh, start_vh, &next_candi_vhs);
			md.search_boundary = true; md.search_inner = false;
			md.searchable_ehs = rest_ehs;
			md.min_valence = 0;
			md.consider_topology_only = true;
			std::vector<OvmEgH> best_path;
			OvmVeH closest_vh;
			auto cur_dist = md.shortest_path (best_path, closest_vh);
			auto closest_eh = vh_eh_mapping[closest_vh];
			foreach (auto eh, best_path)
				candi_polyline.polyline.push_back (eh);
			candi_polyline.polyline.push_back (closest_eh);

			start_vh = mesh->edge (closest_eh).to_vertex () == closest_vh? mesh->edge (closest_eh).from_vertex ():
				mesh->edge (closest_eh).to_vertex ();
		}
	}
	return candi_polyline;
}

std::pair<std::pair<OvmVeH, OvmVeH>, std::vector<OvmEgH> > MeshMatchingHandler::get_polyline_for_chord_inflation (VolumeMesh *mesh, std::pair<std::set<OvmVeH>, 
	std::set<OvmVeH> > vhs_on_geom_egs,
	const std::vector<std::set<OvmEgH> > &candi_interval_ehs)
{
	auto mm_data = get_mesh_matching_data (mesh);
	auto ehs_on_interfaces = mm_data->ehs_on_interface;
	std::vector<OvmEgH> polyline;
	auto start_geom_vhs = vhs_on_geom_egs.first;
	auto end_geom_vhs = vhs_on_geom_egs.second;
	//获得除了ehs_on_interfaces中除了candi_interval_ehs以及集合边上的网格边剩余的网格边
	auto rest_ehs = ehs_on_interfaces;
	//去掉贴合面外围几何边
	foreach (auto eg, bound_geom_edges){
		auto ehs = mm_data->ordered_ehs_on_edges[eg];
		foreach (auto eh, ehs)
			rest_ehs.erase (eh);
	}

	std::vector<OvmEgH> best_polyline;
	std::set<CandiPolyline> all_candi_polylines;
	foreach (auto start_vh, start_geom_vhs){
		//foreach (auto end_vh, end_geom_vhs){
		//	if (end_vh == start_vh) continue;
		//	auto candi_ehs = get_best_polyline (mesh, start_vh, end_vh, candi_interval_ehs, rest_ehs);
		//	all_candi_polylines.insert (candi_ehs);
		//}
		auto end_geom_vhs_unordered_set = JC::get_unordered_set (end_geom_vhs);
		ChordPolylineDijkstra cpd (mesh, start_vh, &end_geom_vhs_unordered_set, &candi_interval_ehs, &rest_ehs, 
			get_mesh_matching_data (mesh)->matched_chords);
		CandiPolyline one_candi_polyline;
		one_candi_polyline.dist = cpd.shortest_path (one_candi_polyline.polyline, one_candi_polyline.end_vh);
		all_candi_polylines.insert (one_candi_polyline);
	}
	std::pair<std::pair<OvmVeH, OvmVeH>, std::vector<OvmEgH> > best_result;
	best_result.first.first = (*(all_candi_polylines.begin ())).start_vh;
	best_result.first.second = (*(all_candi_polylines.begin ())).end_vh;
	best_result.second = (*(all_candi_polylines.begin ())).polyline;
	return best_result;
}

bool MeshMatchingHandler::can_two_chords_match (DualChord *chord1, DualChord *chord2)
{
	//两个chord的类型必须一致
	if (chord1->is_closed != chord2->is_closed)
		return false;

	//获得chord位置描述子
	auto cpd1 = get_chord_pos_desc (chord1),
		cpd2 = get_chord_pos_desc (chord2);

	//如果相交序列长度不一样，那肯定不能匹配
	if (cpd1.inter_graph.size () != cpd2.inter_graph.size ())
		return false;

	int graph_size = cpd1.inter_graph.size ();

	//下面逐个判断相交序列中的chord是否匹配
	if (chord1->is_closed){
		if (graph_size == 0) return true;
		bool same_circle = false;
		for (int i = 0; i != graph_size; ++i){
			if (can_two_int_graphs_match (chord1, cpd1.inter_graph, chord2, cpd2.inter_graph)){
				same_circle = true;
				break;
			}
			auto front_chord2 = JC::pop_begin_element (cpd2.inter_graph);
			cpd2.inter_graph.push_back (front_chord2);
		}
		if (!same_circle){
			std::reverse (cpd2.inter_graph.begin (), cpd2.inter_graph.end ());
			for (int i = 0; i != graph_size; ++i){
				if (can_two_int_graphs_match (chord1, cpd1.inter_graph, chord2, cpd2.inter_graph)){
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
		
		if (!can_two_int_graphs_match (chord1, cpd1.inter_graph, chord2, cpd2.inter_graph))
			return false;
	}

	//下面还需要考虑两个chord的polyline围成的区域内部是否包含内环
	//暂时这部分未实现
	//...

	return true;
}

ChordPosDesc MeshMatchingHandler::get_chord_pos_desc (DualChord *chord)
{
	ChordPosDesc cpd;
	cpd.chord = chord;
	cpd.mesh = chord->mesh;
	VolumeMesh *mesh = chord->mesh;
	get_inter_graph (cpd, chord);
	if (!chord->is_closed){
		auto E_INT_EG_PTR = get_E_INT_EG_PTR (get_mesh_matching_data (mesh));
		auto start_eg = (EDGE*)(E_INT_EG_PTR[chord->ordered_ehs.front ()]);
		auto end_eg = (EDGE*)(E_INT_EG_PTR[chord->ordered_ehs.back ()]);
		auto geom_indices = get_matched_index_on_geom_edge (chord, start_eg, end_eg);
		cpd.start_edge = start_eg;
		cpd.end_edge = end_eg;
		cpd.start_idx = geom_indices.first; cpd.end_idx = geom_indices.second;
	}
	return cpd;
}

void MeshMatchingHandler::get_inter_graph (ChordPosDesc &cpd, DualChord *chord)
{
	VolumeMesh *mesh = chord->mesh;
	cpd.inter_graph.clear ();
	cpd.inter_graph_indices.clear ();

	auto ordered_fhs = chord->ordered_fhs;
	foreach (auto &fh, ordered_fhs){
		auto int_chord = get_int_chord_at_this_fh (chord, fh);
		//如果int_chord为NULL，则说明该chord在该fh上自相交
		if (int_chord == NULL){
			cpd.inter_graph.push_back (NULL);
			cpd.self_intersecting = true;
			cpd.inter_graph_indices.push_back (-1);
		}else{
			//如果相交的chord当前并不在已匹配chord列表中时，则忽略
			if (!has_been_matched (int_chord))
				continue;
			cpd.inter_graph.push_back (int_chord);
			//下面要求得在int_chord上，当前chord和它相交于第几段
			int int_idx = 0;
			foreach (auto fh_on_int_chord, int_chord->ordered_fhs){
				if (fh_on_int_chord == fh)
					break;
				auto int_chord_on_int_chord = get_int_chord_at_this_fh (int_chord, fh_on_int_chord);

				if (int_chord_on_int_chord == NULL || has_been_matched (int_chord_on_int_chord))
					int_idx++;
			}
			cpd.inter_graph_indices.push_back (int_idx);
		}
	}
}

DualChord * MeshMatchingHandler::get_int_chord_at_this_fh (DualChord *chord, OvmFaH fh)
{
	auto mesh = chord->mesh;
	auto E_CHORD_PTR = mesh->request_edge_property<unsigned int> ("chordptr");

	auto heh_vec = mesh->face (fh).halfedges ();
	std::set<DualChord*> chords;
	foreach (auto &heh, heh_vec){
		auto eh = mesh->edge_handle (heh);
		DualChord *tmp_chord = (DualChord*)(E_CHORD_PTR[eh]);
		assert (tmp_chord);
		chords.insert (tmp_chord);
	}
	assert (chords.size () <= 2);
	//如果chords的数量为1，则说明该chord在该fh上自相交
	if (chords.size () == 1){
		return NULL;
	}else{
		chords.erase (chord);
		auto int_chord = *(chords.begin ());
		return int_chord;
	}
}

std::pair<int, int> MeshMatchingHandler::get_matched_index_on_geom_edge (DualChord *chord, EDGE *start_eg, EDGE *end_eg)
{
	VolumeMesh *mesh = chord->mesh;
	MMData *mm_data = get_mesh_matching_data (mesh);
	std::pair<int, int> matched_geom_indices;
	
	auto E_CHORD_PTR = mesh->request_edge_property<unsigned int> ("chordptr");
	auto fGetGeomEdgeMatchedIndex = [&] (EDGE *eg, OvmEgH eh_on_chord)->int{
		std::vector<OvmEgH> &all_ordered_ehs = mm_data->ordered_ehs_on_edges[eg];
		int idx = 0;
		foreach (auto &eh, all_ordered_ehs){
			if (eh == eh_on_chord) break;
			DualChord *chord_on_eh = (DualChord*)(E_CHORD_PTR[eh]);
			if (has_been_matched (chord_on_eh)){
				idx++;
			}
		}
		return idx;
	};

	matched_geom_indices.first = fGetGeomEdgeMatchedIndex (start_eg, chord->ordered_ehs.front ());
	matched_geom_indices.second = fGetGeomEdgeMatchedIndex (end_eg, chord->ordered_ehs.back ());
	return matched_geom_indices;
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

ChordPosDesc MeshMatchingHandler::translate_chord_pos_desc (ChordPosDesc &in_cpd)
{
	ChordPosDesc out_cpd;
	VolumeMesh *in_mesh = in_cpd.chord->mesh;
	out_cpd.mesh = oppo_mesh (in_mesh);

	out_cpd.chord = NULL;
	out_cpd.start_edge = in_cpd.start_edge; out_cpd.end_edge = in_cpd.end_edge;
	out_cpd.start_idx = in_cpd.start_idx; out_cpd.end_idx = in_cpd.end_idx;

	auto fTranslate = [&] (std::vector<DualChord*> &graph)->std::vector<DualChord*>{
		std::vector<DualChord*> translated_graph;
		foreach (auto &chord, graph){
			if (chord == in_cpd.chord){
				translated_graph.push_back (NULL);
			}else if (chord == NULL){
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
	out_cpd.inter_graph_indices = in_cpd.inter_graph_indices;
	out_cpd.self_intersecting = in_cpd.self_intersecting;
	return out_cpd;
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
//	auto E_CHORD_PTR = other_mesh->request_edge_property<unsigned int> ("chordptr");
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
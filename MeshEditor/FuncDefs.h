#ifndef _FUNC_DEFS_H_
#define _FUNC_DEFS_H_

#include <string>
#include <fstream>
#include <iomanip>
#include <map>
#include <set>
#include <unordered_set>
#include <vector>
#include <list>
#include <algorithm>
#include <hash_map>
#include <queue>

#include <QDomDocument>
#include <QDomElement>
#include <QSet>
#include <QHash>
#include <QMap>

#include "acis_headers.h"
#include "MeshDefs.h"
#include "DualDefs.h"

class HoopsView;
namespace JC{
	template <typename T>
	void entity_list_to_vector (ENTITY_LIST &entity_list, std::vector<T> &vec)
	{
		T tmp;
		for (int i = 0; i != entity_list.count (); ++i)
		{
			tmp = static_cast<T> (entity_list[i]);
			vec.push_back (tmp);
		}
	}

	template <typename T>
	void entity_list_to_set (ENTITY_LIST &entity_list, std::set<T> &vec)
	{
		T tmp;
		for (int i = 0; i != entity_list.count (); ++i)
		{
			tmp = static_cast<T> (entity_list[i]);
			vec.insert (tmp);
		}
	}

	template <typename T>
	void entity_list_to_unordered_set (ENTITY_LIST &entity_list, std::unordered_set<T> &vec)
	{
		T tmp;
		for (int i = 0; i != entity_list.count (); ++i)
		{
			tmp = static_cast<T> (entity_list[i]);
			vec.insert (tmp);
		}
	}

	template <typename T>
	void init_matrix (std::vector<std::vector<T> > &m, int row, int column, T val)
	{
		m.resize (row);
		for (int i = 0; i != row; ++i)
			m[i].resize (column, val);
	}

	template <typename T>
	void set_to_vector (const std::set<T> &s, std::vector<T> &v)
	{
		for (auto it = s.begin (); it != s.end (); ++it)
			v.push_back (*it);
	}

	template <typename T>
	void unordered_set_to_vector (const std::unordered_set<T> &s, std::vector<T> &v)
	{
		for (auto it = s.begin (); it != s.end (); ++it)
			v.push_back (*it);
	}

	template <typename T>
	void vector_to_set (const std::vector<T> &v, std::set<T> &s)
	{
		for (auto it = v.begin (); it != v.end (); ++it)
			s.insert (*it);
	}

	template <typename T>
	void vector_to_unordered_set (const std::vector<T> &v, std::unordered_set<T> &s)
	{
		for (auto it = v.begin (); it != v.end (); ++it)
			s.insert (*it);
	}

	template <typename Container, typename T>
	bool contains (Container &container, T &val)
	{
		if (std::find (container.begin (), container.end (), val) != container.end ())
			return true;
		else
			return false;
	}
	template <typename Container, typename T>
	bool contains (Container *container, T &val)
	{
		if (std::find (container->begin (), container->end (), val) != container->end ())
			return true;
		else
			return false;
	}

	template <typename T>
	bool contains (std::set<T> &large_container, std::set<T> &small_container)
	{
		for (auto it = small_container.begin (); it != small_container.end (); ++it){
			if (!contains (large_container, *it)) return false;
		}
		return true;
	}

	template <typename T>
	bool intersects (const std::set<T> &set1, const std::set<T> &set2)
	{
		std::vector<T> comm;
		std::set_intersection (set1.begin (), set1.end (), set2.begin (), set2.end (),
			std::back_inserter (comm));
		return !comm.empty ();
	}

	template <typename T>
	bool intersects (const std::unordered_set<T> &set1, const std::unordered_set<T> &set2)
	{
		foreach (auto &x, set1){
			foreach (auto &y, set2){
				if (x == y) return true;
			}
		}
		return false;
	}

	template <typename T>
	bool equals (const std::unordered_set<T> &set1, const std::unordered_set<T> &set2)
	{
		if (set1.size () != set2.size ()) return false;

		foreach (auto &x, set1){
			if (set2.find (x) == set2.end ())
				return false;
		}
		return true;
	}

	template <typename T>
	bool intersects (const std::vector<T> &vec1, const std::vector<T> &vec2)
	{
		std::vector<T> comm;
		std::set_intersection (vec1.begin (), vec1.end (), vec2.begin (), vec2.end (),
			std::back_inserter (comm));
		return !comm.empty ();
	}

	template <typename T>
	T pop_begin_element (std::set<T> &myset)
	{
		T res = *(myset.begin ());
		myset.erase (myset.begin ());
		return res;
	}

	template <typename T>
	T pop_begin_element (std::vector<T> &myset)
	{
		T res = *(myset.begin ());
		myset.erase (myset.begin ());
		return res;
	}

	template <typename T>
	T pop_begin_element (std::unordered_set<T> &myset)
	{
		T res = *(myset.begin ());
		myset.erase (myset.begin ());
		return res;
	}

	template <typename T>
	T pop_begin_element (QSet<T> &myset)
	{
		T res = *(myset.begin ());
		myset.erase (myset.begin ());
		return res;
	}

	template<typename KeyT, typename ValT>
	ValT find_key_from_value (std::hash_map<KeyT, ValT> &mapping, ValT &val)
	{
		ValT result;
		foreach (auto &p, mapping){
			if (p.second == val){
				result = p.first;
				break;
			}
		}
		return result;
	}

	template<typename KeyT, typename ValT>
	ValT find_key_from_value (std::map<KeyT, ValT> &mapping, ValT &val)
	{
		ValT result;
		foreach (auto &p, mapping){
			if (p.second == val){
				result = p.first;
				break;
			}
		}
		return result;
	}

	bool parse_xml_file (QString xml_path, QString &file_type, QString &data_name, 
		std::vector<std::pair<QString, QString> > &path_pairs);
	VolumeMesh* load_volume_mesh (QString mesh_path);
	VolumeMesh * form_new_mesh (VolumeMesh *mesh, std::unordered_set<OvmCeH> &chs);
	void init_volume_mesh (VolumeMesh *mesh, BODY *body, double myresabs, std::unordered_set<OvmVeH> *left_vhs = NULL);
	bool save_volume_mesh (VolumeMesh *mesh, QString mesh_path);
	BODY* load_acis_model (QString file_path);
	void save_acis_entity(ENTITY *entity, const char * file_name);

	void render_volume_mesh (VolumeMesh *mesh);
	void render_hexa_set (VolumeMesh *mesh, std::unordered_set<OvmCeH> &chs);
	void render_volume_mesh_boundary (VolumeMesh *mesh);
	void render_mesh_group (VolumeMeshElementGroup *group);
	void save_mesh_element_group (VolumeMeshElementGroup *group, QString group_name);
	void save_mesh_element_group (VolumeMeshElementGroup *group, const char *file_path);
	VolumeMeshElementGroup * load_mesh_element_group (QString group_name);
	VolumeMeshElementGroup * load_mesh_element_group (const char *file_path);

	//////////////////////////////////////////////////////////////////////////
	//VolumeMesh toplogy functions
	std::vector<OvmFaH> get_adj_faces_around_edge (VolumeMesh *mesh, OvmEgH eh, bool on_boundary = false);
	std::vector<OvmFaH> get_adj_faces_around_edge (VolumeMesh *mesh, OvmHaEgH heh, bool on_boundary = false);
	void get_adj_faces_around_edge (VolumeMesh *mesh, OvmEgH eh, std::unordered_set<OvmFaH> &faces, bool on_boundary = false);
	void get_adj_faces_around_edge (VolumeMesh *mesh, OvmHaEgH heh, std::unordered_set<OvmFaH> &faces, bool on_boundary = false);

	void get_adj_vertices_around_vertex (VolumeMesh *mesh, OvmVeH vh, std::unordered_set<OvmVeH> &vertices);
	void get_adj_vertices_around_face (VolumeMesh *mesh, OvmFaH fh, std::vector<OvmVeH> &vertices);
	void get_adj_vertices_around_face (VolumeMesh *mesh, OvmFaH fh, std::unordered_set<OvmVeH> &vertices);
	std::vector<OvmVeH> get_adj_vertices_around_face (VolumeMesh *mesh, OvmFaH fh);
	void get_adj_vertices_around_cell (VolumeMesh *mesh, OvmCeH ch, std::unordered_set<OvmVeH> &vertices);
	std::vector<OvmVeH> get_adj_vertices_around_cell (VolumeMesh *mesh, OvmCeH ch);
	std::vector<OvmVeH> get_adj_vertices_around_hexa (VolumeMesh *mesh, OvmCeH ch);
	void get_adj_boundary_vertices_around_vertex (VolumeMesh *mesh, OvmVeH vh, std::unordered_set<OvmVeH> &vertices);
	void get_adj_hexas_around_edge (VolumeMesh *mesh, OvmHaEgH heh, std::unordered_set<OvmCeH> &hexas);
	void get_adj_hexas_around_edge (VolumeMesh *mesh, OvmEgH eh, std::unordered_set<OvmCeH> &hexas);
	void get_adj_hexas_around_hexa (VolumeMesh *mesh, OvmCeH ch, std::unordered_set<OvmCeH> &hexas);
	void get_adj_hexas_around_vertex (VolumeMesh *mesh, OvmVeH vh, std::unordered_set<OvmCeH> &hexas);
	void get_adj_hexas_incident_face (VolumeMesh *mesh, OvmFaH fh, std::unordered_set<OvmCeH> &hexas);
	void get_adj_faces_around_vertex (VolumeMesh *mesh, OvmVeH vh, std::unordered_set<OvmFaH> &faces);
	void get_adj_faces_around_face_edge (VolumeMesh *mesh, OvmFaH fh, std::unordered_set<OvmFaH> &faces);
	void get_adj_faces_around_hexa (VolumeMesh *mesh, OvmCeH ch, std::unordered_set<OvmFaH> &faces);
	void get_adj_edges_around_vertex (VolumeMesh *mesh, OvmVeH vh, std::unordered_set<OvmEgH> &edges);
	void get_adj_edges_around_face (VolumeMesh *mesh, OvmFaH fh, std::unordered_set<OvmEgH> &edges);
	void get_adj_edges_around_cell (VolumeMesh *mesh, OvmCeH ch, std::unordered_set<OvmEgH> &edges);
	void get_adj_boundary_faces_around_vertex (VolumeMesh *mesh, OvmVeH vh, std::unordered_set<OvmFaH> &faces);
	void get_adj_boundary_faces_around_edge (VolumeMesh *mesh, OvmEgH eh, std::unordered_set<OvmFaH> &faces);
	void get_adj_boundary_faces_around_face (VolumeMesh *mesh, OvmFaH fh, std::unordered_set<OvmFaH> &faces);
	void get_adj_boundary_edges_around_vertex (VolumeMesh *mesh, OvmVeH vh, std::unordered_set<OvmEgH> &edges);

	void get_direct_adjacent_hexas (VolumeMesh *mesh, const std::unordered_set<OvmFaH> &patch, std::unordered_set<OvmCeH> &hexas);
	void collect_boundary_element (VolumeMesh *mesh, std::set<OvmCeH> &chs, 
		std::set<OvmVeH> *bound_vhs, std::set<OvmEgH> *bound_ehs, std::set<OvmHaFaH> *bound_hfhs);
	void collect_boundary_element (VolumeMesh *mesh, std::set<OvmCeH> &chs, 
		std::set<OvmVeH> *bound_vhs, std::set<OvmEgH> *bound_ehs, std::set<OvmFaH> *bound_fhs);
	void collect_boundary_element (VolumeMesh *mesh, std::unordered_set<OvmCeH> &chs, 
		std::unordered_set<OvmVeH> *bound_vhs, std::unordered_set<OvmEgH> *bound_ehs, std::unordered_set<OvmHaFaH> *bound_hfhs);
	void collect_boundary_element (VolumeMesh *mesh, std::unordered_set<OvmCeH> &chs, 
		std::unordered_set<OvmVeH> *bound_vhs, std::unordered_set<OvmEgH> *bound_ehs, std::unordered_set<OvmFaH> *bound_fhs);
	void collect_boundary_element (VolumeMesh *mesh, std::unordered_set<OvmFaH> &fhs, 
		std::unordered_set<OvmVeH> *bound_vhs, std::unordered_set<OvmEgH> *bound_ehs);

	bool is_adjacent (VolumeMesh *mesh, OvmEgH eh1, OvmEgH eh2);
	bool is_adjacent (VolumeMesh *mesh, OvmCeH ch1, OvmCeH ch2);
	bool is_adjacent (VolumeMesh *mesh, OvmCeH ch, std::unordered_set<OvmCeH> &chs);

	bool is_manifold (VolumeMesh *mesh, std::unordered_set<OvmFaH> &fhs);
	bool is_manifold (VolumeMesh *mesh, std::unordered_set<OvmCeH> &chs);


	//判断一个六面体集合是否连通
	bool is_hexa_set_connected (VolumeMesh *mesh, const std::unordered_set<OvmCeH> &chs);

	void get_ccw_boundary_edges_faces_around_vertex (VolumeMesh *mesh, OvmVeH vh,
		std::vector<OvmEgH> &ehs, std::vector<OvmFaH> &fhs);
	void get_ccw_boundary_edges_faces_around_vertex (VolumeMesh *mesh, OvmVeH vh,
		std::unordered_set<OvmEgH> &key_ehs, std::vector<OvmEgH> &ehs, std::vector<std::unordered_set<OvmFaH> > &fhs_sets);
	void get_ccw_boundary_edges_faces_around_vertex (VolumeMesh *mesh, OvmVeH vh,
		std::vector<OvmEgH> &key_ehs, std::vector<OvmEgH> &ehs, std::vector<std::unordered_set<OvmFaH> > &fhs_sets);

	void get_cw_boundary_edges_faces_around_vertex (VolumeMesh *mesh, OvmVeH vh,
		std::vector<OvmEgH> &ehs, std::vector<OvmFaH> &fhs);
	void get_cw_boundary_edges_faces_around_vertex (VolumeMesh *mesh, OvmVeH vh, 
		std::unordered_set<OvmEgH> &key_ehs, std::vector<OvmEgH> &ehs, std::vector<std::unordered_set<OvmFaH> > &fhs_sets);
	void get_cw_boundary_edges_faces_around_vertex (VolumeMesh *mesh, OvmVeH vh, 
		std::vector<OvmEgH> &key_ehs, std::vector<OvmEgH> &ehs, std::vector<std::unordered_set<OvmFaH> > &fhs_sets);

	OvmVeH get_other_vertex_on_edge (VolumeMesh *mesh, OvmEgH eh, OvmVeH vh);
	OvmEgH get_opposite_edge_on_face (VolumeMesh *mesh, OvmFaH fh, OvmEgH eh);
	void get_boundary_elements_of_faces_patch (VolumeMesh *mesh, std::unordered_set<OvmFaH> &fhs, 
		std::unordered_set<OvmVeH> &bound_vhs, std::unordered_set<OvmEgH> &bound_ehs);

	//////////////////////////////////////////////////////////////////////////
	//topology calculation functions
	int edge_valence_change (VolumeMesh *mesh, OvmEgH eh, OvmFaH fh);
	int edge_valence_change (VolumeMesh *mesh, OvmEgH eh, OvmFaH fh1, OvmFaH fh2);
	int edge_valence_change (VolumeMesh *mesh, OvmEgH eh);
	int calc_manifold_edge_marks (VolumeMesh *mesh, std::unordered_set<OvmFaH> &fhs);

	OvmFaH get_common_face_handle (VolumeMesh *mesh, OvmCeH &ch1, OvmCeH &ch2);
	OvmVeH get_common_vertex_handle (VolumeMesh *mesh, OvmEgH eh1, OvmEgH eh2);
	OvmEgH get_common_edge_handle (VolumeMesh *mesh, OvmFaH fh1, OvmFaH fh2);

	//从一组首尾相连的网格边中得到一个方向的半边
	void get_piecewise_halfedges_from_edges (VolumeMesh *mesh, std::vector<OvmEgH> &ehs, bool forward,
		std::vector<OvmHaEgH> &hehs);
	//从无序的一个边集合中，整理出一个方向的半边
	void get_piecewise_halfedges_from_edges (VolumeMesh *mesh, std::unordered_set<OvmEgH> &ehs, OvmHaEgH start_heh, bool forward,
		std::vector<OvmHaEgH> &hehs);

	void get_separate_fhs_patches (VolumeMesh *mesh, std::unordered_set<OvmFaH> all_fhs, 
		std::unordered_set<OvmEgH> &seprate_ehs, std::vector<std::unordered_set<OvmFaH> > &fhs_patches);
	void get_separate_chs_sets (VolumeMesh *mesh, std::unordered_set<OvmFaH> *sep_fhs,
		std::vector<std::unordered_set<OvmCeH> > *sep_chs_sets);
	void get_separate_chs_sets (VolumeMesh *mesh, std::unordered_set<OvmCeH> *all_chs,
		std::unordered_set<OvmFaH> *sep_fhs, std::vector<std::unordered_set<OvmCeH> > *sep_chs_sets);
	//获得单个几何面上的网格面
	void get_fhs_on_acis_face (VolumeMesh *mesh, FACE *acis_face, std::unordered_set<OvmFaH> &fhs);
	//获得多个几何面上的网格面
	void get_fhs_on_acis_faces (VolumeMesh *mesh, std::set<FACE *> acis_face, std::unordered_set<OvmFaH> &fhs);
	std::unordered_set<OvmFaH> get_fhs_on_acis_faces (VolumeMesh *mesh, std::set<FACE *> acis_face);

	FACE * get_associated_geometry_face_of_boundary_fh (VolumeMesh *mesh, OvmFaH fh);
	EDGE * get_associated_geometry_edge_of_boundary_eh (VolumeMesh *mesh, OvmEgH eh, OpenVolumeMesh::VertexPropertyT<unsigned long> &V_ENTITY_PTR);
	EDGE * get_associated_geometry_edge_of_boundary_eh (VolumeMesh *mesh, OvmEgH eh);

	void optimize_shrink_set (VolumeMesh *mesh, std::unordered_set<OvmCeH> &hexa_set, 
		std::unordered_set<OvmFaH> &allowed_boundary_fhs, std::unordered_set<OvmCeH> &barrier_set, std::unordered_set<OvmEgH> &allowed_ehs);
	void optimize_shrink_set (VolumeMesh *mesh, std::unordered_set<OvmCeH> &hexa_set, 
		std::unordered_set<OvmFaH> &allowed_boundary_fhs);
	void optimize_shrink_set (VolumeMesh *mesh, std::unordered_set<OvmCeH> &hexa_set);
	void optimize_shrink_set_test (VolumeMesh *mesh, std::unordered_set<OvmCeH> &hexa_set, std::unordered_set<OvmCeH> &barrier_set);
	void optimize_shrink_set_fast (VolumeMesh *mesh, std::unordered_set<OvmCeH> &hexa_set, 
		std::unordered_set<OvmFaH> &allowed_boundary_fhs, std::unordered_set<OvmCeH> &barrier_set, std::unordered_set<OvmEgH> &allowed_ehs);
	void optimize_shrink_set_simple (VolumeMesh *mesh, std::unordered_set<OvmCeH> &hexa_set,
		std::unordered_set<OvmCeH> &barrier_set);
	void optimize_quad_patch (VolumeMesh *mesh, std::unordered_set<OvmFaH> &quad_patch, std::unordered_set<OvmFaH> &allowed_fhs);

	void retrieve_one_sheet (VolumeMesh *mesh, OvmEgH start_eh, OpenVolumeMesh::EdgePropertyT<bool> &E_VISITED,
		std::unordered_set<OvmEgH> &sheet_ehs, std::unordered_set<OvmCeH> &sheet_chs);
	void retrieve_one_sheet (VolumeMesh *mesh, OvmEgH start_eh, std::unordered_set<OvmEgH> &sheet_ehs, std::unordered_set<OvmCeH> &sheet_chs);

	void retrieve_chords (VolumeMesh *mesh, std::unordered_set<OvmFaH> &fhs, ChordSet &chord_set);
	void retrieve_sheets (VolumeMesh *mesh, SheetSet &sheet_set);
	DualColumn * retrieve_column (VolumeMesh *mesh, OvmFaH seed_fh);
	std::set<DualColumn *> retrieve_columns (VolumeMesh *mesh, DualSheet *self_int_sheet);
	std::set<DualColumn *> retrieve_columns (VolumeMesh *mesh, SheetSet &sheet_set);

	bool get_sweeping_quad_set (VolumeMesh *mesh, std::unordered_set<OvmEgH> ehs, OvmFaH direction_fh, std::unordered_set<OvmFaH> &sweeping_quad_set);
	//////////////////////////////////////////////////////////////////////////
	//geometry calculation functions
	double calc_interior_angle (const OvmVec3d &vec1, const OvmVec3d &vec2);
	void attach_mesh_elements_to_ACIS_entities (VolumeMesh *mesh, BODY *body, double myresabs = SPAresabs * 1000, std::unordered_set<OvmVeH> *left_vhs = NULL);
	void smooth_volume_mesh (VolumeMesh *mesh, BODY *body, int smooth_rounds, HoopsView *hoopsview = NULL);
	void smooth_acis_edges (VolumeMesh *mesh, std::vector<EDGE*> &acis_edges, int smooth_rounds, HoopsView *hoopsview = NULL);
	void smooth_acis_faces (VolumeMesh *mesh, std::vector<FACE*> &acis_faces, int smooth_rounds, HoopsView *hoopsview = NULL);
	//////////////////////////////////////////////////////////////////////////
	//mesh topology changing functions
	std::vector<DualSheet *> one_simple_sheet_inflation (VolumeMesh *mesh, std::unordered_set<OvmFaH> &fhs, 
		std::unordered_set<OvmCeH> &shrink_set,
		std::unordered_set<OvmEgH> &int_ehs, 
		std::hash_map<OvmVeH, OvmVeH> &old_new_vhs_mapping = std::hash_map<OvmVeH, OvmVeH>());
	std::vector<DualSheet * > one_simple_sheet_inflation (VolumeMesh *mesh, std::unordered_set<OvmFaH> &fhs, 
		std::unordered_set<OvmCeH> &shrink_set, 
		std::hash_map<OvmVeH, OvmVeH> &old_new_vhs_mapping = std::hash_map<OvmVeH, OvmVeH>());

	bool one_simple_sheet_extraction (VolumeMesh *mesh, 
		DualSheet *sheet, std::unordered_set<OvmFaH> &result_fhs);
	bool one_simple_sheet_partial_extraction (VolumeMesh *mesh, 
		DualSheet *sheet, 
		std::unordered_set<OvmCeH> &chs_on_sheet_to_extract,
		std::unordered_set<OvmEgH> &ehs_after_extraction);
	void general_sheet_inflation (VolumeMesh *mesh, std::unordered_set<OvmFaH> &fhs, std::unordered_set<OvmCeH> &new_chs);
	void get_handle_change_mapping (VolumeMesh *mesh, std::hash_map<OvmVeH, std::vector<OvmVeH> > *vh_prev_curr_mapping,
		std::hash_map<OvmEgH, std::vector<OvmEgH> > *eh_prev_curr_mapping, std::hash_map<OvmFaH, std::vector<OvmFaH> > *fh_prev_curr_mapping,
		std::hash_map<OvmCeH, std::vector<OvmCeH> > *ch_prev_curr_mapping);

	void column_collapse (VolumeMesh *mesh, DualColumn *column, std::pair<OvmVeH, OvmVeH> &collapse_vhs_pair, 
		std::hash_map<OvmVeH, OvmVeH> &old_new_vhs_mapping = std::hash_map<OvmVeH, OvmVeH>());
	void columns_collapse (VolumeMesh *mesh, std::map<DualColumn *, std::pair<OvmVeH, OvmVeH> > &collapse_vhs_pairs,
		std::hash_map<OvmVeH, OvmVeH> &old_new_vhs_mapping = std::hash_map<OvmVeH, OvmVeH>());
	//////////////////////////////////////////////////////////////////////////
	//render
	HC_KEY insert_boundary_shell (VolumeMesh *mesh);
	HC_KEY insert_boundary_shell (VolumeMesh *mesh, std::unordered_set<OvmCeH> &chs);
}
#endif
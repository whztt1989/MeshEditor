#ifndef _MESHMATCHING_H_
#define _MESHMATCHING_H_

#include "MeshDefs.h"
#include "FuncDefs.h"
#include "chordmatchwidget.h"
#include <QObject>

struct MMData{
	VolumeMesh *mesh;
	BODY *body;
	std::set<FACE*> interfaces;
	std::unordered_set<OvmFaH> inter_patch, fixed_patch, free_patch;
	std::unordered_set<OvmEgH> ehs_on_interface;
	std::map<EDGE*, std::vector<OvmEgH> > ordered_ehs_on_edges;
	std::set<DualSheet *> sheets;
	std::set<DualChord *> matched_chords, unmatched_chords;
	std::map<unsigned int, DualSheet*> sheet_id_mapping;
	std::map<unsigned int, DualChord*> chord_id_mapping;
	MMData (){
		mesh = NULL;
		body = NULL;
	}
	~MMData(){
		if (mesh) delete mesh;
		if (body) api_del_entity (body);
		foreach (auto s, sheets) delete s;
		foreach (auto c, matched_chords) delete c;
		foreach (auto c, unmatched_chords) delete c;
	}
	void add_matched_chord (DualChord *chord){
		matched_chords.insert (chord);
		unmatched_chords.erase (chord);
	}
};

struct ChordPosDesc{
	std::vector<DualChord*> inter_graph, start_edge_graph, end_edge_graph;
	std::vector<int> inter_graph_indices;	//对于inter_graph上的每个chord，当前chord和这个chord相交位于这个chord上的哪一段
	EDGE *start_edge, *end_edge;
	int start_idx, end_idx;
	DualChord *chord;
	bool self_intersecting;
	VolumeMesh *mesh;
	ChordPosDesc (){
		start_idx = end_idx = NULL;
		start_edge = end_edge = NULL;
		chord = NULL;
		self_intersecting = false;
		mesh = NULL;
	}
};

namespace MM{
	void adjust_interface_bondary (VolumeMesh *mesh, std::unordered_set<OvmFaH> &inter_fhs, std::set<FACE *> interfaces);
	void get_hexas_within_depth (VolumeMesh *mesh, int depth_num, std::unordered_set<OvmFaH> &inter_fhs,
		std::unordered_set<OvmCeH> &hexas);
	void get_diff_fhs_patch (VolumeMesh *mesh, std::unordered_set<OvmCeH> &hexas, std::unordered_set<OvmFaH> &inter_fhs,
		std::unordered_set<OvmFaH> &fixed_patch, std::unordered_set<OvmFaH> &free_patch);
	void create_mesh_data_for_match (VolumeMesh *old_mesh, std::unordered_set<OvmCeH> &old_hexas, 
		std::unordered_set<OvmFaH> &old_inter_fhs, std::unordered_set<OvmFaH> &old_fixed_patch, 
		std::unordered_set<OvmFaH> &old_free_patch,
		MMData &mm_data);
	void asso_mesh_with_interface (MMData *mm_data, FACE *int_face);
}

class HoopsView;
class MeshMatchingHandler
{
public:
	MeshMatchingHandler (QWidget *parent = NULL);
	~MeshMatchingHandler ();
	//for debug only
	void set_hoopsview1 (HoopsView *_hv) {hoopsview1 = _hv;}
	void set_hoopsview2 (HoopsView *_hv) {hoopsview2 = _hv;}

	bool load_mesh_matching_data (QString dir_path);
	bool save_mesh_matching_data (QString dir_path);
	inline VolumeMesh *mesh1 () {return mm_data1->mesh;}
	inline VolumeMesh *mesh2 () {return mm_data2->mesh;}
	inline BODY *body1 () {return mm_data1->body;}
	inline BODY *body2 () {return mm_data2->body;}
	//获得两个body之间的贴合面interfaces
	bool locate_interfaces ();
	inline std::set<FACE*> common_interfaces () {return interfaces;}
	inline std::set<FACE*> interfaces1 (){return mm_data1->interfaces;}
	inline std::set<FACE*> interfaces2 (){return mm_data2->interfaces;}

	void init_match ();
	bool check_match ();

	std::vector<OvmEgH> get_polyline_for_chord_inflation (VolumeMesh *mesh, std::pair<std::set<OvmVeH>, std::set<OvmVeH> > vhs_on_geom_egs,
		std::vector<std::set<OvmEgH> > candi_interval_ehs);

	ChordPosDesc get_chord_pos_desc (DualChord *chord);
	ChordPosDesc translate_chord_pos_desc (ChordPosDesc &in_cpd);
	bool can_two_chords_match (DualChord *chord1, DualChord *chord2);
	//获得chord的相交chords序列以及该chord是否自相交。相交序列中，自相交处用NULL表示
	std::vector<std::pair<DualChord*,OvmFaH> > get_matched_int_chord_seq (DualChord *chord, bool &self_int);
	//获得起始和终止物理边上的候选节点
	std::pair<std::set<OvmVeH>, std::set<OvmVeH> > get_candidate_end_geom_vertices (ChordPosDesc &translated_cpd);
	//获得相交chord序列中相交位置的网格边集合
	std::vector<std::set<OvmEgH> > get_candidate_interval_ehs (ChordPosDesc &translated_cpd);
	//添加一对匹配chord
	bool add_two_matched_chords (DualChord *chord1, DualChord *chord2);
	//std::vector<OvmEgH> get_polyline ()
	DualChord * get_matched_chord (DualChord *chord);
	//保存当前的匹配chord
	bool save_matched_chords (QString file_path);
	//读取保存的匹配chord
	bool load_matched_chords (QString file_path);
private:
	void get_quads_on_interfaces (MMData *mm_data);
	void get_all_ehs_on_interface (MMData *mm_data);
	void attach_interface_entity_to_mesh (MMData *mm_data);
	
	void get_inter_graph (ChordPosDesc &cpd, DualChord *chord);
	std::pair<int, int> get_matched_index_on_geom_edge (DualChord *chord, EDGE *start_eg, EDGE *end_eg);
	void get_ordered_ehs_on_edge (VolumeMesh *mesh, std::unordered_set<OvmEgH> &ehs_on_eg, 
		EDGE *eg, std::vector<OvmEgH> &ordered_ehs);
	void get_all_vhs_on_interface (std::unordered_set<OvmVeH> &all_vhs);
	std::vector<OvmEgH> find_inflation_polyline (ChordPosDesc &in_cpd, ChordPosDesc &trans_cpd);

	//调整已匹配chords之间的配准关系，让匹配的chords方向一致
	//一致的方向对于获取用于chord生成的polyline非常关键
	void adjust_matched_chords_directions ();
	
	
	//获得相交该chord上fh处的相交chord，返回NULL表示该chord在fh处自相交，否则返回相交chord
	DualChord * get_int_chord_at_this_fh (DualChord *chord, OvmFaH fh);

	//构建或者从六面体网格属性中恢复对偶结构，同时搜集interfaces上的网格边、面
	void construct_dual_structure_and_gather_interface_elements (MMData *mm_data);

	inline OpenVolumeMesh::VertexPropertyT<unsigned int> get_V_ENTITY_PTR (MMData *mm_data){
		return mm_data->mesh->request_vertex_property<unsigned int>("entityptr");
	}

	inline OpenVolumeMesh::EdgePropertyT<unsigned int> get_E_SHEET_PTR (MMData *mm_data){
		return mm_data->mesh->request_edge_property<unsigned int>("sheetptr");
	}

	inline OpenVolumeMesh::EdgePropertyT<unsigned int> get_E_CHORD_PTR (MMData *mm_data){
		return mm_data->mesh->request_edge_property<unsigned int>("chordptr");
	}

	inline OpenVolumeMesh::EdgePropertyT<unsigned int> get_E_CHORD_PTR (VolumeMesh *mesh){
		return mesh->request_edge_property<unsigned int>("chordptr");
	}

	inline OpenVolumeMesh::EdgePropertyT<unsigned int> get_E_INT_EG_PTR (MMData *mm_data){
		return mm_data->mesh->request_edge_property<unsigned int>("intedgeptr");
	}

	inline OvmVeH InvalidVertexHandle (){
		return OvmVeH (-1);
	}

	inline OvmEgH InvalidEdgeHandle (){
		return OvmEgH (-1);
	}

	inline OvmFaH InvalidFaceHandle (){
		return OvmFaH (-1);
	}
	//获得该mesh对面的mesh
	inline VolumeMesh * oppo_mesh (VolumeMesh *mesh){
		if (mesh == mm_data1->mesh) return mm_data2->mesh;
		else if (mesh == mm_data2->mesh) return mm_data1->mesh;
		else
			return NULL;
	}
	//获得该mesh上所对应的的Mesh Matching数据（边、面相关数据等）
	inline MMData * get_mesh_matching_data (VolumeMesh *mesh){
		if (mesh == mm_data1->mesh)
			return mm_data1;
		else if (mesh == mm_data2->mesh)
			return mm_data2;
		else
			return NULL;
	}

	//判断两个相交序列是否匹配
	inline bool can_two_int_graphs_match (DualChord *chord1, const std::vector<DualChord*> &int_graph1, 
		DualChord *chord2, const std::vector<DualChord*> &int_graph2){
		auto graph_size = int_graph1.size ();
		for (int i = 0; i != graph_size; ++i){
			auto int_chord1 = int_graph1[i],
				int_chord2 = int_graph2[i];
			if (int_chord1 == NULL || int_chord2 == NULL){
				if (!(int_chord1 == NULL && int_chord2 == NULL))
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
	}

	//判断一个chord是否已经匹配
	inline bool has_been_matched (DualChord *chord){
		foreach (auto &p, matched_chord_pairs){
			if (p.first == chord || p.second == chord)
				return true;
		}
		return false;
	}
	inline void warning (const char *msg){
		QMessageBox::warning (parent, QObject::tr("警告"), msg);
	}

	inline void warning (QString msg){
		QMessageBox::warning (parent, QObject::tr("警告"), msg);
	}

	inline void informing (const char *msg){
		QMessageBox::information (parent, QObject::tr("提示"), msg);
	}

	inline void informing (QString msg){
		QMessageBox::information (parent, QObject::tr("提示"), msg);
	}

	inline HoopsView * hoopsview (MMData *mm_data){
		if (mm_data == mm_data1) return hoopsview1;
		else return hoopsview2;
	}
private:
	QWidget *parent;
	HoopsView *hoopsview1, *hoopsview2;
	
public:
	MMData *mm_data1, *mm_data2;
	std::set<FACE *> interfaces;
	ChordPairs matched_chord_pairs;
	double myresabs;
};

#endif
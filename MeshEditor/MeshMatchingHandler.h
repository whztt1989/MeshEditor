#ifndef _MESHMATCHING_H_
#define _MESHMATCHING_H_

#include "MeshDefs.h"
#include "FuncDefs.h"
#include "chordmatchwidget.h"

struct MMData{
	VolumeMesh *mesh;
	std::unordered_set<OvmFaH> inter_patch, fixed_patch, free_patch;
	std::unordered_set<OvmEgH> ehs_on_interface;
	std::map<EDGE*, std::vector<OvmEgH> > ordered_ehs_on_edges;
};

struct ChordPosDesc{
	std::vector<OvmEgH> inter_ordered_ehs, start_edge_ordered_ehs, end_edge_ordered_ehs;
	std::vector<DualChord*> inter_graph, start_edge_graph, end_edge_graph;
	std::vector<int> inter_graph_indices;
	EDGE *start_edge, *end_edge;
	int start_idx, end_idx;
	DualChord *chord;
	ChordPosDesc (){
		start_idx = end_idx = NULL;
		start_edge = end_edge = NULL;
		chord = NULL;
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


class MeshMatchingHandler
{
public:
	MeshMatchingHandler (MMData *_data1, MMData *_data2, std::set<FACE *> _interfaces);
	void init_match ();
	bool check_match ();
	//std::vector<OvmEgH> get_polyline ()
private:
	void get_all_ehs_on_interface (MMData *mm_data);
	void attach_interface_entity_to_mesh (MMData *mm_data);
	ChordPosDesc get_chord_pos_desc (MMData *mm_data, DualChord *chord);
	void get_inter_graph (ChordPosDesc &cpd, DualChord *chord);
	void get_edge_graph (ChordPosDesc &cpd, MMData *mm_data, DualChord *chord);
	void get_ordered_ehs_on_edge (VolumeMesh *mesh, std::unordered_set<OvmEgH> &ehs_on_eg, 
		EDGE *eg, std::vector<OvmEgH> &ordered_ehs);
	void get_all_vhs_on_interface (std::unordered_set<OvmVeH> &all_vhs);
	std::vector<OvmEgH> find_inflation_polyline (ChordPosDesc &in_cpd, ChordPosDesc &trans_cpd);

	bool can_two_chords_match (DualChord *chord1, DualChord *chord2);
	void translate_chord_pos_desc (ChordPosDesc &in_cpd, ChordPosDesc &out_cpd);
	DualChord * get_matched_chord (DualChord *chord);
public:
	MMData *mm_data1, *mm_data2;
	std::set<FACE *> interfaces;
	ChordSet chord_set1, chord_set2, all_matched_chords;
	SheetSet sheet_set1, sheet_set2;
	ChordPairs matched_chord_pairs;
	double myresabs;
};

#endif
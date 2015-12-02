#ifndef _ONE_SIMPLE_SHEET_H_
#define _ONE_SIMPLE_SHEET_H_

#include <string>
#include <fstream>
#include <iomanip>
#include <map>
#include <set>
#include <vector>
#include <list>
#include <algorithm>
#include <hash_map>
#include <queue>
#include "MeshDefs.h"
#include "DualDefs.h"
#include "FuncDefs.h"
#include "CrossCrossPath.h"
#include "acis_headers.h"
#include "hoopsview.h"
#include "OneSimpleHandlerBase.h"

struct EndVhPair{
	OvmVeH vh1, vh2;
	int loop_idx;
};

struct LoopClassify{
	std::vector<OvmEgH> loop;
	int fhs_patch_idx;
};

class OneSimpleSheetInflationHandler
{
public:
	OneSimpleSheetInflationHandler (VolumeMesh *_mesh, BODY *_body, HoopsView *_hoopsview);
	~OneSimpleSheetInflationHandler();
public:
	void init ();
	bool analyze_input_edges (std::unordered_set<OvmEgH> &_input_ehs);
	std::unordered_set<OvmEgH> complete_loop ();
	void set_constant_fhs (std::unordered_set<OvmFaH> &_constant_fhs);
	void set_minimum_depth (int _min_depth);
	void set_interface (FACE *_inter_face);
	QString get_last_err () {return last_err_str;}
	bool is_complete () {return end_vhs.empty ();}
	
	std::unordered_set<OvmCeH> get_hexa_set ();
	std::unordered_set<OvmFaH> get_inflation_fhs ();
	DualSheet * sheet_inflation ();
	void reprocess ();
	void set_inter_face (FACE *_inter_face) {inter_face = _inter_face;}
	FACE *get_inter_face () {return inter_face;}
	void set_const_face (FACE *_const_face) {const_face = _const_face;}
	FACE *get_const_face () {return const_face;}
	void update_inter_face_fhs ();
	void update_const_face_fhs ();

	void set_cross_conf (CCPSHConf cross_conf){
		base_handler->cross_hexas = cross_conf;
	}
public:
	std::vector<std::unordered_set<OvmFaH> > retrieve_modify_allowed_fhs_patches ();
	std::vector<LoopClassify> retrieve_interface_loops ();
	std::vector<EndVhPair> classify_end_vhs ();
	void evaluate_vhs_on_modify_allowed_fhs ();
	std::vector<OvmEgH> get_intersect_path (std::vector<std::pair<OvmEgH, OvmEgH> > &edge_pairs);
	std::unordered_set<OvmEgH> connect_nonintersect_loop_parts ();
	int circle_size ();
	std::unordered_set<OvmEgH> connect_intersect_loop_parts ();
public:
	VolumeMesh *mesh;
	BODY *body;
	HoopsView *hoopsview;
	FACE *inter_face, *const_face;
	bool passed_analyzed;
	std::unordered_set<OvmEgH> input_ehs;
	std::unordered_set<OvmFaH> constant_fhs, interface_fhs, inflation_fhs;
	QString last_err_str;
	std::unordered_set<OvmVeH> end_vhs;
	std::vector<OvmVeH> int_vhs;
	std::vector<std::vector<OvmEgH> > complete_loops, incomplete_loops;

	int min_depth;
	std::hash_map<OvmVeH, std::vector<OvmEgH> > vh_adj_ehs_mapping;
	OneSimpleHandlerBase *base_handler;
	std::vector<EndVhPair> end_vh_pairs;
	std::unordered_set<OvmEgH> all_interface_fhs_boundary_ehs;
	std::vector<LoopClassify> interface_loops;
	std::unordered_set<OvmFaH> all_modify_allowed_fhs;
	std::vector<std::unordered_set<OvmFaH> > modify_allowed_fhs_patches;

	std::vector<OvmEgH> inter_path;
	CCPSHConf inter_cross_hexas;
	std::vector<std::pair<OvmEgH, OvmEgH> > inter_edge_pairs;
	std::unordered_set<OvmVeH> intersect_allowed_vhs;
	OvmVeH new_int_vh;
	int new_int_fhs_patch_idx;
	int end_vh_pair_idx_on_intersect_patch;
};

#endif
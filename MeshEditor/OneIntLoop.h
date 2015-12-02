#ifndef _ONE_INT_LOOP_H_
#define _ONE_INT_LOOP_H_
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

struct LoopSegment{
	OvmVeH start_vh, end_vh;
	std::vector<OvmEgH> seg_ehs;
};

struct OneIntLoopInput{
	BODY *body;
	VolumeMesh *mesh;
	HoopsView *hoopsview;
	FACE * face_of_constant;									//不可更改的网格部分所在的几何面
	std::unordered_set<OvmEgH> input_ehs;
	std::unordered_set<OvmFaH> modify_allowed_fhs;
	std::unordered_set<OvmFaH> optimize_allowed_fhs;
	std::unordered_set<OvmFaH> interface_fhs;						//输入网格边所在的四边形面集（是不能修改的）
	std::unordered_set<OvmFaH> constant_fhs;
	bool auto_deduce_allowed_fhs;
	bool auto_deduce_interface_fhs;
	int min_depth;
	void init ()
	{
		body = NULL; mesh = NULL; hoopsview = NULL;
		face_of_constant = NULL;
		input_ehs.clear (); modify_allowed_fhs.clear ();
		interface_fhs.clear ();
		auto_deduce_allowed_fhs = auto_deduce_interface_fhs = false;
		min_depth = 1;
	}
};

struct OneIntLoopOutput{
	std::unordered_set<OvmFaH> manifold_fhs;
	std::vector<OvmEgH> result_loop;
	std::vector<OvmEgH> intersect_line;
	std::pair<OvmVeH, OvmVeH> intersect_vhs;
	QString last_err_str;
	std::unordered_set<OvmFaH> fhs_for_inflation;
	std::vector<std::unordered_set<OvmCeH> > diagonal_hexa_sets;
};

struct OneIntLoopIntermedia{
	bool is_complete;											//输入网格边是否是完整的loop
	bool is_intersect;											//输入网格边是否自交
	FACE * face_of_input_ehs;									//输入网格边所在的几何面
	
	std::unordered_set<OvmEgH> allowed_ehs;
	std::unordered_set<OvmVeH> allowed_vhs;
	std::unordered_set<OvmFaH> deduced_allowed_fhs;
	std::unordered_set<OvmFaH> deduced_interface_fhs;
	std::vector<OvmEgH> complete_loop;
	OvmVeH existed_int_vh, new_int_vh;
	std::hash_map<OvmVeH, std::vector<OvmEgH> > vh_adj_ehs_mapping;
	std::unordered_set<OvmVeH> end_vhs, int_vhs;
	std::vector<LoopSegment> input_loop_segs;
	QString last_err_str;										//记录最近一次操作错误的错误信息
	std::vector<OvmEgH> intersect_path;
	CCPSHConf cross_hexas;
	std::vector<std::pair<OvmEgH, OvmEgH> > edge_pairs;
	bool has_been_analyzed;
	std::unordered_set<OvmFaH> background_fhs_patch;
	std::vector<std::unordered_set<OvmFaH> > diagonal_fhs_patchs;
	std::vector<std::unordered_set<OvmCeH> > diagonal_hexa_sets;
	std::unordered_set<OvmFaH> fhs_for_inflation;
	std::vector<std::unordered_set<OvmFaH> > optimized_quad_patches;
	void init ()
	{
		is_complete = is_intersect = has_been_analyzed = false;
		face_of_input_ehs = NULL;
		allowed_ehs.clear (); allowed_vhs.clear ();
		deduced_allowed_fhs.clear (); deduced_interface_fhs.clear ();
		complete_loop.clear ();
		existed_int_vh = new_int_vh = OvmVeH (-1);
		vh_adj_ehs_mapping.clear ();
		end_vhs.clear (); int_vhs.clear ();
		last_err_str = "";
		intersect_path.clear ();
		cross_hexas.clear ();
		edge_pairs.clear ();
		background_fhs_patch.clear ();
		diagonal_hexa_sets.clear (); diagonal_fhs_patchs.clear ();
		fhs_for_inflation.clear ();
		input_loop_segs.clear ();
	}
	
	OneIntLoopIntermedia ()
	{
		is_complete = false;
		has_been_analyzed = false;
		existed_int_vh = new_int_vh = OvmVeH (-1);
	}
};

bool oil_auto_generation (OneIntLoopInput &oil_input, OneIntLoopOutput &oil_output);
bool oil_auto_deduce_constant_quad_set (OneIntLoopInput &oil_input, OneIntLoopIntermedia &oil_mid);
bool oil_auto_get_hexa_set (OneIntLoopInput &oil_input, OneIntLoopIntermedia &oil_mid);
bool oil_auto_get_quad_set (OneIntLoopInput &oil_input, OneIntLoopIntermedia &oil_mid);

bool oil_analyze_input_ehs (OneIntLoopInput &oil_input, OneIntLoopIntermedia &oil_mid);
bool oil_deduce_interface_fhs (OneIntLoopInput &oil_input, OneIntLoopIntermedia &oil_mid);
bool oil_get_interface_quad_patch (OneIntLoopInput &oil_input, OneIntLoopIntermedia &oil_mid);

bool oil_deduce_allowed_fhs (OneIntLoopInput &oil_input, OneIntLoopIntermedia &oil_mid);
bool oil_get_allowed_vhs_and_ehs (OneIntLoopInput &oil_input, OneIntLoopIntermedia &oil_mid);
bool oil_get_cross_path_hexa_sets (OneIntLoopInput &oil_input, OneIntLoopIntermedia &oil_mid);
bool oil_get_diagonal_hexa_sets (OneIntLoopInput &oil_input, OneIntLoopIntermedia &oil_mid);
bool oil_optimize_surface_quad_set (OneIntLoopInput &oil_input, OneIntLoopIntermedia &oil_mid);
bool oil_optimize_diagonal_hexa_sets (OneIntLoopInput &oil_input, OneIntLoopIntermedia &oil_mid);
//////////////////////////////////////////////////////////////////////////
//private functions
bool _get_allowed_vhs_and_ehs (OneIntLoopInput &oil_input, OneIntLoopIntermedia &oil_mid);
bool _analyze_input_edges (OneIntLoopInput &oil_input, OneIntLoopIntermedia &oil_mid);
bool _get_cross_hexa_sets (OneIntLoopInput &oil_input, OneIntLoopIntermedia &oil_mid);
#endif
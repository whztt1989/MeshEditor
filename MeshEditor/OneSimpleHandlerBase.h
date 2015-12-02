#pragma once
#include "MeshDefs.h"
#include "DualDefs.h"
#include "FuncDefs.h"
#include "CrossCrossPath.h"

class HoopsView;

typedef std::vector<std::pair<std::unordered_set<OvmFaH>, std::unordered_set<OvmFaH> > > CrossFaceSetPairs;
class OneSimpleHandlerBase
{
public:
	OneSimpleHandlerBase(VolumeMesh *_mesh, std::unordered_set<OvmEgH> &_input_ehs);
	~OneSimpleHandlerBase(void);
public:
	virtual std::unordered_set<OvmCeH> get_hexa_set () = 0;
	virtual std::unordered_set<OvmFaH> get_inflation_fhs () = 0;
	virtual DualSheet * inflate_new_sheet () = 0;
	QString get_last_err () {return last_err_str;}
	void set_inter_face_fhs (std::unordered_set<OvmFaH> &_inter_fhs){interface_fhs = _inter_fhs;}
	void set_min_depth (int _min_depth){min_depth = _min_depth;}
protected:
	void get_fhs_on_boundary ();
	void get_fhs_on_interface ();
	void get_optimize_allowed_fhs ();
	
	void get_inflation_fhs_from_hexa_set (std::unordered_set<OvmCeH> &hexa_set);
	void get_intersect_allowed_vhs ();
	void depth_control (std::unordered_set<OvmFaH> &unsuitable_fhs);
	void optimize_diagonal_hexa_sets (std::unordered_set<OvmCeH> &diagonal_hexa_set1, 
		std::unordered_set<OvmCeH> &diagonal_hexa_set2);

	void get_diagonal_quad_patches (std::unordered_set<OvmFaH> &mid_patch, 
		std::unordered_set<OvmFaH> &diagonal_patch1,
		std::unordered_set<OvmFaH> &diagonal_patch2);
	bool search_constrained_cross_hexa_sets (CrossFaceSetPairs &cross_face_set_pairs);
	void get_cross_faces_pairs (std::unordered_set<OvmFaH> &diagonal_patch1, 
		std::unordered_set<OvmFaH> &diagonal_patch2, 
		CrossFaceSetPairs &cross_face_set_pairs);
	void optimize_diagonal_hexa_set_boundary_quads (std::unordered_set<OvmCeH> &diagonal_hexa_set1, 
		std::unordered_set<OvmCeH> &diagonal_hexa_set2);
	void optimize_surface_quad_set (std::unordered_set<OvmFaH> &fhs_need_optimize, 
		std::unordered_set<OvmFaH> barrier_fhs);
	void depth_control (std::vector<std::unordered_set<OvmFaH> > &new_separated_fhs_patches, 
		std::vector<std::unordered_set<OvmCeH> > &hexa_sets_within_depth);
public:
	VolumeMesh *mesh;
	BODY *body;
	HoopsView *hoopsview;
	FACE *inter_face;

	std::unordered_set<OvmCeH> hexa_set;
	std::unordered_set<OvmEgH> input_ehs;
	std::vector<std::unordered_set<OvmFaH> > separated_fhs_patches;
	QString last_err_str;
	std::unordered_set<OvmFaH> interface_fhs, boundary_fhs, optimize_allowed_fhs, inflation_fhs;
	std::unordered_set<OvmFaH> *constant_fhs_ptr;
	std::unordered_set<OvmVeH> end_vhs, intersect_allowed_vhs;
	std::vector<OvmVeH> int_vhs;
	int min_depth;
	std::hash_map<OvmVeH, std::vector<OvmEgH> > *vh_adj_ehs_mapping;

	CCPSHConf cross_hexas;	//CCPSHConf::std::vector<std::unordered_set<OvmCeH> >
	std::vector<OvmEgH> intersect_path;
};


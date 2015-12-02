#pragma once
#include "onesimplehandlerbase.h"
class OneIntHandler :
	public OneSimpleHandlerBase
{
public:
	OneIntHandler(VolumeMesh *_mesh, std::unordered_set<OvmEgH> &_input_ehs, 
		FACE *_inter_face, std::unordered_set<OvmFaH> *_constant_fhs_ptr, 
		std::hash_map<OvmVeH, std::vector<OvmEgH> > *_vh_adj_ehs_mapping, std::vector<OvmVeH> &_int_vhs);
	~OneIntHandler(void);
public:
	std::unordered_set<OvmCeH> get_hexa_set ();
	std::unordered_set<OvmFaH> get_inflation_fhs ();
	DualSheet * inflate_new_sheet ();
private:
	void get_diagonal_quad_patches ();
	bool search_unconstrained_cross_hexa_sets ();
	void get_diagonal_hexa_sets ();
	void complete_surface_loop ();
private:
	std::unordered_set<OvmFaH> diagonal_quad_patch1, diagonal_quad_patch2;
	//std::unordered_set<OvmFaH> optimized_diagonal_quad_patch1, optimized_diagonal_quad_patch2;
	std::unordered_set<OvmCeH> diagonal_hexa_set1, diagonal_hexa_set2;
	std::unordered_set<OvmCeH> all_hexa_set;
	std::vector<std::pair<OvmEgH, OvmEgH> > cross_eh_pairs;
	OvmVeH start_int_vh, end_int_vh;
	std::set<int> diagonal_idx;
};


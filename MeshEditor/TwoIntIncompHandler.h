#pragma once
#include "onesimplehandlerbase.h"
class TwoIntIncompHandler :
	public OneSimpleHandlerBase
{
public:
	TwoIntIncompHandler(VolumeMesh *_mesh, std::unordered_set<OvmEgH> &_input_ehs, 
		FACE *_inter_face, std::unordered_set<OvmFaH> *_constant_fhs_ptr, 
		std::hash_map<OvmVeH, std::vector<OvmEgH> > *_vh_adj_ehs_mapping, std::vector<OvmVeH> &_int_vhs);
	~TwoIntIncompHandler(void);
public:
	std::unordered_set<OvmCeH> get_hexa_set ();
	std::unordered_set<OvmFaH> get_inflation_fhs ();
	DualSheet * inflate_new_sheet ();
private:
	void optimize_diagonal_quad_sets ();
};


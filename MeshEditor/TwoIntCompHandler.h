#pragma once
#include "onesimplehandlerbase.h"
class TwoIntCompHandler :
	public OneSimpleHandlerBase
{
public:
	TwoIntCompHandler(VolumeMesh *_mesh, std::unordered_set<OvmEgH> &_input_ehs, 
		std::hash_map<OvmVeH, std::vector<OvmEgH> > *_vh_adj_ehs_mapping, std::vector<OvmVeH> &_int_vhs);
	~TwoIntCompHandler(void);

public:
	std::unordered_set<OvmCeH> get_hexa_set ();
	std::unordered_set<OvmFaH> get_inflation_fhs ();
	DualSheet * inflate_new_sheet ();
};


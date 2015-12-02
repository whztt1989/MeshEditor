#pragma once
#include "onesimplehandlerbase.h"
class NoIntIncompHandler :
	public OneSimpleHandlerBase
{
public:
	NoIntIncompHandler(VolumeMesh *_mesh, std::unordered_set<OvmEgH> &_input_ehs, FACE *_inter_face,
		std::unordered_set<OvmFaH> *_constant_fhs_ptr);
	~NoIntIncompHandler(void);
public:
	std::unordered_set<OvmCeH> get_hexa_set ();
	std::unordered_set<OvmFaH> get_inflation_fhs ();
	DualSheet * inflate_new_sheet ();
private:
	std::unordered_set<OvmFaH> get_best_patch ();
	void depth_control (std::unordered_set<OvmFaH> &best_patch, std::unordered_set<OvmCeH> &min_hexa_set, 
		std::unordered_set<OvmCeH> &rest_hexa_set);
};


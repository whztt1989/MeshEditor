#pragma once
#include "onesimplehandlerbase.h"
class NoIntCompHandler :
	public OneSimpleHandlerBase
{
public:
	NoIntCompHandler(VolumeMesh *_mesh, std::unordered_set<OvmEgH> &_input_ehs);
	~NoIntCompHandler(void);
public:
	std::unordered_set<OvmCeH> get_hexa_set ();
	std::unordered_set<OvmFaH> get_inflation_fhs ();
	DualSheet * inflate_new_sheet ();
private:
	
};


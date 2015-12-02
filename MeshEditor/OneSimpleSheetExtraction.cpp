#include "StdAfx.h"
#include "OneSimpleSheetExtraction.h"


OneSimpleSheetExtractionHandler::OneSimpleSheetExtractionHandler(VolumeMesh *_mesh, BODY *_body, HoopsView *_hoopsview)
	: mesh (_mesh), body (_body), hoopsview (_hoopsview)
{
	sheet_to_extract = NULL;
}


OneSimpleSheetExtractionHandler::~OneSimpleSheetExtractionHandler(void)
{
}

void OneSimpleSheetExtractionHandler::set_constant_fhs (std::unordered_set<OvmFaH> &_constant_fhs)
{
	constant_fhs = _constant_fhs;
}

void OneSimpleSheetExtractionHandler::set_interface (FACE *_inter_face)
{
	inter_face = _inter_face;
}

void OneSimpleSheetExtractionHandler::set_interface_fhs (std::unordered_set<OvmFaH> &_interface_fhs)
{
	interface_fhs = _interface_fhs;
}

void OneSimpleSheetExtractionHandler::update_interface_fhs ()
{
	interface_fhs.clear ();
	JC::get_fhs_on_acis_face (mesh, inter_face, interface_fhs);
}

void OneSimpleSheetExtractionHandler::update_constant_fhs ()
{
	constant_fhs.clear ();
	JC::get_fhs_on_acis_face (mesh, const_face, constant_fhs);
}
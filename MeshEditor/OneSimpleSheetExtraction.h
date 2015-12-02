#pragma once

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
#include "acis_headers.h"
#include "hoopsview.h"

class OneSimpleSheetExtractionHandler
{
public:
	OneSimpleSheetExtractionHandler(VolumeMesh *_mesh, BODY *_body, HoopsView *_hoopsview);
	~OneSimpleSheetExtractionHandler(void);
public:
	QString get_last_err () {return last_err_str;}
	void set_sheet_to_extract (DualSheet *_sheet) {sheet_to_extract = _sheet;}
	DualSheet *get_sheet_to_extract () {return sheet_to_extract;}
	void set_constant_fhs (std::unordered_set<OvmFaH> &_constant_fhs);
	void set_interface (FACE *_inter_face);
	void set_interface_fhs (std::unordered_set<OvmFaH> &_interface_fhs);
	void set_const_face (FACE *_const_face) {const_face = _const_face;}
	FACE *get_const_face () {return const_face;}
	FACE *get_interface () {return inter_face;}
	void update_constant_fhs ();
	void update_interface_fhs ();
	std::unordered_set<OvmFaH> get_interface_fhs () {return interface_fhs;}
	std::unordered_set<OvmFaH> get_constant_fhs () {return constant_fhs;}
	void set_extract_result_ehs (std::unordered_set<OvmEgH> &_result_ehs) {extract_result_ehs = _result_ehs;}
	std::unordered_set<OvmEgH> get_extract_result_ehs () {return extract_result_ehs;}
private:
	VolumeMesh *mesh;
	BODY *body;
	HoopsView *hoopsview;
	FACE *inter_face, *const_face;

	DualSheet *sheet_to_extract;
	std::unordered_set<OvmFaH> constant_fhs, interface_fhs;
	QString last_err_str;
	std::unordered_set<OvmEgH> extract_result_ehs;
};


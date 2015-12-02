#include "stdafx.h"
#include "DualDefs.h"
#include <queue>

int DualSheet::sheet_idx = 0;
int DualColumn::column_idx = 0;
int DualChord::chord_idx = 0;

DualSheet::DualSheet (VolumeMesh *m)
	: VolumeMeshElementGroup (m)
{
	type = "sheet";
	name = QString ("sheet No.%1").arg (sheet_idx++);
	idx = sheet_idx;
}

DualColumn::DualColumn (VolumeMesh *m)
	: VolumeMeshElementGroup (m)
{
	type = "column";
	name = QString ("column No.%1").arg (column_idx++);
	idx = column_idx;
}

bool DualColumn::if_intersect (DualColumn *col)
{
	return true;
}

DualChord::DualChord (VolumeMesh *m)
	: VolumeMeshElementGroup (m)
{
	type = "chord";
	name = QString ("chord No.%1").arg (chord_idx++);
	idx = chord_idx;
}

ChordPolyline get_chord_polyline (DualChord *chord)
{
	ChordPolyline chord_polyline;
	foreach (OvmEgH eh, chord->ordered_ehs)
		chord_polyline.push_back (chord->mesh->barycenter (eh));
	return chord_polyline;
}

std::vector<SPAposition> get_chord_spa_polyline (DualChord *chord)
{
	std::vector<SPAposition> chord_polyline;
	foreach (OvmEgH eh, chord->ordered_ehs){
		auto pos = chord->mesh->barycenter (eh);
		chord_polyline.push_back (POS2SPA(pos));
	}
	return chord_polyline;
}
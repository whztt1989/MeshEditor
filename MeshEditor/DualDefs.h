#ifndef _DUAL_DEFS_H_
#define _DUAL_DEFS_H_
#include "MeshDefs.h"

class DualSheet : public VolumeMeshElementGroup
{
public:
	DualSheet (VolumeMesh *m);
	~DualSheet() {};
public:
private:
	static int sheet_idx;
};

class DualColumn : public VolumeMeshElementGroup
{
public:
	DualColumn(VolumeMesh *m);
	~DualColumn(){}
public:
	bool if_intersect (DualColumn *col);
	bool is_closed;
private:
	static int column_idx;
};

class DualChord : public VolumeMeshElementGroup
{
public:
	DualChord (VolumeMesh *m);
	~DualChord() {};
public:
	std::set<FACE *> chord_faces;
	DualSheet *chord_sheet;
	bool is_closed;
private:
	static int chord_idx;
};

typedef std::set<DualChord*> ChordSet;
typedef std::pair<DualChord*, DualChord*> ChordPair;
typedef std::set<ChordPair> ChordPairs;
typedef std::set<DualSheet*> SheetSet;
typedef std::pair<DualSheet*, DualSheet*> SheetPair;
typedef std::set<SheetPair> SheetPairs;

typedef std::vector<OvmVec3d> ChordPolyline;

ChordPolyline get_chord_polyline (DualChord *chord);
std::vector<SPAposition> get_chord_spa_polyline (DualChord *chord);
#endif
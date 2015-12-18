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
	//将chord的方向反转
	void reverse (){
		if (!is_closed){
			std::reverse (ordered_ehs.begin (), ordered_ehs.end ());
			std::reverse (ordered_fhs.begin (), ordered_fhs.end ());
		}else{
			std::reverse (ordered_ehs.begin () + 1, ordered_ehs.end () - 1);
			std::reverse (ordered_fhs.begin (), ordered_fhs.end ());
		}
	}
	//如果是封闭chord，则cycle可以让其起始边循环前进times个
	void cycle (int times = 1){
		if (!is_closed) return;
		while (times--){
			ordered_ehs.erase (ordered_ehs.begin ());
			ordered_ehs.push_back (ordered_ehs.front ());
			ordered_fhs.push_back (ordered_fhs.front ());
			ordered_fhs.erase (ordered_fhs.begin ());
		}
	}
	//获得该chord的长度，即边或者面的个数
	int length () {
		return (int)(ordered_fhs.size ());
	}
	std::set<FACE *> chord_faces;
	DualSheet *chord_sheet;
	bool is_closed;
	bool is_self_int;
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
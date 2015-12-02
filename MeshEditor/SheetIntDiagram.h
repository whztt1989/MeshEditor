#ifndef _SHEET_INT_DIAGRAM_H_
#define _SHEET_INT_DIAGRAM_H_

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include "MeshDefs.h"
#include "FuncDefs.h"
#include "DualDefs.h"

//////////////////////////////////////////////////////////////////////////
//definitions for quad meshes
// define my personal traits
struct SheetIntDiagramTraits : OpenMesh::DefaultTraits
{
	typedef OpenMesh::Vec3d Point;
	typedef OpenMesh::Vec3d Normal;
	typedef OpenMesh::Vec3d Vector;
	// already defined in OpenMesh::DefaultTraits
	HalfedgeAttributes (OpenMesh::Attributes::PrevHalfedge);
};

typedef OpenMesh::PolyMesh_ArrayKernelT<SheetIntDiagramTraits>  SheetIntDiagram;
typedef SheetIntDiagram::VertexHandle SidVeH;
typedef SheetIntDiagram::HalfedgeHandle SidHaEgH;
typedef SheetIntDiagram::EdgeHandle SidEgH;
typedef SheetIntDiagram::FaceHandle SidFaH;

SheetIntDiagram * get_sheet_int_diagram (VolumeMesh *mesh, DualSheet *sheet);
void smooth_sheet_int_diagram (SheetIntDiagram *sheet_int_diagram, double radius, int smooth_rounds);
#endif
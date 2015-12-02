#ifndef _MESH_DEFS_H_
#define _MESH_DEFS_H_

#include <OpenVolumeMesh/Geometry/VectorT.hh>
#include <OpenVolumeMesh/Mesh/HexahedralMesh.hh>
#include <OpenVolumeMesh/Mesh/PolyhedralMesh.hh>
#include <OpenVolumeMesh/Mesh/HexahedralMeshTopologyKernel.hh>
#include <OpenVolumeMesh/Mesh/HexahedralMesh.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

#include <string>
#include <fstream>
#include <iomanip>
#include <map>
#include <set>
#include <vector>
#include <unordered_set>
#include <list>
#include "hc.h"
#include "HTools.h"
#include "acis_headers.h"


struct MyTraits : OpenMesh::DefaultTraits
{
	// let PPoint and Normal be a PVector of doubles
	typedef OpenMesh::Vec3d Point;
	typedef OpenMesh::Vec3d Normal;
	typedef OpenMesh::Vec3d Vector;
	// already defined in OpenMesh::DefaultTraits
	HalfedgeAttributes (OpenMesh::Attributes::PrevHalfedge);
};

typedef OpenMesh::PolyMesh_ArrayKernelT<MyTraits>  QuadMesh;
typedef QuadMesh::VertexHandle OmVeH;
typedef QuadMesh::HalfedgeHandle OmHaEgH;
typedef QuadMesh::EdgeHandle OmEgH;
typedef QuadMesh::FaceHandle OmFaH;
//////////////////////////////////////////////////////////////////////////
//definitions for hex meshes
typedef OpenVolumeMesh::Geometry::Vec3d OvmVec3d;
typedef OpenVolumeMesh::Geometry::Vec3d OvmPoint3d;
typedef OpenVolumeMesh::GeometricHexahedralMeshV3d VolumeMesh;

typedef OpenVolumeMesh::VertexHandle OvmVeH;
typedef OpenVolumeMesh::HalfEdgeHandle OvmHaEgH;
typedef OpenVolumeMesh::EdgeHandle OvmEgH;
typedef OpenVolumeMesh::HalfFaceHandle OvmHaFaH;
typedef OpenVolumeMesh::FaceHandle OvmFaH;
typedef OpenVolumeMesh::CellHandle OvmCeH;

#define POS2SPA(x) SPAposition((x)[0],(x)[1],(x)[2])
#define SPA2POS(spa) OvmVec3d ((spa).x(), (spa).y(), (spa).z())

class VolumeMeshElementGroup
{
public:
	VolumeMeshElementGroup (VolumeMesh *m, QString _type = "group", QString _name = "")
		: mesh(m), type (_type), name (_name)
	{}
public:
	void clear (){vhs.clear (); ehs.clear (); fhs.clear (); chs.clear ();}
	void add_edges_on_quad_set ();
public:
	std::unordered_set<OvmVeH> vhs;
	std::unordered_set<OvmEgH> ehs;
	std::unordered_set<OvmFaH> fhs;
	std::unordered_set<OvmCeH> chs;

	std::vector<OvmVeH> ordered_vhs;
	std::vector<OvmEgH> ordered_ehs;
	std::vector<OvmFaH> ordered_fhs;
	std::vector<OvmCeH> ordered_chs;

	VolumeMesh *mesh;
	QString type, name;
	bool delete_when_unrender;
	int idx;
};

#endif
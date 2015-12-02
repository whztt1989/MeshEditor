#include "StdAfx.h"
#include "meshrendercontrolwidget.h"

MeshRenderControlWidget::MeshRenderControlWidget(HoopsView *_hoopview, QWidget *parent)
	: hoopsview (_hoopview), QWidget(parent)
{
	ui.setupUi(this);
	setup_actions ();
}

MeshRenderControlWidget::~MeshRenderControlWidget()
{

}

void MeshRenderControlWidget::setup_actions ()
{
	connect (ui.groupBox_Mesh_Boundary, SIGNAL (toggled (bool)), hoopsview, SLOT (show_boundary (bool)));
	connect (ui.checkBox_Boundary_Vertices, SIGNAL (toggled (bool)), hoopsview, SLOT (show_boundary_vertices (bool)));
	connect (ui.checkBox_Boundary_Edges, SIGNAL (toggled (bool)), hoopsview, SLOT (show_boundary_edges (bool)));
	connect (ui.checkBox_Boundary_Faces, SIGNAL (toggled (bool)), hoopsview, SLOT (show_boundary_faces (bool)));
	connect (ui.checkBox_Boundary_Cells, SIGNAL (toggled (bool)), hoopsview, SLOT (show_boundary_cells (bool)));
	connect (ui.checkBox_Boundary_Vertices_Indices, SIGNAL (toggled (bool)), hoopsview, SLOT (show_boundary_vertices_indices (bool)));
	connect (ui.checkBox_Boundary_Edges_Indices, SIGNAL (toggled (bool)), hoopsview, SLOT (show_boundary_edges_indices (bool)));
	connect (ui.checkBox_Boundary_Faces_Indices, SIGNAL (toggled (bool)), hoopsview, SLOT (show_boundary_faces_indices (bool)));
	connect (ui.checkBox_Boundary_Cells_Indices, SIGNAL (toggled (bool)), hoopsview, SLOT (show_boundary_cells_indices (bool)));

	connect (ui.groupBox_Mesh_Inner, SIGNAL (toggled (bool)), hoopsview, SLOT (show_inner (bool)));
	connect (ui.checkBox_Inner_Vertices, SIGNAL (toggled (bool)), hoopsview, SLOT (show_inner_vertices (bool)));
	connect (ui.checkBox_Inner_Edges, SIGNAL (toggled (bool)), hoopsview, SLOT (show_inner_edges (bool)));
	connect (ui.checkBox_Inner_Faces, SIGNAL (toggled (bool)), hoopsview, SLOT (show_inner_faces (bool)));
	connect (ui.checkBox_Inner_Cells, SIGNAL (toggled (bool)), hoopsview, SLOT (show_inner_cells (bool)));
	connect (ui.checkBox_Inner_Vertices_Indices, SIGNAL (toggled (bool)), hoopsview, SLOT (show_inner_vertices_indices (bool)));
	connect (ui.checkBox_Inner_Edges_Indices, SIGNAL (toggled (bool)), hoopsview, SLOT (show_inner_edges_indices (bool)));
	connect (ui.checkBox_Inner_Faces_Indices, SIGNAL (toggled (bool)), hoopsview, SLOT (show_inner_faces_indices (bool)));
	connect (ui.checkBox_Inner_Cells_Indices, SIGNAL (toggled (bool)), hoopsview, SLOT (show_inner_cells_indices (bool)));
}

void MeshRenderControlWidget::render_wireframe ()
{
	ui.groupBox_Mesh_Boundary->setChecked (true);
	ui.groupBox_Mesh_Inner->setChecked (true);
	ui.checkBox_Boundary_Edges->setChecked (true);
	ui.checkBox_Boundary_Faces->setChecked (false);

	ui.checkBox_Inner_Edges->setChecked (true);
	ui.checkBox_Inner_Faces->setChecked (false);
}

void MeshRenderControlWidget::render_shaded ()
{
	ui.groupBox_Mesh_Boundary->setChecked (true);
	ui.groupBox_Mesh_Inner->setChecked (false);
	ui.checkBox_Boundary_Edges->setChecked (true);
	ui.checkBox_Boundary_Faces->setChecked (true);
}

void MeshRenderControlWidget::render_hiddenline ()
{

}
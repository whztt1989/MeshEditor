#ifndef MESHRENDERCONTROLWIDGET_H
#define MESHRENDERCONTROLWIDGET_H

#include <QWidget>
#include "ui_meshrendercontrolwidget.h"
#include "hoopsview.h"

class MeshRenderControlWidget : public QWidget
{
	Q_OBJECT

public:
	MeshRenderControlWidget(HoopsView *_hoopsview, QWidget *parent = 0);
	~MeshRenderControlWidget();
public:
	void render_wireframe ();
	void render_shaded ();
	void render_hiddenline ();
private slots:
private:
	void setup_actions ();
signals:
	void show_boundary (bool onoff);
	void show_boundary_vertices (bool onoff);
	void show_boundary_edges (bool onoff);
	void show_boundary_faces (bool onoff);
	void show_boundary_cells (bool onoff);
	void show_boundary_vertices_indices (bool onoff);
	void show_boundary_edges_indices (bool onoff);
	void show_boundary_faces_indices (bool onoff);
	void show_boundary_cells_indices (bool onoff);

	void show_inner (bool onoff);
	void show_inner_vertices (bool onoff);
	void show_inner_edges (bool onoff);
	void show_inner_faces (bool onoff);
	void show_inner_cells (bool onoff);
	void show_inner_vertices_indices (bool onoff);
	void show_inner_edges_indices (bool onoff);
	void show_inner_faces_indices (bool onoff);
	void show_inner_cells_indices (bool onoff);
private:
	Ui::MeshRenderControlWidget ui;
	HoopsView *hoopsview;
};

#endif // MESHRENDERCONTROLWIDGET_H

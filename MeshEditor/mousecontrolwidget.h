#ifndef MOUSECONTROLWIDGET_H
#define MOUSECONTROLWIDGET_H

#include <QWidget>
#include "ui_mousecontrolwidget.h"
#include "hoopsview.h"

class MeshEditWidget;
class MouseControlWidget : public QWidget
{
	Q_OBJECT

public:
	MouseControlWidget(HoopsView *_hoopsview, QWidget *parent = 0);
	~MouseControlWidget();
	std::unordered_set<OvmCeH> get_cut_left_chs () {return cut_left_chs;}
public slots:
	void start_selecting_vertices ();
	void start_selecting_edges ();
	void start_selecting_faces ();
	void start_camera_manipulate ();
private:
	void setup_actions ();
signals:
	void begin_selection_by_click ();
	void begin_selection_by_rectangle ();
	void begin_selection_by_polygon ();
	void begin_camera_manipulate ();
	

	void select_vertices (bool onoff);
	void select_edges (bool onoff);
	void select_faces (bool onoff);
	void select_cells (bool onoff);

	void opt_ok ();
private slots:
	void on_begin_selection ();
	void on_begin_camera_manipulate ();
	void on_begin_cutting ();
	void on_calc_cutting ();
	void on_restore_cutting ();
	void on_operate_ok ();
	void on_set_vertices_selectable ();
	void on_set_edges_selectable ();
	void on_set_faces_selectable ();
private:
	Ui::MouseControlWidget ui;
	HoopsView *hoopsview;
	MeshEditWidget *mesh_edit_widget;
	std::unordered_set<OvmCeH> cut_left_chs;
};

#endif // MOUSECONTROLWIDGET_H

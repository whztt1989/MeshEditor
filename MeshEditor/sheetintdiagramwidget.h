#ifndef SHEETINTDIAGRAMWIDGET_H
#define SHEETINTDIAGRAMWIDGET_H

#include <QWidget>
#include <QDialog>
#include "ui_sheetintdiagramwidget.h"
#include "MeshDefs.h"
#include "SheetIntDiagram.h"
#include "DualDefs.h"

class SheetIntDiagramViewer;
class SheetIntDiagramWidget : public QWidget
{
	Q_OBJECT

public:
	SheetIntDiagramWidget(QWidget *parent = 0);
	~SheetIntDiagramWidget();
public:
	void set_sheet_int_diagram (VolumeMesh *m, DualSheet *s, SheetIntDiagram *sid);
	void set_sheet_colors (std::map<DualSheet *, std::pair<Qt::GlobalColor, Qt::PenStyle> > *sc);
private slots:
	void on_show_sid_in_out_window ();
	void on_show_sheet_in_mesh ();
	void on_edge_clicked (SidEgH eh);
	void on_vertex_clicked (SidVeH vh);
signals:
	void show_sheet_in_mesh (DualSheet *s);
	void show_int_sheet_in_mesh (DualSheet *s);
	void show_face_in_mesh (OvmFaH fh);
	void show_cell_in_mesh (OvmCeH ch);
private:
	Ui::SheetIntDiagramWidget ui;
	VolumeMesh *mesh;
	DualSheet *sheet;
	SheetIntDiagram *sheet_int_diagram;
	SheetIntDiagramViewer *sid_viewer;
	std::map<DualSheet *, std::vector<SidEgH> > sid_polylines;
	std::map<DualSheet *, std::pair<Qt::GlobalColor, Qt::PenStyle> > *sheet_colors;
};

#endif // SHEETINTDIAGRAMWIDGET_H

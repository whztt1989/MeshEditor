#ifndef SHEETINTDIAGRAMSDASHBOARD_H
#define SHEETINTDIAGRAMSDASHBOARD_H

#include <QDialog>
#include "ui_sheetintdiagramsdashboard.h"
#include "MeshDefs.h"
#include "SheetIntDiagram.h"
#include "DualDefs.h"

class SheetIntDiagramsDashboard : public QDialog
{
	Q_OBJECT

public:
	SheetIntDiagramsDashboard(QWidget *parent = 0);
	~SheetIntDiagramsDashboard();
public:
	void set_sheet_set (VolumeMesh *m, SheetSet &ss);
	void set_quad_set (VolumeMesh *m, std::unordered_set<OvmFaH> &qs);
private slots:
	void on_show_sheet_in_mesh (DualSheet *s);
	void on_show_int_sheet_in_mesh (DualSheet *s);
	void on_show_face_in_mesh (OvmFaH fh);
	void on_show_cell_in_mesh (OvmCeH ch);
signals:
	void show_sheet_in_mesh (VolumeMesh *m, DualSheet *s);
	void show_int_sheet_in_mesh (VolumeMesh *m, DualSheet *s);
	void show_face_in_mesh (VolumeMesh *m, OvmFaH fh);
	void show_cell_in_mesh (VolumeMesh *m, OvmCeH ch);
private:
	Ui::SheetIntDiagramsDashboard ui;
	VolumeMesh *mesh;
	SheetSet sheet_set;
	std::vector<Qt::GlobalColor> all_sheet_colors;
	std::vector<Qt::PenStyle> all_sheet_line_styles;
	std::map<DualSheet *, std::pair<Qt::GlobalColor, Qt::PenStyle> > sheet_colors;
};

#endif // SHEETINTDIAGRAMSDASHBOARD_H

#include "StdAfx.h"
#include "sheetintdiagramwidget.h"
#include "sheetintdiagramviewer.h"
#include <QDialog>
#include <QVBoxLayout>
#include <hash_map>
#include "MeshDefs.h"
#include "DualDefs.h"

SheetIntDiagramWidget::SheetIntDiagramWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	connect (ui.pushButton_Show_SID_In_Out_Window, SIGNAL (clicked ()),
		SLOT (on_show_sid_in_out_window ()));
	connect (ui.pushButton_Show_Sheet_In_Mesh, SIGNAL (clicked ()),
		SLOT (on_show_sheet_in_mesh ()));
	setWindowFlags (windowFlags () | Qt::WindowMaximizeButtonHint);
	sid_viewer = NULL;
}

SheetIntDiagramWidget::~SheetIntDiagramWidget()
{

}

void SheetIntDiagramWidget::set_sheet_int_diagram (VolumeMesh *m, DualSheet *s, SheetIntDiagram *sid)
{
	if (sid_viewer)
		delete sid_viewer;

	mesh = m; sheet_int_diagram = sid; sheet = s;

	sid_viewer = new SheetIntDiagramViewer (this);
	connect (sid_viewer, SIGNAL(edge_clicked (SidEgH)), this, SLOT(on_edge_clicked(SidEgH)));
	connect (sid_viewer,SIGNAL(vertex_clicked (SidVeH)), this, SLOT(on_vertex_clicked(SidVeH)));
	sid_viewer->set_sheet_int_diagram (sid);
	ui.verticalLayout->addWidget (sid_viewer);
	ui.label_Sheet_Index->setText (QString("Sheet %1").arg (sheet->idx));
	
	OpenMesh::EPropHandleT<DualSheet*> OM_E_SHEET_PTR;
	sheet_int_diagram->get_property_handle (OM_E_SHEET_PTR, "sheetptr");

	for (auto e_it = sheet_int_diagram->edges_begin (); e_it != sheet_int_diagram->edges_end (); ++e_it)
	{
		if (sheet_int_diagram->is_boundary (*e_it))
			continue;
		DualSheet *tmp_sheet = sheet_int_diagram->property (OM_E_SHEET_PTR, *e_it);
		sid_viewer->get_edge_ptr (*e_it)->set_tooltips (QString ("Sheet %1").arg (tmp_sheet->idx));
		sid_polylines[tmp_sheet].push_back (*e_it);
	}
}

void SheetIntDiagramWidget::on_show_sid_in_out_window ()
{
	QDialog *dlg = new QDialog (qobject_cast<QWidget*> (parent ()));
	QVBoxLayout *vbox_layout = new QVBoxLayout;
	SheetIntDiagramViewer *sid_viewer_dlg = new SheetIntDiagramViewer (this);
	sid_viewer_dlg->set_sheet_int_diagram (sheet_int_diagram);
	vbox_layout->addWidget (sid_viewer_dlg);
	dlg->setLayout (vbox_layout);
	dlg->resize (800, 600);

	connect (sid_viewer_dlg,SIGNAL(edge_clicked (SidEgH)), this, SLOT(on_edge_clicked(SidEgH)));
	connect (sid_viewer_dlg,SIGNAL(vertex_clicked (SidVeH)), this, SLOT(on_vertex_clicked(SidVeH)));

	for (auto it = sid_polylines.begin (); it != sid_polylines.end (); ++it)
	{
		DualSheet *sheet = it->first;
		std::vector<SidEgH> ehs = it->second;
		auto locate = sheet_colors->find (sheet);
		assert (locate != sheet_colors->end ());

		for (auto e_it = ehs.begin (); e_it != ehs.end (); ++e_it)
		{
			SidEdge *sid_edge = sid_viewer_dlg->get_edge_ptr (*e_it);
			sid_edge->edge_color = locate->second.first;
			sid_edge->pen_style = locate->second.second;
			sid_edge->set_tooltips (QString ("Sheet %1").arg (sheet->idx));
			sid_edge->update ();
		}
	}

	dlg->show ();
}

void SheetIntDiagramWidget::set_sheet_colors (std::map<DualSheet *, std::pair<Qt::GlobalColor, Qt::PenStyle> > *sc)
{
	sheet_colors = sc;
	for (auto it = sid_polylines.begin (); it != sid_polylines.end (); ++it)
	{
		DualSheet *sheet = it->first;
		std::vector<SidEgH> ehs = it->second;
		auto locate = sheet_colors->find (sheet);
		assert (locate != sheet_colors->end ());

		for (auto e_it = ehs.begin (); e_it != ehs.end (); ++e_it)
		{
			SidEdge *sid_edge = sid_viewer->get_edge_ptr (*e_it);
			sid_edge->edge_color = locate->second.first;
			sid_edge->pen_style = locate->second.second;
			sid_edge->update ();
		}
	}
	
	QColor this_sheet_color = sheet_colors->find (sheet)->second.first;
	ui.label_Sheet_Index->setStyleSheet(QString ("QLabel {color : %1; }").arg (this_sheet_color.name ()));
}

void SheetIntDiagramWidget::on_show_sheet_in_mesh ()
{
	emit show_sheet_in_mesh (sheet);
}

void SheetIntDiagramWidget::on_edge_clicked (SidEgH eh)
{
	OpenMesh::EPropHandleT<DualSheet*> OM_E_SHEET_PTR;
	sheet_int_diagram->get_property_handle (OM_E_SHEET_PTR, "sheetptr");
	emit show_int_sheet_in_mesh(sheet_int_diagram->property (OM_E_SHEET_PTR, eh));
}

void SheetIntDiagramWidget::on_vertex_clicked (SidVeH vh)
{
	OpenMesh::VPropHandleT<OvmCeH> OM_V_CH;
	OpenMesh::VPropHandleT<OvmFaH> OM_V_FH;
	sheet_int_diagram->get_property_handle (OM_V_CH, "cellhandle");
	sheet_int_diagram->get_property_handle (OM_V_FH, "facehandle");
	if (mesh->InvalidCellHandle != sheet_int_diagram->property (OM_V_CH, vh))
		emit show_cell_in_mesh (sheet_int_diagram->property (OM_V_CH, vh));
	else if (mesh->InvalidFaceHandle != sheet_int_diagram->property (OM_V_FH, vh))
		emit show_face_in_mesh (sheet_int_diagram->property (OM_V_FH, vh));
}
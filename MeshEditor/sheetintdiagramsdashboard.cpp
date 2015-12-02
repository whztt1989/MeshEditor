#include "StdAfx.h"
#include "sheetintdiagramsdashboard.h"
#include "sheetintdiagramwidget.h"

SheetIntDiagramsDashboard::SheetIntDiagramsDashboard(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	setWindowFlags (windowFlags () | Qt::WindowMaximizeButtonHint);

	for (int i = 7; i < 19; ++i)
		all_sheet_colors.push_back (Qt::GlobalColor(i));

	all_sheet_line_styles.push_back (Qt::SolidLine);
	all_sheet_line_styles.push_back (Qt::DashLine);
	all_sheet_line_styles.push_back (Qt::DotLine);
	all_sheet_line_styles.push_back (Qt::DashDotLine);
	all_sheet_line_styles.push_back (Qt::DashDotDotLine);                        
}

SheetIntDiagramsDashboard::~SheetIntDiagramsDashboard()
{

}

void SheetIntDiagramsDashboard::set_sheet_set (VolumeMesh *m, SheetSet &ss)
{
	mesh = m; sheet_set = ss;
	int row_num = 0, col_num = 0, item_num = 0;
	std::set<SheetIntDiagramWidget*> sid_widgets;
	for (auto ss_it = sheet_set.begin (); ss_it != sheet_set.end (); ++ss_it)
	{
		DualSheet *sheet = *ss_it;
		SheetIntDiagram *sid = get_sheet_int_diagram (mesh, sheet);
		smooth_sheet_int_diagram (sid, 2.5 * sid->n_vertices (), 20);
		SheetIntDiagramWidget *sid_widget = new SheetIntDiagramWidget (this);
		sid_widget->set_sheet_int_diagram (mesh, sheet, sid);
		row_num = item_num / 3; col_num = item_num % 3;
		ui.gridLayout->addWidget (sid_widget, row_num, col_num);
		connect (sid_widget, SIGNAL(show_sheet_in_mesh (DualSheet*)), 
			this, SLOT (on_show_sheet_in_mesh (DualSheet*)));
		connect (sid_widget, SIGNAL(show_int_sheet_in_mesh (DualSheet*)), 
			this, SLOT (on_show_int_sheet_in_mesh (DualSheet*)));
		connect (sid_widget, SIGNAL(show_face_in_mesh (OvmFaH)), 
			this, SLOT (on_show_face_in_mesh (OvmFaH)));
		connect (sid_widget, SIGNAL(show_cell_in_mesh (OvmCeH)), 
			this, SLOT (on_show_cell_in_mesh (OvmCeH)));

		int sheet_line_style_idx = item_num / all_sheet_colors.size ();
		sheet_line_style_idx %= all_sheet_line_styles.size ();
		int sheet_color_idx = item_num % all_sheet_colors.size ();
		
		sheet_colors.insert (std::make_pair (sheet, 
			std::make_pair (all_sheet_colors[sheet_color_idx], all_sheet_line_styles[sheet_line_style_idx])));

		sid_widgets.insert (sid_widget);
		item_num++;
	}

	for (auto it = sid_widgets.begin (); it != sid_widgets.end (); ++it)
	{
		SheetIntDiagramWidget *sid_widget = *it;
		sid_widget->set_sheet_colors (&sheet_colors);
	}
}

void SheetIntDiagramsDashboard::set_quad_set (VolumeMesh *m, std::unordered_set<OvmFaH> &qs)
{

}

void SheetIntDiagramsDashboard::on_show_sheet_in_mesh (DualSheet *s)
{
	emit show_sheet_in_mesh (mesh, s);
}

void SheetIntDiagramsDashboard::on_show_int_sheet_in_mesh (DualSheet *s)
{
	emit show_int_sheet_in_mesh (mesh, s);
}

void SheetIntDiagramsDashboard::on_show_face_in_mesh (OvmFaH fh)
{
	emit show_face_in_mesh (mesh, fh);
}

void SheetIntDiagramsDashboard::on_show_cell_in_mesh (OvmCeH ch)
{
	emit show_cell_in_mesh (mesh, ch);
}
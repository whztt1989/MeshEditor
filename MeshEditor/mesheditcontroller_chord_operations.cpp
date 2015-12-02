#include "StdAfx.h"
#include "mesheditcontroller.h"
#include "hoopsview.h"
#include <QMessageBox>
#include <QInputDialog>

void MeshEditController::on_select_edges_for_co ()
{
	hoopsview->begin_select_by_click ();
	hoopsview->set_edges_selectable (true);
	hoopsview->show_mesh_faces (true);
}

void MeshEditController::on_select_edges_ok_for_co ()
{
	selected_ehs.clear ();
	if (!hoopsview->get_selected_elements (std::vector<OvmVeH>(), selected_ehs, std::vector<OvmFaH> ())){
		QMessageBox::warning (this, tr("警告"), tr("未选中任何网格边！"), QMessageBox::Ok);
		return;
	}
	if (selected_ehs.size () != 1){
		QMessageBox::warning (this, tr("警告"), tr("只能选中一条网格边！"), QMessageBox::Ok);
		return;
	}

	hoopsview->clear_selection ();
	hoopsview->begin_camera_manipulate ();

	auto eh = selected_ehs.front ();
	auto E_CHORD_PTR = mesh->request_edge_property<unsigned long> ("chordptr");
	DualChord *chord = (DualChord*)(E_CHORD_PTR[eh]);
	assert (chord);
	selected_chord = chord;
	QStringList sl;
	sl<<"red"<<"blue"<<"green"<<"pink"<<"black"<<"white";

	bool ok = false;
	QString color_str = QInputDialog::getItem (this, tr("选择颜色"), tr("Chord颜色"), sl, 0, false, &ok);

	if (ok)
		hoopsview->render_chord (selected_chord, color_str.toAscii ().data (), 8);
	else
		hoopsview->render_chord (selected_chord, "red", 8);
}
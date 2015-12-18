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
	auto E_CHORD_PTR = mesh->request_edge_property<unsigned int> ("chordptr");
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

void MeshEditController::on_show_chord_info_for_co ()
{
	if (!selected_chord)
		return;
	QString msg;
	msg = QString ("No. %1").arg (selected_chord->idx);
	QMessageBox::information (this, tr("Chord信息"), msg);
}

void MeshEditController::on_clear_selection_for_co ()
{
	if (selected_chord)
		hoopsview->derender_chord (selected_chord);
	selected_chord = NULL;
}

void MeshEditController::on_select_chord_by_idx_for_co ()
{
	auto chord_idx = ui.le_Chord_Idx_For_Co->text ().toInt ();
	ChordSet chord_set;
	JC::retrieve_chords (mesh, get_interface_quads (), chord_set);

	if (selected_chord)
		hoopsview->derender_chord (selected_chord);
	selected_chord = NULL;
	foreach (auto c, chord_set){
		if (c->idx == chord_idx){
			selected_chord = c;
			break;
		}
	}
	if (!selected_chord){
		QMessageBox::warning (this, tr("警告"), tr("没有这个chord！"));
		return;
	}

	hoopsview->render_chord (selected_chord);
}

void MeshEditController::on_select_edge_for_adjust_chord ()
{
	if (!mouse_controller){
		mouse_controller = new MouseControlWidget (hoopsview, this);
		mouse_controller->setWindowFlags (mouse_controller->windowFlags () | Qt::Tool);
	}
	connect (mouse_controller, SIGNAL (opt_ok()), SLOT (on_finish_adjusting_chord ()));
	mouse_controller->start_selecting_edges ();
	mouse_controller->show ();
	hoopsview->show_mesh_faces (true);
}

void MeshEditController::on_select_as_1st_edge_for_chord ()
{
	selected_ehs.clear ();
	if (!hoopsview->get_selected_elements (std::vector<OvmVeH>(), selected_ehs, std::vector<OvmFaH> ())){
		QMessageBox::warning (this, tr("警告"), tr("未选中任何网格边！"), QMessageBox::Ok);
		return;
	}
	if (selected_ehs.size () != 1){
		QMessageBox::warning (this, tr("警告"), tr("只能选择一条网格边！"), QMessageBox::Ok);
		return;
	}
	first_eh_for_adjusting_chord = selected_ehs.front ();
	hoopsview->derender_mesh_groups ("chord adjusting", "1st edge");
	//render
	MeshRenderOptions render_options;
	auto group = new VolumeMeshElementGroup (mesh, "chord adjusting", "1st edge");
	group->ehs.insert (first_eh_for_adjusting_chord);
	render_options.edge_color = "red";
	render_options.edge_width = 12;
	hoopsview->render_mesh_group (group, render_options);
	hoopsview->set_mesh_group_selectability (group, false, false, false);

	
}

void MeshEditController::on_select_as_2nd_edge_for_chord ()
{
	selected_ehs.clear ();
	if (!hoopsview->get_selected_elements (std::vector<OvmVeH>(), selected_ehs, std::vector<OvmFaH> ())){
		QMessageBox::warning (this, tr("警告"), tr("未选中任何网格边！"), QMessageBox::Ok);
		return;
	}
	if (selected_ehs.size () != 1){
		QMessageBox::warning (this, tr("警告"), tr("只能选择一条网格边！"), QMessageBox::Ok);
		return;
	}
	second_eh_for_adjusting_chord = selected_ehs.front ();
	hoopsview->derender_mesh_groups ("chord adjusting", "2nd edge");

	MeshRenderOptions render_options;
	auto group = new VolumeMeshElementGroup (mesh, "chord adjusting", "2nd edge");
	group->ehs.insert (second_eh_for_adjusting_chord);
	render_options.edge_color = "blue";
	render_options.edge_width = 12;
	hoopsview->render_mesh_group (group, render_options);
	hoopsview->set_mesh_group_selectability (group, false, false, false);
}

void MeshEditController::on_finish_adjusting_chord ()
{
	disconnect (mouse_controller, SIGNAL (opt_ok()), this, SLOT (on_finish_adjusting_chord ()));

	if (first_eh_for_adjusting_chord == mesh->InvalidEdgeHandle || second_eh_for_adjusting_chord == mesh->InvalidEdgeHandle){
		QMessageBox::warning (this, tr("警告"), tr("两条边有无效边！"), QMessageBox::Ok);
		on_select_edge_for_adjust_chord ();
		return;
	}
	if (JC::get_common_face_handle (mesh, first_eh_for_adjusting_chord, second_eh_for_adjusting_chord) == mesh->InvalidFaceHandle){
		QMessageBox::warning (this, tr("警告"), tr("两条边必须为同一个四边形的拓扑平行边！"), QMessageBox::Ok);
		on_select_edge_for_adjust_chord ();
		return;
	}
	auto E_CHORD_PTR = mesh->request_edge_property<unsigned int> ("chordptr");
	DualChord *chord = (DualChord*)(E_CHORD_PTR[first_eh_for_adjusting_chord]),
		*chord2 = (DualChord*)(E_CHORD_PTR[second_eh_for_adjusting_chord]);

	if (chord != chord2){
		QMessageBox::warning (this, tr("警告"), tr("两条边不在同一个chord上！"), QMessageBox::Ok);
		on_select_edge_for_adjust_chord ();
		return;
	}

	if (!chord->is_closed){
		QMessageBox::warning (this, tr("警告"), tr("当前选中的chord不是环形chord！"), QMessageBox::Ok);
		on_select_edge_for_adjust_chord ();
		return;
	}
	auto fCycleChord = [&] (DualChord *chord){
		chord->ordered_ehs.erase (chord->ordered_ehs.begin ());
		chord->ordered_ehs.push_back (chord->ordered_ehs.front ());
		chord->ordered_fhs.push_back (chord->ordered_fhs.front ());
		chord->ordered_fhs.erase (chord->ordered_fhs.begin ());
	};
	auto fReverseChord = [&] (DualChord *chord){
		std::reverse (chord->ordered_ehs.begin () + 1, chord->ordered_ehs.end () - 1);
		std::reverse (chord->ordered_fhs.begin (), chord->ordered_fhs.end ());
	};

	bool adjust_ok = false;
	for (int i = 0; i != chord->ordered_ehs.size () - 1; ++i){
		if (chord->ordered_ehs[0] == first_eh_for_adjusting_chord && 
			chord->ordered_ehs[1] == second_eh_for_adjusting_chord){
			adjust_ok = true;
			break;
		}else{
			fCycleChord (chord);
		}
	}
	if (!adjust_ok){
		fReverseChord (chord);
		for (int i = 0; i != chord->ordered_ehs.size () - 1; ++i){
			if (chord->ordered_ehs[0] == first_eh_for_adjusting_chord && 
				chord->ordered_ehs[1] == second_eh_for_adjusting_chord){
					adjust_ok = true;
					break;
			}else{
				fCycleChord (chord);
			}
		}
	}

	if (!adjust_ok){
		QMessageBox::warning (this, tr("警告"), tr("发生了奇怪的错误！无法调整成功！"), QMessageBox::Ok);
		return;
	}

	QMessageBox::information (this, tr("提示"), tr("调整成功！"));
	hoopsview->derender_mesh_groups ("chord adjusting", "1st edge");
	hoopsview->derender_mesh_groups ("chord adjusting", "2nd edge");
	hoopsview->clear_selection ();
	mouse_controller->start_camera_manipulate ();

}
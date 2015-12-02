#include "StdAfx.h"
#include "groupcontrolwidget.h"
#include "FuncDefs.h"
#include <QFileDialog>
#include <QFile>
#include <QDataStream>

GroupControlWidget::GroupControlWidget(HoopsView *_hoopsview, QWidget *parent)
	: hoopsview (_hoopsview), QWidget(parent)
{
	ui.setupUi(this);
	setup_actions ();
}

GroupControlWidget::~GroupControlWidget()
{

}

void GroupControlWidget::setup_actions ()
{
	connect (ui.pushButton_Refresh, SIGNAL (clicked ()), SLOT (on_fresh ()));
	connect (ui.pushButton_Delete, SIGNAL (clicked ()), SLOT (on_delete ()));
	connect (ui.pushButton_Setting, SIGNAL (clicked ()), SLOT (on_setting ()));
	connect (ui.pushButton_Clear_Highlights, SIGNAL (clicked ()), SLOT (on_clear_highlight ()));
	connect (ui.listWidget_Groups, SIGNAL (itemSelectionChanged()), SLOT (on_selection_changed ()));
	connect (ui.listWidget_Groups, SIGNAL (itemChanged(QListWidgetItem *)), SLOT (on_item_changed (QListWidgetItem*)));
	connect (ui.pushButton_Calc_Irre_Degree, SIGNAL (clicked()), SLOT (on_calc_irre_degree ()));
	connect (ui.pushButton_Save, SIGNAL (clicked ()), SLOT (on_save ()));
	connect (ui.pushButton_Hide_All, SIGNAL (clicked ()), SLOT (on_hide_all ()));
	connect (ui.pushButton_Show_All, SIGNAL (clicked ()), SLOT (on_show_all ()));
	connect (ui.pushButton_Calc_Int_Line, SIGNAL (clicked ()), SLOT (on_calc_int_line ()));
	connect (ui.pushButton_Show_Boundary_Chord, SIGNAL (clicked ()), SLOT (on_show_chord_on_boundary ()));
	connect (ui.pushButton_Show_Info, SIGNAL (clicked ()), SLOT (on_show_info ()));
}

void GroupControlWidget::fresh_group_list ()
{
	std::vector<VolumeMeshElementGroup*> groups, invisibile_groups, highlighted_groups;
	hoopsview->get_mesh_groups (groups, invisibile_groups, highlighted_groups);

	auto all_items = ui.listWidget_Groups->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard);
	foreach (auto item, all_items){
		auto group = (VolumeMeshElementGroup*)(item->data (Qt::UserRole).toUInt ());
		if (!JC::contains (groups, group) && !JC::contains (invisibile_groups, group) && !JC::contains (highlighted_groups, group)){
			ui.listWidget_Groups->removeItemWidget (item);
			delete item;
		}
		if (JC::contains (groups, group))
			groups.erase (std::find (groups.begin (), groups.end (), group));
		if (JC::contains (invisibile_groups, group))
			invisibile_groups.erase (std::find (invisibile_groups.begin (), invisibile_groups.end (), group));
		if (JC::contains (highlighted_groups, group))
			highlighted_groups.erase (std::find (highlighted_groups.begin (), highlighted_groups.end (), group));
	}

	auto fInsertItem = [&] (VolumeMeshElementGroup *group)->QListWidgetItem*{
		QListWidgetItem *new_item = new QListWidgetItem (group->type + " : " + group->name);
		new_item->setFlags (new_item->flags () | Qt::ItemIsUserCheckable);
		new_item->setData (Qt::UserRole, QVariant (unsigned int (group)));
		new_item->setData (Qt::UserRole + 1, QVariant (false));
		new_item->setCheckState (Qt::Unchecked);
		ui.listWidget_Groups->addItem (new_item);
		return new_item;
	};
	foreach (auto group, groups){
		auto item = fInsertItem (group);
		item->setCheckState (Qt::Checked);
	}

	foreach (auto group, highlighted_groups){
		auto item = fInsertItem (group);
		item->setSelected (true);
		item->setCheckState (Qt::Checked);
	}

	foreach (auto group, invisibile_groups){
		auto item = fInsertItem (group);
		item->setCheckState (Qt::Unchecked);
	}
}

void GroupControlWidget::on_fresh ()
{
	fresh_group_list ();
}

void GroupControlWidget::on_delete ()
{
	auto selected_groups = ui.listWidget_Groups->selectedItems ();
	foreach (auto item, selected_groups){
		auto group = (VolumeMeshElementGroup*)(item->data (Qt::UserRole).toUInt ());
		hoopsview->derender_one_mesh_group (group);
		if (group->delete_when_unrender)
			delete group;
		ui.listWidget_Groups->removeItemWidget (item);
		delete item;
	}
}

void GroupControlWidget::on_setting ()
{

}

void GroupControlWidget::on_clear_highlight ()
{
	auto selected_groups = ui.listWidget_Groups->selectedItems ();
	foreach (auto item, selected_groups){
		auto group = (VolumeMeshElementGroup*)(item->data (Qt::UserRole).toUInt ());
		hoopsview->dehighlight_mesh_group (group);
		item->setData (Qt::UserRole + 1, QVariant (false));
		item->setSelected (false);
	}
}

void GroupControlWidget::on_selection_changed ()
{
	auto selected_groups = ui.listWidget_Groups->selectedItems ();
	QList<QListWidgetItem*> all_groups;
	for (int i = 0; i != ui.listWidget_Groups->count (); ++i){
		auto item = ui.listWidget_Groups->item (i);
		if (item->data (Qt::UserRole + 1).toBool ()){
			if (!selected_groups.contains (item)){
				auto group = (VolumeMeshElementGroup*)(item->data (Qt::UserRole).toUInt ());
				hoopsview->dehighlight_mesh_group (group);
				item->setData (Qt::UserRole + 1, QVariant (false));
			}
		}else{
			if (selected_groups.contains (item)){
				auto group = (VolumeMeshElementGroup*)(item->data (Qt::UserRole).toUInt ());
				hoopsview->highlight_mesh_group (group);
				item->setData (Qt::UserRole + 1, QVariant (true));
			}
		}
	}
}

void GroupControlWidget::on_item_changed (QListWidgetItem *item)
{
	if (!item) return;
	if (item->checkState () == Qt::Checked){
		auto group = (VolumeMeshElementGroup*)(item->data (Qt::UserRole).toUInt ());
		hoopsview->show_mesh_group (group, true);
	}else{
		auto group = (VolumeMeshElementGroup*)(item->data (Qt::UserRole).toUInt ());
		hoopsview->show_mesh_group (group, false);
	}
}

int airregular_degree (VolumeMesh *mesh, const std::set<OvmCeH> &chs, OvmEgH eh){
	int irre_degree = 0;
	std::unordered_set<OvmCeH> adj_chs;
	JC::get_adj_hexas_around_edge (mesh, eh, adj_chs);
	int in_count = 0, out_count = 0;
	foreach (auto &ch, adj_chs){
		if (JC::contains (chs, ch)) in_count++;
	}
	out_count = adj_chs.size () - in_count;
	if (in_count == 0 || in_count == adj_chs.size ()) return 0;

	if (mesh->is_boundary (eh)){
		irre_degree += std::abs(in_count - 1) + std::abs (out_count - 1) - std::abs ((int)(adj_chs.size () - 2));
	}else{
		irre_degree += std::abs(in_count - 2) + std::abs (out_count - 2) - std::abs ((int)(adj_chs.size () - 4));
	}
	return irre_degree;
}

int awhole_irregular_degree (VolumeMesh *mesh, std::unordered_set<OvmFaH> &fhs){
	std::hash_map<OvmEgH, std::vector<OvmFaH> > ehs_fhs_mapping;
	foreach (auto &fh, fhs){
		std::unordered_set<OvmEgH> adj_ehs;
		JC::get_adj_edges_around_face (mesh, fh, adj_ehs);
		foreach (auto &eh, adj_ehs){
			ehs_fhs_mapping[eh].push_back (fh);
		}
	}

	int irre_degree = 0;
	foreach (auto &p, ehs_fhs_mapping){
		if (p.second.size () == 1){
			irre_degree += JC::edge_valence_change (mesh, p.first, p.second.front ());
		}else if (p.second.size () == 2){
			irre_degree += JC::edge_valence_change (mesh, p.first, p.second.front (), p.second.back ());
		}
	}

	return irre_degree;
}

void GroupControlWidget::on_calc_irre_degree ()
{
	auto selected_groups = ui.listWidget_Groups->selectedItems ();
	foreach (auto item, selected_groups){
		auto group = (VolumeMeshElementGroup*)(item->data (Qt::UserRole).toUInt ());
		auto irre_degree = awhole_irregular_degree (group->mesh, group->fhs);
		QMessageBox::information (NULL, "Irregular degree:", QString ("%1 Degree:%2").arg (group->name)
			.arg (irre_degree));
	}
}

void GroupControlWidget::on_save ()
{
	auto selected_groups = ui.listWidget_Groups->selectedItems ();
	if (selected_groups.size () > 1){
		QMessageBox::information (NULL, "Only one!", "Only One!");return;
	}
	
	auto group = (VolumeMeshElementGroup*)(selected_groups.first ()->data (Qt::UserRole).toUInt ());

	QString file_path = QFileDialog::getSaveFileName (this, "选择文件");
	if (file_path != ""){
		QFile file (file_path);
		file.open (QIODevice::WriteOnly);
		QDataStream out (&file);
		QVector<int> vs, es, fs, cs;
		foreach (auto &vh, group->vhs) vs.push_back (vh.idx ());
		foreach (auto &eh, group->ehs) es.push_back (eh.idx ());
		foreach (auto &fh, group->fhs) fs.push_back (fh.idx ());
		foreach (auto &ch, group->chs) cs.push_back (ch.idx ());
		out<<vs<<es<<fs<<cs;
		file.close ();
		QMessageBox::information (NULL, "保存成功!", "Success!");
	}
}

void GroupControlWidget::on_hide_all ()
{
	int item_count = ui.listWidget_Groups->count ();
	for (int i = 0; i != item_count; ++i){
		auto cur_item = ui.listWidget_Groups->item (i);
		auto group = (VolumeMeshElementGroup*)(cur_item->data (Qt::UserRole).toUInt ());
		cur_item->setCheckState (Qt::Unchecked);
		hoopsview->show_mesh_group (group, false);
	}
}

void GroupControlWidget::on_show_all ()
{
	int item_count = ui.listWidget_Groups->count ();
	for (int i = 0; i != item_count; ++i){
		auto cur_item = ui.listWidget_Groups->item (i);
		auto group = (VolumeMeshElementGroup*)(cur_item->data (Qt::UserRole).toUInt ());
		cur_item->setCheckState (Qt::Checked);
		hoopsview->show_mesh_group (group, true);
	}
}

void GroupControlWidget::on_calc_int_line ()
{
	auto selected_groups = ui.listWidget_Groups->selectedItems ();
	if (selected_groups.size () > 1){
		QMessageBox::information (NULL, "Only one!", "Only One!");return;
	}

	auto group = (VolumeMeshElementGroup*)(selected_groups.first ()->data (Qt::UserRole).toUInt ());

	if (group->fhs.empty ()){
		QMessageBox::information (NULL, "fhs empty!", "empty!");return;
	}
	std::hash_map<OvmEgH, int> ehs_int_counting;
	foreach (auto &fh, group->fhs){
		std::unordered_set<OvmEgH> adj_ehs;
		JC::get_adj_edges_around_face (group->mesh, fh, adj_ehs);
		foreach (auto &eh, adj_ehs)
			ehs_int_counting[eh]++;
	}

	std::unordered_set<OvmEgH> int_path, inner_ehs, bound_ehs;
	foreach (auto &p, ehs_int_counting){
		if (p.second == 4) int_path.insert (p.first);
		else if (p.second == 2) inner_ehs.insert (p.first);
		else bound_ehs.insert (p.first);
	}
	auto tmp_group = new VolumeMeshElementGroup (group->mesh, "gm", "int edges");
	tmp_group->ehs = int_path;
	MeshRenderOptions render_options;
	render_options.edge_color = "blue";
	render_options.edge_width = 15;
	hoopsview->render_mesh_group (tmp_group, render_options);

	tmp_group = new VolumeMeshElementGroup (group->mesh, "gm", "inner edges");
	tmp_group->ehs = inner_ehs;
	render_options.edge_color = "black";
	render_options.edge_width = 3;
	hoopsview->render_mesh_group (tmp_group, render_options);

	tmp_group = new VolumeMeshElementGroup (group->mesh, "gm", "bound edges");
	tmp_group->ehs = bound_ehs;
	render_options.edge_color = "black";
	render_options.edge_width = 3;
	hoopsview->render_mesh_group (tmp_group, render_options);
}

void GroupControlWidget::on_show_chord_on_boundary ()
{
	auto selected_groups = ui.listWidget_Groups->selectedItems ();
	if (selected_groups.size () > 1){
		QMessageBox::information (NULL, "Only one!", "Only One!");return;
	}

	auto group = (VolumeMeshElementGroup*)(selected_groups.first ()->data (Qt::UserRole).toUInt ());

	auto mesh = group->mesh;
	std::unordered_set<OvmFaH> other_bound_fhs, tmp_bound_fhs, sheet_bound_fhs;
	JC::collect_boundary_element (mesh, group->chs, NULL, NULL, &tmp_bound_fhs);
	foreach (auto &fh, tmp_bound_fhs){
		if (mesh->is_boundary (fh)) sheet_bound_fhs.insert (fh);
	}

	for (auto bf_it = mesh->bf_iter (); bf_it; ++bf_it){
		if (!JC::contains (sheet_bound_fhs, *bf_it)) other_bound_fhs.insert (*bf_it);
	}

	auto tmp_group = new VolumeMeshElementGroup (group->mesh, "gm", "other boundary quads");
	tmp_group->fhs = other_bound_fhs;
	MeshRenderOptions render_options;
	render_options.face_color = "light grey";
	hoopsview->render_mesh_group (tmp_group, render_options);

	tmp_group = new VolumeMeshElementGroup (group->mesh, "gm", "sheet boundary quads");
	tmp_group->fhs = sheet_bound_fhs;
	render_options.face_color = "red";
	hoopsview->render_mesh_group (tmp_group, render_options);
}

void GroupControlWidget::on_show_info ()
{
	auto selected_groups = ui.listWidget_Groups->selectedItems ();
	if (selected_groups.size () > 1){
		QMessageBox::information (NULL, "Only one!", "Only One!");return;
	}

	auto group = (VolumeMeshElementGroup*)(selected_groups.first ()->data (Qt::UserRole).toUInt ());
	QString str_info = QString ("Quad #%1, Hex #%2").arg (group->fhs.size ())
		.arg (group->chs.size ());
	QMessageBox::information (NULL, "Group Info", str_info);
}
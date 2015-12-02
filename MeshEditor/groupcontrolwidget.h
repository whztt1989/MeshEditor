#ifndef GROUPCONTROLWIDGET_H
#define GROUPCONTROLWIDGET_H

#include <QWidget>
#include "ui_groupcontrolwidget.h"
#include "MeshDefs.h"
#include "hoopsview.h"

class GroupControlWidget : public QWidget
{
	Q_OBJECT

public:
	GroupControlWidget(HoopsView *_hoopsview, QWidget *parent = 0);
	~GroupControlWidget();
	void fresh_group_list ();
private:
	void setup_actions ();
private slots:
	void on_fresh ();
	void on_delete ();
	void on_clear_highlight ();
	void on_setting ();
	void on_selection_changed ();
	void on_item_changed (QListWidgetItem *item);
	void on_calc_irre_degree ();
	void on_save ();
	void on_hide_all ();
	void on_show_all ();
	void on_calc_int_line ();
	void on_show_chord_on_boundary ();
	void on_show_info ();
signals:

private:
	Ui::GroupControlWidget ui;
	HoopsView *hoopsview;
};

#endif // GROUPCONTROLWIDGET_H

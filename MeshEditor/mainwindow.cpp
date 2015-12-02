#include "stdafx.h"
#include "mainwindow.h"
#include "FuncDefs.h"
#include "mesheditwidget.h"
#include "ManualMeshMatchWidget.h"
#include <QFileDialog>
#include "meshcut.h"

MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
	setup_actions ();
	ui.tabWidget->setTabsClosable (true);
	ui.tabWidget->setMovable (true);
	ui.tabWidget->hide ();
}

MainWindow::~MainWindow()
{

}

void MainWindow::setup_actions ()
{
	connect (ui.tabWidget, SIGNAL (currentChanged(int)), SLOT (on_tab_changed (int)));
	connect (ui.tabWidget, SIGNAL (tabCloseRequested(int)), SLOT (on_tab_close_requested (int)));
	connect (ui.action_New_MeshEdit, SIGNAL (triggered ()), SLOT (on_new_mesh_edit ()));
	connect (ui.action_New_ManualMeshMatch, SIGNAL (triggered ()), SLOT (on_new_manual_mesh_match ()));
	connect (ui.action_New_MeshCut, SIGNAL (triggered ()), SLOT (on_new_mesh_cut ()));
}

void MainWindow::on_tab_changed (int index)
{
	QList<QToolBar *> prev_toolbars = findChildren<QToolBar *>();
	foreach (auto toolbar, prev_toolbars){
		//removeToolBar (toolbar);
		toolbar->hide ();
	}
	//当index为-1时，表示没有任何tab窗口了
	if (index == -1){
		ui.tabWidget->hide ();
		return;
	}

	auto widget = ui.tabWidget->widget (index);
	auto object_name = widget->objectName ();
	std::vector<std::vector<QToolBar*> > new_toolbars;
	if (object_name == "MeshEditWidget"){
		new_toolbars = ((MeshEditWidget*)widget)->get_toolbars ();
	}else if (object_name == "MeshMatchWidget"){
		new_toolbars = ((ManualMeshMatchWidget*)widget)->get_toolbars ();
	}
	foreach (auto one_line_toolbar, new_toolbars){
		foreach (auto toolbar, one_line_toolbar){
			if (!JC::contains (widgets_in_tabwidget, widget))
				addToolBar (toolbar);
			toolbar->show ();
		}
		if (!JC::contains (widgets_in_tabwidget, widget))
			addToolBarBreak (Qt::TopToolBarArea);
	}
	widgets_in_tabwidget.insert (widget);
}

void MainWindow::on_tab_close_requested (int index)
{
	auto widget = ui.tabWidget->widget (index);
	ui.tabWidget->removeTab (index);
	delete widget;
	widgets_in_tabwidget.erase (widget);
}

void MainWindow::on_new_mesh_edit ()
{
	auto mesh_edit_widget = new MeshEditWidget (this);
	mesh_edit_widget->setObjectName ("MeshEditWidget");
	ui.tabWidget->addTab (mesh_edit_widget, "MeshEdit");
	ui.tabWidget->setCurrentWidget (mesh_edit_widget);
	ui.tabWidget->show ();
}

void MainWindow::on_new_manual_mesh_match ()
{
	auto manual_mesh_match_widget = new ManualMeshMatchWidget (this);
	manual_mesh_match_widget->setObjectName ("MMMWidget");
	ui.tabWidget->addTab (manual_mesh_match_widget, "ManualMeshMatch");
	ui.tabWidget->setCurrentWidget (manual_mesh_match_widget);
	ui.tabWidget->show ();
}

void MainWindow::on_new_mesh_cut ()
{
	auto mesh_cut = new MeshCut (this);
	mesh_cut->setObjectName ("MeshCut");
	ui.tabWidget->addTab (mesh_cut, "MeshCut");
	ui.tabWidget->setCurrentWidget (mesh_cut);
	ui.tabWidget->show ();
}
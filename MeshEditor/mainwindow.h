#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <map>
#include "ui_mainwindow.h"
#include "mesheditwidget.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
	~MainWindow();
private:
	void setup_actions ();
private slots:
	void on_tab_changed (int index);
	void on_tab_close_requested (int index);
	void on_new_mesh_edit ();
	void on_new_manual_mesh_match ();
	void on_new_mesh_cut ();
private:
	Ui::MainWindowClass ui;
	std::set<QWidget*> widgets_in_tabwidget;
};

#endif // MAINWINDOW_H

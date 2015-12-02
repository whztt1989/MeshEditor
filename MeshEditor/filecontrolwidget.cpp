#include "StdAfx.h"
#include "filecontrolwidget.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QListWidgetItem>

FileControlWidget::FileControlWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setup_actions ();
}

FileControlWidget::~FileControlWidget()
{

}

void FileControlWidget::set_directory_and_filters (QString _dir_str, QString _filters_str)
{
	directory_str = _dir_str; filters_str = _filters_str;
}

void FileControlWidget::file_accepted (QString file_path){
	QFileInfo file_info (file_path);
	QString base_name = file_info.baseName ();
	QString str = tr("文件名：") + base_name;
	file_path_str = file_path;
	ui.pushButton_Open->setEnabled (false);
	ui.pushButton_Save->setEnabled (true);
	ui.pushButton_Save_As->setEnabled (true);
	ui.pushButton_Close->setEnabled (true);
}

void FileControlWidget::setup_actions ()
{
	connect (ui.pushButton_Open, SIGNAL (clicked ()), SLOT (on_open_file ()));
	connect (ui.pushButton_Save, SIGNAL (clicked ()), SIGNAL (save_file ()));
	connect (ui.pushButton_Save_As, SIGNAL (clicked ()), SLOT (on_save_file_as ()));
	connect (ui.pushButton_Close, SIGNAL (clicked ()), SLOT (on_close_file ()));
}

void FileControlWidget::on_open_file ()
{
	QString file_path = QFileDialog::getOpenFileName (this, tr("打开文件"), directory_str, filters_str);
	if (file_path != ""){
		emit open_file (file_path);
	}
}

void FileControlWidget::on_save_file_as ()
{
	QString file_path = QFileDialog::getSaveFileName (this, tr("另存为"), file_path_str, filters_str);
	if (file_path != ""){
		emit save_file_as (file_path);
	}
}

void FileControlWidget::on_close_file ()
{
	ui.pushButton_Open->setEnabled (true);
	ui.pushButton_Save->setEnabled (false);
	ui.pushButton_Save_As->setEnabled (false);
	ui.pushButton_Close->setEnabled (false);
	emit file_close ();
}
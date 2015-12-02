#ifndef FILECONTROLWIDGET_H
#define FILECONTROLWIDGET_H

#include <QWidget>
#include "ui_filecontrolwidget.h"
#include <QListWidget>

class FileControlWidget : public QWidget
{
	Q_OBJECT

public:
	FileControlWidget(QWidget *parent = 0);
	~FileControlWidget();
public:
	void set_directory_and_filters (QString _dir_str, QString _filters_str);
	void file_accepted (QString file_path);
private:
	void setup_actions ();
private slots:
	void on_open_file ();
	void on_save_file_as ();
	void on_close_file ();
signals:
	void open_file (QString file_path);
	void save_file ();
	void save_file_as (QString file_path);
	void file_close ();
private:
	Ui::FileControlWidget ui;
	QString directory_str, filters_str;
	QString file_path_str;
};

#endif // FILECONTROLWIDGET_H

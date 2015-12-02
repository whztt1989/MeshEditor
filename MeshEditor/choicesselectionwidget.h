#ifndef CHOICESSELECTIONWIDGET_H
#define CHOICESSELECTIONWIDGET_H

#include <QWidget>
#include "ui_choicesselectionwidget.h"

class ChoicesSelectionWidget : public QWidget
{
	Q_OBJECT

public:
	ChoicesSelectionWidget(QWidget *parent = 0);
	~ChoicesSelectionWidget();
public:
	void setChoicesNum (int num);
	void setChoicesLabels (QStringList labels);
	void setChoices (int idx, QStringList choices);
signals:
	void choice1_changed (int choice_idx);
	void choice1_changed (const QString &choice_txt);
	void choice2_changed (int choice_idx);
	void choice2_changed (const QString &choice_txt);
	void choice3_changed (int choice_idx);
	void choice3_changed (const QString &choice_txt);
	void choice4_changed (int choice_idx);
	void choice4_changed (const QString &choice_txt);
	void choosing_ok ();
private slots:
	void on_choosing_ok ();
private:
	Ui::ChoicesSelectionWidget ui;
	std::vector<QLabel*> labels;
	std::vector<QComboBox*> comboxs;
};

#endif // CHOICESSELECTIONWIDGET_H

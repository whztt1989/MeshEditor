#include "StdAfx.h"
#include "choicesselectionwidget.h"
#include <QMessageBox>

ChoicesSelectionWidget::ChoicesSelectionWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	labels.push_back (ui.lb_Choices1);
	labels.push_back (ui.lb_Choices2);
	labels.push_back (ui.lb_Choices3);
	labels.push_back (ui.lb_Choices4);

	comboxs.push_back (ui.cb_Choices1);
	comboxs.push_back (ui.cb_Choices2);
	comboxs.push_back (ui.cb_Choices3);
	comboxs.push_back (ui.cb_Choices4);

	connect (comboxs[0], SIGNAL (currentIndexChanged (int)), SIGNAL (choice1_changed (int)));
	connect (comboxs[0], SIGNAL (currentIndexChanged (const QString &)), SIGNAL (choice1_changed (const QString &)));

	connect (comboxs[1], SIGNAL (currentIndexChanged (int)), SIGNAL (choice2_changed (int)));
	connect (comboxs[1], SIGNAL (currentIndexChanged (const QString &)), SIGNAL (choice2_changed (const QString &)));

	connect (comboxs[2], SIGNAL (currentIndexChanged (int)), SIGNAL (choice3_changed (int)));
	connect (comboxs[2], SIGNAL (currentIndexChanged (const QString &)), SIGNAL (choice3_changed (const QString &)));

	connect (comboxs[3], SIGNAL (currentIndexChanged (int)), SIGNAL (choice4_changed (int)));
	connect (comboxs[3], SIGNAL (currentIndexChanged (const QString &)), SIGNAL (choice4_changed (const QString &)));

	connect (ui.pb_OK, SIGNAL (clicked ()), SLOT (on_choosing_ok ()));
}

ChoicesSelectionWidget::~ChoicesSelectionWidget()
{

}

void ChoicesSelectionWidget::setChoicesNum (int num)
{
	for (int i = 0; i != 4; ++i){
		labels[i]->hide ();
		comboxs[i]->hide ();
	}
	if (num < 1 || num > 4){
		QMessageBox::warning (this, tr("选项数目错误！"), tr("当前只能设置最多4个选项！"));
		return;
	}	

	
	for (int i = 0; i != num; ++i){
		labels[i]->show ();
		comboxs[i]->show ();
		comboxs[i]->clear ();
	}
}

void ChoicesSelectionWidget::setChoicesLabels (QStringList labels_text)
{
	for (int i = 0; i != labels_text.size (); ++i)
		labels[i]->setText (labels_text[i]);
}

void ChoicesSelectionWidget::setChoices (int idx, QStringList choices)
{
	comboxs[idx]->clear ();
	comboxs[idx]->addItems (choices);
}

void ChoicesSelectionWidget::on_choosing_ok ()
{
	emit choosing_ok ();
	hide ();
}
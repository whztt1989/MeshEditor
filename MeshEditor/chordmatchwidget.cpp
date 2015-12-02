#include "StdAfx.h"
#include "chordmatchwidget.h"

ChordMatchWidget::ChordMatchWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	connect (ui.listWidget_Chord_Pairs, SIGNAL (itemEntered (QListWidgetItem *)),
		SLOT (on_item_entered (QListWidgetItem*)));
	connect (ui.pushButton_Delete_Pair, SIGNAL (clicked ()),
		SLOT (on_delete_pair ()));
}

ChordMatchWidget::~ChordMatchWidget()
{

}

void ChordMatchWidget::set_matched_pairs (ChordPairs &matched_pairs)
{
	ui.listWidget_Chord_Pairs->clear ();
	ui.listWidget_Chord_Pairs->setMouseTracking (true);
	foreach (auto &p, matched_pairs){
		QListWidgetItem *item = new QListWidgetItem (QString ("chord %1 <-> chord %2")
			.arg (p.first->idx).arg (p.second->idx));
		item->setCheckState (Qt::Unchecked);
		item->setData (Qt::UserRole + 1, (uint)(p.first));
		item->setData (Qt::UserRole + 2, (uint)(p.second));
		ui.listWidget_Chord_Pairs->addItem (item);
	}
}

void ChordMatchWidget::on_item_entered (QListWidgetItem * item)
{
	if (item == NULL) return;
	auto chord1 = item->data (Qt::UserRole + 1).toUInt ();
	auto chord2 = item->data (Qt::UserRole + 2).toUInt ();
	emit chord_pair_highlight (chord1, chord2);
}

void ChordMatchWidget::on_delete_pair ()
{
	QListWidgetItem *item = ui.listWidget_Chord_Pairs->takeItem (ui.listWidget_Chord_Pairs->currentRow ());
	if (!item) return;
	auto chord1 = item->data (Qt::UserRole + 1).toUInt ();
	auto chord2 = item->data (Qt::UserRole + 2).toUInt ();
	emit delete_chord_pair (chord1, chord2);
	delete item;
}

void ChordMatchWidget::closeEvent (QCloseEvent *event)
{
	QWidget::closeEvent (event);
	emit close_widget ();
}
#ifndef CHORDMATCHWIDGET_H
#define CHORDMATCHWIDGET_H

#include <QWidget>
#include "ui_chordmatchwidget.h"
#include "MeshDefs.h"
#include "FuncDefs.h"
#include "DualDefs.h"
#include "hoopsview.h"
#include <QCloseEvent>
#include <QHideEvent>

class ChordMatchWidget : public QWidget
{
	Q_OBJECT

public:
	ChordMatchWidget(QWidget *parent = 0);
	~ChordMatchWidget();
public:
	void set_matched_pairs (ChordPairs &matched_pairs);
protected:
	void closeEvent (QCloseEvent *event);
private slots:
	void on_item_entered (QListWidgetItem * item);
	void on_delete_pair ();
signals:
	void chord_pair_highlight (uint chord_ptr1, uint chord_ptr2);
	void delete_chord_pair (uint chord_ptr1, uint chord_ptr2);
	void close_widget ();
private:
	Ui::ChordMatchWidget ui;
};

#endif // CHORDMATCHWIDGET_H

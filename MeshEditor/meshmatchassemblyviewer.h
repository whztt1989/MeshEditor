#ifndef MESHMATCHASSEMBLYVIEWER_H
#define MESHMATCHASSEMBLYVIEWER_H

#include <QWidget>
#include "ui_meshmatchassemblyviewer.h"
#include "acis_headers.h"
#include "MeshDefs.h"
#include "FuncDefs.h"
#include "DualDefs.h"
#include "MeshMatchingHandler.h"

class HoopsView;
class ManualMeshMatchWidget;

class MeshMatchAssemblyViewer : public QWidget
{
	Q_OBJECT

public:
	MeshMatchAssemblyViewer(QWidget *parent = 0);
	~MeshMatchAssemblyViewer();
private slots:
	void on_refresh_matching ();
public:
	HoopsView * get_hoopsview (){return ui.hoopsview;}
	VolumeMesh *mesh1, *mesh2;
	std::unordered_set<OvmFaH> interface_fhs1, interface_fhs2;
	FACE *inter_face;
	double myresabs;
	ManualMeshMatchWidget *mm_widget;
	ChordPairs matched_chord_pairs;
private:
	Ui::MeshMatchAssemblyViewer ui;
	HoopsView *hoopsview;
	MeshMatchingHandler *mm_handler;
};

#endif // MESHMATCHASSEMBLYVIEWER_H

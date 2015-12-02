#ifndef MESHCUT_H
#define MESHCUT_H

#include <QWidget>
#include "ui_meshcut.h"
#include "MeshDefs.h"
#include "FuncDefs.h"
#include "acis_headers.h"

class HoopsView;
class MeshCut : public QWidget
{
	Q_OBJECT

public:
	MeshCut(QWidget *parent = 0);
	~MeshCut();
private slots:
	void onLoadMeshFile ();
	void onLoadCutCADModel ();
	void onCutMesh ();
	void onRevertCutMesh ();
	void onAttachAndSmooth ();
	void onSave ();

private:
	Ui::MeshCut ui;
	HoopsView *hoopsview1, *hoopsview2, *hoopsview3;
	VolumeMesh *mesh, *match_mesh, *left_mesh;
	BODY *body;
};

#endif // MESHCUT_H

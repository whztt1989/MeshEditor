#ifndef MANUALMESHMATCH_H
#define MANUALMESHMATCH_H

#include <QWidget>
#include "ui_manualmeshmatch.h"
#include "MeshDefs.h"
#include "FuncDefs.h"
#include "hoopsview.h"
#include "mesheditcontroller.h"
#include "meshmatchassemblyviewer.h"
#include "MeshMatchingHandler.h"
#include "chordmatchwidget.h"
class ManualMeshMatchWidget : public QWidget
{
	friend class MeshMatchAssemblyViewer;
	Q_OBJECT

public:
	ManualMeshMatchWidget(QWidget *parent = 0);
	~ManualMeshMatchWidget();
public:
	std::vector<std::vector<QToolBar*> > get_toolbars () {return toolbars;}
	void draw_matched_chords (ChordPairs &matched_chord_pairs);
private slots:
	void on_open ();
	void on_open2 ();
	void on_init_matching ();
	void on_update_matched_chords ();
	void on_match_two_chords ();
	void on_check_match ();
	void on_show_interface ();
	void on_show_chord_pairs ();
	void on_save ();
	void on_auto_match ();
	void on_get_polyline ();
	void on_merge ();

	void on_highlight_chord_pair (uint chord_ptr1, uint chord_ptr2);
	void on_delete_chord_pair (uint chord_ptr1, uint chord_ptr2);
	void on_close_chord_pair_widget ();
	void on_show_left ();
	void on_show_right ();
private:
	std::set<std::pair<FACE *, FACE*> > get_interfaces ();
	DualChord * get_matched_chord (DualChord *chord);

private:                                                                      
	Ui::ManualMeshMatch ui;
	std::vector<std::vector<QToolBar*> > toolbars;
	HoopsView *hoopsview1, *hoopsview2/*, *hoopsview3*/;
	MeshEditController *mesh_edit_controller1, *mesh_edit_controller2;
	//MeshMatchAssemblyViewer *assembly_viewer;

	HC_KEY mesh1key, mesh2key, body1key, body2key;
	HC_KEY mesh3key, mesh4key;

	MeshMatchingHandler *mm_handler;

	ChordMatchWidget *chord_match_widget;
	DualChord *prev_sel_chord1, *prev_sel_chord2;
};

#endif // MANUALMESHMATCH_H

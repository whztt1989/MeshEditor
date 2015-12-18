#ifndef MESHEDITCONTROLLER_H
#define MESHEDITCONTROLLER_H

#include <QWidget>
#include "ui_mesheditcontroller.h"
#include "acis_headers.h"
#include "MeshDefs.h"
#include "FuncDefs.h"
#include "DualDefs.h"
#include "OneSimpleSheetInflation.h"
#include "OneSimpleSheetExtraction.h"
#include "groupcontrolwidget.h"
#include "mousecontrolwidget.h"
#include "choicesselectionwidget.h"

class HoopsView;
class MeshEditController : public QWidget
{
	Q_OBJECT

public:
	MeshEditController(QWidget *parent = 0);
	~MeshEditController();
public:
	DualChord *get_selected_chord () {return selected_chord;}
	std::unordered_set<OvmFaH> get_interface_quads ();
public:
	HoopsView *hoopsview;
	VolumeMesh *mesh;
	BODY *body;
	FACE * inter_face;
	std::set<FACE*> inter_faces;
	std::unordered_set<OvmFaH> int_quads;
private slots:
	void on_get_int_face_quads ();
	void on_adjust_int_face_quads ();
	void on_show_group_manager ();
	void on_show_interfaces ();
	void on_show_body_vertices ();
	void on_show_body_edges ();
	void on_show_body_faces ();
	void on_show_bnd_vertices ();
	void on_show_bnd_edges ();
	void on_show_bnd_faces ();

	void on_select_edges_for_si ();
	void on_select_edges_ok_for_si ();
	void on_read_loop_ehs_for_si ();
	void on_save_loop_ehs_for_si ();
	void on_get_quad_set_directly_for_si ();
	void on_get_quad_set_by_sweep_for_si ();
	void on_get_quad_set_by_sweep_direction_changed_for_si (int idx);
	void on_get_quad_set_by_sweep_direction_changed_ok_for_si ();
	void on_pcs_depth_get_quad_set_for_si ();

	void on_pcs_quad_set_changed_for_si (int idx);
	void on_pcs_depth_changed_for_si (int depth);
	void on_pcs_choosing_ok_for_si ();

	void on_modify_quad_set_for_si ();
	void on_add_quad_for_si ();
	void on_remove_quad_for_si ();
	void on_modify_quad_set_ok_for_si ();
	void on_determine_shrink_set_for_si ();
	void on_shrink_set_changed_for_si (int idx);
	void on_sheet_inflation_for_si ();
	void on_postprocess_for_si ();

	void on_select_edges_for_se ();
	void on_select_edges_ok_for_se ();
	void on_sheet_extraction_for_se ();
	void on_postprocess_for_se ();

	void on_direct_select_for_cc ();
	void on_select_by_sheets_for_cc ();
	void on_select_face_for_cc ();
	void on_select_face_ok_for_cc ();
	void on_select_edges_for_cc ();
	void on_select_edges_ok_for_cc ();
	void on_rotate_for_cc ();
	void on_continue_select_collapse_vhs_for_cc ();
	void on_select_collapse_vhs_for_cc ();
	void on_select_collapse_vhs_ok_for_cc ();
	void on_columns_collapse_for_cc ();
	void on_postprocess_for_cc ();

	void on_select_edges_for_co ();
	void on_select_edges_ok_for_co ();
	void on_show_chord_info_for_co ();
	void on_clear_selection_for_co ();
	void on_select_chord_by_idx_for_co ();
	void on_select_edge_for_adjust_chord ();
	void on_select_as_1st_edge_for_chord ();
	void on_select_as_2nd_edge_for_chord ();
	void on_finish_adjusting_chord ();

	void on_open_for_pd ();
	void on_show_interface_patches_for_pd ();
	void on_reach_depth_for_pd ();
	void on_select_vertices_for_pd ();
	void on_shortest_path_for_pd ();
	void on_save_selected_for_pd ();
	void on_load_selected_for_pd ();
	void on_adjust_diag_hexa_for_pd ();
	void on_select_faces_for_pd ();
	void on_select_faces_ok_for_pd ();
	
	void on_select_smooth_whole_for_mo ();
	void on_select_pair_node_method_for_mo ();
	void on_select_smooth_faces_for_mo ();
	void on_init_move_node_for_mo ();
	void on_select_node_for_move_for_mo ();
	void on_cancel_select_node_for_move_for_mo ();
	void on_move_node_for_mo ();
	void on_move_node_ok_for_mo ();
	void on_select_node_pair_for_mo ();
	void on_pair_node_ok_for_mo ();
	void on_smooth_mesh ();
	void on_select_geom_face_for_mo ();
	void on_smooth_geom_face_for_mo ();
	void on_continue_select_geom_face_for_mo ();
	void on_clear_select_faces_for_mo ();
	void on_rotate_for_mo ();

	void on_select_edges_for_cl ();
	void on_rotate_for_cl ();
	void on_select_edges_ok_for_cl ();
	void on_cluster_for_cl ();

	
private:
	void update_mesh ();
signals:
	void new_sheet_inflated (unsigned long sheet_ptr);
private:
	Ui::MeshEditController ui;
	std::vector<OvmVeH> selected_vhs;
	std::vector<OvmEgH> selected_ehs;
	std::vector<OvmFaH> selected_fhs;
	std::unordered_set<OvmCeH> selected_chs;

	std::unordered_set<OvmEgH> loops_ehs;
	std::unordered_set<OvmCeH> shrink_set;
	std::unordered_set<OvmFaH> inflation_quad_set;
	std::vector<std::unordered_set<OvmFaH> > quad_sets_on_interfaces;
	int selected_quad_set_on_interface_idx;
	std::vector<std::unordered_set<OvmCeH> > hex_sets_sep_by_inf_fhs;
	std::vector<std::unordered_set<OvmFaH> > sweeping_quad_sets;
	
	DualSheet *new_sheet;
	std::vector<DualSheet *> new_sheets;

	OneSimpleSheetInflationHandler *ossi_handler;
	OneSimpleSheetExtractionHandler *osse_handler;
	DualSheet *assist_sheet;
	DualSheet *sheet_to_extract;
	std::set<DualSheet*> sheets_to_extract;
	std::set<DualColumn *> columns_to_collapse;
	std::set<DualSheet*> selected_sheets;
	bool direct_get_inflation_quad_set;

	std::map<DualColumn *, std::pair<OvmVeH, OvmVeH> > collapse_vhs_pairs;

	std::unordered_set<OvmCeH> cut_left_chs;
	std::unordered_set<OvmCeH> chs_on_sheet_to_extract;

	DualChord *selected_chord;
	OvmEgH first_eh_for_adjusting_chord, second_eh_for_adjusting_chord;
	std::hash_map<OvmVeH, OvmVeH> old_new_vhs_mapping;
	VolumeMeshElementGroup *tmp_group_for_pd1, *tmp_group_for_pd2;
	std::unordered_set<OvmFaH> fhs_for_rendering;
	std::unordered_set<OvmCeH> chs_for_rendering;

	GroupControlWidget *group_controller;
	MouseControlWidget *mouse_controller;
	ChoicesSelectionWidget *choices_controller;
};

#endif // MESHEDITCONTROLLER_H

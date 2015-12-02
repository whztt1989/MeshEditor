#ifndef MESHEDITWIDGET_H
#define MESHEDITWIDGET_H

#include <QWidget>
#include <QMenu>
#include <QToolBar>
#include <QAction>
#include <QTabWidget>
#include "ui_mesheditwidget.h"
#include "MeshDefs.h"
#include "FuncDefs.h"
#include "hoopsview.h"
#include "OneIntLoop.h"
#include "DualDefs.h"
#include "meshrendercontrolwidget.h"
#include "mousecontrolwidget.h"
#include "filecontrolwidget.h"
#include "groupcontrolwidget.h"
#include "OneSimpleSheetInflation.h"
#include "OneSimpleSheetExtraction.h"
#include "DepthCutHandler.h"
#include "CrossCrossPath.h"
#include <QSet>
#include <QVector>
#include <QString>
#include <QDataStream>


struct QuadSetData{
	typedef int OEgH;
	typedef int OVeH;
	typedef int OCeH;
	typedef int OFaH;
	QSet<OEgH> input_edges;
	QSet<OFaH> forbidden_faces, allowed_faces;
	QVector<QPair<OVeH, OVeH> > int_vh_pairs;
	QVector<QVector<OEgH> > int_paths;
	QVector<QVector<QSet<OCeH> > > int_hex_sets;
	QSet<OEgH> closed_edges, appended_edges;
	QSet<OCeH> init_hex_set, opt_hex_set;
	QSet<OFaH> init_quad_set, opt_quad_set;

	void serialize (QString file_path){
		QFile file (file_path);
		file.open (QIODevice::WriteOnly);
		QDataStream out (&file);
		out<<input_edges<<forbidden_faces<<allowed_faces
			<<int_vh_pairs<<int_paths<<int_hex_sets
			<<init_quad_set<<opt_quad_set
			<<init_hex_set<<opt_hex_set;
		file.close ();
	}

	void deserialize (QString file_path){
		input_edges.clear (); forbidden_faces.clear (); allowed_faces.clear ();
		int_vh_pairs.clear (); int_paths.clear (); int_hex_sets.clear ();
		init_hex_set.clear (); opt_hex_set.clear (); init_quad_set.clear ();opt_quad_set.clear ();

		QFile file (file_path);
		file.open (QIODevice::ReadOnly);
		QDataStream in (&file);
		in>>input_edges>>forbidden_faces>>allowed_faces
			>>int_vh_pairs>>int_paths>>int_hex_sets
			>>init_quad_set>>opt_quad_set
			>>init_hex_set>>opt_hex_set;
		file.close ();
	}
};

class MeshEditWidget : public QWidget
{
	Q_OBJECT

public:
	MeshEditWidget(QWidget *parent = 0);
	~MeshEditWidget();
public:
	void load_mesh_and_model (QString mesh_path, QString model_path);
	std::vector<std::vector<QToolBar*> > get_toolbars () {return toolbars;}
	VolumeMesh *get_mesh () {return mesh;}
private:
	void setup_actions ();
private slots:
	void on_open_file (QString file_path);
	void on_save_file ();
	void on_save_file_as (QString file_path);
	void on_file_close ();

	void on_highlight_mesh_elements ();
	void on_clear_highlight_mesh_elements ();

	void on_select_edges_ok_for_auto_lsi ();
	void on_get_surf_patches_for_auto_lsi ();
	void on_set_depth_control_for_auto_lsi ();
	void on_select_constant_faces_ok_for_auto_lsi ();
	void on_get_hexa_set_for_auto_lsi ();
	void on_get_quad_set_for_auto_lsi ();
	void on_sheet_inflate_for_auto_lsi ();
	void on_reprocess_for_auto_lsi ();


	void on_select_sheet_for_lse ();
	void on_select_interface_faces_for_lse ();
	void on_select_constant_faces_for_lse ();
	void on_extract_all_for_lse ();
	void on_local_inflate_for_lse ();
	void on_sep_hex_mesh ();
	void on_show_int_diagram_for_lse ();

	void on_analyze_for_lc ();
	void on_set_depth_for_lc ();
	void on_search_inter_path_for_lc ();
	void on_complete_non_int_loop_for_lc ();
	void on_complete_int_loop_for_lc ();

	void on_set_start_face_for_dc ();
	void on_set_depth_for_dc ();
	void on_create_cut_face_for_dc ();
	void on_depth_cut_for_dc ();
	void on_save_for_dc ();

	void on_test1 ();
	void on_test2 ();

	void on_select_edges_ok_for_qs ();
	void on_select_constraint_faces_ok_for_qs ();
	void on_complete_loop_for_qs ();
	void on_optimize_loop_for_qs ();
	void on_get_quad_set_for_qs ();
	void on_optimize_quad_set_for_qs ();
	void on_search_int_path_for_qs ();
	void on_select_cross_hexa_ok_for_qs ();
	void on_read_data_for_qs ();
	void on_save_data_for_qs ();
	void on_inflate_sheet_for_qs ();
	void on_postprocess_for_qs ();
	void on_add_hexs_for_qs ();
	void on_remove_hexs_for_qs ();
	void on_modify_quad_set_ok_for_qs ();
	void on_close_loop_for_qs ();
	void on_append_loop_for_qs ();
	void on_show_int_diagram_for_qs ();

	void on_get_corres_edges_for_qs ();
	void on_optimize_once_for_qs ();
	void on_current_quality_for_qs ();
	void on_which_type_for_qs ();
	void on_pair_int_vhs_for_qs ();
	void on_select_next_int_edge_for_qs ();
	void on_select_int_edges_ok_for_qs ();
	void on_pair_one_int_vhs_ok_for_qs ();
	void on_pair_int_vhs_return_for_qs ();
	void on_get_qs_by_pcs_for_qs ();
	void on_get_qs_by_sweeping_for_qs ();

	void on_start_smoothing ();
	void on_select_geom_faces ();
	void on_clear_select_geom_faces ();
	void on_smooth_faces ();

	void on_set_color_for_pd ();
	void on_set_line_weight_for_pd ();
	void on_set_cell_ratio_for_pd ();
	void on_add_cells_for_pd ();
	void on_remove_cells_for_pd ();
	void on_add_element_for_pd ();
	void on_remove_element_for_pd ();
	void on_add_element_group_for_pd ();
	void on_recursive_spreading ();
	void on_loop_sep ();
	void on_get_st_node ();
	void on_finally_sep ();
	void on_sep_hex_seeds ();
	void on_get_init_quad_set_for_pd ();
	void on_int_sep_hex_seeds ();
	void on_algo_test_for_pd ();
	void on_show_sheet_edges_and_faces_for_pd ();
	void on_append_edges_on_faces_for_pd ();
	void on_turn_to_inflation_for_pd ();
private:
	void update_cross_hexas ();
	void update_inflation_fhs ();
	void update_temp_element_group ();
private:
	Ui::MeshEditWidget ui;
	HoopsView *hoopsview;
	VolumeMesh *mesh;
	BODY *body;
	
	QString xml_file_path, mesh_file_path, model_file_path;
	SheetSet sheet_set;
	ChordSet chord_set;
	std::unordered_set<OvmEgH> completed_loop_ehs;
	std::vector<OvmVeH> selected_vhs;
	std::vector<OvmEgH> selected_ehs;
	std::vector<OvmFaH> selected_fhs;
	std::unordered_set<OvmVeH> selected_vhs_set;
	std::unordered_set<OvmEgH> selected_ehs_set;
	std::unordered_set<OvmFaH> selected_fhs_set;


	OneSimpleSheetInflationHandler *ossi_handler;
	OneSimpleSheetExtractionHandler *osse_handler;

	std::vector<std::vector<QToolBar*> > toolbars;
	MeshRenderControlWidget *mesh_render_controller;
	MouseControlWidget *mouse_controller;
	GroupControlWidget *group_controller;
	FileControlWidget *file_controller;

	DepthCutHandler *depth_cut_handler;

	std::unordered_set<OvmEgH> qs_loop;
	std::vector<OvmEgH> qs_int_path;
	std::unordered_set<OvmEgH> qs_int_path_set;
	std::unordered_set<OvmFaH> qs_inf_fhs;
	std::vector<std::unordered_set<OvmFaH> > qs_obs_fhs;
	std::unordered_set<OvmCeH> qs_hexa_set;
	CCPSHConf qs_cross_conf;
	OvmVeH qs_vh, qs_first_int_vh, qs_second_int_vh;
	DualSheet *new_sheet;
	std::unordered_set<OvmCeH> all_chs_around_int_path, ungrouped_chs_around_int_path;

	QuadSetData quad_set_data;

	QColor pd_group_color;
	double pd_line_weight, pd_cell_ratio;
	QString pd_group_type, pd_group_name;
	VolumeMeshElementGroup *pd_temp_group;
	std::vector<std::unordered_set<OvmFaH> > pd_fhs_patches;
	std::vector<std::unordered_set<OvmCeH> > pd_hex_sets;
	std::vector<std::unordered_set<OvmFaH> > pd_init_fhs_sets;
};

#endif // MESHEDITWIDGET_H

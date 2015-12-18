#ifndef SIMPLEHQWIDGET_H 
#define SIMPLEHQWIDGET_H 

// Qt Includes 
#include <QLabel> 
#include <QWidget> 
#include <QMenu> 
#include <QSlider> 
#include <QContextMenuEvent>
#include <QAction>
#include <QActionGroup>
#include <QThread>
#include <QTimerEvent>
#include <QProgressDialog>
#include <QList>
#include <QVector>
#include <QMap>
#include <QFile>
#include <QFileInfo>

// HOOPS/Qt Includes 
#include "HQWidget.h" 
#include "FuncDefs.h"
#include "MeshDefs.h"

#define POINTER_SIZED_HIGHBIT ((HC_KEY)1 << (8 * sizeof (HC_KEY) - 1))
#define POINTER_TO_KEY(p) ((((HC_KEY)p)>>1) & ~(POINTER_SIZED_HIGHBIT))
#define KEY_TO_POINTER(k) (k<<1)

#define OPEN_MESHES_SEG HC_Open_Segment_By_Key (m_pHView->GetModelKey ());{ \
HC_Open_Segment ("meshes");{

#define CLOSE_MESHES_SEG }HC_Close_Segment ();\
}HC_Close_Segment ();

#define OPEN_GROUPS_SEG HC_Open_Segment_By_Key (m_pHView->GetModelKey ());{ \
HC_Open_Segment ("groups");{

#define CLOSE_GROUPS_SEG }HC_Close_Segment (); \
}HC_Close_Segment ();

#define OPEN_HIGHLIGHT_SEG HC_Open_Segment_By_Key (m_pHView->GetModelKey ());{ \
	HC_Open_Segment ("highlights");{

#define CLOSE_HIGHLIGHT_SEG }HC_Close_Segment (); \
}HC_Close_Segment ();

struct MeshGroupRenderParam{
	QString vertex_color, edge_color, face_color, cell_color, text_color;
	double vertex_size, edge_weight, cell_shrink_ratio;
	bool vertex_visible, edge_visible, face_visible, cell_visible, text_visible;
	MeshGroupRenderParam(){
		vertex_color = edge_color = face_color = cell_color = "";
		vertex_size = edge_weight = cell_shrink_ratio = 0.0f;
		vertex_visible  = false; edge_visible = true; face_visible = true; cell_visible = true; text_visible = true;
	}
};

struct MeshRenderOptions{
	MeshRenderOptions (){
		vertex_color = edge_color = face_color = cell_color = text_color = NULL;
		vertex_size = edge_width = text_size = -1;
		show_vertex_indices = show_edge_indices = show_face_indices = show_cell_indices = false;
		cell_size = 0.8;
	}
	char *vertex_color, *edge_color, *face_color, *cell_color, *text_color;
	double vertex_size, edge_width, text_size, cell_size;
	bool show_vertex_indices, show_edge_indices, show_face_indices, show_cell_indices;
};

class HoopsView : public HQWidget 
{ 
	Q_OBJECT 

public: 
	HoopsView(QWidget* parent); 
	~HoopsView(); 
public:
	void render_VolumeMesh (VolumeMesh *mesh);
	void rerender_VolumeMesh (VolumeMesh *mesh);
	void derender_VolumeMesh ();
	void show_VolumeMesh (bool show);
	void render_chord (DualChord *chord, const char *color = NULL, double width = -1);
	void derender_chord (DualChord *chord);
	void derender_all_chords ();
	void derender_polylines ();
	void render_polyline (std::vector<SPAposition> &polyline, const char *color);
	void render_shell_wireframe ();
	void render_shell_shaded ();
	void render_shell_hiddenline ();
	void set_selected_elements (VolumeMeshElementGroup *group);
	void move_vertices (VolumeMesh *mesh, std::vector<OvmVeH> &vhs);
	void move_vertices_ok (BODY *body);
	bool segment_existing (HC_KEY key);
public slots:
	void begin_camera_manipulate ();
	void begin_cutting ();
	void begin_select_by_click ();
	void begin_select_by_rectangle ();
	void begin_select_by_polygon ();
	void clear_selection ();
	void set_vertices_selectable (bool selectable);
	void set_edges_selectable (bool selectable);
	void set_faces_selectable (bool selectable);
	void show_boundary (bool show);
	void show_boundary_vertices (bool show);
	void show_boundary_edges (bool show);
	void show_boundary_faces (bool show);
	void show_boundary_cells (bool show);
	void show_boundary_vertices_indices (bool show);
	void show_boundary_edges_indices (bool show);
	void show_boundary_faces_indices (bool show);
	void show_boundary_cells_indices (bool show);
	void show_inner (bool show);
	void show_inner_vertices (bool show);
	void show_inner_edges (bool show);
	void show_inner_faces (bool show);
	void show_inner_cells (bool show);
	void show_inner_vertices_indices (bool show);
	void show_inner_edges_indices (bool show);
	void show_inner_faces_indices (bool show);
	void show_inner_cells_indices (bool show);

	void show_mesh_vertices (bool show);
	void show_mesh_edges (bool show);
	void show_mesh_faces (bool show);
	void show_mesh_faces_transparent ();

	void show_acis_body (bool show);
	void show_acis_vertices (bool show);
	void show_acis_edges ( bool show);
	void show_acis_faces ( bool show);
	void set_acis_vertices_selectability (bool selectable);
	void set_acis_edges_selectable (bool selectable);
	void set_acis_faces_selectable (bool selectable);

	void render_one_acis_edge (EDGE *eg);
	void derender_one_acis_edge (EDGE *eg);
	void render_acis_edges (std::set<EDGE*> egs);
	void derender_acis_edges (std::set<EDGE*> egs);
public:
	//////////////////////////////////////////////////////////////////////////
	//group operations
	HC_KEY render_mesh_group (VolumeMeshElementGroup *group, MeshRenderOptions &render_options);
	void render_mesh_groups (std::set<VolumeMeshElementGroup*> &groups);
	void derender_one_mesh_group (VolumeMeshElementGroup *group);
	void derender_mesh_groups (std::set<VolumeMeshElementGroup*> &groups);
	void derender_mesh_groups (const char *group_type, const char *group_name = NULL, bool delete_groups = false);
	void derender_all_mesh_groups ();
	bool mesh_group_exists (VolumeMeshElementGroup *group);
	void get_mesh_groups (std::vector<VolumeMeshElementGroup*> &groups, 
		std::vector<VolumeMeshElementGroup*> &invisible_groups, 
		std::vector<VolumeMeshElementGroup*> &highlighted_groups, 
		const char *group_type = NULL, const char *group_name = NULL);
	void show_mesh_group (VolumeMeshElementGroup *group, bool show);
	void update_mesh_group (VolumeMeshElementGroup *group);
	void update_mesh_groups ();
	void update_mesh_group_rendering (VolumeMeshElementGroup *group, MeshGroupRenderParam *param);
	void highlight_mesh_group (VolumeMeshElementGroup *group);
	void dehighlight_mesh_group (VolumeMeshElementGroup *group);
	bool is_mesh_group_highlighted (VolumeMeshElementGroup *group);
	void set_mesh_group_selectability (VolumeMeshElementGroup *group, bool vertices, bool edges, bool faces);
	void set_mesh_group_selectability (const char *group_type, const char *group_name, bool vertices, bool edges, bool faces);
	std::set<VolumeMeshElementGroup*> retrieve_mesh_groups (const char *group_type, const char *group_name);
	int get_selected_elements (std::vector<OvmVeH> &vertices, std::vector<OvmEgH> &edges, std::vector<OvmFaH> &faces);
	int get_selected_elements (std::vector<OvmVeH> *vertices, std::vector<OvmEgH> *edges, std::vector<OvmFaH> *faces);
	int get_selected_acis_entities (std::vector<VERTEX*> *vertices, std::vector<EDGE*> *edges, std::vector<FACE*> *faces);
	HC_KEY get_cutting_plane_key ();
	void clear_cutting_plane ();
public slots: 

	void OnLoad();
	void OnSaveFileAs();

	void OnZoomToExtents();
	void OnZoomToWindow();

	void OnZoom();
	void OnOrbit();
	void OnPan();

	void OnRunMyCode(); 

private:
	void createActions ();
	void createModels ();
	void local_set_operator (HBaseOperator *new_operator);
protected: 

	void SetupView();  
	void Init();
	void contextMenuEvent (QContextMenuEvent *);
	void timerEvent (QTimerEvent *);
	void wheelEvent (QWheelEvent * e);
	void passiveWheelEvent (QWheelEvent *e);
private:
	void init_hoops_db ();
private:
	VolumeMeshElementGroup *temp_group;
}; 
#endif 


#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// qt includes
#include <QPrinter>
#include <QFileDialog>
#include <QLabel> 
#include <QMessageBox>
#include <QMenu> 
#include <QCursor> 
#include <QSlider>
#include <QLayout>
#include <QLineEdit>
#include <QTimer>
#include <QColorDialog>
#include <QColor>
#include <QMenu>
#include <QFileInfo>
#include <QToolBar>
#include <QAction>

//ACIS includes
#include "kernapi.hxx"
#include "ha_bridge.h"
// hoops_mvo includes
#include "HDB.h"

#include "HBaseModel.h"
#include "HBaseView.h"

#include "HModelInfo.h"
#include "HEventInfo.h"
#include "HOpCameraOrbit.h"
#include "HOpCameraZoom.h"
#include "HOpCameraZoomBox.h"
#include "HOpCameraPan.h"
#include "HOpCameraManipulate.h"
#include "HOpCreateCuttingPlane.h"
#include "HOpCreateSphere.h"
#include "HOpCreateCone.h"
#include "HOpCreateCylinder.h"
#include "HOpConstructRectangle.h"
#include "HOpCreateRectangle.h"
#include "HOpSelectAperture.h"
#include "HOpSelectPolygon.h"
#include "HOpSelectArea.h"
#include "HOpCameraOrbitTurntable.h"
#include "HOpCreateBoolean.h"
#include "HOpMoveHandle.h"
#include "HUtilityGeometryCreation.h"
#include "HUtilityGeomHandle.h"
#include <HMarkupManager.h>
#include "HUtility.h"
#include "HStream.h"
#include "HSelectionSet.h"
#include "HStreamFileToolkit.h"
#include "HIOManager.h"
#include "HUtilitySubwindow.h"
#include "HUtilityAnnotation.h"

#undef null

// the qt/hoops base class
#include "hoopsview.h"
#include <HSelectionItem.h>
// hoops include
#include "hc.h"
#include "OpCutMesh.h"

// this is setup in main
extern HDB * m_pHDB;


#define Debug_USE_QGL_CONTEXT           0x00000080

HoopsView::HoopsView(QWidget* parent) 
    : HQWidget(parent)
{ 


	// Create and initialize HOOPS/MVO Model and View objects 
	m_pHBaseModel = new HBaseModel(); 
	m_pHBaseModel->Init(); 

	// Initialize View object to null ; gets created in SimpleHQWidget::Init 
	m_pHView = 0; 

	// enable MouseMoveEvents  
	setMouseTracking(true); 

	// enable key events 
	setEnabled(true); 
	setFocusPolicy(Qt::StrongFocus); 

	createActions ();
	temp_group = NULL;
} 
  

HoopsView::~HoopsView() 
{ 
 // Destructor  

	// Clean up memory 
	if(m_pHView)        delete m_pHView; 
	if(m_pHBaseModel)   delete m_pHBaseModel; 

} 
  
void HoopsView::createActions ()
{

}

void HoopsView::createModels ()
{
	HPoint pts[] = {
		HPoint (0, 0, 0), HPoint (1, 0, 0), HPoint (1, 1, 0), HPoint (0, 1, 0),
		HPoint (0, 0, 1), HPoint (1, 0, 1), HPoint (1, 1, 1), HPoint (0, 1, 1)
	};

	int face_list[] ={
		4, 0, 3, 2, 1,
		4, 0, 1, 5, 4,
		4, 1, 2, 6, 5,
		4, 2, 3, 7, 6,
		4, 3, 0, 4, 7,
		4, 4, 5, 6, 7
	};

	HC_Open_Segment_By_Key (m_pHView->GetModelKey ());
	HC_Open_Segment ("cube");
		HC_Set_Color ("faces=grey");
		HC_Set_Visibility ("vertices=on,markers=on");
		HC_Insert_Shell (8, pts, 30, face_list);
	HC_Close_Segment ();
	HC_Close_Segment ();
	m_pHView->ZoomToExtents ();
	////m_pHView->RenderFakeHiddenLine ();
	//m_pHView->GetSelection ()->SetSubentityFaceSelection (false);
	//m_pHView->GetSelection ()->SetSubentityEdgeSelection (false);
	////m_pHView->GetSelection ()->SetAllowRegionSelection (false);
	//m_pHView->GetSelection ()->SetSubentityVertexSelection (true);
	////m_pHView->GetSelection ()->SetAllowSubentitySelection (false);
	////m_pHView->GetSelection ()->SetAllowRegionSelection (false);
	//HSubwindow subwindow;

	//if (!m_pHView->GetHObjectManager()->GetHObject("subwindow"))
	//	m_pHView->GetHObjectManager()->AddHObject(new HSubwindow(m_pHView));

	//HC_Open_Segment_By_Key(m_pHView->GetModelKey());

	//HC_Open_Segment("subwindows");
	//subwindow.Insert(-1.0f, -0.7f, 
	//	0.7f,1.0f,  SUBWINDOW_POINTER  );
	//HC_Close_Segment();
	//HC_Close_Segment();

	//subwindow.MakeCameraSnapshot(m_pHView);
	//m_pHView->Update();	

	m_pHView->Update ();
	
}

void HoopsView::SetupView() 
{ 

	// set initial HOOPS/3DGS segment tree attributes for the  
	// HOOPS/MVO View Object 

	m_pHView->FitWorld();  // fit the camera to the scene extents 
	m_pHView->RenderGouraud();  // set the render mode to gouraud 

	// configure view segment  
	HC_Open_Segment_By_Key(m_pHView->GetViewKey()); 
	HC_Set_Color_By_Index("windows", 0); 
	HC_Set_Selectability("everything = off"); 
	HC_Close_Segment(); 

	// Configure scene/model segment 
	HC_Open_Segment_By_Key(m_pHView->GetSceneKey()); 
	HC_Set_Color_By_Index("faces", 2); 
	HC_Set_Color_By_Index("text, lights", 1); 
	HC_Set_Color_By_Index("edges, lines", 1); 
	HC_Set_Color_By_Index("markers", 1); 
	HC_Set_Rendering_Options 
		("no color interpolation, no color index interpolation"); 
	HC_Set_Visibility  
		("lights = (faces = on, edges = off, markers = off), markers = off, faces=on, edges=off, lines=on, text = on");
	HC_Set_Visibility  
		("lights = on, markers = off, faces=on, edges=off, lines=on, text = on"); 

	HC_Set_Selectability("everything = off, faces = on"); 
	HC_Set_Text_Font("transforms = off"); 
	HC_Close_Segment(); 

	// configure segment for temporary construction geometry 
	HC_Open_Segment_By_Key (m_pHView->GetConstructionKey()); 
	HC_Set_Heuristics("quick moves"); 
	HC_Set_Visibility("faces = off, edges = on, lines = on"); 
	HC_Close_Segment(); 


	// configure windowspace segment for quickmoves 
	HC_Open_Segment_By_Key(m_pHView->GetWindowspaceKey()); 
	HC_Set_Color_By_Index ("geometry", 3); 
	HC_Set_Color_By_Index ("window contrast", 1); 
	HC_Set_Color_By_Index ("windows", 1); 

	HC_Set_Visibility("markers=on"); 
	HC_Set_Color("markers = green, lines = green"); 
	HC_Set_Marker_Symbol("+"); 
	HC_Set_Selectability("off"); 
	HC_Close_Segment();  

} 

void HoopsView::init_hoops_db ()
{
	OPEN_MESHES_SEG
		HC_Set_Visibility ("markers=on,lines=on,faces=on");
		HC_Set_Selectability ("markers=off,lines=off,faces=off");
		HC_Set_Color ("markers=white,lines=black,faces=light grey,text=red");
		HC_Set_Line_Weight (0.8);
		HC_Set_Marker_Size (0.5);
		HC_Set_Text_Size (0.7);
	CLOSE_MESHES_SEG

	OPEN_GROUPS_SEG
		HC_Set_Visibility ("markers=on,lines=on,faces=on");
		HC_Set_Color ("markers=red,lines=green,faces=blue,text=white");
		HC_Set_Line_Weight (4);
		HC_Set_Marker_Size (0.6);

	CLOSE_GROUPS_SEG

	OPEN_HIGHLIGHT_SEG
		HC_Set_Color ("markers=yellow,lines=yellow,faces=yellow,text=red");
		HC_Set_Line_Weight (4);
		HC_Set_Rendering_Options ("attribute lock = (color,line weight)");
	CLOSE_HIGHLIGHT_SEG
}

void HoopsView::Init() 
{ 
	m_pHView = new HBaseView(m_pHBaseModel, NULL, NULL, NULL, GetWindowId(), GetColorMap(), GetClipOverride()); 

	m_pHView->Init(); 
	HSelectionSet *selectionSet = new HSelectionSet (m_pHView);
	selectionSet->Init ();
	selectionSet->SetSelectionLevel(HSelectSubentity);
	selectionSet->SetSelectionEdgeWeight (4);
	m_pHView->SetSelection(selectionSet);

	// Set up the HOOPS/MVO View's HOOPS/3DGS Attributes 
	SetupView(); 

	// Set View's current Operator 
	m_pHView->SetOperator(new HOpCameraManipulate(m_pHView)); 

	//DEBUG_STARTUP_CLEAR_BLACK = 0x00004000 clear ogl to black on init update
	HC_Open_Segment_By_Key(m_pHView->GetViewKey());
		HC_Set_Driver_Options("debug = 0x00004000");
	HC_Close_Segment();

	m_pHView->SetHandedness (HandednessRight);
	m_pHView->SetViewMode (HViewXY);
	//m_pHView->SetWindowColor (HPoint (1, 1, 1), HPoint (0.2, 0.2, 0.7));
	m_pHView->SetWindowColor (HPoint (1, 1, 1));
	m_pHView->SetLineAntialiasing (true);
	m_pHView->SetTextAntialiasing (true);
	m_pHView->SetAxisMode (AxisOn);
	m_pHView->GetSelection ()->SetSelectionEdgeWeight (3);
	m_pHView->GetSelection ()->SetSelectionMarkerSize (0.8);

	auto radius = m_pHView->GetDefaultSelectionProximity ();
	m_pHView->SetDefaultSelectionProximity (radius * 2);
	m_pHView->SetVisibilitySelectionMode (true);
	m_pHView->SetViewSelectionLevel (HSelectionLevelEntity);
	m_pHView->SetHardwareAntialiasing (16);
	m_pHView->GetSelection ()->SetSelectionLevel (HSelectEntity);

	HC_Open_Segment_By_Key (m_pHView->GetSceneKey ());
	HC_Set_Color ("lines=black,text=black");
	HC_Close_Segment ();
	// Call the Views Update Method - initiates HOOPS/3DGS Update_Display  
	init_hoops_db ();
	//HC_Open_Segment_By_Key (m_pHView->GetViewKey ());{
	//	HC_Set_Driver_Options ("anti-alias=1");
	////	HC_Open_Segment ("myhud");
	//	HC_Set_Rendering_Options ("anti-alias = lines");
	////	HC_Set_Visibility ("text=on");
	////	HC_Set_Text_Size (1);
	////	HC_Insert_Text (0.8, 0.8, 0, "hello\n world!");
	////	HC_Close_Segment ();
	//}HC_Close_Segment ();
	m_pHView->Update(); 
}

void HoopsView::contextMenuEvent (QContextMenuEvent *event)
{
}

void HoopsView::wheelEvent (QWheelEvent * e)
{
	HQWidget::wheelEvent (e);
}

void HoopsView::passiveWheelEvent (QWheelEvent *e)
{
	HQWidget::wheelEvent (e);
}

static void GetExtension(wchar_t const * filter, wchar_t * extension)
{
	const wchar_t *start = wcsstr(filter, L"(*.")+3;
	wcscpy(extension, start);
	*(wcsstr(extension, L")")) = L'\0';
}

void HoopsView::OnSaveFileAs()
{ 
}


// open a load file dialog
void HoopsView::OnLoad()
{
}


void HoopsView::timerEvent (QTimerEvent *event)
{

}

void HoopsView::OnOrbit()  
{ 
	// Set MVO View Object current Operator to HOpCameraOrbit  

	if (m_pHView->GetCurrentOperator()) 
	delete m_pHView->GetCurrentOperator(); 

	m_pHView->SetCurrentOperator(new HOpCameraOrbit(m_pHView));  

} 
  

// real time zoomer
void HoopsView::OnZoom() 
{
	if (m_pHView->GetCurrentOperator())
	    delete m_pHView->GetCurrentOperator();

    m_pHView->SetCurrentOperator(new HOpCameraZoom(m_pHView));
}

// window zoomer
void HoopsView::OnZoomToWindow() 
{
	if (m_pHView->GetCurrentOperator())
	    delete m_pHView->GetCurrentOperator();

    m_pHView->SetCurrentOperator(new HOpCameraZoomBox(m_pHView));
}

// resets the camera to view the world space extents of the model
void HoopsView::OnZoomToExtents() 
{
    m_pHView->ZoomToExtents();
}
  
// panner
void HoopsView::OnPan() 
{

    if (m_pHView->GetCurrentOperator())
	    delete m_pHView->GetCurrentOperator();
    m_pHView->SetCurrentOperator(new HOpCameraPan(m_pHView));		

}
 
void HoopsView::OnRunMyCode() 
{
	HC_Open_Segment_By_Key(m_pHView->GetViewKey ());
	HC_Close_Segment ();
	
	m_pHView->Update ();
}

void HoopsView::render_VolumeMesh (VolumeMesh *mesh)
{
	HC_KEY old_key = INVALID_KEY;
	HC_KEY new_key = POINTER_TO_KEY(mesh);
	OPEN_MESHES_SEG
		JC::render_volume_mesh (mesh);
	CLOSE_MESHES_SEG
	m_pHView->ZoomToExtents ();
	m_pHView->SetGeometryChanged ();
	m_pHView->Update ();
}

void HoopsView::show_VolumeMesh (bool show)
{
	show_boundary (show);
	show_inner (show);
}

void HoopsView::derender_polylines ()
{
	HC_Open_Segment_By_Key (m_pHView->GetModelKey ());{
		HC_Open_Segment ("polylines");{
			HC_Flush_Segment ("...");
		}HC_Close_Segment ();
	}HC_Close_Segment ();
	m_pHView->SetGeometryChanged ();
	m_pHView->Update ();
}

void HoopsView::render_polyline (std::vector<SPAposition> &polyline, const char *color)
{
	char buf[200];
	HC_Open_Segment_By_Key (m_pHView->GetModelKey ());{
		HC_Open_Segment ("polylines");{
			HC_Set_Visibility ("lines=on,markers=on,text=off");
			HC_Set_Color ("lines=red,markers=yellow,text=green");
			HC_Set_Line_Pattern ("...");
			HC_Set_Line_Weight (2);
			HC_Set_Text_Size (0.7);
			HC_Open_Segment ("");{
				if (color)
				{
					sprintf (buf, "lines=%s", color);
					HC_Set_Color (buf);
				}
				for (int i = 0; i != polyline.size () - 1; ++i)
				{
					HC_Insert_Line (
						polyline[i].x (), polyline[i].y (), polyline[i].z (),
						polyline[i + 1].x (), polyline[i + 1].y (), polyline[i + 1].z ());
				}
				//sprintf (buf, "%d", chord->idx ());
				//HC_Insert_Text (polyline[0][0], polyline[0][1],
				//	polyline[0][2], buf);
			}HC_Close_Segment();
		}HC_Close_Segment();
	}HC_Close_Segment();
	m_pHView->SetGeometryChanged ();
	m_pHView->Update ();
}

void HoopsView::derender_chord (DualChord *chord)
{
	HC_KEY key = POINTER_TO_KEY(chord);
	char status[100];
	HC_Show_Key_Status (key, status);
	if (strncmp (status, "invalid", 1) != 0){
		HC_Delete_By_Key (key);
		m_pHView->SetGeometryChanged ();
		m_pHView->Update ();
	}
}

void HoopsView::render_chord (DualChord *chord, const char *color, double width)
{
	char buf[200];
	auto polyline = get_chord_spa_polyline (chord);
	HC_KEY new_key = POINTER_TO_KEY(chord);
	char status[100];
	HC_Show_Key_Status (new_key, status);
	if (strncmp (status, "invalid", 1) != 0){
		HC_Delete_By_Key (new_key);
	}

	HC_KEY old_key;
	HC_Open_Segment_By_Key (m_pHView->GetModelKey ());{
		HC_Open_Segment ("chords");{
			HC_Set_Visibility ("lines=on,markers=on,text=off");
			HC_Set_Color ("lines=red,markers=yellow,text=green");
			HC_Set_Line_Pattern ("...");
			HC_Set_Text_Size (0.7);
			old_key = HC_Open_Segment ("");{
				if (color)
				{
					sprintf (buf, "lines=%s", color);
					HC_Set_Color (buf);
				}
				if (width != -1)
					HC_Set_Line_Weight (width);
				else
					HC_Set_Line_Weight (2);
				for (int i = 0; i != polyline.size () - 1; ++i)
				{
					HC_Insert_Line (
						polyline[i].x (), polyline[i].y (), polyline[i].z (),
						polyline[i + 1].x (), polyline[i + 1].y (), polyline[i + 1].z ());
				}
				//sprintf (buf, "%d", chord->idx ());
				//HC_Insert_Text (polyline[0][0], polyline[0][1],
				//	polyline[0][2], buf);
			}HC_Close_Segment();
		}HC_Close_Segment();
	}HC_Close_Segment();
	HC_Renumber_Key (old_key, new_key, "g");
	m_pHView->SetGeometryChanged ();
	m_pHView->Update ();

}

void HoopsView::derender_all_chords ()
{
	HC_Open_Segment_By_Key (m_pHView->GetModelKey ());{
		HC_Open_Segment ("chords");{
			HC_Flush_Segment ("...");
		}HC_Close_Segment();
	}HC_Close_Segment();
	m_pHView->SetGeometryChanged ();
	m_pHView->Update ();
}

void HoopsView::rerender_VolumeMesh (VolumeMesh *mesh)
{
	HC_KEY old_key = INVALID_KEY;
	HC_KEY new_key = POINTER_TO_KEY(mesh);
	OPEN_MESHES_SEG
		//HC_Flush_Segment ("./...");
		JC::render_volume_mesh (mesh);
	CLOSE_MESHES_SEG

	m_pHView->SetGeometryChanged ();
	m_pHView->Update ();
}

void HoopsView::derender_VolumeMesh ()
{
	OPEN_MESHES_SEG
		HC_Flush_Segment ("...");
	CLOSE_MESHES_SEG

	m_pHView->SetGeometryChanged ();
	m_pHView->Update ();
}

void HoopsView::render_shell_wireframe ()
{
	HC_Open_Segment_By_Key (m_pHView->GetModelKey ());{
		HC_Open_Segment ("meshshell");{
			HC_Set_Visibility ("faces=off,edges=on");
			HC_UnSet_One_Rendering_Option ("hsra");
		}HC_Close_Segment ();
		
	}HC_Close_Segment ();
	m_pHView->Update ();
}

void HoopsView::render_shell_shaded ()
{
	HC_Open_Segment_By_Key (m_pHView->GetModelKey ());{
		HC_Open_Segment ("meshshell");{
			HC_Set_Visibility ("faces=on,edges=on");
			HC_UnSet_One_Rendering_Option ("hsra");
		}HC_Close_Segment ();
		
	}HC_Close_Segment ();
	m_pHView->Update ();
	
}

void HoopsView::render_shell_hiddenline ()
{
	HC_Open_Segment_By_Key (m_pHView->GetModelKey ());{
		HC_Open_Segment ("meshshell");{
			HC_Set_Visibility ("faces=off,edges=on");
			HC_Set_Rendering_Options ("hsra=hidden line");
		}HC_Close_Segment ();

	}HC_Close_Segment ();
	m_pHView->Update ();
}

void HoopsView::local_set_operator (HBaseOperator *new_operator)
{
	HBaseOperator *old_operator = m_pHView->GetOperator ();
	m_pHView->SetOperator (new_operator);
	if (old_operator)
		delete old_operator;
}

void HoopsView::begin_camera_manipulate ()
{
	local_set_operator (new HOpCameraManipulate (m_pHView));
}

void HoopsView::begin_select_by_click ()
{
	local_set_operator (new HOpSelectAperture (m_pHView));
}

void HoopsView::begin_select_by_rectangle ()
{
	local_set_operator (new HOpSelectArea (m_pHView));
}

void HoopsView::begin_select_by_polygon ()
{
	auto sel = new HOpSelectPolygon (m_pHView);
	HC_Open_Segment_By_Key (m_pHView->GetWindowspaceKey ());{
		HC_Set_Line_Weight (2);
		HC_Set_Edge_Weight (2);
	}HC_Close_Segment ();
	local_set_operator (sel);
}

void HoopsView::clear_selection ()
{
	set_vertices_selectable (false);
	set_edges_selectable (false);
	set_faces_selectable (false);
	m_pHView->GetSelection ()->DeSelectAll ();
	m_pHView->Update ();
}

void HoopsView::set_vertices_selectable (bool selectable)
{
	OPEN_MESHES_SEG
		QString options = QString ("markers=%1").arg (selectable? "on":"off");
		HC_Set_Selectability (options.toAscii ().data ());
	CLOSE_MESHES_SEG

	//OPEN_GROUPS_SEG
	//	QString options = QString ("markers=%1").arg (selectable? "on":"off");
	//	HC_Set_Selectability (options.toAscii ().data ());	
	//CLOSE_GROUPS_SEG
	m_pHView->SetGeometryChanged ();
	m_pHView->Update ();
}

void HoopsView::set_edges_selectable (bool selectable)
{
	OPEN_MESHES_SEG
		QString options = QString ("lines=%1,edges=%1").arg (selectable? "on":"off");
		HC_Set_Selectability (options.toAscii ().data ());
	CLOSE_MESHES_SEG
	//OPEN_GROUPS_SEG
	//	QString options = QString ("lines=%1,edges=%1").arg (selectable? "on":"off");
	//	HC_Set_Selectability (options.toAscii ().data ());	
	//CLOSE_GROUPS_SEG

	m_pHView->SetGeometryChanged ();
	m_pHView->Update ();
}

void HoopsView::set_faces_selectable (bool selectable)
{
	OPEN_MESHES_SEG
		QString options = QString ("polygons=%1").arg (selectable? "on":"off");
		HC_Set_Selectability (options.toAscii ().data ());		
	CLOSE_MESHES_SEG
	//OPEN_GROUPS_SEG
	//	QString options = QString ("polygons=%1").arg (selectable? "on":"off");
	//	HC_Set_Selectability (options.toAscii ().data ());	
	//CLOSE_GROUPS_SEG
		
	m_pHView->SetGeometryChanged ();
	m_pHView->Update ();
}

void HoopsView::show_boundary (bool show)
{
	OPEN_MESHES_SEG
		HC_Open_Segment ("boundary");{
			if (!show){
				HC_Set_Visibility ("everything=off");
				HC_Set_Rendering_Options ("attribute lock = visibility");
			}else{
				HC_UnSet_Visibility ();
				HC_UnSet_One_Rendering_Option("attribute lock");
			}
		}HC_Close_Segment ();
	CLOSE_MESHES_SEG
	m_pHView->Update ();
}

void HoopsView::show_boundary_vertices (bool show)
{
	OPEN_MESHES_SEG
		HC_Open_Segment ("boundary");{
			HC_Open_Segment ("meshvertices");{
				if (!show){
					HC_Set_Visibility ("everything=off");
				}else{
					HC_UnSet_Visibility ();
					HC_Set_Visibility ("markers=on");
				}
			}HC_Close_Segment ();
		}HC_Close_Segment ();
	CLOSE_MESHES_SEG
	m_pHView->Update ();
}

void HoopsView::show_boundary_edges (bool show)
{
	OPEN_MESHES_SEG
		HC_Open_Segment ("boundary");{
			HC_Open_Segment ("meshedges");{
				if (!show){
					HC_Set_Visibility ("everything=off");
				}else{
					HC_UnSet_Visibility ();
					HC_Set_Visibility ("lines=on");
				}
			}HC_Close_Segment ();
	}HC_Close_Segment ();
	CLOSE_MESHES_SEG
		m_pHView->Update ();
}

void HoopsView::show_boundary_faces (bool show)
{
	OPEN_MESHES_SEG
		HC_Open_Segment ("boundary");{
			HC_Open_Segment ("meshfaces");{
				if (!show){
					HC_Set_Visibility ("everything=off");
				}else{
					HC_UnSet_Visibility ();
					HC_Set_Visibility ("faces=on");
				}
			}HC_Close_Segment ();
	}HC_Close_Segment ();
	CLOSE_MESHES_SEG
		m_pHView->Update ();
}

void HoopsView::show_boundary_cells (bool show)
{
	OPEN_MESHES_SEG
		HC_Open_Segment ("boundary");{
			HC_Open_Segment ("meshcells");{
				if (!show){
					HC_Set_Visibility ("everything=off");
				}else{
					HC_UnSet_Visibility ();
					HC_Set_Visibility ("markers=off,edges=on,faces=on");
				}
			}HC_Close_Segment ();
	}HC_Close_Segment ();
	CLOSE_MESHES_SEG
		m_pHView->Update ();
}

void HoopsView::show_boundary_vertices_indices (bool show)
{
	OPEN_MESHES_SEG
		HC_Open_Segment ("boundary");{
			HC_Open_Segment ("meshvertices");{
				HC_Open_Segment ("indices");{
					if (!show){
						HC_Set_Visibility ("everything=off");
					}else{
						HC_UnSet_Visibility ();
						HC_Set_Visibility ("text=on");
					}
				}HC_Close_Segment ();
			}HC_Close_Segment ();
	}HC_Close_Segment ();
	CLOSE_MESHES_SEG
		m_pHView->Update ();
}

void HoopsView::show_boundary_edges_indices (bool show)
{
	OPEN_MESHES_SEG
		HC_Open_Segment ("boundary");{
			HC_Open_Segment ("meshedges");{
				HC_Open_Segment ("indices");{
					if (!show){
						HC_Set_Visibility ("everything=off");
					}else{
						HC_UnSet_Visibility ();
						HC_Set_Visibility ("text=on");
					}
				}HC_Close_Segment ();
			}HC_Close_Segment ();
	}HC_Close_Segment ();
	CLOSE_MESHES_SEG
		m_pHView->Update ();
}

void HoopsView::show_boundary_faces_indices (bool show)
{
	OPEN_MESHES_SEG
		HC_Open_Segment ("boundary");{
			HC_Open_Segment ("meshfaces");{
				HC_Open_Segment ("indices");{
					if (!show){
						HC_Set_Visibility ("everything=off");
					}else{
						HC_UnSet_Visibility ();
						HC_Set_Visibility ("text=on");
					}
				}HC_Close_Segment ();
			}HC_Close_Segment ();
	}HC_Close_Segment ();
	CLOSE_MESHES_SEG
		m_pHView->Update ();
}

void HoopsView::show_boundary_cells_indices (bool show)
{
	OPEN_MESHES_SEG
		HC_Open_Segment ("boundary");{
			HC_Open_Segment ("meshcells");{
				HC_Open_Segment ("indices");{
					if (!show){
						HC_Set_Visibility ("everything=off");
					}else{
						HC_UnSet_Visibility ();
						HC_Set_Visibility ("text=on");
					}
				}HC_Close_Segment ();
			}HC_Close_Segment ();
	}HC_Close_Segment ();
	CLOSE_MESHES_SEG
		m_pHView->Update ();
}

void HoopsView::show_inner (bool show)
{
	OPEN_MESHES_SEG
		HC_Open_Segment ("inner");{
			if (!show){
				HC_Set_Visibility ("everything=off");
				HC_Set_Rendering_Options ("attribute lock = visibility");
			}else{
				HC_UnSet_Visibility ();
				HC_UnSet_One_Rendering_Option("attribute lock");
			}
	}HC_Close_Segment ();
	CLOSE_MESHES_SEG
		m_pHView->Update ();
}

void HoopsView::show_inner_vertices (bool show)
{
	OPEN_MESHES_SEG
		HC_Open_Segment ("inner");{
			HC_Open_Segment ("meshvertices");{
				if (!show){
					HC_Set_Visibility ("everything=off");
				}else{
					HC_UnSet_Visibility ();
					HC_Set_Visibility ("markers=on");
				}
			}HC_Close_Segment ();
	}HC_Close_Segment ();
	CLOSE_MESHES_SEG
		m_pHView->Update ();
}

void HoopsView::show_inner_edges (bool show)
{
	OPEN_MESHES_SEG
		HC_Open_Segment ("inner");{
			HC_Open_Segment ("meshedges");{
				if (!show){
					HC_Set_Visibility ("everything=off");
				}else{
					HC_UnSet_Visibility ();
					HC_Set_Visibility ("lines=on");
				}
			}HC_Close_Segment ();
	}HC_Close_Segment ();
	CLOSE_MESHES_SEG
		m_pHView->Update ();
}

void HoopsView::show_inner_faces (bool show)
{
	OPEN_MESHES_SEG
		HC_Open_Segment ("inner");{
			HC_Open_Segment ("meshfaces");{
				if (!show){
					HC_Set_Visibility ("everything=off");
				}else{
					HC_UnSet_Visibility ();
					HC_Set_Visibility ("faces=on");
				}
			}HC_Close_Segment ();
	}HC_Close_Segment ();
	CLOSE_MESHES_SEG
		m_pHView->Update ();
}

void HoopsView::show_inner_cells (bool show)
{
	OPEN_MESHES_SEG
		HC_Open_Segment ("inner");{
			HC_Open_Segment ("meshcells");{
				if (!show){
					HC_Set_Visibility ("everything=off");
				}else{
					HC_UnSet_Visibility ();
					HC_Set_Visibility ("markers=off,edges=on,faces=on");
				}
			}HC_Close_Segment ();
	}HC_Close_Segment ();
	CLOSE_MESHES_SEG
		m_pHView->Update ();
}

void HoopsView::show_inner_vertices_indices (bool show)
{
	OPEN_MESHES_SEG
		HC_Open_Segment ("inner");{
			HC_Open_Segment ("meshvertices");{
				HC_Open_Segment ("indices");{
					if (!show){
						HC_Set_Visibility ("everything=off");
					}else{
						HC_UnSet_Visibility ();
						HC_Set_Visibility ("text=on");
					}
				}HC_Close_Segment ();
			}HC_Close_Segment ();
	}HC_Close_Segment ();
	CLOSE_MESHES_SEG
		m_pHView->Update ();
}

void HoopsView::show_inner_edges_indices (bool show)
{
	OPEN_MESHES_SEG
		HC_Open_Segment ("inner");{
			HC_Open_Segment ("meshedges");{
				HC_Open_Segment ("indices");{
					if (!show){
						HC_Set_Visibility ("everything=off");
					}else{
						HC_UnSet_Visibility ();
						HC_Set_Visibility ("text=on");
					}
				}HC_Close_Segment ();
			}HC_Close_Segment ();
	}HC_Close_Segment ();
	CLOSE_MESHES_SEG
		m_pHView->Update ();
}

void HoopsView::show_inner_faces_indices (bool show)
{
	OPEN_MESHES_SEG
		HC_Open_Segment ("inner");{
			HC_Open_Segment ("meshfaces");{
				HC_Open_Segment ("indices");{
					if (!show){
						HC_Set_Visibility ("everything=off");
					}else{
						HC_UnSet_Visibility ();
						HC_Set_Visibility ("text=on");
					}
				}HC_Close_Segment ();
			}HC_Close_Segment ();
	}HC_Close_Segment ();
	CLOSE_MESHES_SEG
		m_pHView->Update ();
}

void HoopsView::show_inner_cells_indices (bool show)
{
	OPEN_MESHES_SEG
		HC_Open_Segment ("inner");{
			HC_Open_Segment ("meshcells");{
				HC_Open_Segment ("indices");{
					if (!show){
						HC_Set_Visibility ("everything=off");
					}else{
						HC_UnSet_Visibility ();
						HC_Set_Visibility ("text=on");
					}
				}HC_Close_Segment ();
			}HC_Close_Segment ();
	}HC_Close_Segment ();
	CLOSE_MESHES_SEG
		m_pHView->Update ();
}

int HoopsView::get_selected_elements (std::vector<OvmVeH> &vertices, 
	std::vector<OvmEgH> &edges, std::vector<OvmFaH> &faces)
{
	HSelectionSet *selec_set = m_pHView->GetSelection();
	int selected_count = selec_set->GetSize();
	char keytype[100];
	selec_set->SetSortSelectionList (true);
	for (int i = 0; i < selected_count; i++)
	{
		HC_KEY key = selec_set->GetAt(i);
		int idx;
		HC_Show_Key_Type (key, keytype);
		if (streq (keytype, "marker")){
			HC_Open_Geometry (key);{
				HC_Show_One_Net_User_Data (0, &idx, sizeof (int));
			}HC_Close_Geometry ();
			vertices.push_back (OvmVeH(idx));
		}else if (streq (keytype, "line")){
			HC_Open_Geometry (key);{
				HC_Show_One_Net_User_Data (0, &idx, sizeof (int));
			}HC_Close_Geometry ();
			edges.push_back (OvmEgH(idx));
		}else if (streq (keytype, "polygon")){
			HC_Open_Geometry (key);{
				HC_Show_One_Net_User_Data (0, &idx, sizeof (int));
			}HC_Close_Geometry ();
			faces.push_back (OvmFaH(idx));
		}else{
			continue;
		}
	}
	selec_set->DeSelectAll ();
	return selected_count;
}

int HoopsView::get_selected_elements (std::vector<OvmVeH> *vertices, 
	std::vector<OvmEgH> *edges, std::vector<OvmFaH> *faces)
{
	HSelectionSet *selec_set = m_pHView->GetSelection();
	int selected_count = selec_set->GetSize();
	char keytype[100];
	selec_set->SetSortSelectionList (true);
	for (int i = 0; i < selected_count; i++)
	{
		HC_KEY key = selec_set->GetAt(i);
		int idx;
		HC_Show_Key_Type (key, keytype);
		if (streq (keytype, "marker")){
			if (vertices){
				HC_Open_Geometry (key);{
					HC_Show_One_Net_User_Data (0, &idx, sizeof (int));
				}HC_Close_Geometry ();
				vertices->push_back (OvmVeH(idx));
			}
		}else if (streq (keytype, "line")){
			if (edges){
				HC_Open_Geometry (key);{
					HC_Show_One_Net_User_Data (0, &idx, sizeof (int));
				}HC_Close_Geometry ();
				edges->push_back (OvmEgH(idx));
			}
		}else if (streq (keytype, "polygon")){
			if (faces){
				HC_Open_Geometry (key);{
					HC_Show_One_Net_User_Data (0, &idx, sizeof (int));
				}HC_Close_Geometry ();
				faces->push_back (OvmFaH(idx));
			}
		}else{
			continue;
		}
	}
	selec_set->DeSelectAll ();
	return selected_count;
}

int HoopsView::get_selected_acis_entities (std::vector<VERTEX*> *vertices, std::vector<EDGE*> *edges, std::vector<FACE*> *faces)
{
	HSelectionSet *selec_set = m_pHView->GetSelection();
	int selected_count = selec_set->GetSize();
	char keytype[100];
	selec_set->SetSortSelectionList (true);
	for (int i = 0; i < selected_count; i++)
	{
		HC_KEY key = selec_set->GetAt(i);
		char skey_type[MVO_BUFFER_SIZE];
		HC_Show_Key_Type(key, skey_type);
		if (streq(skey_type, "reference")){
			key = HC_Show_Reference_Geometry (key);
		}
		auto entity = HA_Compute_Entity_Pointer (key);
		if (is_VERTEX (entity)){
			if (vertices){
				vertices->push_back ((VERTEX*)entity);
			}
		}else if (is_EDGE (entity)){
			if (edges){
				edges->push_back ((EDGE*)entity);
			}
		}else if (is_FACE (entity)){
			if (faces){
				faces->push_back ((FACE*)entity);
			}
		}else{
			continue;
		}
	}
	//selec_set->DeSelectAll ();
	return selected_count;
}

void HoopsView::begin_cutting ()
{
	HC_Open_Segment_By_Key (m_pHView->GetModelKey ());{
		char status[255];
		HC_KEY key = HC_Open_Segment ("OpCutMesh");{
			if (!HC_Show_Existence ("geometry")){
				local_set_operator (new OpCutMesh (m_pHView));
			}else{
				auto move_handle = new HOpMoveHandle (m_pHView, false);
				move_handle->SetupObject (key); 
				local_set_operator (move_handle);
			}
		}HC_Close_Segment ();
		
	}HC_Close_Segment ();
	m_pHView->SetGeometryChanged ();
	m_pHView->Update ();
}

HC_KEY HoopsView::get_cutting_plane_key ()
{
	char type[255];
	HC_KEY key = INVALID_KEY;
	HC_KEY cutting_plane_key = INVALID_KEY;
	HC_Open_Segment_By_Key (m_pHView->GetModelKey ());{
		HC_Open_Segment ("OpCutMesh");{
			HC_Begin_Contents_Search (".", "geometry");{
				while (HC_Find_Contents (type, &key)){
					if (streq (type, "polygon")) {
						cutting_plane_key = key;
						break;
					}
				}
			}HC_End_Contents_Search ();
		}HC_Close_Segment ();
	}HC_Close_Segment ();

	return cutting_plane_key;
}

void HoopsView::clear_cutting_plane ()
{
	
	HC_Open_Segment_By_Key (m_pHView->GetModelKey ());{
		HC_Open_Segment ("OpCutMesh");{
		}HC_Close_Segment ();
		HC_Delete_Segment ("OpCutMesh");
	}HC_Close_Segment ();
	m_pHView->SetGeometryChanged ();
	m_pHView->Update ();
}

void HoopsView::show_mesh_vertices (bool show)
{
	OPEN_MESHES_SEG
		if (!show){
			HC_Set_Visibility ("markers=off");
		}else{
			HC_Set_Visibility ("markers=on");
		}
	CLOSE_MESHES_SEG
	m_pHView->Update ();
}

void HoopsView::show_mesh_edges (bool show)
{
	OPEN_MESHES_SEG
		if (!show){
			HC_Set_Visibility ("lines=off");
		}else{
			HC_Set_Visibility ("lines=on");
		}
	CLOSE_MESHES_SEG
	m_pHView->Update ();
}

void HoopsView::show_mesh_faces (bool show)
{
	OPEN_MESHES_SEG
		if (!show){
			HC_Set_Visibility ("faces=off");
		}else{
			HC_Set_Visibility ("faces=on");
			//HC_Set_Color ("faces=green");
		}
	CLOSE_MESHES_SEG
	m_pHView->Update ();
}

void HoopsView::show_mesh_faces_transparent ()
{
	OPEN_MESHES_SEG
		HC_Set_Visibility ("faces=on");
		HC_Set_Color ("faces=(green,transmission = (r=0.6 g=0.6 b=0.6))");
	CLOSE_MESHES_SEG
	m_pHView->Update ();
}

void HoopsView::move_vertices (VolumeMesh *mesh, std::vector<OvmVeH> &vhs)
{
	auto move_handle = new HOpMoveHandle (m_pHView, false);
	auto group = new VolumeMeshElementGroup (mesh, "node move");
	JC::vector_to_unordered_set (vhs, group->vhs);
	MeshRenderOptions render_options;
	render_options.vertex_color = "red";
	render_options.vertex_size = 3;
	auto key = render_mesh_group (group, render_options);
	move_handle->SetupObject (key);
	local_set_operator (move_handle);
	temp_group = group;
}

void HoopsView::move_vertices_ok (BODY *body)
{
	if (temp_group == NULL) return;
	HC_KEY key = POINTER_TO_KEY(temp_group);
	auto mesh = temp_group->mesh;
	if (!mesh->vertex_property_exists<bool> ("vertexfixedforsmooth")){
		auto tmp = mesh->request_vertex_property<bool> ("vertexfixedforsmooth", false);
		mesh->set_persistent(tmp);
	}
	auto V_FIXED = mesh->request_vertex_property<bool> ("vertexfixedforsmooth");

	assert (mesh->vertex_property_exists<unsigned long> ("entityptr"));
	auto V_ENTITY_PTR = mesh->request_vertex_property<unsigned long>("entityptr");

	auto move_handle = dynamic_cast<HOpMoveHandle*>(m_pHView->GetOperator ());
	if (move_handle == NULL){
		QMessageBox::warning (NULL, "警告", "当前的操作不是移动操作！");
		return;
	}


	float matrix_aa[100] = {0};
	HC_Open_Segment_By_Key (key);{
		HC_Show_Net_Modelling_Matrix (matrix_aa);
	}HC_Close_Segment ();
	
	int points_count = temp_group->vhs.size ();
	HPoint *points = new HPoint[points_count], 
		*transformed_points = new HPoint[points_count];

	std::vector<OvmVeH> vhs_vec;
	JC::unordered_set_to_vector (temp_group->vhs, vhs_vec);
	for (int i = 0; i != points_count; ++i){
		auto pos = mesh->vertex (vhs_vec[i]);
		points[i] = HPoint (pos[0], pos[1], pos[2]);
	}
	HC_Compute_Transformed_Points (points_count, points, matrix_aa, transformed_points);

	ENTITY_LIST face_group_list;
	api_get_faces (body, face_group_list);
	for (int i = 0; i != points_count; ++i){
		auto Ni = vhs_vec[i];
		V_FIXED[Ni] = true;

		HPoint trans_pt = transformed_points[i];
		SPAposition pos = SPAposition (trans_pt.x, trans_pt.y, trans_pt.z);
		SPAposition closest_pos;
		double dist = 0.0f;
		double curr_closest_dist = std::numeric_limits<double>::max ();
		ENTITY *curr_closest_f = NULL;
		SPAposition curr_closest_pos;
		for (int m = 0; m != face_group_list.count (); ++m){
			auto tmp_f = face_group_list[m];
			api_entity_point_distance (tmp_f, pos, closest_pos, dist);
			if (dist < curr_closest_dist){
				curr_closest_dist = dist;
				curr_closest_pos = closest_pos;
				curr_closest_f = tmp_f;
			}
		}
		mesh->set_vertex (Ni, SPA2POS(curr_closest_pos));
		V_ENTITY_PTR[Ni] = (unsigned long)(curr_closest_f);
	}

	delete[] points;
	delete[] transformed_points;
	local_set_operator (new HOpCameraManipulate (m_pHView));
	derender_one_mesh_group (temp_group);
	delete temp_group;
	temp_group = NULL;
}

bool HoopsView::segment_existing (HC_KEY key)
{
	char status[100];
	HC_Show_Key_Status (key, status);
	if (strncmp (status, "invalid", 1) == 0) return false;
		else return true;
}
void HoopsView::show_acis_body (bool show)
{
	if (show){
		HC_Open_Segment_By_Key (m_pHView->GetModelKey ());{
			HC_Open_Segment ("acismodel");{
				HC_UnSet_Visibility ();
			}HC_Close_Segment ();
		}HC_Close_Segment ();
	}else{
		HC_Open_Segment_By_Key (m_pHView->GetModelKey ());{
			HC_Open_Segment ("acismodel");{
				HC_Set_Visibility ("everything=off");
			}HC_Close_Segment ();
		}HC_Close_Segment ();
	}

	m_pHView->Update ();
}

void HoopsView::show_acis_vertices (bool show)
{
	HC_Open_Segment_By_Key (m_pHView->GetModelKey ());{
		HC_Open_Segment ("acismodel");{HC_Open_Segment ("acisvertices");{
			if (show)
				HC_Set_Visibility ("markers=on");
			else
				HC_Set_Visibility ("markers=off");

		}HC_Close_Segment ();}HC_Close_Segment ();
	}HC_Close_Segment ();
	m_pHView->SetGeometryChanged ();
	m_pHView->Update ();
}

void HoopsView::show_acis_edges (bool show)
{
	HC_Open_Segment_By_Key (m_pHView->GetModelKey ());{
		HC_Open_Segment ("acismodel");{HC_Open_Segment ("acisedges");{
			if (show)
				HC_Set_Visibility ("lines=on");
			else
				HC_Set_Visibility ("lines=off");
		}HC_Close_Segment ();}HC_Close_Segment ();
	}HC_Close_Segment ();
	m_pHView->SetGeometryChanged ();
	m_pHView->Update ();
}

void HoopsView::show_acis_faces (bool show)
{
	HC_Open_Segment_By_Key (m_pHView->GetModelKey ());{
		HC_Open_Segment ("acismodel");{HC_Open_Segment ("acisfaces");{
			if (show)
				HC_Set_Visibility ("faces=on");
			else
				HC_Set_Visibility ("faces=off");
			
		}HC_Close_Segment ();}HC_Close_Segment ();
	}HC_Close_Segment ();
	m_pHView->SetGeometryChanged ();
	m_pHView->Update ();
}

void HoopsView::set_acis_vertices_selectability (bool selectable)
{
	HC_Open_Segment_By_Key (m_pHView->GetModelKey ());{
		HC_Open_Segment ("acismodel");{HC_Open_Segment ("body");{
			if (selectable)
				HC_Set_Selectability ("markers=on");
			else
				HC_Set_Selectability ("markers=off");
		}HC_Close_Segment ();}HC_Close_Segment ();
	}HC_Close_Segment ();
	m_pHView->Update ();
}

void HoopsView::set_acis_edges_selectable (bool selectable)
{
	HC_Open_Segment_By_Key (m_pHView->GetModelKey ());{
		HC_Open_Segment ("acismodel");{HC_Open_Segment ("body");{
			if (selectable)
				HC_Set_Selectability ("lines=on");
			else
				HC_Set_Selectability ("lines=off");
		}HC_Close_Segment ();}HC_Close_Segment ();
	}HC_Close_Segment ();
	m_pHView->Update ();
}

void HoopsView::set_acis_faces_selectable (bool selectable)
{
	HC_Open_Segment_By_Key (m_pHView->GetModelKey ());{
		HC_Open_Segment ("acismodel");{
			if (selectable)
				HC_Set_Selectability ("faces=on");
			else
				HC_Set_Selectability ("faces=off");
		}HC_Close_Segment ();
	}HC_Close_Segment ();
	m_pHView->Update ();
	//m_pHView->GetModel ()->
}
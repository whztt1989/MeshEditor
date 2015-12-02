#include "StdAfx.h"
#include "OpCutMesh.h"
#include "HTools.h"
#include "HBaseModel.h"
#include "HBaseView.h"
#include "HBaseOperator.h"
#include "HEventInfo.h"
#include "HUtility.h"
#include "HOpMoveHandle.h"

OpCutMesh::OpCutMesh (HBaseView* view, int DoRepeat, int DoCapture, bool UseCenterMarker)
	: HOpConstructRectangle(view, DoRepeat, DoCapture)
{
}


OpCutMesh::~OpCutMesh(void)
{
}


HBaseOperator * OpCutMesh::Clone()
{
	return (HBaseOperator *)new OpCutMesh(GetView());
}


const char * OpCutMesh::GetName() { return "OpCutMesh"; }

int OpCutMesh::OnLButtonUp(HEventInfo &event)
{
	if(OperatorStarted()){
		HC_KEY polygon_key = INVALID_KEY, old_key = INVALID_KEY;
		HOpConstructRectangle::OnLButtonUp(event);

		HPoint points[4];

		points[0] = m_ptRectangle[0];
		points[1].x = m_ptRectangle[0].x;
		points[1].y = m_ptRectangle[1].y;
		points[2] = m_ptRectangle[1];
		points[3].x = m_ptRectangle[1].x;
		points[3].y = m_ptRectangle[0].y;

		HC_Open_Segment_By_Key(GetView()->GetSceneKey());{
			HC_Compute_Coordinates(".","outer window", &points[0], "world", &points[0]);
			HC_Compute_Coordinates(".","outer window", &points[1], "world", &points[1]);
			HC_Compute_Coordinates(".","outer window", &points[2], "world", &points[2]);
			HC_Compute_Coordinates(".","outer window", &points[3], "world", &points[3]);
		}HC_Close_Segment();

		HC_Open_Segment_By_Key(GetView()->GetModelKey());{
			polygon_key = HC_Open_Segment(GetName());{
				HC_Compute_Coordinates(".","world", &points[0], "object", &points[0]);
				HC_Compute_Coordinates(".","world", &points[1], "object", &points[1]);
				HC_Compute_Coordinates(".","world", &points[2], "object", &points[2]);
				HC_Compute_Coordinates(".","world", &points[3], "object", &points[3]);
				 HC_Insert_Polygon(4, points);
			}HC_Close_Segment();
		}HC_Close_Segment();
		
		auto move_handle = new HOpMoveHandle (GetView (), false);
		move_handle->SetupObject (polygon_key);
		GetView ()->SetOperator (move_handle);
		GetView()->Update(); 
		delete this;
	}

	return(HOP_READY);
}
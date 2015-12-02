#pragma once
#include "hopconstructrectangle.h"
class OpCutMesh :
	public HOpConstructRectangle
{
public:
	OpCutMesh(HBaseView* view, int DoRepeat=0, int DoCapture=1, bool UseCenterMarker=true);
	~OpCutMesh(void);

	virtual const char * GetName();  

	virtual HBaseOperator * Clone(); 
     
	virtual int OnLButtonUp(HEventInfo &hevent); 
};


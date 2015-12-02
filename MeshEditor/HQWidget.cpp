#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <map>

// qt includes
#include <QEvent>
#include <QMessageBox>
#include <QMouseEvent>
#include <QCoreApplication>
#include <QApplication>
#include <QHash>
#include <QColormap>
#include <QAbstractButton>

// hoops_mvo includes
#include "HBaseOperator.h"
#include "HDB.h"
#include "HBaseView.h"
#include "HEventInfo.h"
#include "HEventManager.h"
// hoops
#include "hc.h"
#include "vhash.h"
#include "vlist.h"

#include "HQWidget.h"

// this needs to be created first in the application
extern HDB * m_pHDB;


HQWidget::HQWidget( QWidget *parent, QString const & name, Qt::WFlags f, int aa)
    : QWidget(parent, f)
{
    initDone = false;
	ignore_update = false;

	//setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_PaintOnScreen);
	setBackgroundRole( QPalette::NoRole);

	if ( this == 0 ) {
		QMessageBox::information( 0, "Qt HOOPS Viewer","HQWidget: Paint device cannot be null");
    }

    if (this->devType() != QInternal::Widget &&
		this->devType() != QInternal::Pixmap ) {
			QMessageBox::information( 0, "Qt HOOPS Viewer","HQWidget: Unsupported paint device type");
        }


    m_pHBaseModel = 0;
    m_pHView = 0; 

	my_clip_override=0;
    my_colormap=0;
    my_windowid=0;
    my_have_gl_visual = false;
    my_aa = aa;

    // this does all the work for palettes and windowid

    if(streq(m_pHDB->GetDriverType(), "opengl")){

	setup_window(true);
	
	if(!my_have_gl_visual){

	    QMessageBox::information( 0, "Qt HOOPS Viewer",
	    	"Sorry OpenGL is not available");
	    m_pHDB->SetDriverType("msw");
	}

    }else
	setup_window(false);
}

HQWidget::~HQWidget()
{

}

QPaintEngine* HQWidget::paintEngine() const{
	return NULL;
}

void * HQWidget::GetColorMap()
{
	return my_colormap;
}

void * HQWidget::GetWindowId()
{
	return my_windowid;
}

void * HQWidget::GetClipOverride()
{
	return my_clip_override;
}

void HQWidget::AdjustAxisWindow()
{
 	m_pHView->AdjustAxisWindow( 80 );
}

void HQWidget::paintEvent( QPaintEvent * e)
{
	if(ignore_update)
		return;
	// If any HOOPS error windows come up during this Init call, (such as the no license message)
	// then a paint event gets called again, which calls this Init function again before it has
	// finished...  which leads to trouble.  So we use this event filter (defined in HQWidget) that
	// blocks all paint events, then we remove it at the end of this call.

	if ( !initDone ) {
		installEventFilter(this);
		Init();
		removeEventFilter(this);
		initDone = true;
	}
	else{
		setAttribute(Qt::WA_NoSystemBackground);
	}
	
	AdjustAxisWindow();
	
	if (m_pHView)
		HC_Control_Update_By_Key(m_pHView->GetViewKey(), "redraw everything");
	m_pHView->SetCameraChanged();

	m_pHView->Update();

}


bool HQWidget::eventFilter(QObject* obj, QEvent* ev){
	if (ev->type() == QEvent::Paint){
		return true;
	}
	else{
		return QObject::eventFilter(obj, ev);
	}
}

void HQWidget::resizeEvent( QResizeEvent * e)
{
    if ( initDone ) {
		AdjustAxisWindow();
		if (m_pHView)
			HC_Control_Update_By_Key(m_pHView->GetViewKey(), "redraw everything");
		m_pHView->SetCameraChanged();
	}
}


void HQWidget::focusInEvent(QFocusEvent* e)
{
	if ( initDone ) {
		AdjustAxisWindow();
		if (m_pHView)
			HC_Control_Update_By_Key(m_pHView->GetViewKey(), "redraw everything");
		m_pHView->SetCameraChanged();
	}
}


void HQWidget::focusOutEvent(QFocusEvent* e)
{
	if ( initDone ) {
		AdjustAxisWindow();
		if (m_pHView)
			HC_Control_Update_By_Key(m_pHView->GetViewKey(), "redraw everything");
		m_pHView->SetCameraChanged();
	}
}


// button mappings
unsigned long HQWidget::MapFlags( unsigned long state )
{
	unsigned long flags=0;

	/*map the qt events state to MVO*/
	if(state & Qt::LeftButton) flags|=MVO_LBUTTON;
	if(state & Qt::RightButton) flags|=MVO_RBUTTON;
	if(state & Qt::MidButton) flags|=MVO_MBUTTON;
	if(state & Qt::SHIFT) flags|=MVO_SHIFT;
	if(state & Qt::CTRL) flags|=MVO_CONTROL;

	return flags;
}

void HQWidget::mouseDoubleClickEvent( QMouseEvent * e)
{
    if(e->button() == Qt::LeftButton) 
	OnLeftButtonDblClk(e);
    else if(e->button() == Qt::RightButton) 
	OnRightButtonDblClk(e);
    else{
	    if(e->button() == Qt::MidButton) 
		OnMidButtonDblClk(e);
	}
}

void HQWidget::mousePressEvent( QMouseEvent * e)
{
    if(e->button() == Qt::LeftButton)
	OnLeftButtonDown(e);
    else if(e->button() == Qt::RightButton) 
	OnRightButtonDown(e);
    else {
	if(e->button() == Qt::MidButton) 
	    OnMidButtonDown(e);
    }
}

void HQWidget::mouseReleaseEvent( QMouseEvent * e)
{
    if(e->button() == Qt::LeftButton)
	OnLeftButtonUp(e);
    else if(e->button() == Qt::RightButton) 
	OnRightButtonUp(e);
    else {
	if(e->button() == Qt::MidButton) 
	    OnMidButtonUp(e);
    }
}


void HQWidget::OnLeftButtonDown(QMouseEvent * e)
{
    if (m_pHView) {
	HEventInfo event(m_pHView);

        unsigned long flags=MapFlags(e->buttons()|e->modifiers ());
	event.SetPoint(HE_LButtonDown, e->x(), e->y(), flags);
	HLISTENER_EVENT(HMouseListener, m_pHView->GetEventManager(), OnLButtonDown(event))

    }
}

void HQWidget::OnLeftButtonUp(QMouseEvent * e)
{
    if (m_pHView) {
	HEventInfo event(m_pHView);

        unsigned long flags=MapFlags(e->buttons()|e->modifiers ());
	event.SetPoint(HE_LButtonUp, e->x(), e->y(), flags);
	HLISTENER_EVENT(HMouseListener, m_pHView->GetEventManager(), OnLButtonUp(event))

    }
}

void HQWidget::OnLeftButtonDblClk(QMouseEvent * e)
{
    if (m_pHView) {
	HEventInfo event(m_pHView);

        unsigned long flags=MapFlags(e->buttons()|e->modifiers ());
	event.SetPoint(HE_LButtonDblClk, e->x(), e->y(), flags);
	HLISTENER_EVENT(HMouseListener, m_pHView->GetEventManager(), OnLButtonDblClk(event))

    }
}


void HQWidget::OnMidButtonDown(QMouseEvent * e)
{
    if (m_pHView) {
	HEventInfo event(m_pHView);

        unsigned long flags=MapFlags(e->buttons()|e->modifiers ());
	event.SetPoint(HE_MButtonDown, e->x(), e->y(), flags);
	HLISTENER_EVENT(HMouseListener, m_pHView->GetEventManager(), OnMButtonDown(event))

    }
}

void HQWidget::OnMidButtonUp(QMouseEvent * e)
{
    if (m_pHView) {
	HEventInfo event(m_pHView);

        unsigned long flags=MapFlags(e->buttons()|e->modifiers ());
	event.SetPoint(HE_MButtonUp, e->x(), e->y(), flags);
	HLISTENER_EVENT(HMouseListener, m_pHView->GetEventManager(), OnMButtonUp(event))

    }
}

void HQWidget::OnMidButtonDblClk(QMouseEvent * e)
{
    if (m_pHView) {
	HEventInfo event(m_pHView);

        unsigned long flags=MapFlags(e->buttons()|e->modifiers ());
	event.SetPoint(HE_MButtonDblClk, e->x(), e->y(), flags);
	HLISTENER_EVENT(HMouseListener, m_pHView->GetEventManager(), OnMButtonDblClk(event))

    }
}

void HQWidget::OnRightButtonDown(QMouseEvent * e)
{
    if (m_pHView) {
	HEventInfo event(m_pHView);

        unsigned long flags=MapFlags(e->buttons()|e->modifiers ());
	event.SetPoint(HE_RButtonDown, e->x(), e->y(), flags);
	HLISTENER_EVENT(HMouseListener, m_pHView->GetEventManager(), OnRButtonDown(event))

    }
}


void HQWidget::OnRightButtonUp(QMouseEvent * e)
{
    if (m_pHView) {
	HEventInfo event(m_pHView);

        unsigned long flags=MapFlags(e->buttons()|e->modifiers ());
	event.SetPoint(HE_RButtonUp, e->x(), e->y(), flags);
	HLISTENER_EVENT(HMouseListener, m_pHView->GetEventManager(), OnRButtonUp(event))

    }
}

void HQWidget::wheelEvent(QWheelEvent * e)
{
    if (m_pHView) {
	HEventInfo event(m_pHView);
  	event.SetMouseWheelDelta(e->delta());
        unsigned long flags=MapFlags(e->buttons()|e->modifiers ());
	event.SetPoint(HE_RButtonUp, e->x(), e->y(), flags);
 	HLISTENER_EVENT(HMouseListener, m_pHView->GetEventManager(), OnMouseWheel(event))

    }
}


void HQWidget::OnRightButtonDblClk(QMouseEvent * e)
{
    if (m_pHView) {
	HEventInfo event(m_pHView);

        unsigned long flags=MapFlags(e->buttons()|e->modifiers ());
	event.SetPoint(HE_RButtonDblClk, e->x(), e->y(), flags);
	HLISTENER_EVENT(HMouseListener, m_pHView->GetEventManager(), OnRButtonDblClk(event))

    }
}

void HQWidget::mouseMoveEvent(QMouseEvent * e)
{
	if (m_pHView) {
		HEventInfo	event(m_pHView);
		unsigned long flags=MapFlags(e->buttons()|e->modifiers());
		event.SetPoint(HE_MouseMove, e->x(), e->y(), flags);
		HLISTENER_EVENT(HMouseListener, m_pHView->GetEventManager(), OnMouseMove(event))
	}
}

static std::map<unsigned int,bool> key_states;

void HQWidget::keyPressEvent ( QKeyEvent * e )
{
	unsigned int nChar;
	nChar = e->nativeVirtualKey();

	if (m_pHView) {
		HEventInfo  event(m_pHView);
		HBaseOperator *op = m_pHView->GetCurrentOperator();

		if (op) {
			unsigned long flags=MapFlags(e->modifiers());

			event.SetKey(HE_KeyDown, nChar, (unsigned int)1, flags);			
			HLISTENER_EVENT(HMouseListener, m_pHView->GetEventManager(), OnKeyDown(event))
		}
	}

	key_states[nChar] = true;	
}

void HQWidget::keyReleaseEvent ( QKeyEvent * e )
{
	unsigned int nChar;
	nChar = e->nativeVirtualKey();

	if (m_pHView) {
		HEventInfo  event(m_pHView);
		HBaseOperator *op = m_pHView->GetCurrentOperator();

		if (op) {
			unsigned long flags=MapFlags(e->modifiers());
			
			event.SetKey(HE_KeyUp, nChar, (unsigned int)1, flags);
			HLISTENER_EVENT(HMouseListener, m_pHView->GetEventManager(), OnKeyUp(event))
		}
	}
		
	key_states[nChar] = false;
}


// this is overloaded by the child class
void HQWidget::Init()
{
	
}

void HQWidget::setup_window(bool use_gl)
{
	my_windowid= (void *)((QWidget *) this)->winId();
	my_colormap=0;
	my_have_gl_visual = use_gl;
}


bool HQWidget::GetKeyState(unsigned int key, int &flags)
{
	return key_states.count(key) == 0 ? 0 : key_states[key];
}

struct DriverNamePair
{
	QString UiName;
	const char *DriverName;
};

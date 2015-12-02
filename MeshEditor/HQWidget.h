#ifndef HQWIDGET_H
#define HQWIDGET_H

#include <QWidget>

#include "HBaseModel.h"
#include "HBaseView.h"


/*!
	The HQWidget class provides a HOOPS-specific implementation of the QWidget object. 	It serves as a base HOOPS/Qt widget 
	class and should be sub-classed to create an application specific derivation.

	HQWidget creates and manages the connection of a HOOPS/3dGS driver instance to a QWidget object.  The custom widget:

	- optimally configures associated windowing system resources, such as X11 Visuals & colormaps or 
	  MS Windows color palettes

	- ensures correct handling of "Paint/Expose" and "Resize" events

	- overrides the QWidget's Mouse and Key events and pass the events to the appropriate HOOPS/MVO View's current Operator methods 
*/
class HQWidget: public QWidget
{
	Q_OBJECT
public:
	HQWidget( QWidget* parent=0, QString const & name = "", Qt::WFlags f=0, int aa=0);
	~HQWidget();

	/*! \return A pointer to the HOOPS/MVO HBaseView object associated with the widget */
	HBaseView* GetHoopsView() { return (HBaseView*) m_pHView; }

	/*! \return A pointer to the HOOPS/MVO HBaseModel object associated with the widget */
	HBaseModel* GetHoopsModel() { return (HBaseModel*) m_pHBaseModel; }

	static bool GetKeyState(unsigned int key, int &flags);

protected:

	/*! \return A pointer to the colormap associated with the widget */
	void * GetColorMap();

	/*! \return A pointer to the window id associated with the widget */
	void * GetWindowId();

	/*! \return A pointer to the clip override structure; currently only applies to OS/X */
	void * GetClipOverride();

	/*! Performs HOOPS/QT specific initialization */
    virtual void Init();	

	/*! return null */
	virtual QPaintEngine* paintEngine() const;

	/*! 
		Dispatches the OnLeftButtonDown event to all event listeners that are registered for the OnLeftButtonDown event type.
		The GUI specific flags are mapped to HOOPS/MVO abstracted flags
	*/
	virtual void OnLeftButtonDown(QMouseEvent * e = 0);

	/*! 
		Dispatches the OnLeftButtonUp event to all event listeners that are registered for the OnLeftButtonUp event type.
		The GUI specific flags are mapped to HOOPS/MVO abstracted flags
	*/
	virtual void OnLeftButtonUp(QMouseEvent * e = 0);

	/*! 
		Dispatches the OnLeftButtonDblClk event to all event listeners that are registered for the OnLeftButtonDblClk event type.
		The GUI specific flags are mapped to HOOPS/MVO abstracted flags
	*/
	virtual void OnLeftButtonDblClk(QMouseEvent * e = 0);

	/*! 
		Dispatches the OnMidButtonDown event to all event listeners that are registered for the OnMidButtonDown event type.
		The GUI specific flags are mapped to HOOPS/MVO abstracted flags
	*/
	virtual void OnMidButtonDown(QMouseEvent * e = 0);

	/*! 
		Dispatches the OnMidButtonUp event to all event listeners that are registered for the OnMidButtonUp event type.
		The GUI specific flags are mapped to HOOPS/MVO abstracted flags
	*/
	virtual void OnMidButtonUp(QMouseEvent * e = 0);

	/*! 
		Dispatches the OnMidButtonDblClk event to all event listeners that are registered for the OnMidButtonDblClk event type.
		The GUI specific flags are mapped to HOOPS/MVO abstracted flags
	*/
	virtual void OnMidButtonDblClk(QMouseEvent * e = 0);

	/*! 
		Dispatches the OnRightButtonDown event to all event listeners that are registered for the OnRightButtonDown event type.
		The GUI specific flags are mapped to HOOPS/MVO abstracted flags
	*/
	virtual void OnRightButtonDown(QMouseEvent * e = 0);

	/*! 
		Dispatches the OnRightButtonUp event to all event listeners that are registered for the OnRightButtonUp event type.
		The GUI specific flags are mapped to HOOPS/MVO abstracted flags
	*/
	virtual void OnRightButtonUp(QMouseEvent * e = 0);

	/*! 
		Dispatches the OnRightButtonDblClk event to all event listeners that are registered for the OnRightButtonDblClk event type.
		The GUI specific flags are mapped to HOOPS/MVO abstracted flags
	*/
	virtual void OnRightButtonDblClk(QMouseEvent * e = 0);

	/*! 
		Dispatches the OnRightButtonDblClk event to all event listeners that are registered for the OnMouseWheel event type.
		The GUI specific flags are mapped to HOOPS/MVO abstracted flags
	*/
	virtual void wheelEvent(QWheelEvent * e = 0);

	/*! 
		Overloaded to automatically perform a HOOPS/3dGS update
	*/
	virtual void paintEvent( QPaintEvent* e);

	/*! 
		Overloaded to automatically perform a HOOPS/3dGS update
	*/
	virtual void resizeEvent( QResizeEvent* e);

	virtual void focusInEvent(QFocusEvent* e);

	virtual void focusOutEvent(QFocusEvent* e);


	/*! 
		Calls the OnKeyDown method of the view's current HOOPS/MVO HBaseOperator object. 
		The GUI specific flags are mapped to HOOPS/MVO abstracted flags
	*/
	virtual void keyPressEvent ( QKeyEvent * e );

	/*! 
		Calls the OnKeyUp method of the view's current HOOPS/MVO HBaseOperator object. 
		The GUI specific flags are mapped to HOOPS/MVO abstracted flags
	*/
	virtual void keyReleaseEvent ( QKeyEvent * e );

	/*! 
		Looks at the state of the left/mid/right mouse buttons and calls the appropriate On[type]ButtonDown method of the widget
	*/
	virtual void mousePressEvent( QMouseEvent * mev);

	/*! 
		Looks at the state of the left/mid/right mouse buttons and calls the appropriate On[type]ButtonDblClk method of the widget
	*/
	virtual void mouseDoubleClickEvent( QMouseEvent * mev);

	/*! 
		Looks at the state of the left/mid/right mouse buttons and calls the appropriate On[type]ButtonUp method of the widget
	*/
	virtual void mouseReleaseEvent( QMouseEvent * mev);


	/*! 
		Dispatches the OnMouseMove event to all event listeners that are registered for the OnMouseMove event type.
		The GUI specific flags are mapped to HOOPS/MVO abstracted flags
	*/
	virtual void mouseMoveEvent( QMouseEvent * mev);

	/*! Adjusts the relative size of the Axis window so that it always has the same size if the outer window is resized */
	void AdjustAxisWindow();

	/*!
		Used to stop events from getting passed to the object
	*/
	bool eventFilter(QObject *obj, QEvent *ev);

	/*! Pointer to the HBaseView object associated with this widget */
	HBaseView * m_pHView;

	/*! Pointer to the HBaseModel object associated with this widget */
	HBaseModel * m_pHBaseModel;

	bool ignore_update;

private:

	bool initDone;

	void setup_window(bool use_gl);

	unsigned long MapFlags(unsigned long state);

	void * my_colormap;	
	void * my_windowid;
	void * my_clip_override;
	bool my_have_gl_visual;
	int  my_aa;
};


#endif 


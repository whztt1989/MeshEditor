#ifndef SHEETINTDIAGRAMVIEWER_H
#define SHEETINTDIAGRAMVIEWER_H

#include <QtGui/QGraphicsView>
#include <QtGui/QPainter>
#include <QtGui/QAction>
#include <QtGui/QGraphicsSceneMouseEvent>
#include <QtGui/QMessageBox>
#include <QtGui/QRubberBand>
#include <QtGui/QGraphicsPolygonItem>
#include <QtGui/QInputDialog>
#include <QtGui/QLineEdit>
#include <QtGui/QGraphicsItem>
#include <QtGui/QGraphicsLineItem>
#include <QtGui/QGraphicsEllipseItem>
#include <QtGui/QStyleOptionGraphicsItem>
#include <QtGui/QWheelEvent>

#include "MeshDefs.h"
#include "SheetIntDiagram.h"

class GlobalVars
{
public:
	static QColor vertex_color, vertex_bound_color, vertex_inner_color;
	static QColor edge_color;
	static QColor text_color;
	static QColor face_color;
	static double vertex_radius;
	static double edge_width;
};

class SidEdge;
class SidVertex : public QObject, public QGraphicsEllipseItem
{
	Q_OBJECT
public:
	bool boundary;
	SidVeH vh;
	QList<SidEdge*> edges;
	SheetIntDiagram *sheet_int_diagram;
	QColor vertex_color;
signals:
	void vertex_moved (SidVertex *vertex);
	void vertex_clicked (SidVeH h);
public:
	SidVertex (float x, float y, SidVeH v, SheetIntDiagram *sid, QGraphicsItem *parent = NULL)
		: QGraphicsEllipseItem (parent), vh(v), sheet_int_diagram (sid)
	{
		//setFlags (QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
		setPos (x, y);
		setToolTip (tr("Î´¶¨Òå"));
		vertex_color = GlobalVars::vertex_color;
	}
	~SidVertex(){}
public:
	QRectF boundingRect() const
	{
		double extra = (pen().width() + 20) / 2.0;

		return QRectF(- GlobalVars::vertex_radius * 2, - GlobalVars::vertex_radius * 2 , GlobalVars::vertex_radius * 4, GlobalVars::vertex_radius * 4)
			.normalized()
			.adjusted(-extra, -extra, extra, extra);
	}
	void paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */)
	{
		QStyleOptionGraphicsItem op(*option);
		op.state = QStyle::State_None;

		int penWidth = 1;
		setPen (QPen(Qt::red));
		setRect (- GlobalVars::vertex_radius - penWidth, - GlobalVars::vertex_radius - penWidth,
			GlobalVars::vertex_radius * 2 + 2 * penWidth, GlobalVars::vertex_radius * 2 + 2 * penWidth);
		setBrush (QBrush(vertex_color));

		QGraphicsEllipseItem::paint (painter, &op, widget);
	}

	void mouseMoveEvent (QGraphicsSceneMouseEvent *event)
	{
		emit vertex_moved (this);
		QGraphicsEllipseItem::mouseMoveEvent (event);
	}
	void mousePressEvent (QGraphicsSceneMouseEvent *event)
	{
		emit vertex_clicked (vh);
		QGraphicsEllipseItem::mousePressEvent (event);
	}
	void set_tooltips (QString tips)
	{
		setToolTip (tips);
	}
};

class SidEdge : public QObject, public QGraphicsLineItem
{
	Q_OBJECT
public:
	SidEgH eh;
	SheetIntDiagram *sheet_int_diagram;
	QColor edge_color;
	double edge_width;
	Qt::PenStyle pen_style;
signals:
	void edge_clicked (SidEgH h);
protected:
	void mousePressEvent (QGraphicsSceneMouseEvent *event){
		emit edge_clicked (eh);
	}
public:
	SidEdge (SidEgH e, SheetIntDiagram *sid, QGraphicsItem *parent = NULL)
		:QGraphicsLineItem (parent), eh (e), sheet_int_diagram (sid)
	{
		setFlags (QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);
		setPen (QPen (QBrush(GlobalVars::edge_color), GlobalVars::edge_width));
		setBoundingRegionGranularity (1);
		edge_color = GlobalVars::edge_color;
		edge_width = GlobalVars::edge_width;
		pen_style = Qt::SolidLine;
	}
	~SidEdge(){}
public:
	void paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */)
	{
		QStyleOptionGraphicsItem op(*option);
		op.state = QStyle::State_None;

		SidVeH vh1, vh2;
		SidHaEgH heh = sheet_int_diagram->halfedge_handle (eh, 0);
		vh1 = sheet_int_diagram->from_vertex_handle (heh);
		vh2 = sheet_int_diagram->to_vertex_handle (heh);
		SheetIntDiagram::Point pt1 = sheet_int_diagram->point (vh1),
			pt2 = sheet_int_diagram->point (vh2);
		setLine (QLineF(QPointF (pt1[0], pt1[1]), QPointF (pt2[0], pt2[1])));

		setPen (QPen (QBrush(edge_color), GlobalVars::edge_width, pen_style));
		QGraphicsLineItem::paint (painter, &op, widget);
	}

	QRectF boundingRect() const
	{
		double extra = (pen().width() + 20) / 2.0;

		return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),
			line().p2().y() - line().p1().y()))
			.normalized()
			.adjusted(-extra, -extra, extra, extra);
	}
	
	void set_tooltips (QString tips)
	{
		setToolTip (tips);
	}
};

class SidFace : public QGraphicsPolygonItem
{
public:
	SidFaH fh;
	SheetIntDiagram *sheet_int_diagram;
public:
	SidFace (SidFaH f, SheetIntDiagram *sid, QGraphicsItem *parent = NULL) 
		: QGraphicsPolygonItem (parent), fh (f), sheet_int_diagram (sid)
	{
		setBrush (QBrush (GlobalVars::face_color));
		setPen (QPen (QColor(0, 0, 0, 0)));
	}

	void paint (QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */)
	{
		QPolygonF polygon;
		for (auto fv_iter = sheet_int_diagram->fv_iter (fh); fv_iter; ++fv_iter)
		{
			QuadMesh::Point pt = sheet_int_diagram->point (fv_iter.handle ());
			QPointF ptf = QPointF (pt[0], pt[1]);
			polygon<<ptf;
		}

		setPolygon (polygon);
		QGraphicsPolygonItem::paint (painter, option, widget);
	}
};

class SheetIntDiagramViewer : public QGraphicsView
{
	Q_OBJECT
public:
	SheetIntDiagramViewer (QWidget *parent = NULL);
	~SheetIntDiagramViewer();
public:
	void set_sheet_int_diagram (SheetIntDiagram *sid);

	SidVertex * add_new_vertex (SidVeH vh);
	SidEdge *add_new_edge (SidEgH eh);
	SidFace *add_new_face (SidFaH fh);
	SidVertex *get_vertex_ptr (SidVeH vh);
	SidEdge *get_edge_ptr (SidEgH eh);
protected:
	void wheelEvent (QWheelEvent *event);
private slots:
	void on_move_vertex (SidVertex *vertex);
signals:
	void edge_clicked (SidEgH eh);
	void vertex_clicked (SidVeH vh);
private:
	SheetIntDiagram *sheet_int_diagram;
	OpenMesh::VPropHandleT<SidVertex*> V_VERTEX;
	OpenMesh::EPropHandleT<SidEdge*> E_EDGE;
	OpenMesh::FPropHandleT<SidFace*> F_FACE;
};

#endif // SHEETINTDIAGRAMVIEWER_H

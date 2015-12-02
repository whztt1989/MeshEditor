#include "StdAfx.h"
#include "sheetintdiagramviewer.h"

QColor GlobalVars::vertex_bound_color = QColor (255, 0, 0);
QColor GlobalVars::vertex_inner_color = QColor (0, 255, 0);
QColor GlobalVars::vertex_color = QColor (64, 64, 64);
QColor GlobalVars::edge_color = QColor (0, 0, 0);
QColor GlobalVars::text_color = QColor (128, 128, 128);
QColor GlobalVars::face_color = QColor (200, 200, 200);

double GlobalVars::vertex_radius = 2.0f;
double GlobalVars::edge_width = 2.0f;

SheetIntDiagramViewer::SheetIntDiagramViewer (QWidget *parent /* = NULL */)
	: QGraphicsView (parent)
{
	QGraphicsScene *scene = new QGraphicsScene (this);
	scene->setSceneRect(QRectF(this->rect ()));
	scene->setItemIndexMethod (QGraphicsScene::NoIndex);
	setRenderHint (QPainter::Antialiasing, true);
	setScene (scene);

	setDragMode (QGraphicsView::ScrollHandDrag);
}

SheetIntDiagramViewer::~SheetIntDiagramViewer ()
{}

SidVertex * SheetIntDiagramViewer::add_new_vertex (SidVeH vh)
{
	SheetIntDiagram::Point pt = sheet_int_diagram->point (vh);
	SidVertex *vertex = new SidVertex (pt[0], pt[1], vh, sheet_int_diagram);
	scene ()->addItem (vertex);
	//vertex->setZValue (3);
	sheet_int_diagram->property (V_VERTEX, vh) = vertex;
	vertex->setFlag (QGraphicsItem::ItemIsMovable);
	connect (vertex, SIGNAL (vertex_moved (SidVertex*)), SLOT (on_move_vertex (SidVertex *)));
	return vertex;
}

SidEdge * SheetIntDiagramViewer::add_new_edge (SidEgH eh)
{
	SidEdge *edge = new SidEdge (eh, sheet_int_diagram);
	scene ()->addItem (edge);
	edge->setZValue (-1);
	sheet_int_diagram->property (E_EDGE, eh) = edge;
	return edge;
}

SidFace * SheetIntDiagramViewer::add_new_face (SidFaH fh)
{
	SidFace *f = new SidFace (fh, sheet_int_diagram);
	scene ()->addItem (f);
	f->setZValue (-2);
	sheet_int_diagram->property (F_FACE, fh) = f;
	return f;
}

void SheetIntDiagramViewer::set_sheet_int_diagram (SheetIntDiagram *sid)
{
	sheet_int_diagram = sid;
	sheet_int_diagram->add_property (V_VERTEX, "vertexptr");
	sheet_int_diagram->add_property (E_EDGE, "edgeptr");
	sheet_int_diagram->add_property (F_FACE, "faceptr");

	OpenMesh::VPropHandleT<OvmCeH> OM_V_CH;
	OpenMesh::VPropHandleT<OvmFaH> OM_V_FH;
	sheet_int_diagram->get_property_handle (OM_V_CH, "cellhandle");
	sheet_int_diagram->get_property_handle (OM_V_FH, "facehandle");

	scene ()->clear ();
	for (auto v_it = sheet_int_diagram->vertices_begin (); v_it != sheet_int_diagram->vertices_end (); ++v_it){
		auto vt = add_new_vertex (v_it.handle ());
		if (sheet_int_diagram->is_boundary (*v_it)){

			vt->set_tooltips (QString (tr("Ãז%1").arg (sheet_int_diagram->property (OM_V_FH, *v_it))));
		}else{
			vt->set_tooltips (QString (tr("Ìו%1").arg (sheet_int_diagram->property (OM_V_CH, *v_it))));
		}
		connect (vt, SIGNAL(vertex_clicked (SidVeH)), this, SIGNAL (vertex_clicked (SidVeH)));
	}
	for (auto e_it = sheet_int_diagram->edges_begin (); e_it != sheet_int_diagram->edges_end (); ++e_it){
		auto eg = add_new_edge (e_it.handle ());
		connect (eg, SIGNAL(edge_clicked (SidEgH)), this, SIGNAL(edge_clicked (SidEgH)));
	}
	for (auto f_it = sheet_int_diagram->faces_begin (); f_it != sheet_int_diagram->faces_end (); ++f_it)
		add_new_face (f_it.handle ());
	scene ()->setSceneRect (scene ()->itemsBoundingRect ());
	scene ()->update ();
}

void SheetIntDiagramViewer::on_move_vertex (SidVertex *vertex)
{
	SidVeH vh = vertex->vh;
	QPointF new_pos = vertex->pos ();
	sheet_int_diagram->set_point (vh, SheetIntDiagram::Point(new_pos.x (), new_pos.y (), 0));
	for (auto vf_iter = sheet_int_diagram->vf_iter (vh); vf_iter; ++vf_iter)
	{
		SidFace *f = sheet_int_diagram->property (F_FACE, vf_iter.handle ());
		f->update ();
	}
	for (auto ve_iter = sheet_int_diagram->ve_iter (vh); ve_iter; ++ve_iter)
	{
		SidEdge *e = sheet_int_diagram->property (E_EDGE, ve_iter.handle ());
		e->update ();
	}
}

void SheetIntDiagramViewer::wheelEvent (QWheelEvent *event)
{
	setTransformationAnchor (QGraphicsView::AnchorUnderMouse);
	if (event->delta () > 0)
		scale (1.2, 1.2);
	else
		scale (0.8, 0.8);
}


SidVertex *SheetIntDiagramViewer::get_vertex_ptr (SidVeH vh)
{
	return sheet_int_diagram->property (V_VERTEX, vh);
}

SidEdge *SheetIntDiagramViewer::get_edge_ptr (SidEgH eh)
{
	return sheet_int_diagram->property (E_EDGE, eh);
}
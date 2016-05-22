#include <Python.h>

#include "canvas/scene.h"
#include "canvas/canvas_view.h"

#include "app/app.h"
#include "canvas/inspector/frame.h"
#include "canvas/subdatum/subdatum_frame.h"
#include "canvas/datum_port.h"

#include "undo/undo_move_node.h"
#include "undo/undo_move_datum.h"

#include "graph/datum.h"

CanvasScene::CanvasScene(Graph* g, QObject* parent)
    : QGraphicsScene(parent), g(g)
{
    // Nothing to do here
}

CanvasView* CanvasScene::getView(QWidget* parent)
{
    return new CanvasView(this, parent);
}

void CanvasScene::endDrag(QPointF delta)
{
    App::instance()->beginUndoMacro("'drag'");
    for (auto m : selectedItems())
        if (auto i = dynamic_cast<InspectorFrame*>(m))
            App::instance()->pushUndoStack(new UndoMoveNode(
                        i->getNode(), i->pos() - delta, i->pos()));
        else if (auto s = dynamic_cast<SubdatumFrame*>(m))
            App::instance()->pushUndoStack(new UndoMoveDatum(
                        s->getDatum(), s->pos() - delta, s->pos()));
    App::instance()->endUndoMacro();
}

////////////////////////////////////////////////////////////////////////////////

InputPort* CanvasScene::inputPortNear(QPointF pos, Datum* source) const
{
    float distance = INFINITY;
    InputPort* port = NULL;

    for (auto i : items())
    {
        InputPort* p = dynamic_cast<InputPort*>(i);
        if (p && p->isVisible() && (source == NULL ||
                                    p->getDatum()->acceptsLink(source)))
        {
            QPointF delta = p->mapToScene(p->boundingRect().center()) - pos;
            float d = QPointF::dotProduct(delta, delta);
            if (d < distance)
            {
                distance = d;
                port = p;
            }
        }
    }

    return port;

}

template <class T> T* CanvasScene::itemAt(QPointF pos) const
{
    for (auto i : items(pos))
        if (auto t = dynamic_cast<T*>(i))
            return t;
    return NULL;
}

InputPort* CanvasScene::inputPortAt(QPointF pos) const
{
    return itemAt<InputPort>(pos);
}


#include <Python.h>

#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>

#include "canvas/connection/dummy.h"
#include "canvas/datum_port.h"
#include "canvas/scene.h"
#include "app/colors.h"

DummyConnection::DummyConnection(OutputPort* source, CanvasScene* scene)
    : BaseConnection(Colors::getColor(source->getDatum())), source(source),
      snapping(false), has_snap_pos(false)
{
    setFlags(QGraphicsItem::ItemIsFocusable);
    scene->addItem(this);
}

////////////////////////////////////////////////////////////////////////////////

QPointF DummyConnection::startPos() const
{
    return source->mapToScene(source->boundingRect().center());
}

QPointF DummyConnection::endPos() const
{
    return (snapping && has_snap_pos) ? snap_pos : drag_pos;
}

////////////////////////////////////////////////////////////////////////////////

void DummyConnection::setDragPos(QPointF p)
{
    drag_pos = p;

    if (snapping)
        updateSnap();

    prepareGeometryChange();
}

////////////////////////////////////////////////////////////////////////////////

void DummyConnection::updateSnap()
{
    if (InputPort* p = static_cast<CanvasScene*>(scene())->inputPortNear(
                drag_pos, source->getDatum()))
    {
        has_snap_pos = true;
        snap_pos = p->mapToScene(p->boundingRect().center());
    }
    else
    {
        has_snap_pos = false;
    }
}

////////////////////////////////////////////////////////////////////////////////

void DummyConnection::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    setDragPos(mapToScene(event->pos()));
}

void DummyConnection::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    Q_UNUSED(event);
    deleteLater();
}

////////////////////////////////////////////////////////////////////////////////

void DummyConnection::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Space && !snapping)
    {
        snapping = true;
        updateSnap();
        prepareGeometryChange();
    }
}

void DummyConnection::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Space && snapping)
    {
        snapping = false;
        prepareGeometryChange();
    }
}


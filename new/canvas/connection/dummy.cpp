#include <Python.h>

#include <QGraphicsSceneMouseEvent>

#include "canvas/connection/dummy.h"
#include "canvas/datum_port.h"
#include "canvas/scene.h"
#include "app/colors.h"

DummyConnection::DummyConnection(OutputPort* source, CanvasScene* scene)
    : BaseConnection(Colors::getColor(source->getDatum())), source(source)
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
    return end;
}

////////////////////////////////////////////////////////////////////////////////

void DummyConnection::setDragPos(QPointF p)
{
    end = p;
    prepareGeometryChange();
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

#include <QDebug>
void DummyConnection::keyPressEvent(QKeyEvent* event)
{
    qDebug() << "Pressed";
}

void DummyConnection::keyReleaseEvent(QKeyEvent* event)
{
    qDebug() << "Released";
}


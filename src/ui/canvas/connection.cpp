#include <Python.h>

#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QPainter>

#include "ui/canvas/connection.h"
#include "ui/canvas/scene.h"
#include "ui/canvas/port.h"
#include "ui/canvas/inspector/inspector.h"
#include "ui/util/colors.h"

#include "graph/datum/datum.h"
#include "graph/datum/link.h"

#include "graph/node/node.h"

#include "app/app.h"
#include "app/undo/undo_add_link.h"
#include "app/undo/undo_delete_link.h"

Connection::Connection(Link* link)
    : QGraphicsObject(), link(link),
      drag_state(link->hasTarget() ? CONNECTED : NONE),
      snapping(false), target(NULL), hover(false)
{
    setFlags(QGraphicsItem::ItemIsSelectable|
             QGraphicsItem::ItemIsFocusable);

    setFocus();
    setAcceptHoverEvents(true);

    setZValue(2);

    connect(link, &Link::destroyed, this, &Connection::deleteLater);
}

void Connection::makeSceneConnections()
{
    connect(startInspector(), &NodeInspector::moved,
            this, &Connection::onInspectorMoved);

    if (link->hasTarget())
    {
        connect(endInspector(), &NodeInspector::moved,
                this, &Connection::onInspectorMoved);
    }

    auto s = scene();
    Q_ASSERT(s);
    connect(this, &Connection::destroyed, [=]{s->update();});
}

void Connection::deleteLink()
{
    App::instance()->pushStack(new UndoDeleteLinkCommand(link));
}

void Connection::onInspectorMoved()
{
    if (areInspectorsValid())
        prepareGeometryChange();
}

GraphScene* Connection::gscene() const
{
    return static_cast<GraphScene*>(scene());
}

QRectF Connection::boundingRect() const
{
    return areInspectorsValid() ? path().boundingRect() : QRectF();
}

QPainterPath Connection::shape() const
{
    return QPainterPathStroker(QPen(4)).createStroke(path());
}

bool Connection::areDatumsValid() const
{
    return link && dynamic_cast<Datum*>(link->parent()) &&
            (drag_state != CONNECTED || link->target);
}

bool Connection::areNodesValid() const
{
    return areDatumsValid() &&
        dynamic_cast<Node*>(startDatum()->parent()) &&
            (drag_state != CONNECTED ||
             dynamic_cast<Node*>(endDatum()->parent()));
}

bool Connection::areInspectorsValid() const
{
    return areNodesValid() &&
        gscene()->getInspector(startNode()) &&
        (drag_state != CONNECTED ||
         gscene()->getInspector(endNode()));
}

Datum* Connection::startDatum() const
{
    Datum* d = dynamic_cast<Datum*>(link->parent());
    Q_ASSERT(d);
    return d;
}

Datum* Connection::endDatum() const
{
    Q_ASSERT(drag_state == CONNECTED);
    Q_ASSERT(link->target);
    return link->target;
}

Node* Connection::startNode() const
{
    Node* n = dynamic_cast<Node*>(startDatum()->parent());
    Q_ASSERT(n);
    return n;
}

Node* Connection::endNode() const
{
    Q_ASSERT(drag_state == CONNECTED);
    Node* n = dynamic_cast<Node*>(endDatum()->parent());
    Q_ASSERT(n);
    return n;
}

NodeInspector* Connection::startInspector() const
{
    NodeInspector* i = gscene()->getInspector(startNode());
    Q_ASSERT(i);
    return i;
}

NodeInspector* Connection::endInspector() const
{
    Q_ASSERT(drag_state == CONNECTED);
    NodeInspector* i = gscene()->getInspector(endNode());
    Q_ASSERT(i);
    return i;
}

QPointF Connection::startPos() const
{
    Q_ASSERT(startInspector());
    return startInspector()->datumOutputPosition(startDatum());
}

QPointF Connection::endPos() const
{
    if (drag_state == CONNECTED)
    {
        return endInspector()->datumInputPosition(endDatum());
    }
    else
    {
        return snapping ? snap_pos : drag_pos;
    }
}

QPainterPath Connection::path() const
{
    QPointF start = startPos();
    QPointF end = endPos();

    float length = 50;
    if (end.x() <= start.x())
    {
        length += (start.x() - end.x()) / 2;
    }

    QPainterPath p;
    p.moveTo(start);
    p.cubicTo(start + QPointF(length, 0),
              end - QPointF(length, 0), end);
   return p;
}

void Connection::paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (!areInspectorsValid())
        return;

    QColor color = Colors::getColor(startDatum());
    if (drag_state == INVALID)
        color = Colors::red;
    if (isSelected() || drag_state == VALID)
        color = Colors::highlight(color);

    painter->setPen(QPen(color, 4));
    painter->drawPath(path());
}

void Connection::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (drag_state == CONNECTED)
        return;

    drag_pos = event->pos();
    if (snapping)
        updateSnap();

    gscene()->raiseInspectorAt(drag_pos);

    checkDragTarget();
    prepareGeometryChange();
}

void Connection::checkDragTarget()
{
    target = gscene()->getInputPortAt(endPos());

    if (target && target->getDatum()->acceptsLink(link))
        drag_state = VALID;
    else if (target)
        drag_state = INVALID;
    else
        drag_state = NONE;
}

void Connection::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsObject::mouseReleaseEvent(event);
    if (drag_state == CONNECTED)
        return;

    ungrabMouse();
    clearFocus();
    setFlag(QGraphicsItem::ItemIsFocusable, false);

    InputPort* target = gscene()->getInputPortAt(endPos());
    Datum* datum = target ? target->getDatum() : NULL;
    if (target && datum->acceptsLink(link))
    {
        datum->addLink(link);
        drag_state = CONNECTED;

        connect(endInspector(), &NodeInspector::moved,
                this, &Connection::onInspectorMoved);

        App::instance()->pushStack(new UndoAddLinkCommand(link));
    }
    else
    {
        link->deleteLater();
    }

    prepareGeometryChange();
}

void Connection::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    if (!hover)
    {
        hover = true;
        update();
    }
}

void Connection::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    if (hover)
    {
        hover = false;
        update();
    }
}

void Connection::updateSnap()
{
    if (Port* p = gscene()->getInputPortNear(drag_pos, link))
        snap_pos = p->mapToScene(p->boundingRect().center());
}

void Connection::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Shift && drag_state != CONNECTED)
    {
        snapping = true;
        updateSnap();
        checkDragTarget();
        prepareGeometryChange();
    }
    else
    {
        event->ignore();
    }
}

void Connection::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Shift && drag_state != CONNECTED)
    {
        snapping = false;
        checkDragTarget();
        prepareGeometryChange();
    }
}

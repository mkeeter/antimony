#include <Python.h>

#include <QGraphicsSceneMouseEvent>

#include "ui/connection.h"
#include "ui/canvas.h"
#include "ui/colors.h"
#include "ui/port.h"
#include "ui/inspector/inspector.h"

#include "datum/datum.h"
#include "datum/link.h"

#include "node/node.h"

#include "control/control.h"

Connection::Connection(Link* link, Canvas* canvas)
    : QGraphicsObject(), link(link), canvas(canvas),
      drag_state(link->hasTarget() ? CONNECTED : NONE),
      snapping(false), raised_inspector(NULL), target(NULL),
      hover(false)
{
    setFlags(QGraphicsItem::ItemIsSelectable|
             QGraphicsItem::ItemIsFocusable);

    setFocus();
    setAcceptHoverEvents(true);
    canvas->scene->addItem(this);
    setZValue(2);
    connect(startControl(), &Control::portPositionChanged,
            this, &Connection::onPortPositionChanged);

    connect(link, SIGNAL(destroyed()), this, SLOT(deleteLater()));
    connect(this, SIGNAL(destroyed()), canvas, SLOT(update()));
}

void Connection::onPortPositionChanged()
{
    if (areControlsValid())
    {
        prepareGeometryChange();
    }
}

QRectF Connection::boundingRect() const
{
    return areControlsValid() ? path().boundingRect() : QRectF();
}

QPainterPath Connection::shape() const
{
    QPainterPathStroker stroker;
    stroker.setWidth(4);
    return stroker.createStroke(path());
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

bool Connection::areControlsValid() const
{
    return areNodesValid() &&
        canvas->getControl(startNode()) &&
        (drag_state != CONNECTED ||
         canvas->getControl(endNode()));
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

Control* Connection::startControl() const
{
    Control* c = canvas->getControl(startNode());
    Q_ASSERT(c);
    return c;
}


Control* Connection::endControl() const
{
    Q_ASSERT(drag_state == CONNECTED);
    Control* c = canvas->getControl(endNode());
    Q_ASSERT(c);
    return c;
}

QPointF Connection::startPos() const
{
    Q_ASSERT(startControl());
    return startControl()->datumOutputPosition(startDatum());
}

QPointF Connection::endPos() const
{
    if (drag_state == CONNECTED)
    {
        return endControl()->datumInputPosition(endDatum());
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
    if (!areControlsValid())
    {
        return;
    }

    Q_UNUSED(option);
    Q_UNUSED(widget);

    QColor color = Colors::getColor(startDatum());
    if (drag_state == INVALID)
    {
        color = Colors::red;
    }
    if (isSelected() || drag_state == VALID)
    {
        color = Colors::highlight(color);
    }

    bool faded = drag_state == CONNECTED &&
                 !startControl()->showConnections() &&
                 !endControl()->showConnections() &&
                 !isSelected();
    if (faded)
    {
        color = QColor(color.red(), color.green(), color.blue(),
                       hover ? 150 : 100);
    }

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

    if (raised_inspector)
        raised_inspector->setZValue(-2);
    NodeInspector* insp = canvas->getInspectorAt(drag_pos);
    if (insp)
        insp->setZValue(-1.9);
    raised_inspector = insp;

    checkDragTarget();
    prepareGeometryChange();
}

void Connection::checkDragTarget()
{
    if (target)
        target->hideToolTip();
    target = canvas->getInputPortAt(endPos());
    if (target)
        target->showToolTip();

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
    {
        return;
    }

    ungrabMouse();
    clearFocus();

    if (target)
        target->hideToolTip();
    InputPort* target = canvas->getInputPortAt(endPos());
    Datum* datum = target ? target->getDatum() : NULL;
    if (target && datum->acceptsLink(link))
    {
        datum->addLink(link);
        drag_state = CONNECTED;

        connect(endControl(), &Control::portPositionChanged,
                this, &Connection::onPortPositionChanged);
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
    if (Port* p = canvas->getInputPortNear(drag_pos, link))
    {
        snap_pos = p->mapToScene(p->boundingRect().center());
    }
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
    else if (event->key() == Qt::Key_Delete ||
             event->key() == Qt::Key_Backspace)
    {
        getLink()->deleteLater();
    }
    else
    {
        event->ignore();
    }
}

void Connection::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Shift)
    {
        snapping = false;
        prepareGeometryChange();
    }
}

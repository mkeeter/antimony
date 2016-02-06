#include <Python.h>

#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QPainter>

#include "ui/canvas/connection.h"
#include "ui/canvas/graph_scene.h"
#include "ui/canvas/port.h"
#include "ui/canvas/inspector/inspector.h"
#include "ui/util/colors.h"
#include "ui/viewport/viewport_scene.h"

#include "app/app.h"
#include "app/undo/undo_add_link.h"

Connection::Connection(OutputPort* source)
    : Connection(source, NULL)
{
    // Unfortunate coupling of connection and viewport scene, but it's the
    // easiest way to trigger re-rendering when connections change.
    auto d = source->getDatum();
    auto n = d->parentNode();
    auto vs = App::instance()->getViewScene();
    connect(this, &Connection::changed, [=](){ vs->checkRender(n, d); });

    connect(this, &QObject::destroyed, this, &Connection::changed);
}

Connection::Connection(OutputPort* source, InputPort* target)
    : source(source), target(target), color(Colors::getColor(source->getDatum())),
      drag_state(target ? CONNECTED : NONE),
      snapping(false), hover(false)
{
    setFlags(QGraphicsItem::ItemIsSelectable|
             QGraphicsItem::ItemIsFocusable);
    setZValue(1);

    setFocus();
    setAcceptHoverEvents(true);

    connect(source, &Port::moved,
            this, &Connection::onPortsMoved);
    connect(source, &Port::hiddenChanged,
            this, &Connection::onHiddenChanged);

    if (target)
        target->install(this);

    // Initialize start (and possibly end) positions
    onPortsMoved();
}

void Connection::onPortsMoved()
{
    start_pos = source->mapToScene(source->dropRect().center());
    if (drag_state == CONNECTED)
        end_pos = target->mapToScene(target->dropRect().center());
    prepareGeometryChange();
}

void Connection::onHiddenChanged()
{
    if (isHidden())
        hide();
    else
        show();
    prepareGeometryChange();
}

GraphScene* Connection::gscene() const
{
    return static_cast<GraphScene*>(scene());
}

QRectF Connection::boundingRect() const
{
    QPainterPathStroker s;
    s.setWidth(20);
    return s.createStroke(path()).boundingRect();
}

QPainterPath Connection::shape() const
{
    QPainterPathStroker s;
    s.setWidth(20);
    return s.createStroke(path(true));
}

QPointF Connection::startPos() const
{
    return start_pos;
}

QPointF Connection::endPos() const
{
    if (drag_state == CONNECTED)
        return end_pos;
    else
        return (snapping && has_snap_pos) ? snap_pos : drag_pos;
}

bool Connection::isHidden() const
{
    if (!source->isVisible())
        return true;
    if (drag_state == CONNECTED && !target->isVisible())
        return true;
    return false;
}

QPainterPath Connection::path(bool only_bezier) const
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
    if (only_bezier)
        p.moveTo(start + QPointF(15, 0));
    else
        p.lineTo(start + QPointF(15, 0));

    p.cubicTo(QPointF(start.x() + length, start.y()),
              QPointF(end.x() - length, end.y()),
              QPointF(end.x() - 15, end.y()));

    if (!only_bezier)
        p.lineTo(end);

   return p;
}

void Connection::paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (hover)
    {
        painter->setPen(QPen(QColor(255, 255, 255, Colors::base02.red()), 20));
        painter->drawPath(path(true));
    }

    QColor draw_color = color;
    if (drag_state == INVALID)
        draw_color = Colors::red;
    if (isSelected() || drag_state == VALID)
        draw_color = Colors::highlight(color);

    painter->setPen(QPen(draw_color, 4));
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

    if (target && target->getDatum()->acceptsLink(source->getDatum()))
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

    InputPort* t = gscene()->getInputPortAt(endPos());
    Datum* datum = target ? target->getDatum() : NULL;
    if (t && datum->acceptsLink(source->getDatum()))
    {
        target = t;
        t->install(this);

        datum->installLink(source->getDatum());
        drag_state = CONNECTED;
        App::instance()->pushStack(
                new UndoAddLinkCommand(source->getDatum(), datum));
        onPortsMoved();

        emit(changed());
    }
    else
    {
        deleteLater();
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
    if (Port* p = gscene()->getInputPortNear(drag_pos, source->getDatum()))
    {
        has_snap_pos = true;
        snap_pos = p->mapToScene(p->dropRect().center());
    }
    else
    {
        has_snap_pos = false;
    }
}

void Connection::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Space && drag_state != CONNECTED)
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
    if (event->key() == Qt::Key_Space && drag_state != CONNECTED)
    {
        snapping = false;
        checkDragTarget();
        prepareGeometryChange();
    }
}

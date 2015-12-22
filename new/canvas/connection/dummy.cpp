#include <Python.h>

#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>

#include "canvas/connection/dummy.h"
#include "canvas/datum_port.h"
#include "canvas/scene.h"

#include "app/app.h"
#include "app/colors.h"
#include "undo/undo_change_expr.h"

#include "graph/datum.h"

DummyConnection::DummyConnection(OutputPort* source, CanvasScene* scene)
    : BaseConnection(Colors::getColor(source->getDatum())), source(source),
      target(NULL), drag_state(NONE), snapping(false), has_snap_pos(false)
{
    setFlags(QGraphicsItem::ItemIsFocusable);
    scene->addItem(this);
}

////////////////////////////////////////////////////////////////////////////////

QPointF DummyConnection::startPos() const
{
    return source->mapToScene(source->portRect().center());
}

QPointF DummyConnection::endPos() const
{
    return (snapping && has_snap_pos) ? snap_pos : drag_pos;
}

QColor DummyConnection::color() const
{
    const auto c = BaseConnection::color();
    if (drag_state == VALID)
        return Colors::highlight(c);
    else if (drag_state == INVALID)
        return Colors::red;
    else
        return c;
}

////////////////////////////////////////////////////////////////////////////////

void DummyConnection::setDragPos(QPointF p)
{
    drag_pos = p;

    if (snapping)
        updateSnap();

    checkDragTarget();
    prepareGeometryChange();
}

////////////////////////////////////////////////////////////////////////////////

void DummyConnection::updateSnap()
{
    if (InputPort* p = static_cast<CanvasScene*>(scene())->inputPortNear(
                drag_pos, source->getDatum()))
    {
        has_snap_pos = true;
        snap_pos = p->mapToScene(p->portRect().center());
    }
    else
    {
        has_snap_pos = false;
    }
}

////////////////////////////////////////////////////////////////////////////////

void DummyConnection::checkDragTarget()
{
    target = static_cast<CanvasScene*>(scene())->inputPortAt(endPos());

    if (target && target->getDatum()->acceptsLink(source->getDatum()))
        drag_state = VALID;
    else if (target)
        drag_state = INVALID;
    else
        drag_state = NONE;
}

////////////////////////////////////////////////////////////////////////////////

void DummyConnection::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    setDragPos(mapToScene(event->pos()));
}

void DummyConnection::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    Q_UNUSED(event);

    if (target && target->getDatum()->acceptsLink(source->getDatum()))
    {
        auto d = target->getDatum();

        QString before = QString::fromStdString(d->getText());
        d->installLink(source->getDatum());
        QString after = QString::fromStdString(d->getText());

        auto cmd = new UndoChangeExpr(d, before, after);
        cmd->setText("'create link'");
        App::instance()->pushUndoStack(cmd);
    }
    deleteLater();
}

////////////////////////////////////////////////////////////////////////////////

void DummyConnection::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Space && !snapping)
    {
        snapping = true;
        updateSnap();
        checkDragTarget();
        prepareGeometryChange();
    }
}

void DummyConnection::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Space && snapping)
    {
        snapping = false;
        checkDragTarget();
        prepareGeometryChange();
    }
}


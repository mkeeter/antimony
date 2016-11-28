#include <Python.h>

#include <QMatrix4x4>
#include <QGraphicsSceneMouseEvent>
#include <QKeyEvent>
#include <QMenu>

#include "viewport/control/control_instance.h"
#include "viewport/control/control.h"
#include "viewport/view.h"

ControlInstance::ControlInstance(Control* c, ViewportView* v)
    : control(c), view(v)
{
    // To enable dragging, the item needs to be selectable
    setFlags(QGraphicsItem::ItemIsSelectable);
    setAcceptHoverEvents(true);

    connect(v, &ViewportView::changed, this, &ControlInstance::onViewChanged);
    v->scene()->addItem(this);

    if (v->isUIhidden())
    {
        hide();
    }
}

QMatrix4x4 ControlInstance::getMatrix() const
{
    return view->getMatrix();
}

QRectF ControlInstance::boundingRect() const
{
    return control ? control->bounds(getMatrix())
                   : QRectF();
}

QPainterPath ControlInstance::shape() const
{
    return control ? control->shape(getMatrix())
                   : QPainterPath();
}

QString ControlInstance::getName() const
{
    return control ? control->getName() : "";
}

const Node* ControlInstance::getNode() const
{
    return control ? control->getNode() : nullptr;
}

Control* ControlInstance::getControl() const
{
    return control;
}

void ControlInstance::paint(QPainter* painter,
                         const QStyleOptionGraphicsItem* option,
                         QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (control)
    {
        control->paint(getMatrix(), hover && control->hasDragFunc(), painter);
    }
}

////////////////////////////////////////////////////////////////////////////////

void ControlInstance::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    click_pos = event->pos();
    control->beginDrag();
    QGraphicsObject::mousePressEvent(event);
}

void ControlInstance::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    control->endDrag();
    QGraphicsObject::mouseReleaseEvent(event);
}

void ControlInstance::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsObject::mouseMoveEvent(event);

    QMatrix4x4 mi = getMatrix().inverted();

    QVector3D p0 = mi * QVector3D(click_pos);
    QVector3D p1 = mi * QVector3D(event->pos());

    QVector3D eye = -view->getMatrix(ViewportView::ROT).inverted()
        .column(2).toVector3D().normalized();

    control->drag(p1 + eye * QVector3D::dotProduct(eye, control->pos() - p1),
                  p1 - p0);

    click_pos = event->pos();
}

void ControlInstance::openContextMenu()
{
    QString desc = control->getName();

    QScopedPointer<QMenu> menu(new QMenu());
    auto jump_to = new QAction("Zoom to " + desc, menu.data());

    menu->addAction(jump_to);
    connect(jump_to, &QAction::triggered, this, &ControlInstance::onZoomTo);

    auto delete_node = new QAction("Delete node " + desc, menu.data());
    menu->addAction(delete_node);
    connect(delete_node, &QAction::triggered,
            this, &ControlInstance::onDeleteNode);

    menu->exec(QCursor::pos());
}

void ControlInstance::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsObject::hoverEnterEvent(event);
    if (!hover)
    {
        hover = true;
        update();
    }
    emit(onFocus(true));
}

void ControlInstance::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsObject::hoverLeaveEvent(event);
    if (hover)
    {
        hover = false;
        update();
    }
    emit(onFocus(false));
}

void ControlInstance::onDeleteNode()
{
    control->deleteNode();
}

void ControlInstance::onViewChanged(QMatrix4x4 M)
{
    Q_UNUSED(M);
    redraw();
}

void ControlInstance::redraw()
{
    prepareGeometryChange();
}

#include <Python.h>

#include <algorithm>

#include <QGraphicsSceneMouseEvent>

#include "control/control.h"
#include "node/node.h"

#include "ui/canvas.h"
#include "ui/colors.h"
#include "ui/inspector.h"

#include "datum/datum.h"
#include "datum/float_datum.h"

Control::Control(Canvas* canvas, Node* node, QGraphicsItem* parent)
    : QGraphicsObject(parent), canvas(canvas), node(node), viewer(NULL),
      _hover(false), _dragged(false)
{
    setFlags(QGraphicsItem::ItemIsSelectable |
             QGraphicsItem::ItemIgnoresTransformations);
    setAcceptHoverEvents(true);

    if (parent == NULL)
    {
        canvas->scene->addItem(this);
    }
    setZValue(1);
}

QRectF Control::boundingBox(QVector<QVector3D> points, int padding) const
{
    float xmin =  INFINITY;
    float xmax = -INFINITY;
    float ymin =  INFINITY;
    float ymax = -INFINITY;

    for (auto p : points)
    {
        QPointF o = canvas->worldToScene(p);
        if (o.x() < xmin)   xmin = o.x();
        if (o.x() > xmax)   xmax = o.x();
        if (o.y() < ymin)   ymin = o.y();
        if (o.y() > ymax)   ymax = o.y();
    }

    return QRectF(xmin - padding, ymin - padding,
                  xmax - xmin + 2*padding,
                  ymax - ymin + 2*padding);
}

void Control::watchDatums(QVector<QString> datums)
{
    for (auto n : datums)
    {
        Datum* d = node->getDatum(n);
        Q_ASSERT(d);
        connect(d, SIGNAL(changed()), this, SLOT(redraw()));
    }
}

void Control::redraw()
{
    prepareGeometryChange();
    emit(inspectorPositionChanged());
}

void Control::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    if (!_hover)
    {
        _hover = true;
        update();
    }
}

void Control::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event);
    if (_hover)
    {
        _hover = false;
        update();
    }
}

void Control::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (parentObject())
    {
        dynamic_cast<Control*>(parentObject())->mouseDoubleClickEvent(event);
    }
    else if (viewer.isNull())
    {
        viewer = new NodeInspector(this);
    }
    else
    {
        viewer->animateClose();
    }
}

void Control::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    _dragged = false;
    _click_pos = event->pos();
}

void Control::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (!_dragged && event->button() == Qt::LeftButton)
    {
        setSelected(true);
    }
    ungrabMouse();
}

void Control::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QVector3D p0 = canvas->sceneToWorld(_click_pos);
    QVector3D p1 = canvas->sceneToWorld(event->pos());

    drag(p1, p1 - p0);
    canvas->update();
    _click_pos = event->pos();
    _dragged = true;
}

double Control::getValue(QString name) const
{
    Datum* d = node->getDatum(name);
    Q_ASSERT(d);

    double v = PyFloat_AsDouble(d->getValue());
    Q_ASSERT(!PyErr_Occurred());

    return v;
}

void Control::dragValue(QString name, double delta)
{
    Datum* d = node->getDatum(name);
    Q_ASSERT(d);

    FloatDatum* f = dynamic_cast<FloatDatum*>(d);
    Q_ASSERT(f);

    bool ok = false;
    double v = f->getExpr().toFloat(&ok);
    if (ok)
    {
        f->setExpr(QString::number(v + delta));
    }
}

void Control::setDefaultPen(QPainter *painter) const
{
    if (isSelected() or _hover)
    {
        painter->setPen(QPen(Colors::base3, 2));
    }
    else
    {
        painter->setPen(QPen(Colors::dim(Colors::base3), 2));
    }
}

void Control::setDefaultBrush(QPainter *painter) const
{
    if (isSelected() or _hover)
    {
        painter->setBrush(QBrush(Colors::highlight(Colors::base1)));
    }
    else
    {
        painter->setBrush(QBrush(Colors::base1));
    }
}

////////////////////////////////////////////////////////////////////////////////

DummyControl::DummyControl(Canvas *canvas, Node *node, QGraphicsItem *parent)
    : Control(canvas, node, parent)
{
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setAcceptHoverEvents(false);
}

void DummyControl::drag(QVector3D center, QVector3D delta)
{
    Q_UNUSED(center);
    Q_UNUSED(delta);

    // A DummyControl should never be dragged.
    Q_ASSERT(false);
}

QPainterPath DummyControl::shape() const
{
    return QPainterPath();
}

void DummyControl::paint(QPainter *painter,
                         const QStyleOptionGraphicsItem *option,
                         QWidget *widget)
{
    Q_UNUSED(painter);
    Q_UNUSED(option);
    Q_UNUSED(widget);
}

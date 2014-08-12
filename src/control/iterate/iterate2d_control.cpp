#include <Python.h>

#include <QGraphicsSceneMouseEvent>

#include "control/iterate/iterate2d_control.h"
#include "ui/canvas.h"

#include "node/node.h"
#include "datum/int_datum.h"

Iterate2DButton::Iterate2DButton(Canvas* canvas, Node* node,
                                 bool axis, bool sign, QGraphicsItem* parent)
    : WireframeControl(canvas, node, parent),
      axis(axis), sign(sign)
{
    watchDatums({"_x", "_y"});
}

void Iterate2DButton::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() != Qt::LeftButton)
    {
        return;
    }

    Datum* d = node->getDatum(axis ? "i" : "j");
    Q_ASSERT(d);

    IntDatum* i = dynamic_cast<IntDatum*>(d);
    Q_ASSERT(i);

    bool ok = false;
    int v = i->getExpr().toInt(&ok);
    if (ok && !(v == 1 && sign == false))
    {
        i->setExpr(QString::number(sign ? v + 1 : v - 1));
    }
}

void Iterate2DButton::paintControl(QPainter* painter)
{
    WireframeControl::paintControl(painter);
    setDefaultPen(painter);
    QPointF p = canvas->worldToScene(position());
    painter->drawLine(p.x() - 3, p.y(), p.x() + 3, p.y());
    if (sign)
    {
        painter->drawLine(p.x(), p.y() - 3, p.x(), p.y() + 3);
    }
}

QVector<QPair<QVector3D, float>> Iterate2DButton::points() const
{
    return {{position(), 8}};
}

QVector3D Iterate2DButton::position() const
{
    QVector3D p = QVector3D(getValue("_x"), getValue("_y"), 0);
    QVector3D d = axis ? QVector3D(0.2, 0, 0) : QVector3D(0, 0.2, 0);
    return sign ? p + d : p - d;
}

////////////////////////////////////////////////////////////////////////////////

Iterate2DHandle::Iterate2DHandle(Canvas* canvas, Node* node, bool dir,
                                 QGraphicsItem* parent)
    : WireframeControl(canvas, node, parent), dir(dir)
{
    if (dir)
    {
        watchDatums({"_x", "_y", "dx"});
    }
    else
    {
        watchDatums({"_x", "_y", "dy"});
    }
}

void Iterate2DHandle::drag(QVector3D c, QVector3D d)
{
    Q_UNUSED(c);
    if (dir)
    {
        dragValue("dx", d.x());
    }
    else
    {
        dragValue("dy", d.y());
    }
}

QVector<QVector<QVector3D>> Iterate2DHandle::lines() const
{
    if (dir)
    {
        QVector3D p(getValue("_x") + getValue("dx"), getValue("_y"), 0);
        float scale = getValue("dx")* 0.05;
        return {{p - QVector3D(0, scale, 0),
                 p + QVector3D(0, scale, 0)}};
    }
    else
    {
        QVector3D p(getValue("_x"), getValue("_y") + getValue("dy"), 0);
        float scale = getValue("dy")* 0.05;
        return {{p - QVector3D(scale, 0, 0),
                 p + QVector3D(scale, 0, 0)}};
    }
}

////////////////////////////////////////////////////////////////////////////////

Iterate2DControl::Iterate2DControl(Canvas* canvas, Node* node)
    : WireframeControl(canvas, node),
      handle_x(new Iterate2DHandle(canvas, node, true, this)),
      handle_y(new Iterate2DHandle(canvas, node, false, this))
{
    watchDatums({"_x", "_y", "dx", "dy"});
    new Iterate2DButton(canvas, node, false, false, this);
    new Iterate2DButton(canvas, node, false, true, this);
    new Iterate2DButton(canvas, node, true, false, this);
    new Iterate2DButton(canvas, node, true, true, this);
}

void Iterate2DControl::drag(QVector3D center, QVector3D delta)
{
    Q_UNUSED(center);
    dragValue("_x", delta.x());
    dragValue("_y", delta.y());
}

QPointF Iterate2DControl::inspectorPosition() const
{
    return canvas->worldToScene(position());
}

QVector<QVector<QVector3D>> Iterate2DControl::lines() const
{
    QVector3D p = position();
    return {{p + QVector3D(getValue("dx"), 0, 0),
             p,
             p + QVector3D(0, getValue("dy"), 0)}};
}

QVector<QPair<QVector3D, float>> Iterate2DControl::points() const
{
    return {{position(), 5}};
}

QVector3D Iterate2DControl::position() const
{
    return QVector3D(getValue("_x"), getValue("_y"), 0);
}

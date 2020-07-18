#include <Python.h>

#include <QMatrix4x4>
#include <QPainter>
#include <QPainterPath>

#include "viewport/control/point.h"

#include "app/colors.h"

PointControl::PointControl(NodeProxy* node)
    : Control(node), x(0), y(0), z(0), r(5), color(Colors::blue)
{
    // Nothing to do here
}

void PointControl::update(float x_, float y_, float z_, float r_,
                          QColor color_, bool relative_, PyObject* drag_func)
{
    bool changed = (x != x_) || (y != y_) || (z != z_) || (r != r_) ||
                   (color != color_) || (relative != relative_);

    x = x_;
    y = y_;
    z = z_;
    r = r_;
    color = color_;
    relative = relative_;

    if (changed)
    {
        emit(redraw());
    }

    setDragFunc(drag_func);
}

QPainterPath PointControl::shape(QMatrix4x4 m) const
{
    QPainterPath path;
    QPointF pt = (m * QVector3D(x, y, z)).toPointF();
    path.addEllipse(QRectF(pt.x() - r, pt.y() - r, 2*r, 2*r));
    return path;
}

void PointControl::paint(QMatrix4x4 m, bool highlight, QPainter* painter)
{
    QColor edge = Colors::dim(color);

    auto s = shape(m);
    if (has_focus)
    {
        painter->setPen(QPen(QColor(255, 255, 255, 128), 10));
        painter->drawPath(s);
    }

    painter->setPen(QPen(highlight ? Colors::highlight(edge) : edge, 2));
    painter->setBrush(QBrush(highlight ? Colors::highlight(color) : color));
    painter->drawPath(s);
}


#include "control/point.h"
#include "ui/util/colors.h"

ControlPoint::ControlPoint(Node* node, PyObject* drag_func)
    : Control(node, drag_func), x(0), y(0), z(0), r(5), color(Colors::blue)
{
    // Nothing to do here
}

void ControlPoint::update(float x_, float y_, float z_, float r_,
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
        emit(redraw());

    setDragFunc(drag_func);
}

QPainterPath ControlPoint::shape(QMatrix4x4 m) const
{
    QPainterPath path;
    QPointF pt = (m * QVector3D(x, y, z)).toPointF();
    path.addEllipse(QRectF(pt.x() - r, pt.y() - r, 2*r, 2*r));
    return path;
}

void ControlPoint::paint(QMatrix4x4 m, bool highlight, QPainter* painter)
{
    QColor edge = Colors::dim(color);
    painter->setPen(QPen((highlight || glow) ? Colors::highlight(edge)
                                             : edge, 2));
    painter->setBrush(QBrush((highlight || glow) ? Colors::highlight(color)
                                                 : color));
    painter->drawPath(shape(m));
}

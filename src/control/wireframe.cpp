#include "control/wireframe.h"
#include "ui/util/colors.h"

ControlWireframe::ControlWireframe(Node* node)
    : Control(node), t(3), color(Colors::blue), close(false)
{
    // Nothing to do here
}

void ControlWireframe::update(QVector<QVector3D> pts_, float t_,
                              QColor color_, bool close_,
                              bool relative_, PyObject* drag_func_)
{
    bool changed = (pts != pts_) || (t != t_) ||
                   (color != color_) || (close != close_) ||
                   (relative != relative_);

    pts = pts_;
    t = t_;
    color = color_;
    close = close_;
    relative = relative_;

    if (changed)
        emit(redraw());

    setDragFunc(drag_func_);
}

QPainterPath ControlWireframe::shape(QMatrix4x4 m) const
{
    QPainterPathStroker s;
    s.setWidth(20);
    return s.createStroke(path(m));
}

QPainterPath ControlWireframe::path(QMatrix4x4 m) const
{
    QPainterPath out;
    if (!pts.isEmpty())
        out.moveTo((m * pts[0]).toPointF());
    for (auto p : pts)
        out.lineTo((m * p).toPointF());
    if (close && !pts.isEmpty())
        out.lineTo((m* pts.front()).toPointF());
    return out;
}

void ControlWireframe::paint(QMatrix4x4 m, bool highlight, QPainter* painter)
{
    painter->setPen(QPen((highlight || glow) ? Colors::highlight(color)
                                             : color, t));
    painter->drawPath(path(m));
}

QVector3D ControlWireframe::pos() const
{
    QVector3D center;
    int count = 0;

    for (auto p : pts)
    {
        center += p;
        count++;
    }

    if (close && !pts.isEmpty())
    {
        center += pts.front();
        count++;
    }

    return center / count;
}

#include "control/dummy.h"

DummyControl::DummyControl(Node* node, QObject* parent)
    : Control(node, parent)
{
    // Nothing to do here
}

void DummyControl::drag(QVector3D center, QVector3D delta)
{
    Q_UNUSED(center);
    Q_UNUSED(delta);

    // A DummyControl should never be dragged.
    Q_ASSERT(false);
}

QRectF DummyControl::bounds(QMatrix4x4 m, QMatrix4x4 t) const
{
    auto r = QRectF();
    for (auto c : findChildren<Control*>())
        r = r.united(c->bounds(m, t));
    return r;
}

QPainterPath DummyControl::shape(QMatrix4x4 m, QMatrix4x4 t) const
{
    Q_UNUSED(m);
    Q_UNUSED(t);
    return QPainterPath();
}

void DummyControl::paint(QMatrix4x4 m, QMatrix4x4 t,
                         bool highlight, QPainter* painter)
{
    Q_UNUSED(m);
    Q_UNUSED(t);
    Q_UNUSED(highlight);
    Q_UNUSED(painter);
}

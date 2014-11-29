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

QPainterPath DummyControl::shape(QMatrix4x4 m) const
{
    Q_UNUSED(m);
    return QPainterPath();
}

void DummyControl::paint(QMatrix4x4 m, bool highlight, QPainter* painter)
{
    Q_UNUSED(m);
    Q_UNUSED(highlight);
    Q_UNUSED(painter);
}

#include "node/2d/point2d_node.h"
#include "node/manager.h"

#include "datum/float_datum.h"

Point2D::Point2D(QObject* parent)
    : Node(parent)
{
    // Nothing to do here
}

Point2D::Point2D(float x, float y, float z, float scale,
                 QObject* parent)
    : Node(NodeManager::manager()->getName("p"), parent)
{
    new FloatDatum("x", QString::number(x), this);
    new FloatDatum("y", QString::number(y), this);
    Q_UNUSED(z);
    Q_UNUSED(scale);
}

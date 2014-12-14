#include <Python.h>

#include "graph/node/node.h"
#include "graph/node/manager.h"

#include "graph/node/nodes/2d.h"

#include "graph/datum/datums/name_datum.h"
#include "graph/datum/datums/float_datum.h"

Node* SliderNode(float x, float y, float z, float scale, QObject* parent)
{
    Q_UNUSED(x);
    Q_UNUSED(y);
    Q_UNUSED(z);
    Q_UNUSED(scale);

    Node* n = new Node(
            NodeType::SLIDER, NodeManager::manager()->getName("s"), parent);
    new FloatDatum("min", "0", n);
    new FloatDatum("max", "1", n);
    new FloatDatum("value", "0.5", n);

    return n;
}

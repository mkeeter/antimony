#include <Python.h>

#include "graph/node/node.h"
#include "graph/node/manager.h"

#include "graph/node/nodes/2d.h"

#include "graph/datum/datums/name_datum.h"
#include "graph/datum/datums/float_datum.h"

Node* SliderNode(float x, float y, float z, float scale, QObject* parent)
{
    Q_UNUSED(scale);

    Node* n = new Node(NodeType::SLIDER, parent);
    new NameDatum("name", NodeManager::manager()->getName("s"), n);
    new FloatDatum("_x", QString::number(x), n);
    new FloatDatum("_y", QString::number(y), n);
    new FloatDatum("_z", QString::number(z), n);
    new FloatDatum("min", "0", n);
    new FloatDatum("max", "1", n);
    new FloatDatum("value", "0.5", n);

    return n;
}

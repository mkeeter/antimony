#include <Python.h>

#include "graph/node/node.h"
#include "graph/node/manager.h"

#include "graph/node/nodes/iterate.h"

#include "graph/datum/datums/name_datum.h"
#include "graph/datum/datums/float_datum.h"
#include "graph/datum/datums/int_datum.h"
#include "graph/datum/datums/shape_input_datum.h"
#include "graph/datum/datums/shape_function_datum.h"

////////////////////////////////////////////////////////////////////////////////

Node* Iterate2DNode(float x, float y, float z, float scale, QObject* parent)
{
    Q_UNUSED(z);

    Node* n = new Node(NodeType::ITERATE2D, parent);
    new NameDatum("_name", NodeManager::manager()->getName("i"), n);
    new FloatDatum("_x", QString::number(x), n);
    new FloatDatum("_y", QString::number(y), n);
    new IntDatum("i", "2", n);
    new IntDatum("j", "2", n);
    new FloatDatum("dx", QString::number(scale), n);
    new FloatDatum("dy", QString::number(scale), n);
    new ShapeInputDatum("input", n);
    new ShapeFunctionDatum("shape", n, "iterate2d",
            {"input", "i", "j", "dx", "dy"});

    return n;
}

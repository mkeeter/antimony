#include <Python.h>

#include "node/iterate.h"
#include "node/node.h"
#include "node/manager.h"

#include "datum/name_datum.h"
#include "datum/float_datum.h"
#include "datum/int_datum.h"
#include "datum/shape_datum.h"
#include "datum/function_datum.h"

////////////////////////////////////////////////////////////////////////////////

Node* Iterate2DNode(float x, float y, float z, float scale, QObject* parent)
{
    Q_UNUSED(z);

    Node* n = new Node(NodeType::ITERATE2D, parent);
    new NameDatum("name", NodeManager::manager()->getName("i"), n);
    new FloatDatum("_x", QString::number(x), n);
    new FloatDatum("_y", QString::number(y), n);
    new IntDatum("i", "2", n);
    new IntDatum("j", "2", n);
    new FloatDatum("dx", QString::number(scale), n);
    new FloatDatum("dy", QString::number(scale), n);
    new ShapeDatum("input", n);
    new ShapeFunctionDatum("shape", n, "iterate2d",
            {"input", "i", "j", "dx", "dy"});

    return n;
}

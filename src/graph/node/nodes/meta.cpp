#include <Python.h>

#include "graph/node/node.h"
#include "graph/node/manager.h"

#include "graph/node/nodes/meta.h"

#include "graph/datum/datums/name_datum.h"
#include "graph/datum/datums/float_datum.h"
#include "graph/datum/types/shape_datum.h"
#include "graph/datum/types/function_datum.h"
#include "graph/datum/datums/string_datum.h"
#include "graph/datum/datums/script_datum.h"

////////////////////////////////////////////////////////////////////////////////

Node* ScriptNode(QString name, QString x, QString y, QString z,
                 QString script, QObject* parent)
{
    Node* n = new Node(NodeType::SCRIPT, parent);
    new NameDatum("name", name, n);
    new FloatDatum("_x", x, n);
    new FloatDatum("_y", y, n);
    new FloatDatum("_z", z, n);
    new ScriptDatum("script", script, n);
    return n;
}

Node* ScriptNode(float x, float y, float z, float scale,
                 QObject* parent)
{
    Q_UNUSED(scale);

    Node* n = new Node(NodeType::SCRIPT, parent);
    new NameDatum("name", NodeManager::manager()->getName("a"), n);
    new FloatDatum("_x", QString::number(x), n);
    new FloatDatum("_y", QString::number(y), n);
    new FloatDatum("_z", QString::number(z), n);
    new ScriptDatum("script",
             "from fab import shapes\n\n"
             "output('c', shapes.circle(0, 0, 1))", n);
    return n;
}

Node* EquationNode(float x, float y, float z, float scale,
                       QObject* parent)
{
    Q_UNUSED(scale);

    Node* n = new Node(NodeType::EQUATION, parent);
    new NameDatum("name", NodeManager::manager()->getName("a"), n);
    new FloatDatum("_x", QString::number(x), n);
    new FloatDatum("_y", QString::number(y), n);
    new FloatDatum("_z", QString::number(z), n);
    new ShapeDatum("a", n);
    new ShapeFunctionDatum("shape", n, "buffer", {"a"});

    return n;
}

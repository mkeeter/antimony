#include <Python.h>

#include "node/meta/script_node.h"
#include "node/manager.h"

#include "datum/float_datum.h"
#include "datum/script_datum.h"

ScriptNode::ScriptNode(QString name, QString x, QString y, QString z,
                       QString script, QObject* parent)
    : _Node(name, parent)
{
    new FloatDatum("_x", x, this);
    new FloatDatum("_y", y, this);
    new FloatDatum("_z", z, this);
    new ScriptDatum("script", script, this);
}

ScriptNode::ScriptNode(float x, float y, float z, float scale,
                       QObject* parent)
    : ScriptNode(NodeManager::manager()->getName("s"),
                 QString::number(x),
                 QString::number(y),
                 QString::number(z),
                 "from fab import shapes\n\n"
                 "output('c', shapes.circle(0, 0, 1))",
                 parent)
{
    Q_UNUSED(scale);
}

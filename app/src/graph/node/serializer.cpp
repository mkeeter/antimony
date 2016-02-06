#include <Python.h>

#include <QStringList>

#include "graph/node/serializer.h"

#include "graph/node.h"
#include "graph/script_node.h"
#include "graph/graph.h"
#include "graph/datum.h"

// Protocol version change-log:
// 2 -> 3:
//   Change hard-coded nodes into matching scripts
// 3 -> 4:
//   Remove ShapeInputDatum (replace with ShapeDatum)
//   _name -> __name; _script -> __script
// 4 -> 5:
//   Switch to plain-text.
// 5 -> 6: (refactored graph engine)
//   Store scripts and names at node level
//   Remove explicit connections array
int SceneSerializer::PROTOCOL_VERSION = 6;

QJsonObject SceneSerializer::run(Graph* root, QMap<Node*, QPointF> inspectors)
{
    QJsonObject out;
    out["type"] = "sb";
    out["protocol"] = PROTOCOL_VERSION;
    out["nodes"] = serializeNodes(root, inspectors);

    return out;
}

QJsonArray SceneSerializer::serializeNodes(Graph* r, QMap<Node*, QPointF> inspectors)
{
    QJsonArray out;
    for (auto node : r->childNodes())
        out.append(serializeNode(node, inspectors));
    return out;
}

QJsonObject SceneSerializer::serializeNode(Node* node, QMap<Node*, QPointF> inspectors)
{
    QJsonObject out;

    out["inspector"] = QJsonArray({
            inspectors[node].x(),
            inspectors[node].y()});
    out["name"] = QString::fromStdString(node->getName());
    out["uid"] = int(node->getUID());

    if (auto script_node = dynamic_cast<ScriptNode*>(node))
    {
        auto expr = QString::fromStdString(script_node->getScript());
        auto a = QJsonArray();
        for (auto line : expr.split("\n"))
            a.append(line);
        out["script"] = a;
    }

    QJsonArray datum_array;
    for (auto d : node->childDatums())
        datum_array.append(serializeDatum(d));
    out["datums"] = datum_array;

    return out;
}

QJsonObject SceneSerializer::serializeDatum(Datum* datum)
{
    QJsonObject out;

    out["name"] = QString::fromStdString(datum->getName());
    out["uid"] = int(datum->getUID());
    out["expr"] = QString::fromStdString(datum->getText());

    auto t = PyObject_GetAttrString((PyObject*)datum->getType(), "__name__");
    auto m = PyObject_GetAttrString((PyObject*)datum->getType(), "__module__");
    auto type = QString::fromUtf8(PyUnicode_AsUTF8(t));
    auto module = QString::fromUtf8(PyUnicode_AsUTF8(m));
    Py_DECREF(t);
    Py_DECREF(m);

    out["type"] = (module == "builtins") ? type : (module + "." + type);

    return out;
}

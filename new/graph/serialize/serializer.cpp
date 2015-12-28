#include <Python.h>

#include <QStringList>

#include "graph/serialize/serializer.h"

#include "graph/node.h"
#include "graph/graph.h"
#include "graph/datum.h"
#include "graph/script_node.h"
#include "graph/graph_node.h"

#include "canvas/info.h"

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

QJsonObject SceneSerializer::run(Graph* root, CanvasInfo* info)
{
    QJsonObject out;
    out["type"] = "sb";
    out["protocol"] = PROTOCOL_VERSION;
    out["nodes"] = serializeGraph(root, info);

    return out;
}

QJsonArray SceneSerializer::serializeGraph(Graph* g, CanvasInfo* info)
{
    QJsonArray out;
    for (auto node : g->childNodes())
        out.append(serializeNode(node, info));
    return out;
}

QJsonObject SceneSerializer::serializeNode(Node* node, CanvasInfo* info)
{
    QJsonObject out;

    if (info && info->inspector.contains(node))
        out["inspector"] = QJsonArray({
                info->inspector[node].x(),
                info->inspector[node].y()});

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
    else if (auto graph_node = dynamic_cast<GraphNode*>(node))
    {
        out["subgraph"] = serializeGraph(graph_node->getGraph(), info);
    }

    QJsonArray datum_array;
    for (auto d : node->childDatums())
        datum_array.append(serializeDatum(d, info));
    out["datums"] = datum_array;

    return out;
}

QJsonObject SceneSerializer::serializeDatum(Datum* datum, CanvasInfo* info)
{
    QJsonObject out;

    if (info && info->subdatum.contains(datum))
        out["subdatum"] = QJsonArray({
                info->subdatum[datum].x(),
                info->subdatum[datum].y()});

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

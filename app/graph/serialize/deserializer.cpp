#include <Python.h>

#include <QDataStream>
#include <QJsonDocument>
#include <QBuffer>
#include <QFile>
#include <QTextStream>

#include "app/app.h"

#include "graph/serialize/deserializer.h"
#include "graph/node.h"
#include "graph/script_node.h"
#include "graph/graph_node.h"
#include "graph/graph.h"

////////////////////////////////////////////////////////////////////////////////

static bool checkType(const QJsonObject& in, SceneDeserializer::Info* info)
{
    // Check that the "type" field is "sb"
    if (in.find("type") == in.end() || in["type"].toString() != "sb")
    {
        if (info)
            info->error_message = "Could not recognize file.<br><br>"
                                  "If this file was saved in Antimony 0.7.7 or earlier, "
                                  "open it in Antimony 0.7.8 and re-save to upgrade to "
                                  "the current file format.";
        return false;
    }
    return true;
}

static bool checkProtocol(const QJsonObject& in, SceneDeserializer::Info* info)
{
    if (in.find("protocol") == in.end())
    {
        if (info)
            info->error_message = "Could not detect protocol";
        return false;
    }
    else
    {
        auto protocol_version = in["protocol"].toDouble();
        if (protocol_version < 6)
        {
            if (info)
                info->error_message = "File was saved with a older protocol "
                                      "and can no longer be read.";
            return false;
        }
        else if (protocol_version > 6)
        {
            if (info)
                info->error_message = "File was saved with a newer protocol "
                                      "and cannot yet be read.";
            return false;
        }
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////

bool SceneDeserializer::run(QJsonObject in, Graph* graph, Info* info)
{
    if (!checkType(in, info))
        return false;

    if (!checkProtocol(in, info))
        return false;

    // Make sure there's a "nodes" array
    if (in.find("nodes") == in.end() || !in["nodes"].isArray())
    {
        if (info)
            info->error_message = "File does not contain any nodes.";
        return false;
    }

    deserializeGraph(in["nodes"].toArray(), graph, info);
    return true;
}

////////////////////////////////////////////////////////////////////////////////

void SceneDeserializer::deserializeGraph(QJsonArray in, Graph* g, Info* info)
{
    for (auto n : in)
        deserializeNode(n.toObject(), g, info);
}

void SceneDeserializer::deserializeNode(QJsonObject in, Graph* p, Info* info)
{
    Q_ASSERT(in.contains("script") ^ in.contains("subgraph"));

    Node* node = NULL;
    if (in.contains("script"))
        node = new ScriptNode(in["name"].toString().toStdString(),
                              in["uid"].toDouble(), p);
    else if (in.contains("subgraph"))
        node = new GraphNode(in["name"].toString().toStdString(),
                             in["uid"].toDouble(), p);

    // Deserialize inspector position
    if (info && in.contains("inspector"))
    {
        auto i = in["inspector"].toArray();
        info->frames.inspector[node] = QPointF(i[0].toDouble(), i[1].toDouble());
    }

    for (auto d : in["datums"].toArray())
        deserializeDatum(d.toObject(), node, info);

    if (auto script_node = dynamic_cast<ScriptNode*>(node))
    {
        QStringList s;
        for (auto line : in["script"].toArray())
            s.append(line.toString());
        script_node->setScript(s.join("\n").toStdString());
    }
    else if (auto graph_node = dynamic_cast<GraphNode*>(node))
    {
        deserializeGraph(in["subgraph"].toArray(), graph_node->getGraph(), info);
        graph_node->triggerWatchers();
        graph_node->getGraph()->triggerWatchers();
    }
}

void SceneDeserializer::deserializeDatum(QJsonObject in, Node* node, Info* info)
{
    // Lazy initialization of globals dictionary
    static PyObject* globals = NULL;
    if (!globals)
    {
        globals = Py_BuildValue("{sO}", "__builtins__", PyEval_GetBuiltins());
        PyDict_SetItemString(globals, "_fabtypes",
                             PyImport_ImportModule("_fabtypes"));
    }

    // Evaluate the type string in the Python interpreter,
    // getting a type object out.
    auto t = PyRun_String(
                  in["type"].toString().toStdString().c_str(),
                  Py_eval_input, globals, globals);
    Q_ASSERT(t);
    Q_ASSERT(PyType_Check(t));

    auto datum = new Datum(in["name"].toString().toStdString(),
                           in["uid"].toDouble(),
                           in["expr"].toString().toStdString(),
                           (PyTypeObject*)t, node);

    if (info && in.contains("subdatum"))
    {
        auto i = in["subdatum"].toArray();
        info->frames.subdatum[datum] = QPointF(i[0].toDouble(), i[1].toDouble());
    }
}

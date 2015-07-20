#include <Python.h>

#include <QDataStream>
#include <QBuffer>
#include <QFile>
#include <QTextStream>

#include "app/app.h"

#include "graph/node/deserializer.h"
#include "graph/node.h"
#include "graph/graph.h"

SceneDeserializer::SceneDeserializer(Graph* graph)
    : QObject(), failed(false), graph(graph), globals(NULL)
{
    // Nothing to do here
}

bool SceneDeserializer::run(QJsonObject in)
{
    // Check that the "type" field is "sb"
    if (in.find("type") == in.end() || in["type"].toString() != "sb")
    {
        failed = true;
        error_message = "Could not recognize file.<br><br>"
                        "If this file was saved in Antimony 0.7.7 or earlier, "
                        "open it in Antimony 0.7.8 and re-save to upgrade to the current file format.";
        return failed;
    }

    // Check file saving protocol
    if (in.find("protocol") == in.end())
    {
        failed = true;
        error_message = "Could not detect protocol";
        return failed;
    }
    else
    {
        protocol_version = in["protocol"].toDouble();
        if (protocol_version < 6)
        {
            failed = true;
            error_message = "File was saved with a older protocol and cannot yet be read.";
            return failed;
        }
        else if (protocol_version > 6)
        {
            failed = true;
            error_message = "File was saved with a newer protocol and cannot yet be read.";
            return failed;
        }
    }

    // Make sure there's a "nodes" array
    if (in.find("nodes") == in.end() || !in["nodes"].isArray())
    {
        failed = true;
        error_message = "File does not contain any nodes.";
        return failed;
    }

    deserializeNodes(in["nodes"].toArray(), graph);

    return false;
}

void SceneDeserializer::deserializeNodes(QJsonArray in, Graph* p)
{
    for (auto n : in)
        deserializeNode(n.toObject(), p);
}

void SceneDeserializer::deserializeNode(QJsonObject in, Graph* p)
{
    Node* node = new Node(in["name"].toString().toStdString(),
                          in["uid"].toDouble(), p);

    // Deserialize inspector position
    auto a = in["inspector"].toArray();
    inspectors[node] = QPointF(a[0].toDouble(), a[1].toDouble());

    for (auto d : in["datums"].toArray())
        deserializeDatum(d.toObject(), node);

    QStringList s;
    for (auto line : in["script"].toArray())
        s.append(line.toString());
    node->setScript(s.join("\n").toStdString());
}

void SceneDeserializer::deserializeDatum(QJsonObject in, Node* node)
{
    if (!globals)
        globals = Py_BuildValue("{sO}", "__builtins__", PyEval_GetBuiltins());

    new Datum(in["name"].toString().toStdString(),
              in["uid"].toDouble(),
              in["expr"].toString().toStdString(),
              (PyTypeObject*)PyRun_String(
                  in["type"].toString().toStdString().c_str(),
                  Py_eval_input, globals, globals),
              node);
}


#include <Python.h>

#include <QDataStream>
#include <QBuffer>
#include <QFile>
#include <QTextStream>

#include "app/app.h"

#include "graph/node/deserializer.h"
#include "graph/node/node.h"
#include "graph/node/root.h"

#include "graph/datum/types/eval_datum.h"

SceneDeserializer::SceneDeserializer(NodeRoot* node_root)
    : QObject(), failed(false), node_root(node_root)
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
        if (protocol_version < 5)
        {
            failed = true;
            error_message = "File was saved with a older protocol and cannot yet be read.";
            return failed;
        }
        else if (protocol_version > 5)
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

    deserializeNodes(in["nodes"].toArray(), node_root);
    if (in.find("connections") != in.end())
        deserializeConnections(in["connections"].toArray());

    return false;
}

void SceneDeserializer::deserializeNodes(QJsonArray in, NodeRoot* p)
{
    for (auto n : in)
        deserializeNode(n.toObject(), p);
}

void SceneDeserializer::deserializeNode(QJsonObject in, NodeRoot* p)
{
    Node* node = new Node(p);

    // Deserialize inspector position
    auto a = in["inspector"].toArray();
    inspectors[node] = QPointF(a[0].toDouble(), a[1].toDouble());

    for (auto d : in["datums"].toArray())
        deserializeDatum(d.toObject(), node);

    nodes << node;
}

void SceneDeserializer::deserializeDatum(QJsonObject in, Node* node)
{
    QString type = in["type"].toString();
    QString name = in["name"].toString();

    Datum* datum = Datum::fromTypeString(type, name, node);

    if (auto e = dynamic_cast<EvalDatum*>(datum))
    {
        if (in["expr"].isArray())
        {
            QStringList a;
            for (auto line : in["expr"].toArray())
                a.append(line.toString());
            e->setExpr(a.join("\n"));
        }
        else
        {
            e->setExpr(in["expr"].toString());
        }
    }
}

void SceneDeserializer::deserializeConnections(QJsonArray in)
{
    for (auto c_ : in)
    {
        auto c = c_.toArray();
        auto start = c[0].toArray();
        auto end = c[1].toArray();

        auto start_datum = nodes[start[0].toDouble()]->findChild<Datum*>(
                start[1].toString(), Qt::FindDirectChildrenOnly);
        auto end_datum = nodes[end[0].toDouble()]->findChild<Datum*>(
                end[1].toString(), Qt::FindDirectChildrenOnly);

        end_datum->addLink(start_datum->linkFrom());
    }
}

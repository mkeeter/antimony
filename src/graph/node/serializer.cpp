#include <Python.h>

#include <QStringList>

#include "graph/node/serializer.h"
#include "graph/node/node.h"
#include "graph/node/root.h"

#include "graph/datum/datum.h"
#include "graph/datum/types/eval_datum.h"
#include "graph/datum/datums/script_datum.h"

// Protocol version change-log:
// 2 -> 3:
//   Change hard-coded nodes into matching scripts
// 3 -> 4:
//   Remove ShapeInputDatum (replace with ShapeDatum)
//   _name -> __name; _script -> __script
// 4 -> 5:
//   Switch to plain-text.
int SceneSerializer::PROTOCOL_VERSION = 5;

SceneSerializer::SceneSerializer(NodeRoot* node_root,
                                 QMap<Node*, QPointF> inspectors)
    : QObject(), node_root(node_root), inspectors(inspectors)
{
    // Nothing to do here.
}

QJsonObject SceneSerializer::run(SerializerMode mode)
{
    QJsonObject out;
    out["type"] = "sb";
    out["protocol"] = PROTOCOL_VERSION;

    out["nodes"] = serializeNodes(node_root);

    if (mode == SERIALIZE_ALL)
        out["connections"] = serializeConnections();
    return out;
}

QJsonArray SceneSerializer::serializeNodes(NodeRoot* r)
{
    QJsonArray out;
    nodes = r->findChildren<Node*>(
            QString(), Qt::FindDirectChildrenOnly);

    for (auto node : nodes)
        out.append(serializeNode(node));

    return out;
}

QJsonObject SceneSerializer::serializeNode(Node* node)
{
    QJsonObject out;

    out["inspector"] = QJsonArray({
            inspectors[node].x(),
            inspectors[node].y()});

    QJsonArray datum_array;

    Datum* deferred = NULL;
    auto datums = node->findChildren<Datum*>(
            QString(), Qt::FindDirectChildrenOnly);

    for (auto d : datums)
        if (dynamic_cast<ScriptDatum*>(d))
            deferred = d;
        else
            datum_array.append(serializeDatum(d));

    if (deferred)
        datum_array.append(serializeDatum(deferred));

    out["datums"] = datum_array;

    return out;
}

QJsonObject SceneSerializer::serializeDatum(Datum* datum)
{
    QJsonObject out;
    out["type"] = datum->getDatumTypeString();
    out["name"] = datum->objectName();

    if (auto e = dynamic_cast<EvalDatum*>(datum))
    {
        auto expr = e->getExpr();
        if (expr.contains("\n"))
        {
            auto a = QJsonArray();
            for (auto line : expr.split("\n"))
                a.append(line);
            out["expr"] = a;
        }
        else
        {
            out["expr"] = expr;
        }
    }

    // Save datum and any connections for later
    // (as connections are serialized separately,
    // once all of the datums have been written).
    for (auto d : datum->getInputDatums())
        connections << QPair<Datum*, Datum*>(d, datum);

    return out;
}

QJsonArray SceneSerializer::serializeConnections()
{
    QJsonArray out;
    for (auto p : connections)
    {
        QJsonArray start;
        QJsonArray end;

        // Only serialize connections for which we have serialized
        // both datums (prevents edge cases in copy-paste)
        auto start_node = static_cast<Node*>(p.first->parent());
        auto end_node = static_cast<Node*>(p.second->parent());

        if (nodes.contains(start_node) && nodes.contains(end_node))
        {
            start.append(nodes.indexOf(start_node));
            start.append(p.first->objectName());

            end.append(nodes.indexOf(end_node));
            end.append(p.second->objectName());

            out.append(QJsonArray({start, end}));
        }
    }

    return out;
}

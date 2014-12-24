#include <Python.h>

#include <QDataStream>
#include <QBuffer>

#include "graph/node/serializer.h"
#include "graph/node/node.h"

#include "graph/datum/datum.h"
#include "graph/datum/types/eval_datum.h"
#include "graph/datum/datums/shape_function_datum.h"
#include "graph/datum/datums/script_datum.h"

SceneSerializer::SceneSerializer(QObject* node_root,
                                 QMap<Node*, QPointF> inspectors)
    : QObject(), node_root(node_root), inspectors(inspectors)
{
    // Nothing to do here.
}

QByteArray SceneSerializer::run()
{
    QBuffer buffer;
    buffer.open(QBuffer::WriteOnly);

    QDataStream stream(&buffer);
    run(&stream);
    buffer.seek(0);

    return buffer.data();
}

void SceneSerializer::run(QDataStream* out)
{
    *out << QString("sb") << quint32(2);
    serializeNodes(out, node_root);
    serializeConnections(out);
}

void SceneSerializer::serializeNodes(QDataStream* out, QObject* p)
{
    auto nodes = p->findChildren<Node*>(
            QString(), Qt::FindDirectChildrenOnly);
    *out << quint32(nodes.length());

    for (auto node : nodes)
        serializeNode(out, node);
}

void SceneSerializer::serializeNode(QDataStream* out, Node* node)
{
    *out << quint32(node->getNodeType());
    *out << node->objectName();

    // Serialize position (or default QPointF if not provided)
    *out << (inspectors.contains(node) ? inspectors[node] : QPointF());

    Datum* deferred = NULL;
    auto datums = node->findChildren<Datum*>(QString(),
                                             Qt::FindDirectChildrenOnly);
    *out << quint32(datums.length());
    for (auto d : datums)
    {
        if (dynamic_cast<ScriptDatum*>(d))
        {
            Q_ASSERT(deferred == NULL);
            Q_ASSERT(node->getNodeType() == NodeType::SCRIPT);;
            deferred = d;
        }
        else
        {
            serializeDatum(out, d);
        }
    }

    if (deferred)
        serializeDatum(out, deferred);
}

void SceneSerializer::serializeDatum(QDataStream* out, Datum* datum)
{
    *out << quint32(datum->getDatumType());
    *out << datum->objectName();

    if (auto e = dynamic_cast<EvalDatum*>(datum))
    {
        *out << e->getExpr();
    }
    else if (auto f = dynamic_cast<FunctionDatum*>(datum))
    {
        *out << f->getFunctionName();
        *out << f->getArguments();
    }

    // Save datum and any connections for later
    // (as connections are serialized separately,
    // once all of the datums have been written).
    datums << datum;
    for (auto d : datum->getInputDatums())
        connections << QPair<Datum*, Datum*>(d, datum);
}

void SceneSerializer::serializeConnections(QDataStream* out)
{
    *out << quint32(connections.length());
    for (auto p : connections)
        *out << quint32(datums.indexOf(p.first))
             << quint32(datums.indexOf(p.second));
}

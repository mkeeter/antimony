#include <Python.h>

#include <QDataStream>
#include <QBuffer>

#include "graph/node/serializer.h"
#include "graph/node/node.h"

#include "graph/datum/datum.h"
#include "graph/datum/types/eval_datum.h"
#include "graph/datum/datums/shape_function_datum.h"
#include "graph/datum/datums/script_datum.h"

int SceneSerializer::PROTOCOL_VERSION = 3;

SceneSerializer::SceneSerializer(QObject* node_root,
                                 QMap<Node*, QPointF> inspectors)
    : QObject(), node_root(node_root), inspectors(inspectors)
{
    // Nothing to do here.
}

QByteArray SceneSerializer::run(SerializerMode mode)
{
    QBuffer buffer;
    buffer.open(QBuffer::WriteOnly);

    QDataStream stream(&buffer);
    run(&stream, mode);
    buffer.seek(0);

    return buffer.data();
}

void SceneSerializer::run(QDataStream* out, SerializerMode mode)
{
    *out << QString("sb") << quint32(PROTOCOL_VERSION);
    serializeNodes(out, node_root);

    if (mode == SERIALIZE_NODES)
        connections.clear();

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
    *out << quint32(0); // Dummy node type, since it doesn't exist anymore.
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
    // Only serialize connections for which we have serialized both datums
    // (prevents edge cases in copy-paste)
    QList<QPair<Datum*, Datum*>> valid;
    for (auto p : connections)
        if (datums.contains(p.first) && datums.contains(p.second))
            valid << p;

    *out << quint32(valid.length());
    for (auto p : valid)
        *out << quint32(datums.indexOf(p.first))
             << quint32(datums.indexOf(p.second));
}

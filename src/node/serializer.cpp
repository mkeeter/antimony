#include <Python.h>
#include <QDataStream>

#include "node/serializer.h"
#include "node/manager.h"
#include "node/node.h"

#include "datum/datum.h"
#include "datum/eval_datum.h"
#include "datum/function_datum.h"
#include "datum/script_datum.h"

SceneSerializer::SceneSerializer(QObject* parent)
    : QObject(parent)
{
    // Nothing to do here.
}

void SceneSerializer::run(QDataStream* out)
{
    *out << QString("sb") << quint32(1) << quint32(0);
    serializeNodes(out, NodeManager::manager());
    serializeConnections(out);
}

void SceneSerializer::serializeNodes(QDataStream* out, QObject* p)
{
    auto nodes = p->findChildren<Node*>(QString(),
                                        Qt::FindDirectChildrenOnly);
    *out << quint32(nodes.length());

    for (auto node : nodes)
    {
        serializeNode(out, node);
    }
}

void SceneSerializer::serializeNode(QDataStream* out, Node* node)
{
    *out << quint32(node->getNodeType());
    *out << node->objectName();

    // Serialize child nodes first.
    serializeNodes(out, node);

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
    {
        serializeDatum(out, deferred);
    }
}

void SceneSerializer::serializeDatum(QDataStream* out, Datum* datum)
{
    *out << quint32(datum->getDatumType());
    *out << datum->objectName();

    EvalDatum* e = dynamic_cast<EvalDatum*>(datum);
    FunctionDatum* f = dynamic_cast<FunctionDatum*>(datum);
    if (e)
    {
        *out << e->getExpr();
    }
    else if (f)
    {
        *out << f->getFunctionName();
        *out << f->getArguments();
    }

    // Save datum and any connections for later
    // (as connections are serialized separately,
    // once all of the datums have been written).
    datums << datum;
    for (auto d : datum->getInputDatums())
    {
        connections << QPair<Datum*, Datum*>(d, datum);
    }
}

void SceneSerializer::serializeConnections(QDataStream* out)
{
    *out << quint32(connections.length());
    for (auto p : connections)
    {
        *out << quint32(datums.indexOf(p.first))
             << quint32(datums.indexOf(p.second));
    }
}

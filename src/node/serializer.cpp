#include <Python.h>
#include <QDataStream>

#include "node/serializer.h"
#include "node/manager.h"
#include "node/node.h"

#include "datum/datum.h"

SceneSerializer::SceneSerializer(QObject* parent)
    : QObject(parent)
{
    // Nothing to do here.
}

void SceneSerializer::run(QDataStream* out)
{
    *out << QString("sb") << quint32(1) << quint32(0);
    serializeNodes(out, NodeManager::manager());
}

void SceneSerializer::serializeNodes(QDataStream* out, QObject* p)
{
    *out << quint32(p->findChildren<Node*>().length());
    for (auto node : p->findChildren<Node*>())
    {
        serializeNode(out, node);
    }
}

void SceneSerializer::serializeNode(QDataStream* out, Node* node)
{
    *out << quint32(node->getNodeType());
    serializeNodes(out, node);
    for (auto d : node->findChildren<Datum*>())
    {
        serializeDatum(out, d);
    }
}

void SceneSerializer::serializeDatum(QDataStream* out, Datum* datum)
{
    *out << quint32(datum->getDatumType());
}


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
    serializeNodes(out);
}

void SceneSerializer::serializeNodes(QDataStream* out)
{
    NodeManager* manager = NodeManager::manager();
    *out << quint32(manager->findChildren<Node*>().length());
    for (auto node : manager->findChildren<Node*>())
    {
        serializeNode(node, out);
    }
}

void SceneSerializer::serializeNode(Node* node, QDataStream* out)
{
}

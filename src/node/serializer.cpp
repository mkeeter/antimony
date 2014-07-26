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


#include "node/2d/circle_node.h"
#include "node/3d/cube_node.h"
#include "node/3d/point3d_node.h"
#include "node/meta/script_node.h"

void SceneSerializer::serializeNode(Node* node, QDataStream* out)
{
}

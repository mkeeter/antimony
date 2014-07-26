#include <Python.h>

#include <QDataStream>

#include "node/deserializer.h"
#include "node/node.h"
#include "node/manager.h"

#include "node/2d/circle_node.h"
#include "node/3d/cube_node.h"
#include "node/3d/point3d_node.h"
#include "node/meta/script_node.h"

SceneDeserializer::SceneDeserializer(QObject* parent)
    : QObject(parent)
{
    // Nothing to do here
}

void SceneDeserializer::run(QDataStream* in)
{
    QString sb;
    quint32 version_major;
    quint32 version_minor;
    *in >> sb >> version_major >> version_minor;

    deserializeNodes(in, NodeManager::manager());
}

void SceneDeserializer::deserializeNodes(QDataStream* in, QObject* p)
{
    quint32 count;
    *in >> count;
    for (unsigned i=0; i < count; ++i)
    {
        deserializeNode(in, p);
    }
}

void SceneDeserializer::deserializeNode(QDataStream* in, QObject* p)
{
    quint32 t;
    *in >> t;

    Node* node;
    NodeType::NodeType node_type = static_cast<NodeType::NodeType>(t);

    switch (node_type)
    {
        case NodeType::CIRCLE:
            node = new CircleNode(p); break;
        case NodeType::CUBE:
            node = new CubeNode(p); break;
        case NodeType::POINT3D:
            node = new Point3D(p); break;
        case NodeType::SCRIPT:
            node = new ScriptNode(p); break;
    }
}

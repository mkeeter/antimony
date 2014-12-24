#include <Python.h>

#include <QtTest/QtTest>
#include <QSignalSpy>

#include "test_serialize.h"
#include "graph/node/nodes/2d.h"
#include "graph/node/nodes/3d.h"
#include "graph/node/manager.h"

#include "graph/node/serializer.h"
#include "graph/node/deserializer.h"

void TestSerialize::SerializeSimpleScene()
{
    QByteArray empty = SceneSerializer(NodeManager::manager()).run();

    Node* c = CircleNode("c", "0.0", "0.0", "1.0");
    QByteArray out = SceneSerializer(NodeManager::manager()).run();

    NodeManager::manager()->clear();
    c = NULL;

    QCOMPARE(empty, SceneSerializer(NodeManager::manager()).run());

    SceneDeserializer(NodeManager::manager()).run(out);
    QCOMPARE(out,  SceneSerializer(NodeManager::manager()).run());

    NodeManager::manager()->clear();
}

void TestSerialize::SerializeNestedDatum()
{
    Node* c = CubeNode(0, 0, 0, 1);
    QByteArray out = SceneSerializer(NodeManager::manager()).run();

    NodeManager::manager()->clear();
    c = NULL;

    SceneDeserializer(NodeManager::manager()).run(out);
    QCOMPARE(out,  SceneSerializer(NodeManager::manager()).run());

    NodeManager::manager()->clear();
}

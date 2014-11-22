#include <Python.h>

#include <QtTest/QtTest>
#include <QSignalSpy>

#include "test_serialize.h"
#include "graph/node/nodes/2d.h"
#include "graph/node/nodes/3d.h"
#include "graph/node/manager.h"

void TestSerialize::SerializeSimpleScene()
{
    QByteArray empty = NodeManager::manager()->getSerializedScene();

    Node* c = CircleNode("c", "0.0", "0.0", "1.0");
    QByteArray out = NodeManager::manager()->getSerializedScene();

    NodeManager::manager()->clear();
    c = NULL;

    QCOMPARE(empty, NodeManager::manager()->getSerializedScene());

    NodeManager::manager()->deserializeScene(out);
    QCOMPARE(out,  NodeManager::manager()->getSerializedScene());

    NodeManager::manager()->clear();
}

void TestSerialize::SerializeNestedDatum()
{
    Node* c = CubeNode(0, 0, 0, 1);
    QByteArray out = NodeManager::manager()->getSerializedScene();

    NodeManager::manager()->clear();
    c = NULL;

    NodeManager::manager()->deserializeScene(out);
    QCOMPARE(out,  NodeManager::manager()->getSerializedScene());

    NodeManager::manager()->clear();
}

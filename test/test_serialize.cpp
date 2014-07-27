#include <Python.h>

#include <QtTest/QtTest>
#include <QSignalSpy>

#include "test_serialize.h"
#include "node/2d/circle_node.h"
#include "node/manager.h"

void TestSerialize::SerializeSimpleScene()
{
    QByteArray empty = NodeManager::manager()->getSerializedScene();

    Node* c = new CircleNode("c", "0.0", "0.0", "1.0");
    QByteArray out = NodeManager::manager()->getSerializedScene();

    NodeManager::manager()->clear();
    c = NULL;

    QCOMPARE(empty, NodeManager::manager()->getSerializedScene());

    NodeManager::manager()->deserializeScene(out);
    QCOMPARE(out,  NodeManager::manager()->getSerializedScene());

    NodeManager::manager()->clear();
}

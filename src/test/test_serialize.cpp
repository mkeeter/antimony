#include <Python.h>

#include <QtTest/QtTest>
#include <QSignalSpy>

#include "test_serialize.h"

#include "graph/node/root.h"
#include "graph/node/nodes/2d.h"
#include "graph/node/nodes/3d.h"

#include "graph/node/serializer.h"
#include "graph/node/deserializer.h"

void TestSerialize::SerializeSimpleScene()
{
    auto r = new NodeRoot;
    QByteArray empty = SceneSerializer(r).run();

    Node* c = CircleNode("c", "0.0", "0.0", "1.0", r);
    QByteArray out = SceneSerializer(r).run();

    delete r;
    r = new NodeRoot;
    c = NULL;

    QCOMPARE(empty, SceneSerializer(r).run());

    SceneDeserializer(r).run(out);
    QCOMPARE(out,  SceneSerializer(r).run());

    delete r;
}

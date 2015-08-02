#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <QObject>
#include <QList>
#include <QPair>
#include <QMap>
#include <QPointF>
#include <QJsonObject>
#include <QJsonArray>

class Datum;
class Node;
class Graph;

namespace SceneSerializer
{
    QJsonObject run(
            Graph* root,
            QMap<Node*, QPointF> inspectors=QMap<Node*, QPointF>());

    QJsonArray serializeNodes(Graph* r, QMap<Node*, QPointF> inspectors);
    QJsonObject serializeNode(Node* node, QMap<Node*, QPointF> inspectors);
    QJsonObject serializeDatum(Datum* datum);

    extern int PROTOCOL_VERSION;
};

#endif // SERIALIZER_H

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

class SceneSerializer : public QObject
{
    Q_OBJECT
public:
    explicit SceneSerializer(
            Graph* root,
            QMap<Node*, QPointF> inspectors=QMap<Node*, QPointF>());

    QJsonObject run();

protected:
    QJsonArray serializeNodes(Graph* r);
    QJsonObject serializeNode(Node* node);
    QJsonObject serializeDatum(Datum* datum);

    Graph* graph;
    QMap<Node*, QPointF> inspectors;

    QList<Node*> nodes;
    QList<QPair<Datum*, Datum*>> connections;

    static int PROTOCOL_VERSION;
};

#endif // SERIALIZER_H

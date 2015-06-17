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
class NodeRoot;

class SceneSerializer : public QObject
{
    Q_OBJECT
public:
    explicit SceneSerializer(
            NodeRoot* node_root,
            QMap<Node*, QPointF> inspectors=QMap<Node*, QPointF>());

    enum SerializerMode { SERIALIZE_ALL, SERIALIZE_NODES };

    QJsonObject run(SerializerMode mode=SERIALIZE_ALL);

protected:
    QJsonArray serializeNodes(NodeRoot* r);
    QJsonObject serializeNode(Node* node);
    QJsonObject serializeDatum(Datum* datum);
    QJsonArray serializeConnections();

    NodeRoot* node_root;
    QMap<Node*, QPointF> inspectors;

    QList<Node*> nodes;
    QList<QPair<Datum*, Datum*>> connections;

    static int PROTOCOL_VERSION;
};

#endif // SERIALIZER_H

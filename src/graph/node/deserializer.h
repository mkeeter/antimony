#ifndef DESERIALIZER_H
#define DESERIALIZER_H

#include <QObject>
#include <QMap>
#include <QPointF>
#include <QJsonObject>
#include <QJsonArray>

class Node;
class NodeRoot;

class SceneDeserializer : public QObject
{
    Q_OBJECT
public:
    explicit SceneDeserializer(NodeRoot* node_root);

    bool run(QJsonObject in);

    QMap<Node*, QPointF> inspectors;
    bool failed;
    QString error_message;
    QString warning_message;

protected:
    void deserializeNodes(QJsonArray in, NodeRoot* p);
    void deserializeNode(QJsonObject in, NodeRoot* p);
    void deserializeDatum(QJsonObject in, Node* node);
    void deserializeConnections(QJsonArray in);

    quint32 protocol_version;
    NodeRoot* node_root;
    QList<Node*> nodes;
};

#endif // DESERIALIZER_H

#ifndef DESERIALIZER_H
#define DESERIALIZER_H

#include <QObject>
#include <QMap>
#include <QPointF>

class Datum;
class Node;
class NodeRoot;

class SceneDeserializer : public QObject
{
    Q_OBJECT
public:
    explicit SceneDeserializer(NodeRoot* node_root);

    bool run(QDataStream* in);
    bool run(QByteArray in);

    bool hasError() const { return failed; }
    QString errorMessage() const { return error_message; }

    QMap<Node*, QPointF> inspectors;
    bool failed;
    QString error_message;

protected:
    void deserializeNodes(QDataStream* in, NodeRoot* p);
    void deserializeNode(QDataStream* in, NodeRoot* p);
    void deserializeDatum(QDataStream* in, Node* node);
    void deserializeConnections(QDataStream* in);

    NodeRoot* node_root;
    QList<Datum*> datums;
};

#endif // DESERIALIZER_H

#ifndef DESERIALIZER_OLD_H
#define DESERIALIZER_OLD_H

#include <QObject>
#include <QMap>
#include <QPointF>

class Datum;
class Node;
class NodeRoot;

class SceneDeserializerOld : public QObject
{
    Q_OBJECT
public:
    explicit SceneDeserializerOld(NodeRoot* node_root);

    bool run(QDataStream* in);
    bool run(QByteArray in);

    QMap<Node*, QPointF> inspectors;
    bool failed;
    QString error_message;
    QString warning_message;

protected:
    void deserializeNodes(QDataStream* in, NodeRoot* p);
    void deserializeNode(QDataStream* in, NodeRoot* p);
    void deserializeDatum(QDataStream* in, Node* node);
    void deserializeConnections(QDataStream* in);

    quint32 protocol_version;
    NodeRoot* node_root;
    QList<Datum*> datums;
};

#endif // DESERIALIZER_OLD_H

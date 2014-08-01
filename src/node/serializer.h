#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <QObject>
#include <QList>
#include <QPair>

class Datum;
class Node;

class SceneSerializer : public QObject
{
public:
    explicit SceneSerializer(QObject* parent=0);
    void run(QDataStream* out);
protected:
    void serializeNodes(QDataStream* out, QObject* p);
    void serializeNode(QDataStream* out, Node* node);
    void serializeDatum(QDataStream* out, Datum* datum);
    void serializeConnections(QDataStream* out);

    QList<Datum*> datums;
    QList<QPair<Datum*, Datum*>> connections;
};

#endif // SERIALIZER_H

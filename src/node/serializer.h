#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <QObject>
#include <QList>
#include <QPair>

class Datum;
class Node;

class SceneSerializer : public QObject
{
    Q_OBJECT
public:
    explicit SceneSerializer(QObject* parent=0);
    void run(QDataStream* out);
protected:
    void serializeNodes(QDataStream* out);
    void serializeNode(Node* node, QDataStream* out);

    QList<Datum*> datums;
    QList<QPair<Datum*, Datum*>> connections;
};

////////////////////////////////////////////////////////////////////////////////

class SceneDeserializer : public QObject
{
    Q_OBJECT
public:
    explicit SceneDeserializer(QString filename, QObject* parent=0);
    QDataStream run();
    bool hasError() const { return failed; }
    QString errorMessage() const { return error_message; }
protected:
    bool failed;
    QString error_message;
};

#endif // SERIALIZER_H

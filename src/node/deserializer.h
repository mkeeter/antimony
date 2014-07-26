#ifndef DESERIALIZER_H
#define DESERIALIZER_H

#include <QObject>

class Datum;
class Node;

class SceneDeserializer : public QObject
{
    Q_OBJECT
public:
    explicit SceneDeserializer(QObject* parent=0);
    void run(QDataStream* in);
    bool hasError() const { return failed; }
    QString errorMessage() const { return error_message; }
protected:
    void deserializeNodes(QDataStream* in, QObject* p);
    void deserializeNode(QDataStream* in, QObject* p);
    void deserializeDatum(QDataStream* in, Node* node);
    bool failed;
    QString error_message;
};

#endif // DESERIALIZER_H

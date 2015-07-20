#ifndef DESERIALIZER_H
#define DESERIALIZER_H

#include <Python.h>

#include <QObject>
#include <QMap>
#include <QPointF>
#include <QJsonObject>
#include <QJsonArray>

class Node;
class Graph;

class SceneDeserializer : public QObject
{
    Q_OBJECT
public:
    explicit SceneDeserializer(Graph* graph);

    bool run(QJsonObject in);

    QMap<Node*, QPointF> inspectors;
    bool failed;
    QString error_message;
    QString warning_message;

protected:
    void deserializeNodes(QJsonArray in, Graph* p);
    void deserializeNode(QJsonObject in, Graph* p);
    void deserializeDatum(QJsonObject in, Node* node);

    quint32 protocol_version;
    Graph* graph;
    PyObject* globals;
};

#endif // DESERIALIZER_H

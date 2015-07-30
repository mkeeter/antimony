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

#define SUPPORT_PROTOCOL_5

namespace SceneDeserializer
{
    struct Info {
        QMap<Node*, QPointF> inspectors;
        QString error_message;
        QString warning_message;
    };

    /*
     *  Attempst to deserialize the given object as a graph.
     *
     *  Returns true on success, false otherwise.
     *  If info is provided, it is populated with relevant data.
     */
    bool run(QJsonObject in, Graph* graph, Info* info=NULL);

    void deserializeNode(QJsonObject in, Graph* p, Info* info=NULL);
    void deserializeDatum(QJsonObject in, Node* node);

#ifdef SUPPORT_PROTOCOL_5
    void updateGraph(QJsonObject* in);
    void updateNode(QJsonObject* in);
    void updateDatum(QJsonObject* in);
#endif

    extern PyObject* globals;
};

#endif // DESERIALIZER_H

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

namespace SceneSerializer
{
    QJsonObject run(
            Graph* root,
            QMap<Node*, QPointF> inspectors=QMap<Node*, QPointF>());

    /*
     *  A serialized graph is simply a list of serialized nodes
     */
    QJsonArray serializeGraph(Graph* g, QMap<Node*, QPointF> inspectors);

    /*
     *  A serialized node is a JSON object with fields
     *      "name", "uid", {"script"|"subgraph"}, "datums", ["inspector"]
     */
    QJsonObject serializeNode(Node* node, QMap<Node*, QPointF> inspectors);

    /*
     *  A serialized datum is a JSON object with fields
     *      "name", "uid", "expr", "type"
     */
    QJsonObject serializeDatum(Datum* datum);

    extern int PROTOCOL_VERSION;
};

#endif // SERIALIZER_H

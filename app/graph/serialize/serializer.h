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
struct CanvasInfo;

namespace SceneSerializer
{
    QJsonObject run(Graph* root, CanvasInfo* info=NULL);

    /*
     *  A serialized graph is simply a list of serialized nodes
     */
    QJsonArray serializeGraph(Graph* g, CanvasInfo* info);

    /*
     *  A serialized node is a JSON object with fields
     *      "name", "uid", {"script"|"subgraph"}, "datums", ["inspector"]
     */
    QJsonObject serializeNode(Node* node, CanvasInfo* info);

    /*
     *  A serialized datum is a JSON object with fields
     *      "name", "uid", "expr", "type"
     */
    QJsonObject serializeDatum(Datum* datum, CanvasInfo* info);

    extern int PROTOCOL_VERSION;
};

#endif // SERIALIZER_H

#pragma once

#include <QMap>
#include <QPointF>

class Node;
class Datum;

struct CanvasInfo
{
    QMap<Node*, QPointF> inspector;
    QMap<Datum*, QPointF> subdatum;

    void unite(const CanvasInfo& other);
};

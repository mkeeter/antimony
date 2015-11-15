#pragma once

#include <QObject>
#include <QHash>
#include <QSharedPointer>
#include <QMainWindow>

#include "graph/watchers.h"

class Node;
class NodeProxy;
class SuperDatumProxy;

class CanvasScene;
class CanvasWindow;

class GraphProxy : public QObject, public GraphWatcher
{
Q_OBJECT

public:
    GraphProxy(Graph* g, QObject* parent);
    ~GraphProxy();

    void trigger(const GraphState& state) override;

    template <class W, class S>
    W* newWindow(S* scene);

    CanvasWindow* newCanvasWindow();

protected:
    QHash<Node*, QString> title_cache;

    CanvasScene* canvas_scene;
    QList<QMainWindow*> windows;

    QHash<Node*, NodeProxy*> nodes;
    QHash<Datum*, SuperDatumProxy*> datums;
};

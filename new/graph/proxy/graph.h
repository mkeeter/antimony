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

    /*
     *  Create NodeProxies and SuperDatumProxies
     */
    void trigger(const GraphState& state) override;

    /*
     *  Returns the CanvasScene object
     */
    CanvasScene* canvasScene() const { return canvas_scene; }

    /*
     *  Constructs a new window of the given class, storing it in the
     *  windows list (and setting up automatic list pruning)
     */
    template <class W, class S> W* newWindow(S* scene);

    /*
     *  Opens a new Canvas window with this object's scene
     */
    CanvasWindow* newCanvasWindow();

protected:
    QHash<Node*, QString> title_cache;

    CanvasScene* canvas_scene;
    QList<QMainWindow*> windows;

    QHash<Node*, NodeProxy*> nodes;
    QHash<Datum*, SuperDatumProxy*> datums;
};

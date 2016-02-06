#ifndef CONTROL_ROOT_H
#define CONTROL_ROOT_H

#include <QObject>
#include <QMap>
#include <QSharedPointer>

#include "graph/watchers.h"

class Node;
class Datum;

class Control;
class Viewport;
class ViewportScene;
class RenderWorker;

class ControlRoot : public QObject, NodeWatcher
{
    Q_OBJECT
public:
    ControlRoot(Node* n, ViewportScene* vs);
    void registerControl(long index, Control* c);
    Control* get(long index) const;
    void makeProxiesFor(Viewport* v);

    /*
     *  On node change, update RenderWorkers.
     */
    void trigger(const NodeState& state) override;

    /*
     *  If there's a render worker for d, trigger a re-render.
     */
    void checkRender(Datum* d);

    void setGlow(bool g);

signals:
    void changeProxySelection(bool b);

protected:
    ViewportScene* vscene;

    QMap<long, QSharedPointer<Control>> controls;
    QMap<Datum*, QSharedPointer<RenderWorker>> workers;

    bool selected;
};

#endif

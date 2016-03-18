#pragma once

#include <QObject>
#include <QHash>

#include "graph/watchers.h"

////////////////////////////////////////////////////////////////////////////////

class Datum;

class OutputPort;
class InputPort;
class GraphProxy;

class DatumRow;
class Connection;

class ViewportView;
class ViewportScene;

////////////////////////////////////////////////////////////////////////////////

class BaseDatumProxy : public QObject, public DatumWatcher
{
    Q_OBJECT

public:
    BaseDatumProxy(Datum* d, QObject* parent, ViewportScene* scene, bool sub);
    virtual ~BaseDatumProxy();

    /*
     *  Return input and output ports (from the associated row)
     */
    virtual InputPort* inputPort() const=0;
    virtual OutputPort* outputPort() const=0;

    /*
     *  Return the enclosing graph proxy
     */
    virtual GraphProxy* graphProxy() const=0;

    /*
     *  Return the actual pointed-to datum
     */
    Datum* getDatum() { return datum; }

    /*
     *  Creates a new RenderInstance for this datum and the given view,
     *  then makes various connections to make everything work
     */
    void addViewport(ViewportView* view);

signals:
    /*
     *  Emitted to ask RenderInstances to update themselves
     */
    void datumChanged(Datum* d);

protected:
    Datum* const datum;
    const bool sub;

    /*  True if we should be making RenderInstances for this datum */
    const bool should_render;

    QHash<const Datum*, Connection*> connections;
};

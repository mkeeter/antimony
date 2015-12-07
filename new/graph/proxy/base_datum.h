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

////////////////////////////////////////////////////////////////////////////////

class BaseDatumProxy : public QObject, public DatumWatcher
{
    Q_OBJECT

public:
    BaseDatumProxy(Datum* d, QObject* parent)
        : QObject(parent), datum(d) {}
    virtual ~BaseDatumProxy() {}

    /*
     *  Return input and output ports (from the associated row)
     */
    virtual InputPort* inputPort() const=0;
    virtual OutputPort* outputPort() const=0;

    /*
     *  Return the enclosing graph proxy
     */
    virtual GraphProxy* graphProxy() const=0;

protected:
    Datum* const datum;
    QHash<const Datum*, Connection*> connections;
};

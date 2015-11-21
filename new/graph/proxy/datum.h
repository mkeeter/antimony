#pragma once

#include <QObject>
#include <QHash>

#include "graph/watchers.h"

class NodeProxy;
class InspectorRow;

class DatumProxy : public QObject, public DatumWatcher
{
Q_OBJECT

public:
    DatumProxy(Datum* d, NodeProxy* parent);
    ~DatumProxy();

    void trigger(const DatumState& state) override;

    /*
     *  Sets the index of this datum (for sorting in inspector)
     */
    void setIndex(int i);

protected:
    const Datum* datum;

    InspectorRow* row;
};

#pragma once

#include <QObject>
#include <QHash>

#include "graph/watchers.h"

class NodeProxy;

class DatumProxy : public QObject, public DatumWatcher
{
Q_OBJECT

public:
    DatumProxy(Datum* d, NodeProxy* parent);
    void trigger(const DatumState& state) override;

protected:
    const Datum* d;
};

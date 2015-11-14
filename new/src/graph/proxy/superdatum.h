#pragma once

#include <QObject>

#include "graph/watchers.h"

class GraphProxy;

class SuperDatumProxy : public QObject, public DatumWatcher
{
Q_OBJECT

public:
    SuperDatumProxy(Datum* d, GraphProxy* parent);
    void trigger(const DatumState& state) override;
};

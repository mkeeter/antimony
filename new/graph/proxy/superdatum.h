#pragma once

#include <QObject>

#include "graph/watchers.h"

class GraphProxy;
class DatumFrame;

class SuperDatumProxy : public QObject, public DatumWatcher
{
Q_OBJECT

public:
    SuperDatumProxy(Datum* d, GraphProxy* parent);
    ~SuperDatumProxy();

    void trigger(const DatumState& state) override;

protected:
    DatumFrame* frame;
};

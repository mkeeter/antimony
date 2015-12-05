#pragma once

#include <QObject>

#include "graph/watchers.h"

class GraphProxy;
class SubdatumFrame;

class SubdatumProxy : public QObject, public DatumWatcher
{
Q_OBJECT

public:
    SubdatumProxy(Datum* d, GraphProxy* parent);
    ~SubdatumProxy();

    void trigger(const DatumState& state) override;

protected:
    SubdatumFrame* frame;
};

#pragma once

#include <QObject>

#include "graph/watchers.h"

class RenderProxy;
class Viewport;

class RenderWorker : public QObject, DatumWatcher
{
    Q_OBJECT
public:
    explicit RenderWorker(Datum* datum);
    void trigger(const DatumState& state) override;

    static bool accepts(Datum* d);
signals:
    void changed();

protected:
    Datum* datum;

    friend class RenderProxy;
};

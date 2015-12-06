#pragma once

#include <QObject>

#include "graph/watchers.h"
#include "canvas/info.h"

class GraphProxy;
class SubdatumFrame;

class SubdatumProxy : public QObject, public DatumWatcher
{
Q_OBJECT

public:
    SubdatumProxy(Datum* d, GraphProxy* parent);
    ~SubdatumProxy();

    void trigger(const DatumState& state) override;

    /*
     *  Returns the position of the subdatum frame
     */
    CanvasInfo canvasInfo() const;

    /*
     * Sets the position of the subdatum frame
     */
    void setPositions(const CanvasInfo& info);

protected:
    Datum* const datum;

    SubdatumFrame* frame;
};

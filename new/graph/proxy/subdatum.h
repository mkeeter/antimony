#pragma once

#include <QObject>

#include "graph/proxy/base_datum.h"
#include "canvas/info.h"

class GraphProxy;
class SubdatumFrame;

class SubdatumProxy : public BaseDatumProxy
{
Q_OBJECT

public:
    SubdatumProxy(Datum* d, GraphProxy* parent);

    void trigger(const DatumState& state) override;

    /*
     *  Returns the position of the subdatum frame
     */
    CanvasInfo canvasInfo() const;

    /*
     * Sets the position of the subdatum frame
     */
    void setPositions(const CanvasInfo& info, bool select=false);

    /*
     *  Return input and output ports (from the associated row)
     *  (used in creating connections)
     */
    InputPort* inputPort() const override;
    OutputPort* outputPort() const override;

    /*
     *  Return the parent object (cast to a GraphProxy)
     */
    GraphProxy* graphProxy() const override;

protected:
    /*  Strong pointer to UI frame.  It is connected to this proxy's destroyed
     *  signal, but may also be deleted when the window closes (because it is
     *  owned by the QGraphicsScene)  */
    SubdatumFrame* frame;
};

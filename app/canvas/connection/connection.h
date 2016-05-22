#pragma once

#include "canvas/connection/base.h"

////////////////////////////////////////////////////////////////////////////////

class Datum;
class BaseDatumProxy;

class CanvasScene;

class OutputPort;
class InputPort;

////////////////////////////////////////////////////////////////////////////////

class Connection : public BaseConnection
{
    Q_OBJECT
public:
    explicit Connection(const Datum* source, BaseDatumProxy* target);

    /*
     *  Helper functions to get relevant datums
     */
    Datum* sourceDatum() const;
    Datum* targetDatum() const;

protected slots:
    /*
     *  When ports have moved, call prepareGeometryChange()
     */
    void onPortsMoved();

    /*
     *  When the hidden state of a port changes, hide or show ourself
     */
    void onHiddenChanged();

protected:
    /*
     *  Check if either of the ports is hidden
     */
    bool isHidden() const;

    QPointF startPos() const override;
    QPointF endPos() const override;

    InputPort* target_port;
    OutputPort* source_port;
};

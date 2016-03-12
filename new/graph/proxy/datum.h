#pragma once

#include <QObject>
#include <QMap>

#include "graph/proxy/base_datum.h"

class NodeProxy;
class RenderInstance;

class DatumProxy : public BaseDatumProxy
{
Q_OBJECT

public:
    DatumProxy(Datum* d, NodeProxy* parent);

    void trigger(const DatumState& state) override;

    /*
     *  Sets the index of this datum (for sorting in inspector)
     */
    void setIndex(int i);

    /*
     *  Return input and output ports (from the associated row)
     *  (used in creating connections)
     */
    InputPort* inputPort() const override;
    OutputPort* outputPort() const override;

    /*
     *  Return the parent's parent object (cast to a GraphProxy)
     */
    GraphProxy* graphProxy() const override;

protected:
    /*  Weak pointer to UI row in the canvas / inspector view
     *  (since it is owned by the parent InspectorFrame  */
    DatumRow* row;
};

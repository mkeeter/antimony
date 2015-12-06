#pragma once

#include "canvas/connection/base.h"

class Datum;
class DatumProxy;

class OutputPort;
class InputPort;

class Connection : public BaseConnection
{
    Q_OBJECT
public:
    explicit Connection(const Datum* source, DatumProxy* target);

protected:
    QPointF startPos() const override { return QPointF(); }
    QPointF endPos() const override { return QPointF(); }

    void setPorts(OutputPort* source, InputPort* sink) {}
};

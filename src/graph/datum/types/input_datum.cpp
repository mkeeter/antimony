#include "graph/datum/types/input_datum.h"
#include "graph/datum/input.h"

InputDatum::InputDatum(QString name, QObject* parent)
    : Datum(name, parent)
{
    // Nothing to do here
    // (child constructors must call update())
}

QString InputDatum::getString() const
{
    Q_ASSERT(input_handler);
    return input_handler->getString();
}

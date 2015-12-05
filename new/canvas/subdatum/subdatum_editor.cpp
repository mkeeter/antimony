#include <Python.h>

#include "canvas/subdatum/subdatum_editor.h"

#include "graph/datum.h"

void SubdatumEditor::update(const DatumState& state)
{
}

void SubdatumEditor::setDatumText(QString s)
{
    DatumEditor::setDatumText(Datum::SIGIL_SUBGRAPH_OUTPUT + s);
}

QString SubdatumEditor::getDatumText() const
{
    QString raw = DatumEditor::getDatumText();

    if (raw.startsWith(Datum::SIGIL_SUBGRAPH_OUTPUT))
        raw = raw.mid(1);
    else if (raw.startsWith(Datum::SIGIL_SUBGRAPH_CONNECTION))
        raw[0] = Datum::SIGIL_CONNECTION;

    return raw;
}

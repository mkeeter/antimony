#include <Python.h>

#include "canvas/subdatum/subdatum_editor.h"

#include "graph/datum.h"

void SubdatumEditor::update(const DatumState& state)
{
    DatumState mod = state;
    if (mod.sigil == Datum::SIGIL_NONE)
        mod.sigil = Datum::SIGIL_OUTPUT;
    else if (mod.sigil == Datum::SIGIL_SUBGRAPH_OUTPUT)
        mod.sigil = Datum::SIGIL_NONE;
    else if (mod.sigil == Datum::SIGIL_SUBGRAPH_CONNECTION)
        mod.sigil = Datum::SIGIL_CONNECTION;

    DatumEditor::update(mod);
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

QString SubdatumEditor::formatSpecial(QString t, const DatumState& state) const
{
    auto s = DatumEditor::formatSpecial(t, state);
    return s.replace("[output]", "[input]");
}

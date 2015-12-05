#include <Python.h>

#include "canvas/subdatum/subdatum_row.h"
#include "canvas/subdatum/subdatum_editor.h"

#include "canvas/datum_port.h"
#include "canvas/datum_frame.h"

#include "graph/datum.h"

////////////////////////////////////////////////////////////////////////////////

SubdatumRow::SubdatumRow(Datum* d, DatumFrame* parent)
    : DatumRow(d, static_cast<QGraphicsItem*>(parent))
{
    connect(this, &DatumRow::layoutChanged,
            parent, &DatumFrame::redoLayout);
    editor = new SubdatumEditor(d, this);
}

void SubdatumRow::update(const DatumState& state)
{
    editor->update(state);

    Q_ASSERT(state.sigil != Datum::SIGIL_OUTPUT);
    if (state.sigil == Datum::SIGIL_NONE)
        input->hide();
    else
        input->show();
}

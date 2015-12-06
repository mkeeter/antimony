#pragma once

#include "canvas/datum_editor.h"

class SubdatumEditor : public DatumEditor
{
public:
    SubdatumEditor(Datum* d, QGraphicsItem* parent)
        : DatumEditor(d, parent) {}

    /*
     *  Update from the given state, setting enabled and text field
     */
    void update(const DatumState& state) override;

    /*
     *  Set datum text, prepending SIGIL_SUBGRAPH_OUTPUT
     */
    void setDatumText(QString s) override;

    /*
     *  Returns datum text with sigil stripped or tweaked
     *
     *  No sigil becomes SIGIL_OUTPUT
     *  SIGIL_SUBGRAPH_OUTPUT becomes no sigil
     *  SIGIL_SUBGRAPH_CONNECTION becomes SIGIL_CONNECTION
     */
    QString getDatumText() const override;

    /*
     *  Override formatSpecial so that inputs from the parent graph
     *  are labelled [input] (though they're disguised with SIGIL_OUTPUT
     *  in the override for update
     */
    QString formatSpecial(QString t, const DatumState& s) const override;
};

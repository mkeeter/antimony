#pragma once

#include "canvas/datum_row.h"

class SubdatumRow : public DatumRow
{
public:
    explicit SubdatumRow(Datum* d, DatumFrame* parent);

    /*
     *  Override trigger function to properly handle subgraph
     */
    void update(const DatumState& state) override;
};

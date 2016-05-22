#pragma once

#include "canvas/datum_row.h"

class SubdatumFrame;

class SubdatumRow : public DatumRow
{
public:
    explicit SubdatumRow(Datum* d, SubdatumFrame* parent);

    /*
     *  Override trigger function to properly handle subgraph
     */
    void update(const DatumState& state) override;
};

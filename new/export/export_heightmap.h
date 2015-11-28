#pragma once

#include <Python.h>

#include "export/export_worker.h"
#include "fab/types/shape.h"

////////////////////////////////////////////////////////////////////////////////

class ExportHeightmapWorker : public ExportWorker
{
public:
    explicit ExportHeightmapWorker(Shape s, Bounds b, QString f,
                                   float r, float mm)
        : ExportWorker(s, b, f, r), mm_per_unit(mm) {}

    /*
     *  Top-level function that accepts user input and start the export
     */
    void run() override;

    /*
     *  Asynchronous function that actually performs rendering
     */
    void async() override;

protected:
    /*
     *  Call-time settings
     */
    const float mm_per_unit;

    /*
     *  Run-time, set by dialogs
     */
    float _mm_per_unit;

    /*
     *  Flag set by async call
     */
    volatile bool success;
};

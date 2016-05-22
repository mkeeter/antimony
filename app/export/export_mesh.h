#pragma once

#include <Python.h>

#include "export/export_worker.h"
#include "fab/types/shape.h"

////////////////////////////////////////////////////////////////////////////////

class ExportMeshWorker : public ExportWorker
{
public:
    explicit ExportMeshWorker(Shape s, Bounds b, QString f, float r, bool d)
        : ExportWorker(s, b, f, r), detect_features(d) {}

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
    const bool detect_features;

    /*
     *  Run-time, set by dialogs
     */
    bool _detect_features;
};

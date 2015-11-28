#pragma once

#include <Python.h>
#include <QObject>

#include "fab/types/shape.h"
#include "fab/types/bounds.h"

/*
 *  Abstract base class used to export files from a Shape
 */
class ExportWorker : public QObject
{
    Q_OBJECT
public:
    explicit ExportWorker(Shape s, Bounds b, QString f, float r)
        : shape(s), bounds(b), filename(f), resolution(r), halt(0) {}

    /*
     *  Synchronous part of export (e.g. getting resolution from dialog)
     */
    virtual void run()=0;

    /*
     *  Asynchronous part of export (e.g. actually meshing the model)
     */
    virtual void async()=0;

    /*
     *  Handles async running and stopping
     */
    void runAsync();

    /*
     *  Checks if _filename is writable.
     *  If so, returns true; otherwise, shows a warning and returns false.
     */
    bool checkWritable() const;

protected:
    /*
     *  These are constants for the export worker and set at call-time
     */
    const Shape shape;
    const Bounds bounds;
    const QString filename;
    const float resolution;

    /*
     *  These are variables that are set by dialogs (and don't persist
     *  between calls to the worker).
     */
    float _resolution;
    QString _filename;

    /*
     *  Flag used to abort rendering
     */
    volatile int halt;
};

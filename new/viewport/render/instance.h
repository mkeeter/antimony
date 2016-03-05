#pragma once

#include <Python.h>
#include <QObject>

#include "viewport/image.h"

class Datum;
class DatumProxy;
class ViewportView;
class RenderTask;

class RenderInstance : public QObject
{
    Q_OBJECT
public:
    RenderInstance(DatumProxy* parent, ViewportView* view);

    ~RenderInstance();

public slots:
    /*
     *  Marks that either the view or parent have been destroyed.
     *  If there's not a running task, the instance is deleted immediately;
     *  otherwise, it is deleted when the task finishes
     */
    void makeOrphan();

    /*
     *  Mark that the datum has changed.
     *  If the datum should be rendered, stores a pointer to its shape
     *  function in shape; otherwise, sets shape to nullptr.
     */
    void datumChanged(Datum* d);

    /*
     *  Sets M to the current viewport transform matrix
     */
    void viewChanged(QMatrix4x4 m);

protected slots:
    /*
     *  When a render task finishes, load in its image and start the next
     *  render operation (if one is queued up)
     */
    void onTaskFinished();

protected:
    /*
     *  Mark that there's a pending render task
     */
    void setPending();

    /*
     *  Start a new render from cached shape and matrix
     */
    void startNextRender();

    /*  Set to true if the datum or viewport are gone  */
    bool orphan=false;

    /*  Pointer to object of type fab.types.Shape      *
     *  If null, we shouldn't be rendering this shape  */
    PyObject* shape=nullptr;

    /*  Current transform matrix  */
    QMatrix4x4 M;

    /*  Set to true if we should render again after the task finishes  */
    bool pending=false;

    /*  Current (active) render task, or null  */
    QScopedPointer<RenderTask> current;

    DepthImage image;
};

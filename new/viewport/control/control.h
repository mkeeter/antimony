#pragma once

#include <Python.h>

#include <QObject>
#include <QMap>
#include <QPainter>
#include <QVector3D>

class NodeProxy;
class ViewportView;
class ControlInstance;

class Control : public QObject
{
public:
    Control(NodeProxy* parent);

    /*
     *  Destructor asks all instances to delete themselves
     */
    ~Control();

    /*
     *  Create a new ControlInstance for the given viewport
     */
    void makeInstanceFor(ViewportView* v);

    /*
     *  This function is overloaded by children to return bounds.
     *  By default, returns the bounding box of shape().
     */
    virtual QRectF bounds(QMatrix4x4 m) const;

    /*
     *  Returns the shape of this object (for selection and highlighting)
     */
    virtual QPainterPath shape(QMatrix4x4 m) const=0;

    /*
     *  This function should be defined by child nodes
     *  m is the world-to-screen transform matrix.
     */
    virtual void paint(QMatrix4x4 m, bool highlight, QPainter* painter)=0;

    /*
     *  Return a central point associated with this control
     */
    virtual QVector3D pos() const=0;

    /*
     *  Check whether this control uses a relative drag function
     */
    bool getRelative() const { return relative; }

    /*
     *  Assigns to drag_func
     *      Removes a reference from the previous drag_func
     *      Steals a reference from new_drag_func
     */
    void setDragFunc(PyObject* new_drag_func);

    /*
     *  Drags the node around using the Python drag_func
     */
    void drag(QVector3D center, QVector3D diff);

    /*
     *  Delete the parent node
     */
    void deleteNode();

    /*  Flag indicating whether this control has been touched  */
    bool touched=false;

protected:
    /*  Instances (which are QGraphicsItems) for each viewport               *
     *  These are owned by the relevant ViewportView and configured to       *
     *  auto-remove themselves from the list on deletion, but the Control's  *
     *  destructor will call deleteLater on each Instance.                   */
    QMap<ViewportView*, ControlInstance*> instances;

    /*  Function that is called when the control is dragged  *
     *  The function is invoked as drag_func(this, x, y, z)  *
     *      this is a mutable node proxy                     *
     *      x, y, z are either absolute positions or deltas  *
     *        (depending on whether relative is set)         */
    PyObject* drag_func=nullptr;

    /*  Flag indicating whether drag function is relative or absolute  */
    bool relative=true;
};

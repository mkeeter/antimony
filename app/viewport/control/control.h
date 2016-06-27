#pragma once

#include <Python.h>

#include <QObject>
#include <QMap>
#include <QPainter>
#include <QVector3D>

class NodeProxy;
class ViewportView;
class ControlInstance;

class Datum;
class Node;

class Control : public QObject
{
Q_OBJECT

public:
    Control(NodeProxy* parent);

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
     *  Assigns to drag_func
     *      Removes a reference from the previous drag_func
     *      Steals a reference from new_drag_func
     */
    void setDragFunc(PyObject* new_drag_func);

    /*
     *  Returns true if the given control has a drag function
     */
    bool hasDragFunc() const { return drag_func; }

    /*
     *  Drags the node around using the Python drag_func
     */
    void drag(QVector3D center, QVector3D diff);

    /*
     *  Saves all datum expressions
     *  (so that we can detect which ones were dragged)
     */
    void beginDrag();

    /*
     *  If datum expressions changed, push an undo command to undo the drag
     */
    void endDrag();

    /*
     *  Delete the parent node
     */
    void deleteNode();

    /*
     *  Looks up Node's name
     */
    QString getName() const;

    /*
     *  Looks up a node by value
     */
    const Node* getNode() const;

    /*  Flag indicating whether this control has been touched  */
    bool touched=false;

signals:
    /*
     *  Used to request that instances redraw themselves
     */
    void redraw();

    /*
     *  Emitted when focus changes
     */
    void onFocus(bool focus);

    /*
     *  Used to request zooming to the given node in all relevant windows
     */
    void onZoomTo();

public slots:
    /*
     *  Sets whether the control is highlighted
     */
    void setFocus(bool focus);

protected:
    /*  Function that is called when the control is dragged  *
     *  The function is invoked as drag_func(this, x, y, z)  *
     *      this is a mutable node proxy                     *
     *      x, y, z are either absolute positions or deltas  *
     *        (depending on whether relative is set)         */
    PyObject* drag_func=nullptr;

    /*  Flag indicating whether drag function is relative or absolute  */
    bool relative=true;

    /*  Map of datum string values (used to undo dragging)  */
    QMap<Datum*, QString> datum_text;

    /*  Set to true when we should highlight the control  */
    bool has_focus=false;

    friend struct ScriptUIHooks;
};

#ifndef CONTROL_H
#define CONTROL_H

#include <boost/python.hpp>

#include <QObject>
#include <QPointer>
#include <QVector3D>
#include <QMatrix4x4>
#include <QPainter>

// Forward declarations
class Datum;
class EvalDatum;
class Node;

class Control : public QObject
{
    Q_OBJECT
public:
    /*
     *  A control is a UI object created by certain script hooks.
     *
     *  node is the target Node
     */
    explicit Control(Node* node, PyObject* drag_func=NULL);

    void deleteLater();
    /*
     *  Destructor removes a reference to the drag function.
     */
    ~Control();

    /*
     * Returns this control's relevant node.
     */
    Node* getNode() const;

    /*
     *  Swaps in a new drag_func, removing a reference from the old one.
     *  Steals a reference from new_drag_func.
     */
    void setDragFunc(PyObject* new_drag_func);

    /*
     *  Schedules the top-level node for deletion in an undo-able way
     *  text is an optional argument that sets the text in the undo command.
     */
    void deleteNode(QString text=QString());

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
     *  Saves watched datum's expressions
     *  (used in undo/redo framework)
     */
    void beginDrag();

    /*
     *  Called to drag the node around with the mouse.
     */
    void drag(QVector3D center, QVector3D diff);

    /*
     *  Pushes an UndoCommand to the stack that undoes the
     *  previous drag operation.
     */
    void endDrag();

    /*
     *  Sets the value of glow, emitting redraw if changed.
     */
    void setGlow(bool g);

    /*
     *  Set touched to true.
     *  This should be called in a UI hook to mark that this Control has been
     *  used in the most recent script evaluation (and hence shouldn't be
     *  deleted in a pruning pass).
     */
    void touch() { touched = true; }

    bool isDragging() const { return is_dragging; }

    bool getRelative() const { return relative; }

    /*
     *  Returns the position of this control.
     *  Used to constrain the 2D -> 3D projection problem
     *  for non-relative dragging.
     */
    virtual QVector3D pos() const=0;

    bool isDeleteScheduled() const { return delete_scheduled; }

public slots:
    void clearTouchedFlag();
    void deleteIfNotTouched();

signals:
    void redraw();
    void glowChanged(Node* node, bool g);

    /*
     *  When emitted, changes selection status of children proxies.
     */
    void changeProxySelection(bool g);

    /*
     *  When emitted, informs the parent ControlRoot of a selection change.
     */
    void proxySelectionChanged(bool g);

protected:
    QPointer<Node> node;
    QMap<EvalDatum*, QString> datums;

    PyObject* drag_func;
    bool is_dragging;

    // Defines whether the top-level ControlRoot for this node
    // has been selected.
    bool glow;
    bool touched;

    bool relative;

    /*
     *  Set when deleteLater is called
     *  (because otherwise things can go wrong where getControl
     *   return this Control but it's about to be deleted).
     */
    bool delete_scheduled;
};


#endif // CONTROL_H

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
    explicit Control(Node* node);

    /*
     *  Destructor removes a reference to the drag function.
     */
    ~Control();

    /*
     * Returns this control's relevant node.
     */
    Node* getNode() const;

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
    void drag(QVector3D center);

    /*
     *  Pushes an UndoCommand to the stack that undoes the
     *  previous drag operation.
     */
    void endDrag();

    /*
     *  Sets the value of glow, emitting redraw if changed.
     */
    void setGlow(bool g);

signals:
    void redraw();
    void glowChanged(Node* node, bool g);

protected:
    QPointer<Node> node;
    QMap<EvalDatum*, QString> datums;

    PyObject* drag_func;

    bool glow;
};


#endif // CONTROL_H

#ifndef CONTROL_H
#define CONTROL_H

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

    /** Returns this control's relevant node.
     */
    Node* getNode() const;

    /** Gets the value of a specific datum (which must be a double).
     */
    double getValue(QString name) const;

    /*
     *  Schedules the top-level node for deletion in an undo-able way
     *  text is an optional argument that sets the text in the undo command.
     */
    void deleteNode(QString text=QString());

    /*
     *  This function is overloaded by children to return bounds.
     */
    virtual QRectF bounds(QMatrix4x4 m, QMatrix4x4 t) const=0;

    /*
     *  Equivalent to QGraphicsObject::shape
     *  By default, returns the bounding rect
     */
    virtual QPainterPath shape(QMatrix4x4 m, QMatrix4x4 t) const;

    /*
     *  This function should be defined by child nodes
     *  m is the world-to-screen transform matrix.
     *  t is just the rotation component of this matrix
     *      (used for cylinders and spheres to make lines
     *       face user at all times)
     */
    virtual void paint(QMatrix4x4 m, QMatrix4x4 t,
                       bool highlight, QPainter* painter)=0;

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

    void setGlow(bool g);

signals:
    void redraw();
    void glowChanged(Node* node, bool g);

protected:
    QPointer<Node> node;
    QMap<EvalDatum*, QString> datums;

    bool glow;
};


#endif // CONTROL_H

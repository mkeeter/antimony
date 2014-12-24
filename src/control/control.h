#ifndef CONTROL_H
#define CONTROL_H

#include <QObject>
#include <QPointer>
#include <QVector3D>
#include <QMatrix4x4>
#include <QPainter>

// Forward declarations
class Datum;
class Node;

class Control : public QObject
{
    Q_OBJECT
public:
    /*
     *  A control is a UI representation of a Node.
     *
     *  node is the target Node (or NULL in special cases)
     *  parent is a parent Control (as nested controls are allowed)
     */
    explicit Control(Node* node, QObject* parent=0);

    /** Returns this control's relevant node.
     */
    Node* getNode() const;

    /** Gets the value of a specific datum (which must be a double).
     */
    double getValue(QString name) const;

    /** Calls deleteLater on the top-level node.
     */
    void deleteNode();

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

    /** Called to drag the node around with the mouse.
     */
    virtual void drag(QVector3D center, QVector3D delta)=0;

    /*
     *  Overloaded to make buttons inside the Control.
     *  Returns true if the Control has used the click event
     *  (which prevents the event from being used for selection)
     */
    virtual bool onClick() { return false; }

signals:
    void redraw();

protected:
    /** Mark a set of datums as causing a re-render when changed.
     */
    void watchDatums(QVector<QString> datums);

    /** Attempts to drag a particular datum's value.
     */
    void dragValue(QString name, double delta);

    /** Sets a specific datum's value to the given value.
     */
    void setValue(QString name, double new_value);

    /** Returns the color to be used by the default pen.
     */
    virtual QColor defaultPenColor() const;

    /** Returns the color to be used by the default brush.
     */
    virtual QColor defaultBrushColor() const;

    /** Sets the painter pen to a reasonable default value.
     */
    void setDefaultPen(bool highlight, QPainter* painter) const;

    /** Sets the painter brush to a reasonable value.
     */
    void setDefaultBrush(bool highlight, QPainter* painter) const;

    QPointer<Node> node;
};


#endif // CONTROL_H

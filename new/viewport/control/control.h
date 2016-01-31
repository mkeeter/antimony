#pragma once

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

    /*  Flag indicating whether this control has been touched  */
    bool touched=false;
protected:
    /*  Instances (which are QGraphicsItems) for each viewport  *
     *  (ownership unclear at the moment)                       */
    QMap<ViewportView*, ControlInstance*> instances;

    /*  Flag indicating whether drag function is relative or absolute  */
    bool relative=false;
};

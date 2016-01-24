#pragma once

#include <QObject>
#include <QMap>
#include <QPainter>

class ControlRoot;
class ViewportView;
class ControlInstance;

class Control : public QObject
{
public:
    Control(ControlRoot* parent);

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

protected:
    /*  Instances (which are QGraphicsItems) for each viewport  *
     *  (ownership unclear at the moment)                       */
    QMap<ViewportView*, ControlInstance*> instances;
};

#pragma once

#include <QObject>
#include <QMap>

class ViewportView;
class Control;
class NodeProxy;

class ControlRoot : public QObject
{
public:
    ControlRoot(NodeProxy* parent);

    /*
     *  For all controls, make a ControlInstance for the given viewport
     */
    void makeInstancesFor(ViewportView* v);

protected:
    /*  Mapping from line numbers to control objects (owned by this root)  */
    QMap<long, Control*> controls;
};

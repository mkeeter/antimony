#include <QMatrix4x4>

#include "viewport/control/root.h"
#include "viewport/control/control.h"
#include "viewport/control/instance.h"
#include "viewport/view.h"

Control::Control(ControlRoot* parent)
    : QObject(parent)
{
    // Nothing to do here
}

Control::~Control()
{
    for (auto i : instances)
    {
        disconnect(i, &QGraphicsObject::destroyed, 0, 0);
        i->deleteLater();
    }
}

void Control::makeInstanceFor(ViewportView* v)
{
    auto i = new ControlInstance(this, v);
    instances[v] = i;
    connect(i, &QObject::destroyed, [=]{ this->instances.remove(v); });
}

QRectF Control::bounds(QMatrix4x4 m) const
{
    // Default implementation is the bounding rect of shape().
    return shape(m).boundingRect();
}

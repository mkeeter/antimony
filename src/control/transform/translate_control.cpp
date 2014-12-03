#include <Python.h>
#include "control/transform/translate_control.h"

_TranslateHandle::_TranslateHandle(Node* node, QObject* parent)
    : WireframeControl(node, parent)
{
    watchDatums({"_x", "_y", "_z", "dx", "dy", "dz"});
}

void _TranslateHandle::drag(QVector3D c, QVector3D d)
{
    Q_UNUSED(c);
    dragValue("dx", d.x());
    dragValue("dy", d.y());
    dragValue("dz", d.z());
}

QVector3D _TranslateHandle::position() const
{
    return QVector3D(getValue("_x"), getValue("_y"), getValue("_z")) + 
           QVector3D(getValue("dx"), getValue("dy"), getValue("dz"));
}

QVector<QPair<QVector3D, float>> _TranslateHandle::points() const
{
    return {{position(), 5}};
}

////////////////////////////////////////////////////////////////////////////////

TranslateControl::TranslateControl(Node* node, QObject* parent)
    : WireframeControl(node, parent),
      handle(new _TranslateHandle(node, this))
{
    watchDatums({"_x", "_y", "_z", "dx", "dy", "dz"});
}

void TranslateControl::drag(QVector3D c, QVector3D d)
{
    Q_UNUSED(c);
    dragValue("_x", d.x());
    dragValue("_y", d.y());
    dragValue("_z", d.z());
}

QVector3D TranslateControl::position() const
{
    return QVector3D(getValue("_x"), getValue("_y"), getValue("_z"));
}

QVector<QVector<QVector3D>> TranslateControl::lines() const
{
    QVector3D p = position();
    QVector3D delta(getValue("dx"), getValue("dy"), getValue("dz"));
    return {{p + QVector3D(-0.3, 0, 0), p + QVector3D(0.3, 0, 0)},
            {p + QVector3D(0, -0.3, 0), p + QVector3D(0, 0.3, 0)},
            {p + QVector3D(0, 0, -0.3), p + QVector3D(0, 0, 0.3)},
            {p, p + delta}};
}

QVector<QPair<QVector3D, float>> TranslateControl::points() const
{
    return {{position(), 5}};
}

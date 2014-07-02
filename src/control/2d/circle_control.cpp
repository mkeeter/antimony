#include "control/2d/circle_control.h"
#include "ui/canvas.h"

_RadiusControl::_RadiusControl(Canvas *canvas, Node *node, QGraphicsItem *parent)
    : MultiLineControl(canvas, node, parent)
{
    // Nothing to do here
}

QVector<QVector<QVector3D>> _RadiusControl::lines() const
{
    QVector3D c = center();
    float r = getValue("r");
    int n = 64;

    QVector<QVector3D> circle;
    for (int i=0; i <= n; ++i)
    {
        circle << c + r*QVector3D(cos(i*2*M_PI/n), sin(i*2*M_PI/n), 0);
    }

    return {circle};
}

QVector3D _RadiusControl::center() const
{
    return QVector3D(getValue("x"), getValue("y"), 0);
}

void _RadiusControl::drag(QVector3D c, QVector3D d)
{
    dragValue("r", QVector3D::dotProduct((c - center()).normalized(), d));
}

////////////////////////////////////////////////////////////////////////////////

void CircleControl::drag(QVector3D center, QVector3D delta)
{
    // nothing to do here for now
}

QPointF CircleControl::inspectorPosition() const
{
    return canvas->worldToScene(radius->center());
}

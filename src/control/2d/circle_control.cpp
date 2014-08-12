#include "control/2d/circle_control.h"
#include "ui/canvas.h"

_RadiusControl::_RadiusControl(Canvas *canvas, Node *node, QGraphicsItem *parent)
    : WireframeControl(canvas, node, parent)
{
    watchDatums({"x","y","r"});
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

CircleControl::CircleControl(Canvas *canvas, Node *node)
    : DummyControl(canvas, node),
      radius(new _RadiusControl(canvas, node, this)),
      center(new Point2DControl(canvas, node, this))
{
    watchDatums({"x","y","r"});
}

void CircleControl::drag(QVector3D c, QVector3D delta)
{
    center->drag(c, delta);
}

QPointF CircleControl::inspectorPosition() const
{
    return canvas->worldToScene(radius->center());
}

QPointF CircleControl::baseInputPosition() const
{
    return (bounds().bottomLeft() + bounds().topLeft()) / 2;
}

QPointF CircleControl::baseOutputPosition() const
{
    return (bounds().bottomRight() + bounds().topRight()) / 2;
}

QRectF CircleControl::bounds() const
{
    return radius->boundingRect();
}

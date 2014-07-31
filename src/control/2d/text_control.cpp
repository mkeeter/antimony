#include "control/2d/text_control.h"
#include "ui/canvas.h"

_CaretTopControl::_CaretTopControl(Canvas* canvas, Node* node, QGraphicsItem* parent)
    : MultiLineControl(canvas, node, parent)
{
    watchDatums({"x", "y", "scale"});
}

QVector<QVector<QVector3D>> _CaretTopControl::lines() const
{
    float s = getValue("scale");
    float x = getValue("x");
    float y = getValue("y");

    return {{QVector3D(x - s/4, y + s, 0),
             QVector3D(x - s/16, y + s, 0),
             QVector3D(x, y + s*0.9, 0),
             QVector3D(x + s/16, y + s, 0),
             QVector3D(x + s/4, y + s, 0)}};
}

void _CaretTopControl::drag(QVector3D c, QVector3D d)
{
    Q_UNUSED(c);
    dragValue("scale", d.y());
}

////////////////////////////////////////////////////////////////////////////////

TextControl::TextControl(Canvas* canvas, Node* node)
    : MultiLineControl(canvas, node),
      caret_top(new _CaretTopControl(canvas, node, this))
{
    watchDatums({"x", "y", "scale"});
}

void TextControl::drag(QVector3D c, QVector3D d)
{
    Q_UNUSED(c);
    dragValue("x", d.x());
    dragValue("y", d.y());
}

QVector<QVector<QVector3D>> TextControl::lines() const
{
    float s = getValue("scale");
    float x = getValue("x");
    float y = getValue("y");

    return {{QVector3D(x - s/4, y, 0),
             QVector3D(x - s/16, y, 0),
             QVector3D(x, y + s*0.1, 0),
             QVector3D(x + s/16, y, 0),
             QVector3D(x + s/4, y, 0)},
           {QVector3D(x, y + s*0.1, 0),
            QVector3D(x, y + s*0.9, 0)}};
}

QPointF TextControl::inspectorPosition() const
{
    return canvas->worldToScene(QVector3D(
                getValue("x") + getValue("scale")/4,
                getValue("y"), 0));
}

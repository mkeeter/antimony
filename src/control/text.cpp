#include <QFont>
#include <QFontMetrics>

#include "control/text.h"
#include "ui/canvas.h"
#include "ui/colors.h"

TextLabelControl::TextLabelControl(QString label, QColor color,
                                   Canvas* canvas, Node* node)
    : Control(canvas, node), label(label), color(color),
      font("Courier", 12), font_metrics(font)
{
    watchDatums({"_x", "_y", "_z"});
}

void TextLabelControl::drag(QVector3D center, QVector3D delta)
{
    Q_UNUSED(center);
    dragValue("_x", delta.x());
    dragValue("_y", delta.y());
    dragValue("_z", delta.z());
}

void TextLabelControl::paintControl(QPainter *painter)
{
    painter->setPen(QPen(color));
    QColor c(isSelected() ? Colors::highlight(color) : color);
    c.setAlpha(150);
    painter->setBrush(QBrush(c));

    QRectF b = boundingRect();
    painter->drawRect(b);

    painter->setFont(font);
    painter->setPen(QPen(Qt::white));
    painter->drawText(b.topLeft() + QPoint(10, 20), getLabel());
}

QRectF TextLabelControl::bounds() const
{
    const QPointF p = canvas->worldToScene(position());
    QRectF br = font_metrics.boundingRect(getLabel());
    br.setRight(br.right() + 20);
    br.setBottom(br.bottom() + 20);
    return br.translated(p);
}

QPointF TextLabelControl::baseInputPosition() const
{
    return (bounds().bottomLeft() + bounds().topLeft()) / 2;
}

QPointF TextLabelControl::baseOutputPosition() const
{
    return (bounds().bottomRight() + bounds().topRight()) / 2;
}

QVector3D TextLabelControl::position() const
{
    return QVector3D(getValue("_x"), getValue("_y"), getValue("_z"));
}

QPointF TextLabelControl::inspectorPosition() const
{
    return bounds().bottomRight();
}

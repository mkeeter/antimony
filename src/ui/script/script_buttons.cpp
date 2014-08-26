#include <QDebug>

#include <QPainter>
#include <QGraphicsSceneMouseEvent>

#include "ui/script/script_buttons.h"
#include "ui/colors.h"

ScriptEditorButton::ScriptEditorButton(QGraphicsItem* parent)
    : QGraphicsObject(parent)
{
    // Nothing to do here
}

QRectF ScriptEditorButton::boundingRect() const
{
    return QRectF(0, 0, 20, 20);
}

////////////////////////////////////////////////////////////////////////////////

ScriptEditorCloseButton::ScriptEditorCloseButton(QGraphicsItem* parent)
    : ScriptEditorButton(parent)
{
    // Nothing to do here
}

void ScriptEditorCloseButton::paint(QPainter* p,
                                    const QStyleOptionGraphicsItem* o,
                                    QWidget* w)
{
    Q_UNUSED(o);
    Q_UNUSED(w);

    auto br = boundingRect();
    p->setPen(QPen(Colors::base07, 4));

    int offset = 6;
    p->drawLine(br.bottomLeft() + QPointF(offset, -offset),
                br.topRight() - QPointF(offset, -offset));
    p->drawLine(br.topLeft() + QPointF(offset, offset),
                br.bottomRight() - QPointF(offset, offset));
}

void ScriptEditorCloseButton::mousePressEvent(QGraphicsSceneMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
    {
        parentObject()->deleteLater();
    }
}
/*
 *
 *    offset /= 1.41;
    p->setPen(QPen(Colors::base07, 3));
    auto move = moveButton() - QMarginsF(offset, offset, offset, offset);
    p->drawEllipse(move);
    */

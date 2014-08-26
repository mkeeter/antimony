#include <QDebug>

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QMarginsF>

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
    p->setPen(QPen(Colors::base06, 3));

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

////////////////////////////////////////////////////////////////////////////////

ScriptEditorMoveButton::ScriptEditorMoveButton(QGraphicsItem* parent)
    : ScriptEditorButton(parent)
{
    // Nothing to do here
}

void ScriptEditorMoveButton::paint(QPainter* p,
                                    const QStyleOptionGraphicsItem* o,
                                    QWidget* w)
{
    Q_UNUSED(o);
    Q_UNUSED(w);

    int offset = 6 / 1.41;

    auto br = boundingRect() - QMarginsF(offset, offset, offset, offset);
    p->setPen(QPen(Colors::base06, 2));

    p->drawEllipse(br);
}

void ScriptEditorMoveButton::mousePressEvent(QGraphicsSceneMouseEvent* e)
{
    Q_UNUSED(e);
    // Nothing to do here, just need an event handler to grab the mouse
}

void ScriptEditorMoveButton::mouseMoveEvent(QGraphicsSceneMouseEvent* e)
{
    auto p = dynamic_cast<QGraphicsItem*>(parentObject());
    p->setPos(p->pos() + e->pos() - e->lastPos());
}

/*
 *

    */

#ifndef MULTILINE_H
#define MULTILINE_H

#include <QGraphicsObject>
#include <QPointer>

#include "control/control.h"

class MultiLineControl : public Control
{
    Q_OBJECT
public:
    explicit MultiLineControl(Canvas* canvas, Node* node,
                              QGraphicsItem* parent=0);

    virtual QPainterPath shape() const;

    virtual QRectF bounds() const override;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;

protected:
    /** Generates a painter path (in scene coordinates)
     */
    QPainterPath path() const;

    /** Returns a set of lines to draw (in scene coordinates).
     *
     *  Must be overloaded by child classes.
     */
    virtual QVector<QVector<QVector3D>> lines() const=0;
};


#endif // MULTILINE_H

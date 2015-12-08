#pragma once

#include <QGraphicsObject>

class BaseConnection : public QGraphicsObject
{
public:
    explicit BaseConnection(QColor color);
    virtual ~BaseConnection() {}

protected:
    /*
     *  Returns starting and ending position in scene coordinates.
     *  (overloaded by derived classes)
     */
    virtual QPointF startPos() const=0;
    virtual QPointF endPos() const=0;

    /*
     *  Overloaded color function to change color on highlight, etc
     */
    virtual QColor color() const;

    /*
     *  Overloaded QGraphicsItem functions
     */
    QRectF boundingRect() const override;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;
    QPainterPath shape() const override;

    /*
     *  Overload hover events to set the hover flag
     */
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    /*
     *  Returns a path for the connection.
     *
     *  If only_bezier is true, return only the curving section of the path
     *  (used in selection to avoid paths being drawn over ports)
     */
    QPainterPath path(bool only_bezier=false) const;

    const QColor base_color;
    bool hover;
};

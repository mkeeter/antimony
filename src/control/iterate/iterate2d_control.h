#ifndef ITERATE2D_CONTROL_H
#define ITERATE2D_CONTROL_H

#include <QVector3D>

#include "control/wireframe.h"
#include "ui/colors.h"

class Iterate2DButton : public WireframeControl
{
public:
    explicit Iterate2DButton(Canvas* canvas, Node* node, bool axis,
                             bool sign, QGraphicsItem* parent);

protected:

    /** Dummy drag operation (buttons can't be dragged).
     */
    void drag(QVector3D, QVector3D) override {}

    /** On left-click, increment iteration count.
     */
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

    /** Overload paint to draw + and - buttons over circle.
     */
    void paintControl(QPainter* painter) override;

    /** Draw a single point at button's position.
     */
    QVector<QPair<QVector3D, float>> points() const override;

    /** Special case: buttons cannot be selected.
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent*)
        { ungrabMouse(); }

    /** Special case: buttons do nothing on double-click
     */
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* e)
        { mousePressEvent(e); }

    /** Return the position (in 3D) of this button.
     */
    QVector3D position() const;

    QColor defaultPenColor() const
        { return Colors::teal; }

    bool axis;
    bool sign;
};

class Iterate2DHandle : public WireframeControl
{
public:
    explicit Iterate2DHandle(Canvas* canvas, Node* node, bool dir,
                             QGraphicsItem* parent);

protected:
    void drag(QVector3D center, QVector3D delta) override;
    QVector<QVector<QVector3D>> lines() const override;

    QColor defaultPenColor() const
        { return Colors::teal; }

    bool dir;
};

////////////////////////////////////////////////////////////////////////////////

class Iterate2DControl : public WireframeControl
{
public:
    explicit Iterate2DControl(Canvas* canvas, Node* node);

protected:
    void drag(QVector3D center, QVector3D delta) override;
    QPointF inspectorPosition() const override;
    QVector<QVector<QVector3D>> lines() const override;
    QVector<QPair<QVector3D, float>> points() const override;

    QVector3D position() const;

    QColor defaultPenColor() const
        { return Colors::teal; }

    Iterate2DHandle* handle_x;
    Iterate2DHandle* handle_y;
};

#endif

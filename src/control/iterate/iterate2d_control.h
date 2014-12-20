#ifndef ITERATE2D_CONTROL_H
#define ITERATE2D_CONTROL_H

#include <QVector3D>

#include "control/wireframe.h"
#include "ui/util/colors.h"

class Iterate2DButton : public WireframeControl
{
public:
    explicit Iterate2DButton(Node* node, bool axis, bool sign,
                             QObject* parent);

protected:

    /** Dummy drag operation (buttons can't be dragged).
     */
    void drag(QVector3D, QVector3D) override {}

    /** On left-click, increment iteration count.
     */
    bool onClick() override;

    /** Overload paint to draw + and - buttons over circle.
     */
    void paint(QMatrix4x4 m, QMatrix4x4 t,
               bool highlight, QPainter* painter) override;

    /** Draw a single point at button's position.
     */
    QVector<QPair<QVector3D, float>> points() const override;

    /** Return the position (in 3D) of this button.
     */
    QVector3D position() const;

    QColor defaultPenColor() const
        { return Colors::teal; }

    bool axis;
    bool sign;
};

////////////////////////////////////////////////////////////////////////////////

class Iterate2DHandle : public WireframeControl
{
public:
    explicit Iterate2DHandle(Node* node, bool dir,
                             QObject* parent);

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
    explicit Iterate2DControl(Node* node, QObject* parent=0);

protected:
    void drag(QVector3D center, QVector3D delta) override;
    QVector<QVector<QVector3D>> lines() const override;
    QVector<QPair<QVector3D, float>> points() const override;

    QVector3D position() const;

    QColor defaultPenColor() const
        { return Colors::teal; }

    Iterate2DHandle* handle_x;
    Iterate2DHandle* handle_y;
};

#endif

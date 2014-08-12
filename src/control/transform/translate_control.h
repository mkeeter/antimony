#ifndef TRANSLATE_CONTROL_H
#define TRANSLATE_CONTROL_H

#include "control/wireframe.h"
#include "ui/colors.h"

class TranslateHandle : public WireframeControl
{
public:
    explicit TranslateHandle(Canvas* canvas, Node* node, QGraphicsItem* parent);
    void drag(QVector3D center, QVector3D delta) override;
    QVector<QPair<QVector3D, float>> points() const override;
protected:
    QColor defaultPenColor() const
        { return Colors::teal; }
    QVector3D position() const;
};

class TranslateControl : public WireframeControl
{
public:
    explicit TranslateControl(Canvas* canvas, Node* node);

    void drag(QVector3D center, QVector3D delta) override;
    QPointF inspectorPosition() const override;
    QVector<QVector<QVector3D>> lines() const override;
    QVector<QPair<QVector3D, float>> points() const override;

protected:
    QVector3D position() const;
    QColor defaultPenColor() const
        { return Colors::teal; }

    TranslateHandle* handle;
};

#endif


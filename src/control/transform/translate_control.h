#ifndef TRANSLATE_CONTROL_H
#define TRANSLATE_CONTROL_H

#include "control/wireframe.h"
#include "ui/util/colors.h"

class _TranslateHandle : public WireframeControl
{
public:
    explicit _TranslateHandle(Node* node, QObject* parent);
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
    explicit TranslateControl(Node* node, QObject* parent=NULL);

    void drag(QVector3D center, QVector3D delta) override;
    QVector<QVector<QVector3D>> lines() const override;
    QVector<QPair<QVector3D, float>> points() const override;

protected:
    QVector3D position() const;
    QColor defaultPenColor() const
        { return Colors::teal; }

    _TranslateHandle* handle;
};

#endif


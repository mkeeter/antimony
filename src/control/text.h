#ifndef TEXT_H
#define TEXT_H

#include <QFont>
#include <QFontMetrics>

#include "control/control.h"

class TextLabelControl : public Control
{
public:
    explicit TextLabelControl(QString label, QColor color, Canvas* canvas, Node* node);

    void drag(QVector3D center, QVector3D delta) override;
    QPointF inspectorPosition() const override;
    QRectF bounds() const override;

    void paintControl(QPainter *painter) override;
protected:
    virtual QString getLabel() const { return label; }
    QPointF baseInputPosition() const override;
    QPointF baseOutputPosition() const override;

    QVector3D position() const;

    QString label;
    QColor color;
    QFont font;
    QFontMetrics font_metrics;
};

#endif // TEXT_H

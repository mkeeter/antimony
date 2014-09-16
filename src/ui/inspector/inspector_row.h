#ifndef INSPECTOR_ROW_H
#define INSPECTOR_ROW_H

#include <QGraphicsObject>

class Datum;
class InputPort;
class OutputPort;
class NodeInspector;

class InspectorRow : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit InspectorRow(Datum* d, NodeInspector* parent);

    InputPort* input;
    OutputPort* output;
    QGraphicsTextItem* label;
    QGraphicsTextItem* editor;
    QRectF boundingRect() const;

public slots:
    void updateLayout();

signals:
    void layoutChanged();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;
    float globalLabelWidth() const;

    friend class NodeInspector;
};

#endif // INSPECTOR_ROW_H

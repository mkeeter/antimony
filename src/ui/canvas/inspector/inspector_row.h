#ifndef INSPECTOR_ROW_H
#define INSPECTOR_ROW_H

#include <QGraphicsObject>

class Datum;
class DatumTextItem;
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
    DatumTextItem* editor;
    QRectF boundingRect() const;

    void setWidth(float width);
    float minWidth() const;

public slots:
    /*
     *  Updates the row's layout.
     *  Returns true if anything changed.
     */
    bool updateLayout();

signals:
    void layoutChanged();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;
    float labelWidth() const;

    friend class NodeInspector;
};

#endif // INSPECTOR_ROW_H

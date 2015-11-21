#pragma once

#include <QGraphicsObject>

class InputPort;
class OutputPort;
class InspectorFrame;

typedef QGraphicsTextItem InspectorDatumText;

class InspectorRow : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit InspectorRow(QString name, InspectorFrame* parent);

    /*
     *  Updates the datum's text field
     */
    void setText(QString t);

    /*
     *  Returns the width of the datum's label
     */
    float labelWidth() const;

    /*
     *  Pads the label to the given width
     */
    void padLabel(float width);

    /*
     *  Sets the row's width
     */
    void setWidth(float width);

    /*
     *  Returns the minimum width (with the given label padding
     *  but no text-field padding)
     */
    float minWidth() const;

    /*
     *  Getters and setters for index (i.e. datum order)
     */
    int getIndex() const { return index; }
    void setIndex(int i) { index = i; }

    /*
     *  Overloaded QGraphicsItem functions
     */
    QRectF boundingRect() const override;

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

    /*
     *  Parameters used for layout
     */
    static const float PORT_SIZE;
    static const float GAP_PADDING;
    static const float TEXT_WIDTH;

    InputPort* input;
    OutputPort* output;
    QGraphicsTextItem* label;
    InspectorDatumText* editor;

    int index;
};

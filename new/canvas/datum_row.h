#pragma once

#include <QGraphicsObject>

#include "graph/watchers.h"

class InputPort;
class OutputPort;
class InspectorFrame;

class DatumEditor;
class Datum;

class DatumRow : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit DatumRow(Datum* d, QGraphicsItem* parent);
    explicit DatumRow(Datum* d, InspectorFrame* parent);

    /*
     *  Updates the datum's text field
     */
    void setText(QString t, QString error);

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

    /*
     *  Updates from the given state
     */
    void update(const DatumState& state);

    /*
     *  Returns true if this row's name begins with '_'
     */
    bool shouldBeHidden() const;

signals:
    /*
     *  Emitted when the parent (be it an inspector or a superdatum frame)
     *  should redo its layout.
     */
    void layoutChanged();

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
    DatumEditor* editor;

    int index;
};

#pragma once

#include <QGraphicsObject>

class Node;
class InspectorTitle;

class InspectorFrame : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit InspectorFrame(Node* node, QGraphicsScene* scene);

    /*
     *  Overloaded QGraphicsObject functions
     */
    QRectF boundingRect() const override;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

    /*
     *  Set color of name field
     */
    void setNameValid(bool valid);

    /*
     *  Callback from Python to set the title field.
     */
    void setTitle(QString title);

    /*
     *  Returns the title row
     *  (used to insert buttons)
     */
    InspectorTitle* getTitleRow() const { return title_row; }

public slots:
    /*
     *  Update layout of text labels and fields
     */
    void redoLayout();

protected:
    InspectorTitle* title_row;
};

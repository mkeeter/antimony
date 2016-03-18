#pragma once

#include <QGraphicsObject>

class Node;
class InspectorFrame;
class InspectorButton;

class InspectorTitle : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit InspectorTitle(Node* n, InspectorFrame* parent);

    /*
     *  Override QGraphicsObject functions
     */
    QRectF boundingRect() const override;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

    /*
     *  Update border of name field
     */
    void setNameValid(bool valid);

    /*
     *  Update text of title field
     */
    void setTitle(QString title);

    /*
     *  Returns the minimum width of this row
     *  (used in inspector layout)
     */
    float minWidth() const;

    /*
     *  Sets the row's width
     */
    void setWidth(float width);

    /*
     *  Installs a new button and updates layout
     */
    void addButton(InspectorButton* b);

signals:
    /*
     *  Emitted when the layout has changed
     *  (which may cause the parent InspectorFrame to redo layout)
     */
    void layoutChanged();

protected:
    static int TOP_PADDING;
    static int MIN_TITLE_PADDING;
    static int BUTTON_PADDING;

    QGraphicsTextItem* name;
    QGraphicsTextItem* title;
    QList<InspectorButton*> buttons;

    /*
     *  Represents padding between name and title fields
     */
    int title_padding;
};

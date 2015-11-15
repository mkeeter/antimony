#pragma once

#include <QGraphicsObject>

class Node;
class InspectorFrame;

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
     *  Update color of script button
     */
    void setScriptValid(bool valid);

    /*
     *  Update text of title field
     */
    void setTitle(QString title);

    /*
     *  Returns the minimum width of this row
     *  (used in inspector layout)
     */
    float minWidth() const;

public slots:
    /*
     *  When the name field is edited, call node->setName
     */
    //void onNameChanged();

protected:
    static int MIN_TITLE_PADDING;
    static int BUTTON_PADDING;

    QGraphicsTextItem* name;
    QGraphicsTextItem* title;

    /*
     *  Represents padding between name and title fields
     */
    int title_padding;
};

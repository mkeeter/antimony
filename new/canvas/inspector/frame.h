#pragma once

#include <QGraphicsObject>

class Node;
class InspectorTitle;
class InspectorExportButton;
class ExportWorker;

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

    /*
     *  Get the node object
     */
    Node* getNode() const { return node; }

    /*
     *  Clears the inspector's 'Export' button
     */
    void clearExportWorker();

    /*
     *  Assigns the given export worker to the inspector's 'Export' button
     */
    void setExportWorker(ExportWorker* worker);

public slots:
    /*
     *  Update layout of text labels and fields
     */
    void redoLayout();

protected:
    /*
     *  On mouse move, fake the left button being held down.
     */
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

    /*
     *  On mouse press, open a 'jump to node' menu
     */
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;

    /*
     *  On mouse move, fake the left button being held down.
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

    /*
     *  On hover, set glow to true.
     */
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;

    /*
     *  On hover, set glow to true.
     */
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

    static const float PADDING_ROWS;

    Node* const node;
    InspectorTitle* title_row;
    InspectorExportButton* export_button;

    // Ugly hack because simply grabbing the mouse doesn't set up all of the
    // magic that QGraphicsScene uses to drag items: upon first insertion,
    // set this flag to true (then overload mouseMoveEvent to work correctly)
    bool dragging;
};

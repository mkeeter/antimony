#pragma once

#include <QGraphicsObject>

class Node;
class InspectorTitle;
class InspectorExportButton;
class ExportWorker;
class DatumEditor;
class DatumRow;

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
     *  Sets whether hidden datums should be drawn or not
     *  Triggers a layout update if a change is made.
     */
    void setShowHidden(bool h);

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

    /*
     *  Sets the dragging flag
     *  (should only be called on first insertion, see comment
     *   below for why this is necessary)
     */
    void setDragging(bool d) { dragging = d; }

signals:
    /*
     *  Emitted when focus changes
     */
    void onFocus(bool focus);

    /*
     *  Emitted when the "zoom to" action is used
     */
    void onZoomTo();

public slots:
    /*
     *  Update layout of text labels and fields
     */
    void redoLayout();

    /*
     *  Sets whether the inspector is highlighted
     */
    void setFocus(bool focus);

    /*
     *  Skips to the next or previous datum editor
     */
    void focusNext(DatumEditor* d);
    void focusPrev(DatumEditor* d);

protected:
    /*
     *  On mouse move, fake the left button being held down.
     */
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

    /*
     *  On mouse press, open a 'jump to node' menu
     */
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;

    /*
     *  On hover, set glow to true.
     */
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

    /*
     *  Returns the tight bounding rectangle (not including focus border)
     */
    QRectF tightBoundingRect() const;

    /*
     *  Returns an ordered list of visible DatumRows
     */
    QList<DatumRow*> visibleRows() const;

    static const float PADDING_ROWS;

    Node* const node;
    InspectorTitle* title_row;
    InspectorExportButton* export_button;

    bool show_hidden;

    /*  Set to true when we should highlight the inspector  */
    bool has_focus=false;

    // Ugly hack because simply grabbing the mouse doesn't set up all of the
    // magic that QGraphicsScene uses to drag items: upon first insertion,
    // set this flag to true (then overload mouseMoveEvent to work correctly)
    bool dragging;
};

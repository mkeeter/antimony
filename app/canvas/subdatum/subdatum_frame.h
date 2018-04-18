#pragma once

#include <Python.h>
#include <QGraphicsObject>

#include "graph/watchers.h"

class Datum;
class SubdatumRow;

class SubdatumFrame : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit SubdatumFrame(Datum* d, QGraphicsScene* scene);

    /*
     *  Overloaded QGraphicsObject functions
     */
    QRectF boundingRect() const override;
    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

    /*
     *  Update port visibility and editor state
     */
    void update(const DatumState& state);

    /*
     *  Returns the datum associated with this subdatum frame
     */
    Datum* getDatum() const { return datum; }

    /*
     *  Returns the datum row associated with this frame
     */
    SubdatumRow* getRow() const { return datum_row; }

    /*
     *  Sets the dragging flag
     *  (should only be called on first insertion, see comment
     *   below for why this is necessary)
     */
    void setDragging(bool d) { dragging = d; }

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
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

    Datum* const datum;
    SubdatumRow* datum_row;

    // Ugly hack because simply grabbing the mouse doesn't set up all of the
    // magic that QGraphicsScene uses to drag items: upon first insertion,
    // set this flag to true (then overload mouseMoveEvent to work correctly)
    bool dragging;

    static const float PADDING;
};

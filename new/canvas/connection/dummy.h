#pragma once

#include <QGraphicsObject>

#include "canvas/connection/base.h"

class OutputPort;
class CanvasScene;

class DummyConnection : public BaseConnection
{
    Q_OBJECT
public:
    explicit DummyConnection(OutputPort* source, CanvasScene* scene);

    QPointF startPos() const override;
    QPointF endPos() const override;

    /*
     *  Set the ending position of the connection, update state, and redraw
     */
    void setDragPos(QPointF p);

protected:
    /*
     *  On mouse move, update the end position and call setDragPos
     */
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

    /*
     *  On mouse release, construct a real Connection if we're on a valid port
     */
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

    /*
     *  Catch spacebar and snap to nearest port when it is pressed
     */
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

    OutputPort* source;
    QPointF end;
};

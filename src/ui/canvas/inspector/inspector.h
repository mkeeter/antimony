#ifndef INSPECTOR_H
#define INSPECTOR_H

#include "graph/node/node.h"

#include <QWidget>
#include <QLineEdit>
#include <QPointer>
#include <QGraphicsObject>

class Datum;
class ScriptDatum;
class Node;
class Canvas;

class InspectorRow;
class InspectorMenuButton;
class InputPort;
class OutputPort;

class DatumTextItem;

////////////////////////////////////////////////////////////////////////////////

class NodeInspector : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit NodeInspector(Node* node);

    QRectF boundingRect() const override;

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

    Node* getNode();

    ScriptDatum* getScriptDatum() const;

    OutputPort* datumOutputPort(Datum *d) const;
    InputPort* datumInputPort(Datum* d) const;

    QPointF datumOutputPosition(Datum* d) const;
    QPointF datumInputPosition(Datum* d) const;

signals:
    void moved();

public slots:
    /** Updates layout of text labels and fields.
     */
    void onLayoutChanged();

    /*
     *  On mouse move, fake the left button being held down.
     */
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

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

    /** When datums are changed, update rows and layout.
     */
    void onDatumsChanged();

    /** Change focus to the next text item.
     */
    void focusNext(DatumTextItem* prev);

    /** Change focus to the previous text item.
     */
    void focusPrev(DatumTextItem* prev);

    void setDragging(bool d) { dragging = d; }

    void setGlow(bool g);

protected:
    /*
     *  On object moved, emit moved signal.
     */
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

    /** Returns the width of the largest label.
     */
    float labelWidth() const;

    /** Fills in the grid from the source node.
     */
    void populateLists(Node* node);

    QPointer<Node> node;
    QMap<Datum*, InspectorRow*> rows;
    DatumTextItem* name;
    QGraphicsTextItem* title;
    InspectorMenuButton* menu_button;

    // Ugly hack because simply grabbing the mouse doesn't set up all of the
    // magic that QGraphicsScene uses to drag items: upon first insertion,
    // set this flag to true (then overload mouseMoveEvent to work correctly)
    bool dragging;

    // Extra padding around the inspector (for glowing effect)
    int border;

    // Boolean to determine whether to draw glow
    bool glow;

    friend class InspectorRow;
};

#endif // INSPECTOR_H

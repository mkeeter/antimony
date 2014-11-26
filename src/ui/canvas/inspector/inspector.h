#ifndef INSPECTOR_H
#define INSPECTOR_H

#include "graph/node/node.h"

#include <QWidget>
#include <QLineEdit>
#include <QPointer>
#include <QGraphicsObject>

class Datum;
class Node;
class Canvas;

class InspectorRow;
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

    OutputPort* datumOutputPort(Datum *d) const;
    InputPort* datumInputPort(Datum* d) const;

    QPointF datumOutputPosition(Datum* d) const;
    QPointF datumInputPosition(Datum* d) const;

signals:
    void portPositionChanged();

public slots:
    /** Updates layout of text labels and fields.
     */
    void onLayoutChanged();

#if 0
    /** Opens the script editor for the given datum
     *  (which must be a ScriptDatum).
     */
    void openScript(Datum* d) const;
#endif

    /** When datums are changed, update rows and layout.
     */
    void onDatumsChanged();

    /** Change focus to the next text item.
     */
    void focusNext(DatumTextItem* prev);

    /** Change focus to the previous text item.
     */
    void focusPrev(DatumTextItem* prev);

protected:
    /** On delete or backspace, delete node.
     */
    void keyPressEvent(QKeyEvent* event) override;

    /** Returns the width of the largest label.
     */
    float labelWidth() const;

    /** Fills in the grid from the source node.
     */
    void populateLists(Node* node);

    QPointer<Node> node;
    QMap<Datum*, InspectorRow*> rows;
    QGraphicsTextItem* title;

    friend class InspectorRow;
};

#endif // INSPECTOR_H

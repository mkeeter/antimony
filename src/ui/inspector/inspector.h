#ifndef INSPECTOR_H
#define INSPECTOR_H

#include <QWidget>
#include <QLineEdit>
#include <QGraphicsProxyWidget>
#include <QPointer>

class Control;
class Datum;
class Node;
class Canvas;
class InspectorRow;
class InputPort;
class OutputPort;

////////////////////////////////////////////////////////////////////////////////

class NodeInspector : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit NodeInspector(Control* control, bool show_hidden=false);

    QRectF boundingRect() const override;

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

    OutputPort* datumOutputPort(Datum *d) const;
    InputPort* datumInputPort(Datum* d) const;

    float getMaskSize() const;
    void setMaskSize(float m);
    Q_PROPERTY(float mask_size READ getMaskSize WRITE setMaskSize)

signals:
    void portPositionChanged();

public slots:
    /** Updates layout of text labels and fields.
     */
    void onLayoutChanged();

    /** Animates the window sliding open.
     */
    void animateOpen();

    /** Animates the window sliding closed, deleting when done.
     */
    void animateClose();

    /** Change the position of the inspector window.
     */
    void onPositionChange();

    /** Returns a pointer to the Canvas object.
     */
    Canvas* getCanvas() const;

    /** Opens the script editor for the given datum
     *  (which must be a ScriptDatum).
     */
    void openScript(Datum* d) const;

    /** When datums are changed, update rows and layout.
     */
    void onDatumsChanged();
protected:
    /** Returns the width of the largest label.
     */
    float labelWidth() const;

    /** Fills in the grid from the source node.
     */
    void populateLists(Node* node);

    QPointer<Control> control;
    QMap<Datum*, InspectorRow*> rows;
    float mask_size;
    bool show_hidden;

    friend class InspectorRow;
};

#endif // INSPECTOR_H

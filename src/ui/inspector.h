#ifndef INSPECTOR_H
#define INSPECTOR_H

#include <QWidget>
#include <QLineEdit>
#include <QGraphicsProxyWidget>
#include <QPointer>

class Datum;
class Control;
class Node;
class InputPort;
class OutputPort;
class Canvas;

class _DatumTextItem : public QGraphicsTextItem
{
    Q_OBJECT
public:
    _DatumTextItem(Datum* datum, QGraphicsItem* parent);
public slots:
    void onTextChanged();
    void onDatumChanged();
signals:
    void boundsChanged();
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);

    Datum* d;
    QTextDocument* txt;
    QColor background;
    QRectF bbox;
};

class _DatumTextButton : public QGraphicsTextItem
{
    Q_OBJECT
public:
    _DatumTextButton(Datum* datum, QString label, QGraphicsItem* parent);

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);

    Datum* d;
    bool hover;
    QColor background;
};

////////////////////////////////////////////////////////////////////////////////

class NodeInspector : public QGraphicsObject
{
    Q_OBJECT

public:
    explicit NodeInspector(Control* control);

    virtual QRectF boundingRect() const override;

    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;

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

protected:
    /** Returns the width of the largest label.
     */
    float labelWidth() const;

    /** Fills in the grid from the source node.
     */
    void populateLists(Node* node);

    float getMaskSize() const;
    void setMaskSize(float m);
    Q_PROPERTY(float mask_size READ getMaskSize WRITE setMaskSize)

    QPointer<Control> control;
    QList<InputPort*> inputs;
    QList<OutputPort*> outputs;
    QList<QGraphicsTextItem*> labels;
    QList<QGraphicsTextItem*> editors;
    float mask_size;
};

#endif // INSPECTOR_H

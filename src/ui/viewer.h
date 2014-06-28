#ifndef VIEWER_H
#define VIEWER_H

#include <QWidget>
#include <QLineEdit>
#include <QGraphicsProxyWidget>

class Datum;
class Control;
class Node;

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

////////////////////////////////////////////////////////////////////////////////

class NodeViewer : public QGraphicsObject
{
    Q_OBJECT

public:
    explicit NodeViewer(Control* control);

    virtual QRectF boundingRect() const override;

    virtual void paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget) override;

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

    QList<QGraphicsTextItem*> labels;
    QList<_DatumTextItem*> editors;
    float mask_size;
};

#endif // VIEWER_H

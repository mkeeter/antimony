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
    void onLayoutChanged();

protected:
    /** Returns the width of the largest label.
     */
    float labelWidth() const;

    /** Fills in the grid from the source node.
     */
    void populateLists(Node* node);

    QList<QGraphicsTextItem*> labels;
    QList<_DatumTextItem*> editors;
};

#endif // VIEWER_H

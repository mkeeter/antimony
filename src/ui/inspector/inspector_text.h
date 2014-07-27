#ifndef INSPECTOR_TEXT_H
#define INSPECTOR_TEXT_H

#include <QGraphicsTextItem>

class Datum;

class DatumTextItem : public QGraphicsTextItem
{
    Q_OBJECT
public:
    DatumTextItem(Datum* datum, QGraphicsItem* parent);
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
    QColor border;
    QRectF bbox;
};

#endif // INSPECTOR_TEXT_H

#ifndef INSPECTOR_TEXT_H
#define INSPECTOR_TEXT_H

#include <QGraphicsTextItem>

class Datum;

class DatumTextItem : public QGraphicsTextItem
{
    Q_OBJECT
public:
    DatumTextItem(Datum* datum, QGraphicsItem* parent);

    /*
     *  Adjusts formatting so that the text box blends in
     *  with an Inspector title bar
     */
    void setAsTitle();
public slots:
    void onTextChanged();
    void onDatumChanged();
signals:
    void boundsChanged();
    void tabPressed(DatumTextItem* item);
    void shiftTabPressed(DatumTextItem* item);
    void returnPressed();
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);

    /** Filter tab events to shift focus to next text panel on tab.
     */
    bool eventFilter(QObject* obj, QEvent* event);

    Datum* d;
    QTextDocument* txt;
    QColor background;
    QColor foreground;
    QColor border;
    QRectF bbox;
};

#endif // INSPECTOR_TEXT_H

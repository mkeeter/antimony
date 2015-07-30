#ifndef INSPECTOR_TEXT_H
#define INSPECTOR_TEXT_H

#include <QGraphicsTextItem>

#include "graph/watchers.h"

class Datum;

class DatumTextItem : public QGraphicsTextItem, DatumWatcher
{
    Q_OBJECT
public:
    DatumTextItem(Datum* datum, QGraphicsItem* parent);

    void trigger(const DatumState& state) override;

public slots:
    void onTextChanged();

protected slots:
    void onUndoCommandAdded();

signals:
    void boundsChanged();
    void tabPressed(DatumTextItem* item);
    void shiftTabPressed(DatumTextItem* item);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

    /** Filter tab events to shift focus to next text panel on tab.
     */
    bool eventFilter(QObject* obj, QEvent* event) override;

    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

    void dragFloat(float a);
    void dragInt(int a);

    Datum* d;
    QTextDocument* txt;
    QColor background;
    QColor foreground;
    QColor border;
    QRectF bbox;

    QString drag_start;
    float drag_accumulated;
    bool recursing;
};

#endif // INSPECTOR_TEXT_H

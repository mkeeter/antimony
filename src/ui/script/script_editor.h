#ifndef SCRIPT_H
#define SCRIPT_H

#include <QGraphicsTextItem>
#include <QPointer>

class ScriptDatum;
class Canvas;

class ScriptEditorItem : public QGraphicsTextItem
{
    Q_OBJECT
public:
    explicit ScriptEditorItem(ScriptDatum* datum, Canvas* canvas);

    void setDatum(ScriptDatum* d);

public slots:
    void onTextChanged();
    void onDatumChanged();

protected:
    QRectF boundingRect() const override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

    void paintButtons(QPainter* p);

    /** Handle mouse clicks by preparing to drag or closing.
     */
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    /** On mouse drag, call the virtual function drag.
     */
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    QRectF moveButton() const;
    QRectF closeButton() const;

    void highlightError(QPainter* p, int lineno);

    QRectF getLineRect(int lineno) const;


    QPointer<ScriptDatum> datum;
    int border;
};

#endif

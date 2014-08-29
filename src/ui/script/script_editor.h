#ifndef SCRIPT_H
#define SCRIPT_H

#include <QGraphicsTextItem>
#include <QPointer>

class ScriptDatum;
class Canvas;
class ScriptEditorCloseButton;
class ScriptEditorMoveButton;

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

    /** Draws a red rectangle about the input line.
     */
    void highlightError(QPainter* p, int lineno);

    /** Returns a bounding rectangle for the given line.
     */
    QRectF getLineRect(int lineno) const;

    QPointer<ScriptDatum> datum;
    int border;
    ScriptEditorCloseButton* close;
    ScriptEditorMoveButton* move;
};

#endif

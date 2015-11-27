#pragma once

#include <Python.h>

#include <QGraphicsTextItem>
#include <QRegularExpression>
#include <QToolTip>
#include <QTextCursor>

#include "graph/watchers.h"

#include "undo/undo_catcher.h"
#include "undo/undo_change_expr.h"

class Datum;

class DatumEditor :
    public UndoCatcher<QGraphicsTextItem, QGraphicsItem, Datum, UndoChangeExpr>
{
    Q_OBJECT
public:
    DatumEditor(Datum* d, QGraphicsItem* parent);

    /*
     *  Updates from the given state
     */
    void update(const DatumState& state);

signals:
    void tabPressed(DatumEditor* item);
    void shiftTabPressed(DatumEditor* item);

protected:
    /*
     *  Overload paint to color in background
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override;

    /*
     *  Filter tab events to shift focus to next text panel on tab.
     */
    bool eventFilter(QObject* obj, QEvent* event) override;

    /*
     *  Overload mouse actions to drag numbers with shift + click
     */
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

    /*
     *  Uses regular expressions to truncate floats to sane length
     */
    static QString trimFloat(QString t);

    /*
     *  Formats special datum text (links, outputs, etc)
     */
    static QString formatSpecial(QString t, const DatumState& state);

    /*
     *  Change the datum's value by a small amount
     */
    void tweakValue(int dx);

    /*
     *  Specialized functions to drag different number types
     */
    void dragFloat(float a);
    void dragInt(int a);

    Datum* getUndoTarget() const override { return datum; }

    Datum* datum;
    QTextDocument* txt;

    bool valid;

    QString drag_start;
    float drag_accumulated;
    bool recursing;
};

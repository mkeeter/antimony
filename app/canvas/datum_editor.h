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
     *
     *  Overloaded by derived class to work correctly in subgraph
     */
    virtual void update(const DatumState& state);

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
     *  Sets the datum's text
     *
     *  Overloaded by derived class to work correctly in subgraph
     */
    virtual void setDatumText(QString s);

    /*
     *  Gets datum text
     *
     *  Overloaded by derived class to work correctly in subgraph
     */
    virtual QString getDatumText() const;

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
    virtual QString formatSpecial(QString t, const DatumState& state) const;

    /*
     *  Change the datum's value by a small amount
     */
    void tweakValue(int dx);

    /*
     *  Specialized functions to drag different number types
     */
    void dragFloat(float a);
    void dragInt(int a);

    QTextDocument* txt;

    bool valid;

    QString drag_start;
    float drag_accumulated;
    bool recursing;
};

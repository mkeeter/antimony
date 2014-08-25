#include <Python.h>

#include <QDebug>

#include <QPainter>
#include <QTextDocument>
#include <QTextCursor>
#include <QHelpEvent>
#include <QToolTip>
#include <QMarginsF>
#include <QGraphicsSceneMouseEvent>

#include "datum/script_datum.h"

#include "ui/canvas.h"
#include "ui/script/script_editor.h"
#include "ui/syntax.h"
#include "ui/colors.h"

ScriptEditorItem::ScriptEditorItem(ScriptDatum* datum, Canvas* canvas)
    : QGraphicsTextItem("HELLO WORLD"), datum(datum), border(10)
{
    QFont font;
    font.setFamily("Courier");
    setFont(font);

    canvas->scene->addItem(this);

    new SyntaxHighlighter(document());
    setDefaultTextColor(Colors::base04);

    setTextInteractionFlags(Qt::TextEditorInteraction);

    connect(document(), SIGNAL(contentsChanged()),
            this, SLOT(onTextChanged()));
    connect(datum, SIGNAL(changed()),
            this, SLOT(onDatumChanged()));
    connect(datum, SIGNAL(destroyed()),
            this, SLOT(deleteLater()));

    setPlainText(datum->getExpr());
}

void ScriptEditorItem::onTextChanged()
{
    QToolTip::hideText();
    if (datum)
    {
        datum->setExpr(document()->toPlainText());
    }
}

void ScriptEditorItem::onDatumChanged()
{
    if (datum && !datum->getValid())
    {
        setToolTip(datum->getErrorTraceback());
    }
    else
    {
        QToolTip::hideText();
    }
}

QRectF ScriptEditorItem::boundingRect() const
{
    auto br = QGraphicsTextItem::boundingRect();
    br.setBottom(br.bottom() + border);
    br.setTop(br.top() - border);
    br.setLeft(br.left() - border);
    br.setRight(br.right() + border);
    return br;
}

void ScriptEditorItem::paint(QPainter* painter,
                             const QStyleOptionGraphicsItem* o,
                             QWidget* w)
{
    painter->setBrush(Colors::base02);
    painter->setPen(Colors::base04);
    painter->drawRect(boundingRect());

    if (datum && datum->getErrorLine() != -1)
    {
        highlightError(painter, datum->getErrorLine());
    }

    QGraphicsTextItem::paint(painter, o, w);
    paintButtons(painter);
}

QRectF ScriptEditorItem::closeButton() const
{
    auto br = boundingRect();
    return QRectF(br.width() - 20 - border, -border, 20, 20);
}

QRectF ScriptEditorItem::moveButton() const
{
    auto br = closeButton();
    br.moveLeft(br.left() - br.width());
    return br;
}

void ScriptEditorItem::paintButtons(QPainter* p)
{
    auto close = closeButton();
    p->setPen(QPen(Colors::base07, 4));

    int offset = 6;
    p->drawLine(close.bottomLeft() + QPointF(offset, -offset),
                close.topRight() - QPointF(offset, -offset));
    p->drawLine(close.topLeft() + QPointF(offset, offset),
                close.bottomRight() - QPointF(offset, offset));

    offset /= 1.41;
    p->setPen(QPen(Colors::base07, 3));
    auto move = moveButton() - QMarginsF(offset, offset, offset, offset);
    p->drawEllipse(move);
}

void ScriptEditorItem::mousePressEvent(QGraphicsSceneMouseEvent* e)
{
    qDebug() << closeButton() << e->pos();
    if (closeButton().contains(e->pos()))
    {
        qDebug() << "Deleting oneself";
        e->accept();
        deleteLater();
    }
    else
    {
        QGraphicsTextItem::mousePressEvent(e);
    }
}

void ScriptEditorItem::mouseMoveEvent(QGraphicsSceneMouseEvent* e)
{
    qDebug() << e->pos();
    QGraphicsTextItem::mouseMoveEvent(e);
}

void ScriptEditorItem::highlightError(QPainter* p, int lineno)
{
    // Fill in the entire line with an error bar
    QColor err = Colors::red;
    err.setAlpha(100);
    p->setBrush(QBrush(err));
    p->setPen(Qt::NoPen);
    p->drawRect(getLineRect(lineno));
}

QRectF ScriptEditorItem::getLineRect(int lineno) const
{
    int line_count = document()->toPlainText().count("\n") + 1;
    QRectF br = QGraphicsTextItem::boundingRect();

    float line_height = br.height() / line_count;
    return QRectF(0, line_height * lineno - 1.5*border, br.width(), line_height);
}

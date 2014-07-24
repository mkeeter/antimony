#include <Python.h>

#include <QPushButton>
#include <QGridLayout>
#include <QScrollBar>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QPainter>
#include <QToolTip>

#include "ui/script.h"
#include "ui/syntax.h"
#include "ui/colors.h"

#include "datum/script_datum.h"

ScriptEditor::ScriptEditor(QWidget *parent) :
    QPlainTextEdit(parent)
{
    QFont font;
    font.setFamily("Courier");
    setFont(font);

    QFontMetrics fm(font);
    setTabStopWidth(fm.width("    "));
    baseWidth = fm.width(" ") * 60;

    setLineWrapMode(NoWrap);

    new SyntaxHighlighter(document());
    QColor background = Colors::base03;
    background.setAlpha(150);
    setStyleSheet(QString(
        "QPlainTextEdit {"
        "    background-color: %1;"
        "    color: %2;"
        "}").arg(background.name(QColor::HexArgb)).arg(Colors::base0.name()));

    horizontalScrollBar()->setStyleSheet("QScrollBar {height:0px;}");
    verticalScrollBar()->setStyleSheet("QScrollBar {width:0px;}");
    setMouseTracking(true);

    makeButtons();
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);

    setWidth(0);
    hide();

    connect(document(), SIGNAL(contentsChanged()),
            this, SLOT(onTextChanged()));
}

void ScriptEditor::makeButtons()
{
    QString ss = QString(
            "QPushButton {"
            "    background-color: %1;"
            "    padding: 4px;"
            "    margin: 0px;"
            "    color: %2;"
            "    border-style: none;"
            "}"

            "QPushButton:hover {"
            "   background-color: %3;"
            "}").arg(Colors::highlight(Colors::base02).name())
                .arg(Colors::highlight(Colors::base1).name())
                .arg(Colors::base01.name());

    QPushButton* close_button = new QPushButton("✖", this);
    close_button->setCursor(Qt::ArrowCursor);
    close_button->setStyleSheet(ss);
    connect(close_button, SIGNAL(clicked()),
            this, SLOT(animateClose()));

    QPushButton* resize_button = new ResizeButton("⇔", this);
    resize_button->setCursor(Qt::SizeHorCursor);
    resize_button->setStyleSheet(ss);

    QGridLayout* grid = new QGridLayout(this);
    grid->setColumnStretch(0, 1);
    grid->setColumnStretch(1, 0);
    grid->setRowStretch(0, 0);
    grid->setRowStretch(1, 0);
    grid->setRowStretch(2, 1);
    grid->addWidget(close_button, 0, 1);
    grid->addWidget(resize_button, 1, 1);

    grid->setContentsMargins(0, 0, 0, 0);
    setLayout(grid);
}

int ScriptEditor::getWidth() const { return width(); }
void ScriptEditor::setWidth(int w) { resize(w, height()); }

void ScriptEditor::setDatum(ScriptDatum *d)
{
    if (datum)
    {
        disconnect(datum, 0, this, 0);
    }
    datum = d;
    document()->setPlainText(d->getExpr());
    if (isHidden())
    {
        animateOpen();
    }
    connect(d, SIGNAL(destroyed()), this, SLOT(animateClose()));
}

void ScriptEditor::onTextChanged()
{
    QToolTip::hideText();
    if (datum)
    {
        datum->setExpr(document()->toPlainText());
    }
}

void ScriptEditor::animateOpen()
{
    QPropertyAnimation* a = new QPropertyAnimation(this, "_width", this);
    a->setDuration(100);
    a->setStartValue(0);
    a->setEndValue(baseWidth);
    show();
    a->start(QPropertyAnimation::DeleteWhenStopped);
}

void ScriptEditor::animateClose()
{
    datum.clear();

    QPropertyAnimation* a = new QPropertyAnimation(this, "_width", this);
    a->setDuration(100);
    a->setStartValue(width());
    a->setEndValue(0);
    a->start(QPropertyAnimation::DeleteWhenStopped);
    connect(a, SIGNAL(finished()),
            this, SLOT(hide()));
    connect(a, SIGNAL(finished()),
            parent(), SLOT(update()));
}

bool ScriptEditor::event(QEvent *event)
{
     if (event->type() == QEvent::ToolTip) {
         QHelpEvent *helpEvent = static_cast<QHelpEvent*>(event);
         if (datum->getErrorLine() != -1 &&
             getLineRect(datum->getErrorLine()).contains(helpEvent->pos()))
         {
             QToolTip::showText(helpEvent->globalPos(), datum->getErrorType());
         } else {
             QToolTip::hideText();
             event->ignore();
         }
         return true;
     }
     return QPlainTextEdit::event(event);
}

void ScriptEditor::paintEvent(QPaintEvent *e)
{
    {
        QPainter p(this->viewport());
        if (datum->getErrorLine() != -1)
        {
            highlightError(&p, datum->getErrorLine());
        }
    }
    QPlainTextEdit::paintEvent(e);
}

QRect ScriptEditor::getLineRect(int lineno) const
{
    // Find the index of the appropriate newline
    QString doc = document()->toPlainText();
    int index = 0;
    while (--lineno)
    {
        index = doc.indexOf("\n", index) + 1;
    }

    QTextCursor c = textCursor();
    c.setPosition(index);
    QRect r = cursorRect(c);
    return QRect(0, r.y(), width(), r.height());
}

void ScriptEditor::highlightError(QPainter *p, int lineno)
{
    // Fill in the entire line with an error bar
    QColor err = Colors::red;
    err.setAlpha(100);
    p->setBrush(QBrush(err));
    p->setPen(Qt::NoPen);
    p->drawRect(getLineRect(lineno));
}

////////////////////////////////////////////////////////////////////////////////

ResizeButton::ResizeButton(const QString &text, QWidget *parent)
    : QPushButton(text, parent), resizing(false)
{
    // Nothing to do here
}

void ResizeButton::mousePressEvent(QMouseEvent *e)
{
    QPushButton::mousePressEvent(e);
    resizing = true;
    mx = e->x();
}

void ResizeButton::mouseReleaseEvent(QMouseEvent *e)
{
    QPushButton::mouseReleaseEvent(e);
    resizing = false;
}

void ResizeButton::mouseMoveEvent(QMouseEvent *e)
{
    if (resizing)
    {
        ScriptEditor* ed = dynamic_cast<ScriptEditor*>(parent());
        ed->setWidth(std::max(40, ed->width() + e->x() - mx));
    }

}

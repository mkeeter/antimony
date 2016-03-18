#include <Python.h>

#include <QPainter>
#include <QStyleOption>
#include <QVBoxLayout>

#include "script/frame.h"
#include "script/editor.h"

#include "app/colors.h"

////////////////////////////////////////////////////////////////////////////////

ScriptFrame::ScriptFrame(Script* script, QWidget* parent)
    : QWidget(parent), editor(new ScriptEditor(script, this)),
      output(new QPlainTextEdit), error(new QPlainTextEdit)
{
    for (auto txt : {output, error})
    {
        txt->document()->setDefaultFont(editor->document()->defaultFont());
        txt->setReadOnly(true);
        txt->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        txt->setStyleSheet(editor->styleSheet());
        txt->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,
                                       QSizePolicy::Fixed));
    }

    setStyleSheet(QString(
            "QWidget { "
            "   background-color: %1;"
            "   border: none"
            "}").arg(Colors::base01.name()));

    auto layout = new QVBoxLayout;
    layout->addWidget(editor);
    layout->addWidget(output);
    layout->addWidget(error);

    layout->setSpacing(10);
    layout->setContentsMargins(20, 0, 20, 0);

    setLayout(layout);
}

////////////////////////////////////////////////////////////////////////////////

void ScriptFrame::resizeEvent(QResizeEvent* event)
{
    resizePanes();
    QWidget::resizeEvent(event);
}

void ScriptFrame::resizePanes()
{
    for (auto txt : {output, error})
    {
        int lines = txt->document()->size().height() + 1;
        QFontMetrics fm(txt->document()->defaultFont());
        txt->setFixedHeight(std::min(height()/3, lines * fm.lineSpacing()));
    }
}

////////////////////////////////////////////////////////////////////////////////

void ScriptFrame::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

////////////////////////////////////////////////////////////////////////////////

void ScriptFrame::setOutput(QString out)
{
    if (out.isEmpty())
    {
        output->hide();
    }
    else
    {
        output->setPlainText(out);
        output->show();
    }

    resizePanes();
}

void ScriptFrame::setError(QString err)
{
    if (err.isEmpty())
    {
        error->hide();
    }
    else
    {
        error->setPlainText(err);
        error->show();
    }

    resizePanes();
}

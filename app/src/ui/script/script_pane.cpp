#include <Python.h>

#include <QVBoxLayout>
#include <QStyleOption>
#include <QPainter>

#include "ui/script/script_pane.h"
#include "ui/util/colors.h"
#include "ui/main_window.h"

#include "graph/script_node.h"
#include "graph/graph.h"

ScriptPane::ScriptPane(ScriptNode* node, QWidget* parent)
    : QWidget(parent), node(node), graph(node->parentGraph()),
      editor(new ScriptEditor(node, this)),
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

    node->installScriptWatcher(this);
    graph->installWatcher(this);

    setLayout(layout);
    trigger(node->getScriptState());
}

ScriptPane::~ScriptPane()
{
    if (node)
        node->uninstallScriptWatcher(this);
    graph->uninstallWatcher(this);
}

void ScriptPane::trigger(const ScriptState& state)
{
    editor->trigger(state);

    QString o = QString::fromStdString(state.output);
    if (o.isEmpty())
    {
        output->hide();
    }
    else
    {
        output->setPlainText(o);
        output->show();
    }

    QString e = QString::fromStdString(state.error);
    if (e.isEmpty())
    {
        error->hide();
    }
    else
    {
        error->setPlainText(e);
        error->show();
    }

    resizePanes();
}

void ScriptPane::trigger(const GraphState& state)
{
    if (state.nodes.count(node) == 0)
    {
        node = NULL;
        static_cast<MainWindow*>(parent())->close();
    }
}

void ScriptPane::customizeUI(Ui::MainWindow* ui)
{
    editor->customizeUI(ui);
}

void ScriptPane::resizeEvent(QResizeEvent* event)
{
    resizePanes();
    QWidget::resizeEvent(event);
}

void ScriptPane::resizePanes()
{
    for (auto txt : {output, error})
    {
        int lines = txt->document()->size().height() + 1;
        QFontMetrics fm(txt->document()->defaultFont());
        txt->setFixedHeight(std::min(height()/3, lines * fm.lineSpacing()));
    }
}

void ScriptPane::paintEvent(QPaintEvent* event)
 {
     Q_UNUSED(event);

     QStyleOption opt;
     opt.init(this);
     QPainter p(this);
     style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
 }

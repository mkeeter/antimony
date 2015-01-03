#include <Python.h>
#include <QVBoxLayout>

#include "graph/datum/datums/script_datum.h"
#include "ui/script/script_pane.h"

ScriptPane::ScriptPane(ScriptDatum* datum, QWidget* parent)
    : QWidget(parent), d(datum), editor(new ScriptEditor(datum, this)),
      output(new QPlainTextEdit), error(new QPlainTextEdit)
{
    auto layout = new QVBoxLayout;
    layout->addWidget(editor);
    layout->addWidget(output);
    layout->addWidget(error);

    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    connect(datum, &ScriptDatum::changed,
            this, &ScriptPane::onDatumChanged);

    setLayout(layout);
}

void ScriptPane::setupUI(Ui::MainWindow* ui)
{
    editor->setupUI(ui);
}

void ScriptPane::onDatumChanged()
{
    QString o = d->getOutput();
    if (o.isEmpty())
    {
        output->hide();
    }
    else
    {
        output->setPlainText(o);
        output->show();
    }

    QString e = d->getErrorTraceback();
    if (e.isEmpty())
    {
        error->hide();
    }
    else
    {
        error->setPlainText(e);
        error->show();
    }
}

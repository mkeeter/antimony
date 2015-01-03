#ifndef SCRIPT_PANE_H
#define SCRIPT_PANE_H

#include <QPlainTextEdit>
#include "ui/script/editor.h"

namespace Ui { class MainWindow; }

class ScriptPane : public QWidget
{
    Q_OBJECT
public:
    ScriptPane(ScriptDatum* datum, QWidget* parent);

    /*
     *  Connect to appropriate UI actions and modify menus.
     */
    void setupUI(Ui::MainWindow* ui);

protected slots:
    void onDatumChanged();

protected:
    ScriptDatum* d;

    ScriptEditor* editor;
    QPlainTextEdit* output;
    QPlainTextEdit* error;
};

#endif

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
    void customizeUI(Ui::MainWindow* ui);

    /*
     *  Returns the target datum.
     */
    ScriptDatum* getDatum() const { return d; }

    /*
     *  Override paint event so that we can style the widget with CSS.
     */
    void paintEvent(QPaintEvent* event) override;

    /*
     *  On resize event, adjust the box sizes for output and errors.
     */
    void resizeEvent(QResizeEvent* event) override;

protected slots:
    void onDatumChanged();

protected:
    void resizePanes();

    ScriptDatum* d;

    ScriptEditor* editor;
    QPlainTextEdit* output;
    QPlainTextEdit* error;
};

#endif

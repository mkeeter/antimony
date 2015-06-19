#ifndef SCRIPT_H
#define SCRIPT_H

#include <QPlainTextEdit>
#include <QPointer>

class ScriptDatum;
namespace Ui { class MainWindow; }

class ScriptEditor: public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit ScriptEditor(ScriptDatum* datum, QWidget* parent=NULL);

    /*
     *  Connect to appropriate UI actions and modify menus.
     */
    void customizeUI(Ui::MainWindow* ui);

    void setDatum(ScriptDatum* d);

public slots:
    void onTextChanged();
    void onDatumChanged();

protected slots:
    void onUndoCommandAdded();
    void openShapesLibrary();

protected:
    /*
     *  Applies fixed-width font and Python syntax highlighter.
     */
    static void styleEditor(QPlainTextEdit* ed);

    /*
     *  Filter out control+Z events.
     */
    bool eventFilter(QObject* obj, QEvent* event);

    /*
     *  Returns the bounding rectangle of the given line.
     */
    void highlightError(int lineno);

    QPointer<ScriptDatum> datum;
};

#endif

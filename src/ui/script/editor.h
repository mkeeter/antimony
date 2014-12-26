#ifndef SCRIPT_H
#define SCRIPT_H

#include <QPlainTextEdit>
#include <QPointer>

class ScriptDatum;

class ScriptEditor: public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit ScriptEditor(ScriptDatum* datum, QWidget* parent=NULL);
    void setDatum(ScriptDatum* d);

public slots:
    void onTextChanged();
    void onDatumChanged();

protected:
    /*
     *  Returns the bounding rectangle of the given line.
     */
    void highlightError(int lineno);

    QPointer<ScriptDatum> datum;
};

#endif

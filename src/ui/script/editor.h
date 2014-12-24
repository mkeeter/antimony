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
    QPointer<ScriptDatum> datum;
};

#endif

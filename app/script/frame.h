#pragma once

#include <QWidget>
#include <QPlainTextEdit>

struct Script;
class ScriptEditor;

class ScriptFrame : public QWidget
{
public:
    ScriptFrame(Script* script, QWidget* parent=NULL);

    /*
     *  Override paint event so that we can style the widget with CSS.
     */
    void paintEvent(QPaintEvent* event) override;

    /*
     *  On resize event, adjust the box sizes for output and errors.
     */
    void resizeEvent(QResizeEvent* event) override;

    /*
     *  Returns the editor object
     */
    ScriptEditor* getEditor() const { return editor; }

    /*
     *  Fills the output pane and resizes all three panes
     */
    void setOutput(QString out);

    /*
     *  Fills the error pane and resizes all three panes
     */
    void setError(QString err);

protected:
    void resizePanes();

    ScriptEditor* editor;
    QPlainTextEdit* output;
    QPlainTextEdit* error;
};

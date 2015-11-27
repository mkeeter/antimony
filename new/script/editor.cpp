#include <Python.h>

#include <QTextBlock>

#include "script/editor.h"
#include "script/syntax.h"

#include "app/colors.h"
#include "app/app.h"

#include "undo/undo_change_script.h"

#include "graph/script_node.h"

ScriptEditor::ScriptEditor(Script* script, QWidget* parent)
    : UndoCatcher(script->parentNode(), parent)
{
    //  Propagate script changes back to the node
    auto doc = document();
    connect(doc, &QTextDocument::contentsChanged,
            [=](){ script->setText(doc->toPlainText().toStdString()); });

    {   // Use Courier as our default font
        QFont font;
        font.setFamily("Courier");
        QFontMetrics fm(font);
        setTabStopWidth(fm.width("    "));
        document()->setDefaultFont(font);
    }

    // Set up a syntax highlighter and default colors
    new ScriptHighlighter(document());
    setStyleSheet(QString(
        "QPlainTextEdit {"
        "    background-color: %1;"
        "    color: %2;"
        "}").arg(Colors::base00.name())
            .arg(Colors::base04.name()));

    // Disable scroll bar for aesthetics
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

////////////////////////////////////////////////////////////////////////////////

void ScriptEditor::setText(QString text)
{
    if (text != document()->toPlainText())
    {
        // Keep the cursor at the same position in the document
        // (not 100% reliable)
        QTextCursor cursor = textCursor();
        int p = textCursor().position();
        document()->setPlainText(text);

        if (p < text.length())
        {
            cursor.setPosition(p);
            setTextCursor(cursor);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

void ScriptEditor::highlightError(int lineno)
{
    if (lineno == -1)
    {
        setExtraSelections({});
    }
    else
    {
        QTextEdit::ExtraSelection line;

        line.format.setBackground(Colors::dim(Colors::red));
        line.format.setProperty(QTextFormat::FullWidthSelection, true);

        line.cursor = textCursor();
        line.cursor.setPosition(
                document()->findBlockByLineNumber(lineno - 1).position() + 1);
        line.cursor.clearSelection();

        setExtraSelections({line});
    }
}

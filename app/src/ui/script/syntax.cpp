#include <Python.h>

#include "ui/script/syntax.h"
#include "ui/util/colors.h"

SyntaxHighlighter::SyntaxHighlighter(QTextDocument* doc)
    : QSyntaxHighlighter(doc)
{
    PyObject* kwmod = PyImport_ImportModule("keyword");
    PyObject* kwlist = PyObject_GetAttrString(kwmod, "kwlist");

    QList<QString> keywords = {"input", "output", "title", "meta"};

    // Get all of Python's keywords and add them to a list.
    for (int i=0; i < PyList_Size(kwlist); ++i)
    {
        PyObject* kw = PyList_GetItem(kwlist, i);

        wchar_t* w = PyUnicode_AsWideCharString(kw, NULL);
        keywords << QString::fromWCharArray(w);
        PyMem_Free(w);
    }

    Py_DECREF(kwlist);
    Py_DECREF(kwmod);

    // Make rules for all the Python keywords.
    QTextCharFormat kw_format;
    kw_format.setForeground(Colors::green);
    for (auto k : keywords)
        rules << SyntaxRule("\\b" + k + "\\b", kw_format);

    QTextCharFormat quote_format;
    quote_format.setForeground(Colors::brown);
    // Triple-quoted (multiline) strings
    // Single-line triple-quoted string
    rules << SyntaxRule("'''.*?'''", quote_format);
    rules << SyntaxRule("\"\"\".*?\"\"\"", quote_format);
    // Beginning of multiline string
    rules << SyntaxRule("'''.*$", quote_format, BASE, MULTILINE_SINGLE);
    rules << SyntaxRule("\"\"\".*$", quote_format, BASE, MULTILINE_DOUBLE);
    // End of multiline string
    rules << SyntaxRule("^.*'''", quote_format, MULTILINE_SINGLE, BASE);
    rules << SyntaxRule("^.*\"\"\"", quote_format, MULTILINE_DOUBLE, BASE);
    // Inside of multiline string
    rules << SyntaxRule("^.+$", quote_format, MULTILINE_SINGLE, MULTILINE_SINGLE);
    rules << SyntaxRule("^.+$", quote_format, MULTILINE_DOUBLE, MULTILINE_DOUBLE);

    // Regular strings
    rules << SyntaxRule("\".*?\"", quote_format);
    rules << SyntaxRule("'.*?'", quote_format);

    // String that can be prepended to a regex to make it detect negative
    // numbers (but not subtraction).  Note that a closing parenthesis is
    // needed and the desired number is the last match group.
    QString neg = "(^|\\*\\*|[(+\\-=*\\/,\\[])([+\\-\\s]*";

    QTextCharFormat float_format;
    float_format.setForeground(Colors::yellow);
    rules << SyntaxRule(neg + "\\b\\d+\\.\\d*)", float_format);
    rules << SyntaxRule(neg + "\\b\\d+\\.\\d*e\\d+)", float_format);
    rules << SyntaxRule(neg + "\\b\\d+e\\d+)", float_format);

    QTextCharFormat int_format;
    int_format.setForeground(Colors::orange);
    rules << SyntaxRule(neg + "\\b\\d+\\b)", int_format);

    QTextCharFormat comment_format;
    comment_format.setForeground(Colors::base03);

    rules << SyntaxRule("#.*", comment_format);
}

void SyntaxHighlighter::highlightBlock(const QString& text)
{
    int offset = 0;
    int state = previousBlockState();

    while (offset <= text.length())
    {
        int match_start = -1;
        int match_length;
        SyntaxRule rule;

        for (auto r : rules)
        {
            if (r.state_in != state)
                continue;

            auto match = r.regex.match(text, offset);
            if (!match.hasMatch())
                continue;
            auto index = match.lastCapturedIndex();

            if (match_start == -1 || match.capturedStart(index) < match_start)
            {
                match_start = match.capturedStart(index);
                match_length = match.capturedLength(index);
                rule = r;
            }
        }

        if (match_start == -1)
            break;

        setFormat(match_start, match_length, rule.format);
        offset = match_start + match_length;
        state = rule.state_out;
    }

    setCurrentBlockState(state);
}

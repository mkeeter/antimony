#include <Python.h>

#include "ui/syntax.h"
#include "ui/colors.h"

SyntaxHighlighter::SyntaxHighlighter(QTextDocument* doc)
    : QSyntaxHighlighter(doc)
{
    PyObject* kwmod = PyImport_ImportModule("keyword");
    PyObject* kwlist = PyObject_GetAttrString(kwmod, "kwlist");

    QList<QString> keywords = {"input", "output"};

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
    {
        rules << QPair<QRegularExpression, QTextCharFormat>(
                QRegularExpression("\\b" + k + "\\b"),
                kw_format);
    }

    QTextCharFormat quote_format;
    quote_format.setForeground(Colors::brown);
    rules << QPair<QRegularExpression, QTextCharFormat>(
            QRegularExpression("\\\".*\\\""), quote_format);
    rules << QPair<QRegularExpression, QTextCharFormat>(
            QRegularExpression("\\'.*\\'"), quote_format);

    // String that can be prepended to a regex to make it detect negative
    // numbers (but not subtraction).  Note that a closing parenthesis is
    // needed and the desired number is the last match group.
    QString neg = "(^|\\*\\*|[(+\\-=*\\/])([+\\-\\s]*";

    QTextCharFormat int_format;
    int_format.setForeground(Colors::orange);
    rules << QPair<QRegularExpression, QTextCharFormat>(
            QRegularExpression(neg + "\\b\\d+\\b)"), int_format);

    QTextCharFormat float_format;
    float_format.setForeground(Colors::yellow);
    rules << QPair<QRegularExpression, QTextCharFormat>(
            QRegularExpression(neg + "\\b\\d+\\.\\d*)"), float_format);
    rules << QPair<QRegularExpression, QTextCharFormat>(
            QRegularExpression(neg + "\\b\\d+\\.\\d*e\\d+)"), float_format);
    rules << QPair<QRegularExpression, QTextCharFormat>(
            QRegularExpression(neg + "\\b\\d+e\\d+)"), float_format);

    QTextCharFormat comment_format;
    comment_format.setForeground(Colors::base01);

    rules << QPair<QRegularExpression, QTextCharFormat>(
            QRegularExpression("#.*"), comment_format);
}

void SyntaxHighlighter::highlightBlock(const QString& text)
{
    for (auto r : rules)
    {
        auto iter = r.first.globalMatch(text);
        while (iter.hasNext())
        {
            auto match = iter.next();
            auto index = match.lastCapturedIndex();
            QTextCharFormat format = r.second;
            if (dim)
            {
                format.setForeground(
                        Colors::adjust(format.foreground().color(), 0.6));
            }
            setFormat(match.capturedStart(index),
                      match.capturedLength(index),
                      format);
        }
    }
}

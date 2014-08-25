#include <Python.h>

#include "ui/syntax.h"
#include "ui/colors.h"

QList<QPair<QRegExp, QTextCharFormat>> SyntaxHighlighter::rules;

SyntaxHighlighter::SyntaxHighlighter(QTextDocument* doc) :
    QSyntaxHighlighter(doc)
{
    if (rules.length())
    {
        return;
    }

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

        Py_DECREF(kw);
    }

    Py_DECREF(kwlist);
    Py_DECREF(kwmod);

    // Make rules for all the Python keywords.
    QTextCharFormat kw_format;
    kw_format.setForeground(Colors::green);
    for (auto k : keywords)
    {
        rules << QPair<QRegExp, QTextCharFormat>(QRegExp("\\b" + k + "\\b"),
                                                 kw_format);
    }

    QTextCharFormat quote_format;
    quote_format.setForeground(Colors::teal);
    rules << QPair<QRegExp, QTextCharFormat>(QRegExp("\\\".*\\\""),
                                             quote_format);
    rules << QPair<QRegExp, QTextCharFormat>(QRegExp("\\'.*\\'"),
                                             quote_format);

    QTextCharFormat num_format;
    num_format.setForeground(Colors::blue);

    rules << QPair<QRegExp, QTextCharFormat>(QRegExp("\\b\\d+"),
                                             num_format);
    rules << QPair<QRegExp, QTextCharFormat>(QRegExp("\\b\\d+\\.\\d*"),
                                             num_format);

    QTextCharFormat comment_format;
    comment_format.setForeground(Colors::base01);

    rules << QPair<QRegExp, QTextCharFormat>(QRegExp("#.*"),
                                             comment_format);
}

void SyntaxHighlighter::highlightBlock(const QString& text)
{
    for (auto r : rules)
    {
        int index = r.first.indexIn(text);
        while (index >= 0)
        {
            int length = r.first.matchedLength();
            setFormat(index, length, r.second);
            index = r.first.indexIn(text, index + length);
        }
    }
}

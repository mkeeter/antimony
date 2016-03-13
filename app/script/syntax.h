#pragma once

#include <QSyntaxHighlighter>
#include <QRegularExpression>

class ScriptHighlighter : public QSyntaxHighlighter
{
public:
    explicit ScriptHighlighter(QTextDocument* doc);

protected:
    /*
     *  Function called by the highlighting engine
     */
    void highlightBlock(const QString& text) override;

    /*
     *  Define states to keep track of multiline strings.
     *  (with single quotes ''' or double quotes """)
     */
    enum State { BASE = -1,
                 MULTILINE_SINGLE = 1,
                 MULTILINE_DOUBLE = 2 };

    /*
     *  Structure to use as a rule when highlighting
     */
    struct Rule
    {
        Rule() { /* Nothing to do here */ }
        Rule(QString r, QTextCharFormat f,
             State si=BASE, State so=BASE)
            : regex(QRegularExpression(r)), format(f),
              state_in(si), state_out(so)
        { /* Nothing to do here */ }

        QRegularExpression regex;
        QTextCharFormat format;
        State state_in, state_out;
    };

    QList<Rule> rules;
};

#ifndef SYNTAX_H
#define SYNTAX_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>

enum SyntaxState { BASE = -1, MULTILINE_SINGLE = 1, MULTILINE_DOUBLE = 2 };

struct SyntaxRule
{
    SyntaxRule() { /* Nothing to do here */ }
    SyntaxRule(QString r, QTextCharFormat f,
               SyntaxState si=BASE, SyntaxState so=BASE)
        : regex(QRegularExpression(r)), format(f), state_in(si), state_out(so)
    { /* Nothing to do here */ }

    QRegularExpression regex;
    QTextCharFormat format;
    SyntaxState state_in, state_out;
};

class SyntaxHighlighter : public QSyntaxHighlighter
{
public:
    explicit SyntaxHighlighter(QTextDocument* doc);
    void highlightBlock(const QString& text) override;
protected:
    QList<SyntaxRule> rules;
};

#endif // SYNTAX_H

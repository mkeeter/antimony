#ifndef SYNTAX_H
#define SYNTAX_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>

class SyntaxHighlighter : public QSyntaxHighlighter
{
public:
    explicit SyntaxHighlighter(QTextDocument* doc);
    void highlightBlock(const QString& text) override;
    bool dim;
protected:
    QList<QPair<QRegularExpression, QTextCharFormat>> rules;
};

#endif // SYNTAX_H

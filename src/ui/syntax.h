#ifndef SYNTAX_H
#define SYNTAX_H

#include <QSyntaxHighlighter>

class SyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit SyntaxHighlighter(QTextDocument* doc);
    void highlightBlock(const QString& text) override;
protected:
    QList<QPair<QRegExp, QTextCharFormat>> rules;
};

#endif // SYNTAX_H

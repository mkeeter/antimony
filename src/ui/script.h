#ifndef SCRIPT_H
#define SCRIPT_H

#include <QPlainTextEdit>
#include <QPushButton>

class ResizeButton : public QPushButton
{
    Q_OBJECT
public:
    explicit ResizeButton(const QString& text, QWidget* parent=0);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
protected:
    bool resizing;
    int mx;
};

////////////////////////////////////////////////////////////////////////////////

class ScriptEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit ScriptEditor(QWidget* parent=0);

    void setWidth(int w);
    int getWidth() const;

    Q_PROPERTY(int _width READ getWidth WRITE setWidth)

public slots:
    void animateOpen();
    void animateClose();

protected:
    void makeButtons();
    int baseWidth;
};

#endif // SCRIPT_H

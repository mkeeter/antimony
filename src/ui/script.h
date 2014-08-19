#ifndef SCRIPT_H
#define SCRIPT_H

#include <QPlainTextEdit>
#include <QPushButton>
#include <QPointer>

class ResizeButton : public QPushButton
{
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

class ScriptDatum;

class ScriptEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit ScriptEditor(QWidget* parent=0);

    // Property to animate width
    void setWidth(int w);
    int getWidth() const;
    Q_PROPERTY(int _width READ getWidth WRITE setWidth)

    /** Sets the given datum, opening the script if hidden.
     */
    void setDatum(ScriptDatum* d);

public slots:
    void animateOpen();
    void animateClose();
    void onTextChanged();

protected:
    /** Overload event to show tooltip on line with error.
     */
    bool event(QEvent *event);

    void paintEvent(QPaintEvent *e) override;

    /** Returns the bounding box of the given line
     *  (indexed starting at 1)
     */
    QRect getLineRect(int lineno) const;

    void highlightError(QPainter* p, int lineno);
    void makeButtons();
    int baseWidth;
    QPointer<ScriptDatum> datum;
};

#endif // SCRIPT_H

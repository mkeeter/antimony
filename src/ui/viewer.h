#ifndef VIEWER_H
#define VIEWER_H

#include <QWidget>
#include <QLineEdit>
#include <QGraphicsProxyWidget>

namespace Ui {
class NodeViewer;
}

class Control;
class Node;

class NodeViewer : public QWidget
{
    Q_OBJECT

public:
    explicit NodeViewer(Control* control);
    ~NodeViewer();

    /* Override paintEvent so that the widget can be styled using qss.
     */
    void paintEvent(QPaintEvent *) override;

public slots:
    /** Redraws the proxy widget
     *  (used to prevent graphical glitches)
     */
    void redrawProxy();

    /** Animates sliding open.
     */
    void animateOpen();

    /** Animates sliding closed, deleting widget at the end.
     */
    void animateClose();

protected:

    /** Fills in the grid from the source node.
     */
    void populateGrid(Node* node);

    float getMaskSize() const;
    void setMaskSize(float m);
    Q_PROPERTY(float mask_size READ getMaskSize WRITE setMaskSize)

    Ui::NodeViewer *ui;
    QGraphicsProxyWidget* proxy;
    float _mask_size;
};

////////////////////////////////////////////////////////////////////////////////

class Datum;

class _DatumLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    _DatumLineEdit(Datum* datum, QWidget* parent);
public slots:
    void onDatumChanged();
};

#endif // VIEWER_H

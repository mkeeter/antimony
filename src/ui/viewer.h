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

protected:

    /** Fills in the grid from the source node.
     */
    void populateGrid(Node* node);

    Ui::NodeViewer *ui;
    QGraphicsProxyWidget* proxy;
};

////////////////////////////////////////////////////////////////////////////////

class Datum;

class _DatumLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    _DatumLineEdit(Datum* datum, QWidget* parent=0);
public slots:
    void onDatumChanged();
};

#endif // VIEWER_H

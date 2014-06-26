#ifndef VIEWER_H
#define VIEWER_H

#include <QWidget>

namespace Ui {
class NodeViewer;
}

class NodeViewer : public QWidget
{
    Q_OBJECT

public:
    explicit NodeViewer(QWidget *parent = 0);
    ~NodeViewer();

private:

    Ui::NodeViewer *ui;
};

#endif // VIEWER_H

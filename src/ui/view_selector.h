#ifndef VIEW_SELECTOR_H
#define VIEW_SELECTOR_H

#include <QWidget>

namespace Ui {
class ViewSelector;
}

class ViewSelector : public QWidget
{
    Q_OBJECT

public:
    explicit ViewSelector(QWidget *parent = 0);
    ~ViewSelector();

private:
    Ui::ViewSelector *ui;
};

#endif // VIEW_SELECTOR_H

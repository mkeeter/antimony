#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

class Canvas;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    /** Creates a canvas widget and adds it to the window.
     */
    void addCanvas();

    /** Sets up main keyboard shortcuts
     *  (because Qt Designer doesn't have a good way to do so)
     */
    void setShortcuts();

    /** Makes a new object of the given class.
     */
    template <class T>
    void createNew();

    /** Adds a particular node to the "Add" menu.
     */
    template <class T>
    void addNodeToAddMenu(QMap<QString, QMenu*> submenus);

    /** Populate a menu with all of the widgets.
     */
    void makeAddMenu();

    Ui::MainWindow *ui;
    Canvas* canvas;

    friend class App;
};

#endif // MAIN_WINDOW_H

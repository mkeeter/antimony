#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

class Canvas;
class ScriptDatum;
class Node;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    /*
     *  Updates menus once the central widget is set.
     */
    void updateMenus();

    /** Populate a menu with all of the widgets.
     */
    void populateMenu(QMenu* menu, bool recenter=true);

    /** Returns True if the Shaded option is checked.
     */
    bool isShaded() const;

private:
    /** Sets up main keyboard shortcuts
     *  (because Qt Designer doesn't have a good way to do so)
     */
    void setShortcuts();

    /** Makes a new object of the given class.
     */
    template <Node* (*f)(float, float, float, float, QObject*), bool recenter>
    void createNew();

    /** Adds a particular node to the "Add" menu.
     */
    template <Node* (*f)(float, float, float, float, QObject*), bool recenter>
    void addNodeToMenu(QString category, QString name, QMenu* menu,
                       QMap<QString, QMenu*>* submenus);

    /** Populate a menu with all of the widgets.
     */
    template <bool recenter>
    void _populateMenu(QMenu* menu);

    Ui::MainWindow *ui;
};

#endif // MAIN_WINDOW_H

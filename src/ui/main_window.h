#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

class Canvas;
class Viewport;
class ScriptDatum;
class Node;
class NodeRoot;
class App;

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
    void populateMenu(QMenu* menu, bool recenter=true, Viewport* v=NULL);

    /** Returns True if the Shaded option is checked.
     */
    bool isShaded() const;

private:
    /*
     *  Connects menu actions to App slots.
     */
    void connectActions(App* app);

    /** Sets up main keyboard shortcuts
     *  (because Qt Designer doesn't have a good way to do so)
     */
    void setShortcuts();

    /*
     *  Makes a new object of the given class.
     *
     *  If recenter is true, snaps object to center of canvas or viewport
     *  If v is given, use it as the viewport in which to add the object
     *      (which enables Shift+A adding objects in quad windows)
     */
    template <Node* (*f)(float, float, float, float, NodeRoot*)>
    void createNew(bool recenter, Viewport* v=NULL);

    /** Adds a particular node to the "Add" menu.
     */
    template <Node* (*f)(float, float, float, float, NodeRoot*)>
    void addNodeToMenu(QString category, QString name, QMenu* menu,
                       QMap<QString, QMenu*>* submenus,
                       bool recenter, Viewport* v=NULL);

    Ui::MainWindow *ui;
};

#endif // MAIN_WINDOW_H

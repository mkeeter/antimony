#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <Python.h>
#include <QMainWindow>

#include "graph/node/constructor.h"

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
    void setCentralWidget(QWidget* w);

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
    void createNew(bool recenter, NodeConstructorFunction f, Viewport* v=NULL);

    /** Adds a particular node to the "Add" menu.
     */
    void addNodeToMenu(QStringList category, QString name, QMenu* menu,
                       bool recenter, NodeConstructorFunction f,
                       Viewport* v=NULL);

    void populateNodeMenu(QMenu* menu, bool recenter=true, Viewport* v=NULL);

    QString window_type;
    Ui::MainWindow *ui;
};

#endif // MAIN_WINDOW_H

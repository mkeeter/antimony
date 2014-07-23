#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

class Canvas;
class ScriptEditor;
class ScriptDatum;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void openScript(ScriptDatum* d);

private:
    /** Creates a canvas widget and adds it to the window.
     */
    void addCanvas();

    /** Create a script editor widget and add it to the window.
     */
    void addScriptEditor();

    /** Sets up main keyboard shortcuts
     *  (because Qt Designer doesn't have a good way to do so)
     */
    void setShortcuts();

    /** Makes a new object of the given class.
     */
    template <class N, class C>
    void createNew();

    /** Adds a particular node to the "Add" menu.
     */
    template <class N, class C>
    void addNodeToMenu(QString category, QString name, QMenu* menu,
                          QMap<QString, QMenu*>* submenus);

    /** Populate a menu with all of the widgets.
     */
    void populateMenu(QMenu* menu);

    Ui::MainWindow *ui;
    Canvas* canvas;
    ScriptEditor* script;

    friend class App;
};

#endif // MAIN_WINDOW_H

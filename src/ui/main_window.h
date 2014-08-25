#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

class Canvas;
class ScriptEditor;
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

    void openScript(ScriptDatum* d);
    Canvas* getCanvas() const { return canvas; }

private:
    /** Sets up main keyboard shortcuts
     *  (because Qt Designer doesn't have a good way to do so)
     */
    void setShortcuts();

    /** Makes a new object of the given class.
     */
    template <Node* (*f)(float, float, float, float, QObject*)>
    void createNew();

    /** Adds a particular node to the "Add" menu.
     */
    template <Node* (*f)(float, float, float, float, QObject*)>
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

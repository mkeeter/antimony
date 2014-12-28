#ifndef APP_H
#define APP_H

#include <QApplication>
#include <QAction>

class GraphScene;
class ViewportScene;
class Node;
class Link;
class Connection;
class ScriptDatum;
class MainWindow;
class NodeRoot;
class UndoStack;
class UndoCommand;

class App : public QApplication
{
    Q_OBJECT
public:
    explicit App(int& argc, char **argv);
    ~App();

    /*
     *  Helper function to get running instance.
     */
    static App* instance();

    /*
     *  Creates UI elements for a new Node and adds them to scenes.
     */
    void newNode(Node* n);

    /*
     *  Create UI elements for a new link, returning the associated Connection.
     */
    Connection* newLink(Link* link);

    /*
     *  Returns the root of the node tree.
     */
    NodeRoot* getNodeRoot() const { return root; }

public slots:
    /*
     *  Opens a new MainWindow with a Canvas as its central widget.
     */
    MainWindow* newCanvasWindow();

    /*
     *  Opens a new MainWindow with a Viewport as its central widget.
     */
    MainWindow* newViewportWindow();

    /*
     *  Opens a new MainWindow with four Viewports.
     */
    MainWindow* newQuadWindow();

    /*
     *  Opens a new editor window targetting the given datum.
     */
    MainWindow* newEditorWindow(ScriptDatum* datum);

    QAction* undoAction();
    QAction* redoAction();

    void pushStack(UndoCommand* c);
    void undo();
    void redo();

public slots:
    void onAbout();

    void onNew();
    void onSave();
    void onSaveAs();
    void onOpen();

    void onExportSTL();
    void onExportHeightmap();
    void onExportJSON();

private:

    void setGlobalStyle();

    GraphScene* graph_scene;
    ViewportScene* view_scene;
    QString filename;
    NodeRoot* root;
    UndoStack* stack;
};

#endif // APP_H

#ifndef APP_H
#define APP_H

#include <QApplication>
#include <QAction>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class GraphScene;
class ViewportScene;

class Graph;
class Node;
class ScriptNode;

class MainWindow;
class UndoStack;
class UndoCommand;

class App : public QApplication
{
    Q_OBJECT
public:
    explicit App(int& argc, char **argv);
    ~App();

    void makeDefaultWindows();

    /*
     *  Helper function to get running instance.
     */
    static App* instance();

    /*
     *  Returns the root of the node tree.
     */
    Graph* getGraph() const { return root; }

    GraphScene* getGraphScene() const { return graph_scene; }
    ViewportScene* getViewScene() const { return view_scene; }

    /*
     *  Returns a window title in the form
     *      antimony [Untitled]*
     *  or
     *      antimony [filename.sb]*
     */
    QString getWindowTitle() const;

    /*
     *  Returns the path to the nodes directory
     *  (which varies depending on OS).
     */
    QString bundledNodePath() const;
    QString userNodePath() const;

    QAction* undoAction();
    QAction* redoAction();

    void pushStack(UndoCommand* c);
    void undo();
    void redo();
    void beginUndoMacro(QString text);
    void endUndoMacro();
    bool isUndoStackClean() const;

    void loadFile(QString f);

signals:
    void windowTitleChanged(QString new_title);
    void jumpToInViewport(Node* node);
    void jumpToInGraph(Node* node);

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
    MainWindow* newEditorWindow(ScriptNode* n);
    void onAbout();

    void onNew();
    void onSave();
    void onSaveAs();
    void onOpen();
    void onQuit();

    void startUpdateCheck();
    void onUpdateCheckFinished(QNetworkReply* reply);

private:
    bool event(QEvent* event);
    void setGlobalStyle();

    Graph* const root;

    GraphScene* const graph_scene;
    ViewportScene* const view_scene;
    QString filename;
    UndoStack* const stack;

    QNetworkAccessManager* const network;
};

#endif // APP_H

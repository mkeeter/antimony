#ifndef APP_H
#define APP_H

#include <QApplication>

class GraphScene;
class ViewportScene;
class Node;
class Link;
class Connection;

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

public slots:
    /*
     *  Opens a new MainWindow with a Canvas as its central widget.
     */
    void newCanvasWindow();

    /*
     *  Opens a new MainWindow with a Viewport as its central widget.
     */
    void newViewportWindow();

private slots:
    void onAbout();
    void onControls();

#if 0
    void onNew();
    void onSave();
    void onSaveAs();
    void onOpen();
    void onExportSTL();
    void onExportHeightmap();
    void onExportJSON();
#endif
private:

    void setGlobalStyle();
    void connectActions();

    GraphScene* graph_scene;
    ViewportScene* view_scene;
    QString filename;
};

#endif // APP_H

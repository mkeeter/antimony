#ifndef APP_H
#define APP_H

#include <QApplication>

class MainWindow;
class GraphScene;
class Node;

class App : public QApplication
{
    Q_OBJECT
public:
    explicit App(int& argc, char **argv);
    ~App();

    GraphScene* getScene() const { return scene; }

    /** Helper function to get running instance.
     */
    static App* instance();

    /*  Creates UI elements for a new Node and adds them to scenes.
     */
    void newNode(Node* n);

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
    /*  Opens a new MainWindow with a Canvas as its central widget.
     */
    void newCanvasWindow();

    void setGlobalStyle();
    void connectActions();

    GraphScene* scene;
    QString filename;
};

#endif // APP_H

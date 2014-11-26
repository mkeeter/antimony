#ifndef APP_H
#define APP_H

#include <QApplication>

class MainWindow;
class Canvas;
class GraphScene;

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
    void newCanvasWindow();

    void setGlobalStyle();
    void connectActions();

    GraphScene* scene;
    QString filename;
};

#endif // APP_H

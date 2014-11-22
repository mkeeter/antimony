#ifndef APP_H
#define APP_H

#include <QApplication>

class MainWindow;
class Canvas;

class App : public QApplication
{
    Q_OBJECT
public:
    explicit App(int& argc, char **argv);
    ~App();

    /** Helper function to get Canvas widget.
     */
    Canvas* getCanvas() const;

    /** Helper function to get main window.
     */
    MainWindow* getWindow() const;

    /** Helper function to get running instance.
     */
    static App* instance();

private slots:
    void onAbout();
    void onControls();
    void onNew();
    void onSave();
    void onSaveAs();
    void onOpen();
    void onExportSTL();
    void onExportHeightmap();
    void onExportJSON();
private:
    void setGlobalStyle();
    void connectActions();

    MainWindow* window;
    QString filename;
};

#endif // APP_H

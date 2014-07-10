#ifndef APP_H
#define APP_H

#include <QApplication>

class MainWindow;
class Canvas;

class App : public QApplication
{
    Q_OBJECT
public:
    explicit App(int argc, char *argv[]);
    ~App();

    /** Helper function to get Canvas widget.
     */
    Canvas* getCanvas() const;

private slots:
    void onAbout();
private:
    void connectActions();

    MainWindow* window;
};

#endif // APP_H

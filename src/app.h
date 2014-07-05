#ifndef APP_H
#define APP_H

#include <QApplication>

class MainWindow;

class App : public QApplication
{
    Q_OBJECT
public:
    explicit App(int argc, char *argv[]);
    ~App();

private slots:
    void onAbout();
private:
    void connectActions();

    MainWindow* window;
};

#endif // APP_H

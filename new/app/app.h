#pragma once

#include <QApplication>

class Graph;
class GraphProxy;

class App : public QApplication
{
    Q_OBJECT
public:
    explicit App(int& argc, char **argv);

    /*
     *  Constructs a canvas and viewport window for the root graph.
     */
    void makeDefaultWindows();

    /*
     *  Helper function to get running instance.
     */
    static App* instance();

    /*
     *  Returns the path to the node directories
     *  (which varies depending on OS).
     */
    QString bundledNodePath() const;
    QString userNodePath() const;

protected:
    Graph* graph;
    GraphProxy* proxy;
};

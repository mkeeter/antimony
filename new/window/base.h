#pragma once

#include <QMainWindow>

#include "ui_base_window.h"

class App;

class BaseWindow : public QMainWindow
{
public:
    /*
     *  Constructs a MainWindow object.
     *
     *  type is a string used in the window's title.
     *  n is a node which this window depends on:
     *    If this is a script window, it's the script's parent node
     *    If this is a canvas or viewport, it's the parent node of the
     *      target subgraph (or NULL if we're using the root graph)
     */
    explicit BaseWindow(QString type);

protected:
    /*
     *  Connects menu actions to App slots.
     */
    void connectActions(App* app);

    /*
     *  Sets up main keyboard shortcuts
     *  (because Qt Designer doesn't have a good way to do so)
     */
    void setShortcuts();

    QString window_type;
    QScopedPointer<Ui::BaseWindow> ui;
};

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
    virtual ~BaseWindow();

    /*
     *  On close attempt, check to see if this is the last window
     */
    void closeEvent(QCloseEvent *event) override;

public slots:
    /*
     *  Adjusts the window's title based on filename
     */
    void setFilename(QString file);

    /*
     *  Sets the nested subgraph name
     */
    void setSub(QString s);

    /*
     *  Adjusts the window's title based on whether the file has been saved
     */
    void setClean(bool changed);

    /*
     *  Closes the window if there are no unsaved changes; asks otherwise
     */
    void tryClose();

    /*
     *  Returns true if the window should close (either because there are other
     *  windows open, there aren't unsaved changes, or the user accepts)
     */
    bool askClose();

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

    /*
     *  Changes the window's title based on filename, type, unsaved
     */
    void updateTitle();

    QString window_type;
    QString filename="";
    QString sub="";
    bool clean=true;

    QScopedPointer<Ui::BaseWindow> ui;

    /*  Global count of windows (used to warn when last closes)  */
    static int window_count;
};

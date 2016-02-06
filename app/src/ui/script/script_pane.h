#ifndef SCRIPT_PANE_H
#define SCRIPT_PANE_H

#include <QPlainTextEdit>
#include "ui/script/editor.h"

#include "graph/watchers.h"

namespace Ui { class MainWindow; }

class ScriptPane : public QWidget, ScriptWatcher, GraphWatcher
{
    Q_OBJECT
public:
    ScriptPane(ScriptNode* node, QWidget* parent);

    /*
     *  The destructor calls uninstallWatcher for itself and the child
     *  editor object (unless node is set to NULL)
     */
    ~ScriptPane();

    /*
     *  Connect to appropriate UI actions and modify menus.
     */
    void customizeUI(Ui::MainWindow* ui);

    /*
     *  On node change, update stdout and error panes.
     */
    void trigger(const ScriptState& state) override;

    /*
     *  On graph change, delete self if the node is gone.
     */
    void trigger(const GraphState& state) override;

    /*
     *  Returns the target datum.
     */
    ScriptNode* getNode() const { return node; }

    /*
     *  Override paint event so that we can style the widget with CSS.
     */
    void paintEvent(QPaintEvent* event) override;

    /*
     *  On resize event, adjust the box sizes for output and errors.
     */
    void resizeEvent(QResizeEvent* event) override;

protected:
    void resizePanes();

    ScriptNode* node;
    Graph* graph;

    ScriptEditor* editor;
    QPlainTextEdit* output;
    QPlainTextEdit* error;
};

#endif

#ifndef SCRIPT_PANE_H
#define SCRIPT_PANE_H

#include <QPlainTextEdit>
#include "ui/script/editor.h"

#include "graph/watchers.h"

namespace Ui { class MainWindow; }

class ScriptPane : public QWidget, NodeWatcher
{
    Q_OBJECT
public:
    ScriptPane(Node* node, QWidget* parent);

    /*
     *  Connect to appropriate UI actions and modify menus.
     */
    void customizeUI(Ui::MainWindow* ui);

    /*
     *  On node change, update stdout and error panes.
     */
    void trigger(const NodeState& state) override;

    /*
     *  Returns the target datum.
     */
    Node* getNode() const { return node; }

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

    Node* node;

    ScriptEditor* editor;
    QPlainTextEdit* output;
    QPlainTextEdit* error;
};

#endif

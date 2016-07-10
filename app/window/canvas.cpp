#include <Python.h>

#include "window/canvas.h"
#include "canvas/scene.h"
#include "canvas/canvas_view.h"

#include "graph/constructor/populate.h"

CanvasWindow::CanvasWindow(CanvasView* view)
    : BaseWindow("Graph")
{
    setCentralWidget(view);

    populateNodeMenu(ui->menuAdd,
            static_cast<CanvasScene*>(view->scene())->getGraph());

    // Connect copy / paste actions
    connect(ui->actionCopy, &QAction::triggered,
            view, &CanvasView::onCopy);
    connect(ui->actionCut, &QAction::triggered,
            view, &CanvasView::onCut);
    connect(ui->actionPaste, &QAction::triggered,
            view, &CanvasView::onPaste);

    ui->menuView->deleteLater();
    ui->menuReference->deleteLater();

    show();
}

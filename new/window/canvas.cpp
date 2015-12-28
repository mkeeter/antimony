#include "window/canvas.h"
#include "canvas/scene.h"
#include "canvas/view.h"

#include "ui_base_window.h"

CanvasWindow::CanvasWindow(CanvasScene* scene)
    : BaseWindow("Graph")
{
    auto view = scene->getView();
    setCentralWidget(view);

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

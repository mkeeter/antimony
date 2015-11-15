#include "window/canvas.h"
#include "canvas/scene.h"
#include "canvas/view.h"

#include "ui_base_window.h"

CanvasWindow::CanvasWindow(CanvasScene* scene)
    : BaseWindow("Graph")
{
    setCentralWidget(scene->getView());

    ui->menuView->deleteLater();
    ui->menuReference->deleteLater();

    show();
}

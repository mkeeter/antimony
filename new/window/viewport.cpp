#include "window/viewport.h"

#include "viewport/scene.h"
#include "viewport/view.h"

#include "ui_base_window.h"

ViewportWindow::ViewportWindow(ViewportScene* scene)
    : BaseWindow("Viewport")
{
    auto view = scene->getView();
    setCentralWidget(view);

    ui->menuAdd->deleteLater();
    ui->menuReference->deleteLater();

    show();
}

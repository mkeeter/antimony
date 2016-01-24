#include "window/viewport.h"

#include "viewport/scene.h"
#include "viewport/view.h"

#include "ui_base_window.h"

ViewportWindow::ViewportWindow(ViewportView* view)
    : BaseWindow("Viewport")
{
    setCentralWidget(view);

    ui->menuAdd->deleteLater();
    ui->menuReference->deleteLater();

    show();
}

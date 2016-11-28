#include "app/app.h"

#include "viewport/scene.h"
#include "viewport/view.h"

#include "window/base_viewport_window.h"

BaseViewportWindow::BaseViewportWindow(QList<ViewportView*> views)
    : BaseWindow("Viewport")
{
    ui->menuAdd->deleteLater();
    ui->menuReference->deleteLater();

    // Make heightmap and shaded options mutually exclusive
    QActionGroup* view_actions = new QActionGroup(this);
    view_actions->addAction(ui->actionShaded);
    view_actions->addAction(ui->actionHeightmap);
    view_actions->setExclusive(true);

    // Accept the global command-line argument '--heightmap'
    // to always open scenes in height-map view.
    if (App::instance()->arguments().contains("--heightmap"))
        ui->actionHeightmap->setChecked(true);

    for (auto view : views)
    {
        connect(ui->actionShaded, &QAction::triggered,
                [=]{ view->scene()->invalidate(); });
        connect(ui->actionHeightmap, &QAction::triggered,
                [=]{ view->scene()->invalidate(); });
        connect(ui->actionHideUI, &QAction::triggered,
                [=](bool b){ view->hideUI(b); });
    }

    show();
}

bool BaseViewportWindow::isShaded() const
{
    return ui->actionShaded->isChecked();
}

#include "window/viewport.h"

#include "viewport/scene.h"
#include "viewport/view.h"

#include "app/app.h"

#include "ui_base_window.h"

ViewportWindow::ViewportWindow(ViewportView* view)
    : BaseWindow("Viewport")
{
    setCentralWidget(view);

    ui->menuAdd->deleteLater();
    ui->menuReference->deleteLater();

    // Make heightmap and shaded optiosn mutually exclusive
    QActionGroup* view_actions = new QActionGroup(this);
    view_actions->addAction(ui->actionShaded);
    view_actions->addAction(ui->actionHeightmap);
    view_actions->setExclusive(true);

    // Accept the global command-line argument '--heightmap'
    // to always open scenes in height-map view.
    if (App::instance()->arguments().contains("--heightmap"))
        ui->actionHeightmap->setChecked(true);

    connect(ui->actionShaded, &QAction::triggered,
            [=]{ view->scene()->invalidate(); });
    connect(ui->actionHeightmap, &QAction::triggered,
            [=]{ view->scene()->invalidate(); });

    show();
}

bool ViewportWindow::isShaded() const
{
    return ui->actionShaded->isChecked();
}

#include "window/viewport.h"
#include "viewport/view.h"

ViewportWindow::ViewportWindow(ViewportView* view)
    : BaseViewportWindow({view})
{
    setCentralWidget(view);
}


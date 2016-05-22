#include <Python.h>

#include "window/quad.h"
#include "viewport/view.h"

#include "app/colors.h"

QuadWindow::QuadWindow(ViewportView* front, ViewportView* top,
                       ViewportView* side,  ViewportView* ortho)
    : BaseViewportWindow({front, top, side, ortho})
{
    for (auto a : {front, top, side})
        for (auto b : {front, top, side})
            if (a != b)
            {
                connect(a, &ViewportView::scaleChanged,
                        b, &ViewportView::setScale);
                connect(a, &ViewportView::centerChanged,
                        b, &ViewportView::setCenter);
            }

    top->lockAngle(0, 0);
    front->lockAngle(0, -M_PI/2);
    side->lockAngle(-M_PI/2, -M_PI/2);

    auto g = new QGridLayout();

    g->addWidget(top, 1, 0);
    g->addWidget(front, 0, 0);
    g->addWidget(side, 0, 1);
    g->addWidget(ortho, 1, 1);
    g->setContentsMargins(0, 0, 0, 0);
    g->setSpacing(2);

    auto w = new QWidget();
    w->setStyleSheet(QString(R"(
                QWidget {
                   background-color: %1;
                }
                QMenu {
                   background-color: none;
                })").arg(Colors::base01.name()));
    w->setLayout(g);

    setCentralWidget(w);
    show();
}

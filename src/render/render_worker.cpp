#include <Python.h>

#include <QDebug>
#include <QTime>

#include "render_worker.h"

RenderWorker::RenderWorker(QObject* parent)
    : QObject(parent)
{
    // Nothing to do here
}

void RenderWorker::render()
{
    qDebug() << "Starting render";

    QTime dieTime= QTime::currentTime().addSecs(1);
    while(QTime::currentTime() < dieTime);

    qDebug() << "Render done";
    emit(finished());
}

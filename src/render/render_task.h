#ifndef RENDER_TASK_H
#define RENDER_TASK_H

#include <QObject>
#include <QPointer>
#include <QThread>

class Datum;
class RenderWorker;

class RenderTask : public QObject
{
    Q_OBJECT
public:
    explicit RenderTask(Datum* datum);
    static bool accepts(Datum* d);
public slots:
    void onDatumChanged();
    void onDatumDeleted();
    void onThreadFinished();

protected:
    /** Starts rendering the task in next.
     *  Moves next to current when starting.
     */
    void startNextRender();

    QPointer<Datum> datum;

    QThread* thread;
    RenderWorker* current;
    RenderWorker* next;

    bool running;
};

#endif // RENDER_TASK_H

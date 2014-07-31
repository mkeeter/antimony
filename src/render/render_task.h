#ifndef RENDER_TASK_H
#define RENDER_TASK_H

#include <QObject>
#include <QPointer>
#include <QThread>

class Datum;
class RenderWorker;
class RenderImage;
class Canvas;

class RenderTask : public QObject
{
    Q_OBJECT
public:
    explicit RenderTask(Datum* datum);
    static bool accepts(Datum* d);
public slots:
    void onDatumChanged();
    void onDatumDeleted();
    void onWorkerFinished();
    void onThreadFinished();

protected:
    /** Checks to see if the datum has output.
     *  If so, returns false and deletes image.
     */
    bool hasNoOutput();

    /** Starts rendering the task in next.
     *  Moves next to current when starting.
     */
    void startNextRender();

    /** Calls deleteLater on image and sets it to NULL.
     */
    void clearImage();

    QPointer<Datum> datum;

    QThread* thread;
    RenderWorker* current;
    RenderWorker* next;
    RenderImage* image;

    bool running;

    Canvas* canvas;
};

#endif // RENDER_TASK_H

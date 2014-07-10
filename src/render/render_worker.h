#ifndef RENDER_WORKER_H
#define RENDER_WORKER_H

#include <QObject>

class RenderWorker : public QObject
{
    Q_OBJECT
public:
    explicit RenderWorker(QObject* parent=0);

public slots:
    void render();
signals:
    void finished();
};

#endif // RENDER_WORKER_H

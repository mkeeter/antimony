#pragma once

#include <list>

template <class T, class S>
class Watched
{
public:
    Watched() : watcher(NULL) {}

    void installWatcher(T* w) {
        assert(watcher == NULL);
        watcher = w;
        watcher->trigger(getState());
    }

    void uninstallWatcher(T* w) {
        (void)w;
        assert(watcher == w);
        watcher = NULL;
    }

    void triggerWatcher() {
        if (watcher != NULL)
            watcher->trigger(getState());
    }

    virtual S getState() const=0;

protected:
    T* watcher;
};

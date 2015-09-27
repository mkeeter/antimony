#pragma once

#include <list>

template <class T, class S>
class Watched
{
public:
    void installWatcher(T* w) {
        watchers.push_back(w);
    }

    void uninstallWatcher(T* w) {
        watchers.remove_if([&](T* w_) { return w_ == w; });
    }

    void triggerWatchers() {
        if (!watchers.empty())
        {
            auto s = getState();
            for (auto w : watchers)
                w->trigger(s);
        }
    }

    virtual S getState() const=0;

protected:
    std::list<T*> watchers;
};

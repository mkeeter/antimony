#ifndef HOOKS_H
#define HOOKS_H

#include <string>

namespace hooks {

    struct HookException
    {
        HookException(std::string m) : message(m) {}
        std::string message;
    };

    void onHookException(const HookException& e);
    void preInit();
}

#endif

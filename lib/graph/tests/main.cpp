#include <Python.h>

#define CATCH_CONFIG_RUNNER
#include <catch/catch.hpp>

#include "graph/proxy.h"
#include "graph/hooks/hooks.h"

int main(int argc, char** argv)
{
    Proxy::preInit();
    Hooks::preInit();
    Py_Initialize();
    return Catch::Session().run( argc, argv );
}

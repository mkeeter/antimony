#include <Python.h>

#define CATCH_CONFIG_RUNNER
#include <catch/catch.hpp>

#include "fab/fab.h"

int main(int argc, char** argv)
{
    fab::preInit();
    Py_Initialize();
    return Catch::Session().run(argc, argv);
}

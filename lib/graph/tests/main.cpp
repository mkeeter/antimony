#include <Python.h>

#include <iostream>

#define CATCH_CONFIG_RUNNER
#include <catch/catch.hpp>

#include "graph/graph.h"
#include "graph/node.h"
#include "graph/datum.h"
#include "graph/proxy.h"

TEST_CASE("Datum evaluation")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    SECTION("Valid")
    {
        auto d1 = new Datum("x", "1.0", &PyFloat_Type, n);
        REQUIRE(d1->isValid() == true);
        REQUIRE(d1->currentValue() != NULL);
        REQUIRE(PyFloat_AsDouble(d1->currentValue()) == 1.0);
    }

    SECTION("Invalid")
    {
        auto d2 = new Datum("y", "1.aaagh", &PyFloat_Type, n);
        REQUIRE(d2->isValid() == false);
    }

    delete g;
}

TEST_CASE("Datum lookup")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    auto x = new Datum("x", "1.0", &PyFloat_Type, n);

    SECTION("Within node")
    {
        auto y = new Datum("y", "x", &PyFloat_Type, n);
        REQUIRE(y->isValid() == true);
        REQUIRE(y->currentValue() != NULL);
        REQUIRE(PyFloat_AsDouble(y->currentValue()) == 1.0);
    }

    SECTION("Within graph")
    {
        auto z = new Datum("z", "n.x", &PyFloat_Type, n);
        REQUIRE(z->isValid() == true);
        REQUIRE(z->currentValue() != NULL);
        REQUIRE(PyFloat_AsDouble(z->currentValue()) == 1.0);
    }

    SECTION("Invalid lookup")
    {
        auto v = new Datum("v", "1.aagh", &PyFloat_Type, n);
        auto u = new Datum("u", "v", &PyFloat_Type, n);
        REQUIRE(u->isValid() == false);
    }

    delete g;
}

int main(int argc, char** argv)
{
    Proxy::preInit();
    Py_Initialize();
    int result = Catch::Session().run( argc, argv );
    return result;
}

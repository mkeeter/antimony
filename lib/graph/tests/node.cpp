#include <Python.h>

#include <catch/catch.hpp>

#include "graph/graph.h"
#include "graph/node.h"
#include "graph/datum.h"

TEST_CASE("Cross-node datum deletion")
{
    auto g = new Graph();
    auto a = new Node("a", g);
    auto ax = new Datum("x", "3.0", &PyFloat_Type, a);
    auto b = new Node("b", g);
    auto bx = new Datum("x", "a.x", &PyFloat_Type, b);

    a->uninstall(ax);
    REQUIRE(bx->isValid() == false);
    CAPTURE(bx->getError());
    REQUIRE(bx->getError().find("Name 'x' is not defined")
            != std::string::npos);

    g->uninstall(a);
    REQUIRE(bx->isValid() == false);
    CAPTURE(bx->getError());
    REQUIRE(bx->getError().find("Name 'a' is not defined")
            != std::string::npos);
}

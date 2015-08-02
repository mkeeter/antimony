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

TEST_CASE("Cross-node node deletion")
{
    auto g = new Graph();
    auto a = new Node("a", g);
    auto ax = new Datum("x", "3.0", &PyFloat_Type, a);
    auto b = new Node("b", g);
    auto bx = new Datum("x", "a.x", &PyFloat_Type, b);

    g->uninstall(a);
    REQUIRE(bx->isValid() == false);
    CAPTURE(bx->getError());
    REQUIRE(bx->getError().find("Name 'a' is not defined")
            != std::string::npos);
}

TEST_CASE("Cross-node name creation")
{
    auto g = new Graph();
    auto a = new Node("a", g);
    auto ax = new Datum("x", "b.x", &PyFloat_Type, a);
    CAPTURE(ax->getError());
    REQUIRE(ax->isValid() == false);

    auto b = new Node("b", g);
    auto bx = new Datum("x", "1.0", &PyFloat_Type, b);

    CAPTURE(ax->getError());
    REQUIRE(ax->isValid() == true);
}

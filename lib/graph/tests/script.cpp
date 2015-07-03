#include <Python.h>

#include <catch/catch.hpp>

#include "graph/graph.h"
#include "graph/node.h"
#include "graph/datum.h"
#include "graph/proxy.h"

TEST_CASE("Script evaluation")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    n->setScript("print('hi there!')");
    CAPTURE(n->getError());
    REQUIRE(n->getErrorLine() == -1);
    delete g;
}

TEST_CASE("Invalid script")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    n->setScript("wargarble");
    CAPTURE(n->getError());
    REQUIRE(n->getErrorLine() == 1);
    delete g;
}

TEST_CASE("Script input")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    n->setScript("input('x', float)");
    CAPTURE(n->getError());
    REQUIRE(n->getErrorLine() == -1);

    auto x = n->getDatum("x");
    REQUIRE(x != NULL);
    REQUIRE(x->isValid() == true);
    REQUIRE(x->currentValue() != NULL);
    REQUIRE(PyFloat_AsDouble(x->currentValue()) == 0.0);
    delete g;
}

TEST_CASE("Script input with default argument")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    n->setScript("input('x', float, 3.0)");
    CAPTURE(n->getError());
    REQUIRE(n->getErrorLine() == -1);

    auto x = n->getDatum("x");
    REQUIRE(x != NULL);
    REQUIRE(x->isValid() == true);
    REQUIRE(x->currentValue() != NULL);
    REQUIRE(PyFloat_AsDouble(x->currentValue()) == 3.0);
    delete g;
}

TEST_CASE("Inter-script datum lookup")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    n->setScript("input('x', float, 3.0)\n"
                 "print(x)");
    CAPTURE(n->getError());
    REQUIRE(n->getErrorLine() == -1);
}

TEST_CASE("Script re-evaluation on datum change")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    n->setScript("input('x', float)\n"
                 "raise RuntimeError(str(x))");
    n->getDatum("x")->setText("2.0");

    CAPTURE(n->getError());
    REQUIRE(n->getErrorLine() == 2);
    REQUIRE(n->getError().find("RuntimeError: 2.0") != std::string::npos);
}

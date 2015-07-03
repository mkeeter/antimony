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
}

#include <Python.h>

#include <catch/catch.hpp>

#include "graph/graph.h"
#include "graph/graph_node.h"

TEST_CASE("Graph node lookups")
{
    auto g = new Graph();
    auto a = new GraphNode("a", g);
    auto ax = new Datum("x", "3.0", &PyFloat_Type, a);

    auto sub = a->getGraph();
    auto b = new Node("b", sub);
    auto bx = new Datum("x", "__parent.x", &PyFloat_Type, b);

    CAPTURE(bx->getError());
    REQUIRE(bx->isValid() == true);

    delete g;
}

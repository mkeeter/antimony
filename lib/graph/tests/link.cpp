#include <Python.h>

#include <catch/catch.hpp>

#include "graph/graph.h"
#include "graph/node.h"
#include "graph/datum.h"
#include "graph/proxy.h"

TEST_CASE("Link detection")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    auto x = new Datum("x", "1.0", &PyFloat_Type, n);
    auto y = new Datum("y", Datum::SIGIL_CONNECTION + std::string("n.x"),
                       &PyFloat_Type, n);
    REQUIRE(y->isValid());
    REQUIRE(y->getLinks().count(x) == 1);
}

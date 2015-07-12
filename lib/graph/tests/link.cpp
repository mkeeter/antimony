#include <Python.h>

#include <catch/catch.hpp>

#include "graph/graph.h"
#include "graph/node.h"
#include "graph/datum.h"
#include "graph/proxy.h"

TEST_CASE("Link detection")
{
    REQUIRE(Datum::SIGIL_CONNECTION == '$');

    auto g = new Graph();
    auto n = new Node("n", g);
    auto x = new Datum("x", "1.0", &PyFloat_Type, n);
    auto y = new Datum("y", "$[__0.__0]", &PyFloat_Type, n);
    REQUIRE(y->isValid());
    REQUIRE(y->getLinks().count(x) == 1);

    auto z = new Datum("z", "n.x", &PyFloat_Type, n);
    REQUIRE(z->isValid());
    REQUIRE(z->getLinks().count(x) == 0);
}

TEST_CASE("Empty link pruning")
{
    REQUIRE(Datum::SIGIL_CONNECTION == '$');

    auto g = new Graph();
    auto n = new Node("n", g);
    auto x = new Datum("x", "1.0", &PyFloat_Type, n);

    // Set the expression to an empty list to force pruning
    x->setText("$[]");
    REQUIRE(x->isValid() == true);
    REQUIRE(x->getLinks().size() == 0);
    REQUIRE(x->currentValue() != NULL);
    REQUIRE(PyFloat_AsDouble(x->currentValue()) == 1.0);
}

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
    auto z = new Datum("z", "n.x", &PyFloat_Type, n);
    REQUIRE(y->isValid());
    REQUIRE(z->isValid());

    auto links = y->getLinks();
    REQUIRE(std::find(links.begin(), links.end(), x) != links.end());
    REQUIRE(std::find(links.begin(), links.end(), z) == links.end());
}

TEST_CASE("Empty link pruning")
{
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

TEST_CASE("Link pruning on deletion")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    auto x = new Datum("x", "1.0", &PyFloat_Type, n);
    auto y = new Datum("y", "$[__0.__0]", &PyFloat_Type, n);

    n->uninstall(x);
    REQUIRE(y->isValid() == true);
    REQUIRE(y->getLinks().size() == 0);
    REQUIRE(y->currentValue() != NULL);
    REQUIRE(PyFloat_AsDouble(y->currentValue()) == 1.0);

    delete g;
}

TEST_CASE("Type-based link acceptance")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    auto x = new Datum("x", "1.0", &PyFloat_Type, n);
    auto y = new Datum("y", "'hi'", &PyUnicode_Type, n);
    auto z = new Datum("z", "1.0", &PyFloat_Type, n);

    REQUIRE(x->acceptsLink(z));
    REQUIRE(!x->acceptsLink(y));

    delete g;
}

TEST_CASE("Duplicate link rejection")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    auto x = new Datum("x", "1.0", &PyFloat_Type, n);
    auto y = new Datum("y", "$[__0.__0]", &PyUnicode_Type, n);

    REQUIRE(!y->acceptsLink(x));

    delete g;
}

TEST_CASE("Recursive link rejection")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    auto x = new Datum("x", "1.0", &PyFloat_Type, n);
    auto y = new Datum("y", "$[__0.__0]", &PyFloat_Type, n);

    REQUIRE(!x->acceptsLink(x));
    REQUIRE(!x->acceptsLink(y));
    delete g;
}


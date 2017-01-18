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
    auto y = new Datum("y", Datum::SIGIL_CONNECTION + std::string("[__0.__0]"),
                       &PyFloat_Type, n);
    auto z = new Datum("z", "n.x", &PyFloat_Type, n);
    REQUIRE(y->isValid());
    REQUIRE(z->isValid());

    auto links = y->getLinks();
    REQUIRE(std::find(links.begin(), links.end(), x) != links.end());
    REQUIRE(std::find(links.begin(), links.end(), z) == links.end());
}

TEST_CASE("Link pruning on deletion")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    auto x = new Datum("x", "1.0", &PyFloat_Type, n);
    auto y = new Datum("y", Datum::SIGIL_CONNECTION + std::string("[__0.__0]"),
                       &PyFloat_Type, n);

    n->uninstall(x);
    REQUIRE(y->isValid() == true);
    REQUIRE(y->getLinks().size() == 0);
    REQUIRE(y->currentValue() != nullptr);
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
    auto y = new Datum("y", Datum::SIGIL_CONNECTION + std::string("[__0.__0]"),
                       &PyUnicode_Type, n);

    REQUIRE(!y->acceptsLink(x));

    delete g;
}

TEST_CASE("Recursive link rejection")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    auto x = new Datum("x", "1.0", &PyFloat_Type, n);
    auto y = new Datum("y", Datum::SIGIL_CONNECTION + std::string("[__0.__0]"),
                       &PyFloat_Type, n);

    REQUIRE(!x->acceptsLink(x));
    REQUIRE(!x->acceptsLink(y));
    delete g;
}

TEST_CASE("Link installation")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    auto x = new Datum("x", "2.0", &PyFloat_Type, n);
    auto y = new Datum("y", "3.0", &PyFloat_Type, n);
    auto z = new Datum("z", "0.0", &PyFloat_Type, n);

    REQUIRE(z->acceptsLink(x));
    z->installLink(x);

    REQUIRE(z->getText() == Datum::SIGIL_CONNECTION +
                            std::string("[__0.__0]"));
    REQUIRE(z->isValid() == true);
    REQUIRE(z->currentValue() != nullptr);
    REQUIRE(PyFloat_AsDouble(z->currentValue()) == 2.0);

    REQUIRE(!z->acceptsLink(y));
    delete g;
}

TEST_CASE("Link installation across nodes")
{
    auto g = new Graph();
    auto a = new Node("a", g);
    auto ax = new Datum("x", "5.0", &PyFloat_Type, a);
    auto b = new Node("b", g);
    auto bx = new Datum("x", "2.0", &PyFloat_Type, b);

    REQUIRE(ax->acceptsLink(bx));
    ax->installLink(bx);

    REQUIRE(ax->getText() == Datum::SIGIL_CONNECTION +
                             std::string("[__1.__0]"));
    REQUIRE(ax->isValid() == true);
    REQUIRE(ax->currentValue() != nullptr);
    REQUIRE(PyFloat_AsDouble(ax->currentValue()) == 2.0);

    REQUIRE(!bx->acceptsLink(ax));
    delete g;
}

TEST_CASE("Link installation with reducer")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    auto x = new Datum("x", "2.0", &PyFloat_Type, n);
    auto y = new Datum("y", "3.0", &PyFloat_Type, n);
    auto z = new Datum("z", "0.0", &PyFloat_Type, n);

    auto op = PyImport_ImportModule("operator");
    Datum::installReducer(&PyFloat_Type, PyObject_GetAttrString(op, "mul"));
    Py_DECREF(op);

    z->installLink(x);

    REQUIRE(z->acceptsLink(y));
    z->installLink(y);

    REQUIRE(z->isValid() == true);
    REQUIRE(z->currentValue() != nullptr);
    REQUIRE(PyFloat_AsDouble(z->currentValue()) == 6.0);

    auto links = z->getLinks();
    REQUIRE(std::find(links.begin(), links.end(), x) != links.end());
    REQUIRE(std::find(links.begin(), links.end(), y) != links.end());

    Datum::clearReducers();
    delete g;
}

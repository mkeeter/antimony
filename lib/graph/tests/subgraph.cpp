#include <Python.h>

#include <catch/catch.hpp>

#include "graph/graph.h"
#include "graph/graph_node.h"

TEST_CASE("Subgraph datum lookups (inbound)")
{
    auto g = new Graph();
    auto a = new GraphNode("a", g);
    auto ax = new Datum("x", "3.0", &PyFloat_Type, a);

    auto sub = a->getGraph();
    auto b = new Node("b", sub);

    SECTION("Allowed")
    {
        auto bx = new Datum("x", Datum::SIGIL_CONNECTION +
                                 std::string("[__parent.__0]"),
                            &PyFloat_Type, b);
        CAPTURE(bx->getError());
        REQUIRE(bx->isValid() == true);
        REQUIRE(PyFloat_AsDouble(bx->currentValue()) == 3.0);
    }

    SECTION("Not allowed")
    {
        auto by = new Datum("y", "__parent.__0", &PyFloat_Type, b);
        CAPTURE(by->getError());
        REQUIRE(by->isValid() == false);
        REQUIRE(by->getError().find("Name '__parent' is not defined")
                != std::string::npos);
    }

    delete g;
}

TEST_CASE("Subgraph datum lookups (outbound)")
{
    auto g = new Graph();
    auto a = new GraphNode("a", g);

    auto sub = a->getGraph();
    auto b = new Node("b", sub);
    auto bx = new Datum("x", "3.0", &PyFloat_Type, b);

    SECTION("Allowed")
    {
        auto ax = new Datum("x", Datum::SIGIL_CONNECTION +
                                 std::string("[__0.__subgraph.__0.__0]"),
                            &PyFloat_Type, a);
        CAPTURE(ax->getError());
        REQUIRE(ax->isValid() == true);
        REQUIRE(PyFloat_AsDouble(ax->currentValue()) == 3.0);
    }

    SECTION("Not allowed")
    {
        auto ay = new Datum("y", "__0.__subgraph.__0.__0", &PyFloat_Type, a);
        CAPTURE(ay->getError());
        REQUIRE(ay->isValid() == false);
        REQUIRE(ay->getError().find("Name '__0' is not defined")
                != std::string::npos);
    }

    delete g;
}

TEST_CASE("Subgraph value tracking (inbound)")
{
    auto g = new Graph();
    auto a = new GraphNode("a", g);
    auto ax = new Datum("x", "3.0", &PyFloat_Type, a);

    auto sub = a->getGraph();
    auto b = new Node("b", sub);

    auto bx = new Datum("x", Datum::SIGIL_CONNECTION +
                             std::string("[__parent.__0]"),
                        &PyFloat_Type, b);
    REQUIRE(bx->currentValue() != NULL);
    REQUIRE(PyFloat_AsDouble(bx->currentValue()) == 3.0);

    ax->setText("2.0");
    REQUIRE(bx->currentValue() != NULL);
    REQUIRE(PyFloat_AsDouble(bx->currentValue()) == 2.0);

    delete g;
}

TEST_CASE("Subgraph value tracking (outbound)")
{
    auto g = new Graph();
    auto a = new GraphNode("a", g);

    auto sub = a->getGraph();
    auto b = new Node("b", sub);
    auto bx = new Datum("x", "3.0", &PyFloat_Type, b);

    auto ax = new Datum("x", Datum::SIGIL_CONNECTION +
                             std::string("[__0.__subgraph.__0.__0]"),
                        &PyFloat_Type, a);

    CAPTURE(ax->getError());
    REQUIRE(ax->currentValue() != NULL);
    REQUIRE(PyFloat_AsDouble(ax->currentValue()) == 3.0);

    bx->setText("2.0");
    REQUIRE(ax->currentValue() != NULL);
    REQUIRE(PyFloat_AsDouble(ax->currentValue()) == 2.0);

    delete g;
}

TEST_CASE("Subgraph link installation (inbound)")
{
    auto g = new Graph();
    auto a = new GraphNode("a", g);
    auto ax = new Datum("x", "3.0", &PyFloat_Type, a);

    auto sub = a->getGraph();
    auto b = new Node("b", sub);

    auto bx = new Datum("x", "1.0", &PyFloat_Type, b);

    bx->installLink(ax);
    CAPTURE(bx->getText());
    CAPTURE(bx->getError());
    REQUIRE(bx->isValid() == true);
    REQUIRE(bx->currentValue() != NULL);
    REQUIRE(PyFloat_AsDouble(bx->currentValue()) == 3.0);

    auto bxLinks = bx->getLinks();
    REQUIRE(bxLinks.size() == 1);
    REQUIRE(bxLinks.count(ax) == 1);

    delete g;
}

TEST_CASE("Subgraph link installation (outbound)")
{
    auto g = new Graph();
    auto a = new GraphNode("a", g);
    auto ax = new Datum("x", "3.0", &PyFloat_Type, a);

    auto sub = a->getGraph();
    auto b = new Node("b", sub);

    auto bx = new Datum("x", "1.0", &PyFloat_Type, b);

    ax->installLink(bx);
    CAPTURE(ax->getText());
    CAPTURE(ax->getError());
    REQUIRE(ax->isValid() == true);
    REQUIRE(ax->currentValue() != NULL);
    REQUIRE(PyFloat_AsDouble(ax->currentValue()) == 1.0);

    auto axLinks = ax->getLinks();
    REQUIRE(axLinks.size() == 1);
    REQUIRE(axLinks.count(bx) == 1);

    delete g;
}

TEST_CASE("Subgraph datum creation (inbound)")
{
    auto g = new Graph();
    auto a = new GraphNode("a", g);

    auto sub = a->getGraph();
    auto b = new Node("b", sub);

    auto bx = new Datum("x", Datum::SIGIL_CONNECTION +
                             std::string("[__parent.__0]"),
                        &PyFloat_Type, b);
    REQUIRE(bx->isValid() == false);

    auto ax = new Datum("x", "3.0", &PyFloat_Type, a);
    CAPTURE(bx->getError());
    REQUIRE(bx->isValid() == true);
    REQUIRE(bx->currentValue() != NULL);
    REQUIRE(PyFloat_AsDouble(bx->currentValue()) == 3.0);

    delete g;
}

TEST_CASE("Subgraph datum creation (outbound)")
{
    auto g = new Graph();
    auto a = new GraphNode("a", g);
    auto ax = new Datum("x", Datum::SIGIL_CONNECTION +
                             std::string("[__0.__subgraph.__0.__0]"),
                        &PyFloat_Type, a);

    auto sub = a->getGraph();
    auto b = new Node("b", sub);
    auto bx = new Datum("x", "3.0", &PyFloat_Type, b);

    CAPTURE(ax->getError());
    REQUIRE(ax->isValid());
    REQUIRE(ax->currentValue() != NULL);
    REQUIRE(PyFloat_AsDouble(ax->currentValue()) == 3.0);

    delete g;
}

TEST_CASE("Subgraph link deletion")
{
    // Install multiplication as a reducer (to create multi-link datums)
    auto op = PyImport_ImportModule("operator");
    Datum::installReducer(&PyFloat_Type, PyObject_GetAttrString(op, "mul"));
    Py_DECREF(op);

    auto g = new Graph();

    auto a = new GraphNode("a", g);
    auto ax = new Datum("x", "3.0", &PyFloat_Type, a);

    auto b = new GraphNode("b", a->getGraph());
    auto bx = new Datum("x", Datum::SIGIL_CONNECTION +
                             std::string("[__parent.__0,__0.__subgraph.__0.__0]"),
                        &PyFloat_Type, b);

    auto c = new GraphNode("c", b->getGraph());
    auto cx = new Datum("x", "4.0", &PyFloat_Type, c);

    CAPTURE(bx->getError());
    REQUIRE(bx->isValid());
    REQUIRE(bx->currentValue() != NULL);
    REQUIRE(PyFloat_AsDouble(bx->currentValue()) == 12.0);

    SECTION("Subgraph")
    {
        c->uninstall(cx);

        CAPTURE(bx->getError());
        CAPTURE(bx->getText());
        REQUIRE(bx->isValid());
        REQUIRE(bx->currentValue() != NULL);
        REQUIRE(PyFloat_AsDouble(bx->currentValue()) == 3.0);
    }

    SECTION("Parent graph")
    {
        a->uninstall(ax);

        CAPTURE(bx->getError());
        CAPTURE(bx->getText());
        REQUIRE(bx->isValid());
        REQUIRE(bx->currentValue() != NULL);
        REQUIRE(PyFloat_AsDouble(bx->currentValue()) == 4.0);
    }

    Datum::clearReducers();

    delete g;
}

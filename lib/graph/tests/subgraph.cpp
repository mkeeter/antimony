#include <Python.h>

#include <catch/catch.hpp>

#include "graph/graph.h"
#include "graph/graph_node.h"

TEST_CASE("Subgraph datum lookups")
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

TEST_CASE("Subgraph value tracking")
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

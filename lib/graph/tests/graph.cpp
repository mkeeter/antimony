#include <Python.h>

#include <catch/catch.hpp>

#include "graph/graph.h"
#include "graph/node.h"
#include "graph/datum.h"

TEST_CASE("Name validity")
{
    REQUIRE(Graph::isNameValid("hithar"));
    REQUIRE(Graph::isNameValid("__hello"));
    REQUIRE(Graph::isNameValid("hi1"));

    REQUIRE(!Graph::isNameValid("1hi"));
    REQUIRE(!Graph::isNameValid(""));
    REQUIRE(!Graph::isNameValid("for"));
}

TEST_CASE("Graph name generation")
{
    auto g = new Graph();
    new Node("n", g);
    REQUIRE(g->nextName("n") == "n0");

    new Node("n0", g);
    REQUIRE(g->nextName("n") == "n1");

    new Node("n2", g);
    REQUIRE(g->nextName("n") == "n1");

    delete g;
}

TEST_CASE("Wildcard node names")
{
    auto g = new Graph();
    new Node("n", g);
    new Node("n1", g);
    REQUIRE(g->nextName("n") == "n0");

    auto n0 = new Node("n*", g);
    REQUIRE(n0->getName() == "n0");

    auto n2 = new Node("n*", g);
    REQUIRE(n2->getName() == "n2");

    delete g;
}

TEST_CASE("Duplicate name validity")
{
    auto g = new Graph();
    auto a = new Node("n", g);
    auto b = new Node("n", g);

    REQUIRE(!g->isNameUnique("n"));
    REQUIRE(g->isNameUnique("n", a));
    REQUIRE(!g->isNameUnique("n", b));
}

TEST_CASE("Getting multiple UIDs")
{
    auto g = new Graph();
    auto a = new Node("a", g);
    auto b = new Node("b", g);
    auto c = new Node("c", g);

    REQUIRE(g->getUIDs(3) == std::list<uint64_t>({3,4,5}));
    g->uninstall(b);
    REQUIRE(g->getUIDs(3) == std::list<uint64_t>({1,3,4}));
}

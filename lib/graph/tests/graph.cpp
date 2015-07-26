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

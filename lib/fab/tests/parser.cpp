#include <Python.h>
#include <catch/catch.hpp>

#include "fab/fab.h"
#include "fab/tree/tree.h"
#include "fab/tree/parser.h"

TEST_CASE("Basic parsing")
{
    MathTree* t;

    SECTION("Parsing 'X'")
    {
        t = parse("X");
        REQUIRE(t != nullptr);
        REQUIRE(t->num_levels == 1);
        REQUIRE(t->num_constants == 0);
        free(t);
    }

    SECTION("Parsing '+Xf1.0'")
    {
        t = parse("+Xf1.0");
        REQUIRE(t != nullptr);
        REQUIRE(t->num_constants == 1);
        REQUIRE(t->num_levels == 2);
        free(t);
    }

    SECTION("Duplicate pruning")
    {
        t = parse("*+Xf2.0+Yf2.0");
        REQUIRE(t != nullptr);
        REQUIRE(t->num_constants == 1);
        REQUIRE(t->num_levels == 3);
        free(t);
    }

    SECTION("Invalid parse")
    {
        t = parse("f");
        REQUIRE(t == nullptr);
        free(t);
    }

    SECTION("Parsing an empty string")
    {
        t = parse("");
        REQUIRE(t == nullptr);
        free(t);
    }

    SECTION("Incomplete expression")
    {
        t = parse("+X");
        REQUIRE(t == nullptr);
        free(t);
    }

    SECTION("Long expression")
    {
        std::string s;
        for (int i=0; i < 99; ++i)
            s += "i";
        for (int i=0; i < 100; ++i)
            s += "X";

        t = parse(s.c_str());
        REQUIRE(t != nullptr);
    }
}

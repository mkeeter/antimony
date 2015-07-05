#include <Python.h>

#include <catch/catch.hpp>

#include "graph/graph.h"
#include "graph/node.h"
#include "graph/datum.h"
#include "graph/proxy.h"

TEST_CASE("Script evaluation")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    n->setScript("print('hi there!')");
    CAPTURE(n->getError());
    REQUIRE(n->getErrorLine() == -1);
    delete g;
}

TEST_CASE("Invalid script")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    n->setScript("wargarble");
    CAPTURE(n->getError());
    REQUIRE(n->getErrorLine() == 1);
    delete g;
}

TEST_CASE("Script input")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    n->setScript("input('x', float)");
    CAPTURE(n->getError());
    REQUIRE(n->getErrorLine() == -1);

    auto x = n->getDatum("x");
    REQUIRE(x != NULL);
    REQUIRE(x->isValid() == true);
    REQUIRE(x->currentValue() != NULL);
    REQUIRE(PyFloat_AsDouble(x->currentValue()) == 0.0);
    delete g;
}

TEST_CASE("Script input with default argument")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    n->setScript("input('x', float, 3.0)");
    CAPTURE(n->getError());
    REQUIRE(n->getErrorLine() == -1);

    auto x = n->getDatum("x");
    REQUIRE(x != NULL);
    REQUIRE(x->isValid() == true);
    REQUIRE(x->currentValue() != NULL);
    REQUIRE(PyFloat_AsDouble(x->currentValue()) == 3.0);
    delete g;
}

TEST_CASE("Inter-script datum lookup")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    n->setScript("input('x', float, 3.0)\n"
                 "print(x)");
    CAPTURE(n->getError());
    REQUIRE(n->getErrorLine() == -1);
    delete g;
}

TEST_CASE("Datum pinning")
{
    auto g = new Graph();
    auto n = new Node("n", g);

    n->setScript("input('x', float, 1.0)");
    auto x = n->getDatum("x");

    n->setScript("input('x', float, 2.0)");
    REQUIRE(n->getErrorLine() == -1);
    REQUIRE(n->getDatum("x") == x);
    REQUIRE(x->isValid() == true);
    REQUIRE(x->currentValue() != NULL);
    REQUIRE(PyFloat_AsDouble(x->currentValue()) == 1.0);
    delete g;
}

TEST_CASE("Script re-evaluation on datum change")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    n->setScript("input('x', float)\n"
                 "raise RuntimeError(str(x))");
    n->getDatum("x")->setText("2.0");

    CAPTURE(n->getError());
    REQUIRE(n->getErrorLine() == 2);
    REQUIRE(n->getError().find("RuntimeError: 2.0") != std::string::npos);
    delete g;
}

TEST_CASE("Script re-evaluation on linked datum change")
{
    auto g = new Graph();
    auto a = new Node("a", g);
    auto ax = new Datum("x", "3.0", &PyFloat_Type, a);

    auto b = new Node("b", g);
    b->setScript("input('x', float, 1.0)\n"
                 "output('y', x)");
    b->getDatum("x")->setText("a.x");
    CAPTURE(b->getError());
    REQUIRE(b->getErrorLine() == -1);

    auto y = b->getDatum("y");
    REQUIRE(y->isValid() == true);
    REQUIRE(y->currentValue() != NULL);
    REQUIRE(PyFloat_AsDouble(y->currentValue()) == 3.0);
    delete g;
}

TEST_CASE("Script output")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    n->setScript("output('x', 1.0)\n");

    CAPTURE(n->getError());
    REQUIRE(n->getErrorLine() == -1);

    auto x = n->getDatum("x");
    REQUIRE(x != NULL);
    REQUIRE(x->isValid() == true);
    REQUIRE(x->currentValue() != NULL);
    REQUIRE(PyFloat_AsDouble(x->currentValue()) == 1.0);
    delete g;
}

TEST_CASE("Script input pruning")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    n->setScript("input('x', float, 1.0)");

    CAPTURE(n->getScript());
    REQUIRE(n->getScript() == "input('x', float)");
    delete g;
}

TEST_CASE("Removing datum from script")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    n->setScript("input('x', float, 1.0)");
    REQUIRE(n->getDatum("x") != NULL);

    n->setScript("");
    REQUIRE(n->getDatum("x") == NULL);
    delete g;
}

TEST_CASE("Datum removal triggering update")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    n->setScript("input('x', float, 1.0)\n"
                 "input('y', float)");

    auto y = n->getDatum("y");
    y->setText("n.x");
    REQUIRE(y->isValid() == true);

    n->setScript("input('y', float)");
    REQUIRE(y != NULL);
    REQUIRE(y->isValid() == false);
    CAPTURE(y->getError());
    REQUIRE(y->getError().find("Name 'x' is not defined")
            != std::string::npos);
    delete g;
}

TEST_CASE("Invalid datum names")
{
    auto g = new Graph();
    auto n = new Node("n", g);

    SECTION("Keyword")
    {
        n->setScript("input('for', float, 1.0)");
        REQUIRE(n->getErrorLine() == 1);
        CAPTURE(n->getError());
        REQUIRE(n->getError().find("Datum name is a reserved Python keyword")
                != std::string::npos);
    }

    SECTION("Double underscore")
    {
        n->setScript("input('__x', float, 1.0)");
        REQUIRE(n->getErrorLine() == 1);
        CAPTURE(n->getError());
        REQUIRE(n->getError().find("Datum name cannot begin with '__'")
                != std::string::npos);
    }

    SECTION("Repeated initialization")
    {
        n->setScript("input('x', float, 1.0)\n"
                     "input('x', int, 2)\n");
        REQUIRE(n->getErrorLine() == 2);
        CAPTURE(n->getError());
        REQUIRE(n->getError().find("Datum was already defined in this script.")
                != std::string::npos);
    }
}

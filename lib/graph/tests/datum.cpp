#include <Python.h>

#include <catch/catch.hpp>

#include "graph/graph.h"
#include "graph/node.h"
#include "graph/datum.h"
#include "graph/proxy.h"
#include "graph/util.h"

TEST_CASE("Datum evaluation")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    SECTION("Valid")
    {
        auto d1 = new Datum("x", "1.0", &PyFloat_Type, n);
        REQUIRE(d1->isValid() == true);
        REQUIRE(d1->currentValue() != nullptr);
        REQUIRE(PyFloat_AsDouble(d1->currentValue()) == 1.0);
    }

    SECTION("Invalid")
    {
        auto d2 = new Datum("y", "1.aaagh", &PyFloat_Type, n);
        REQUIRE(d2->isValid() == false);
    }

    delete g;
}

TEST_CASE("Datum validity changes")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    auto x = new Datum("x", "1.q", &PyFloat_Type, n);
    REQUIRE(x->isValid() == false);
    x->setText("1.0");
    REQUIRE(x->isValid() == true);
    REQUIRE(x->getError() == "");
}

TEST_CASE("Valid datum lookup")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    auto x = new Datum("x", "1.0", &PyFloat_Type, n);
    auto y = new Datum("y", "n.x", &PyFloat_Type, n);
    REQUIRE(y->isValid() == true);
    REQUIRE(y->currentValue() != nullptr);
    REQUIRE(PyFloat_AsDouble(y->currentValue()) == 1.0);

    delete g;
}

TEST_CASE("Invalid datum lookups")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    new Datum("x", "1.!", &PyFloat_Type, n);

    SECTION("Invalid lookup")
    {
        auto y = new Datum("y", "n.x", &PyFloat_Type, n);
        REQUIRE(y->isValid() == false);
        CAPTURE(y->getError());
        REQUIRE(y->getError().find("Datum 'x' is invalid") != std::string::npos);
    }

    SECTION("Missing lookup")
    {
        auto z = new Datum("z", "n.q", &PyFloat_Type, n);
        REQUIRE(z->isValid() == false);
        CAPTURE(z->getError());
        REQUIRE(z->getError().find("Name 'q' is not defined") != std::string::npos);
    }
    delete g;
}

TEST_CASE("Tracking changes")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    auto x = new Datum("x", "1.0", &PyFloat_Type, n);
    auto y = new Datum("y", "n.x", &PyFloat_Type, n);

    x->setText("2.0");
    REQUIRE(y->isValid() == true);
    REQUIRE(y->currentValue() != nullptr);
    REQUIRE(PyFloat_AsDouble(y->currentValue()) == 2.0);

    x->setText("3.0");
    REQUIRE(y->isValid() == true);
    REQUIRE(y->currentValue() != nullptr);
    REQUIRE(PyFloat_AsDouble(y->currentValue()) == 3.0);
    delete g;
}

TEST_CASE("Name creation")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    auto y = new Datum("y", "n.x", &PyFloat_Type, n);
    auto x = new Datum("x", "1.0", &PyFloat_Type, n);

    REQUIRE(y->isValid() == true);
    REQUIRE(y->currentValue() != nullptr);
    REQUIRE(PyFloat_AsDouble(y->currentValue()) == 1.0);
    delete g;
}

TEST_CASE("Recursive lookup")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    auto x = new Datum("x", "n.y", &PyFloat_Type, n);
    auto y = new Datum("y", "n.x", &PyFloat_Type, n);

    REQUIRE(x->isValid() == false);
    CAPTURE(x->getError());
    REQUIRE(x->getError().find("Recursive lookup of datum 'y'") != std::string::npos);
    REQUIRE(y->isValid() == false);
    CAPTURE(y->getError());
    REQUIRE(y->getError().find("Recursive lookup of datum 'x'") != std::string::npos);

    delete g;
}

TEST_CASE("UID lookup")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    auto x = new Datum("x", "1.0", &PyFloat_Type, n);

    SECTION("Allowed")
    {
        auto y = new Datum("y", Datum::SIGIL_CONNECTION +
                                std::string("[__0.__0]"),
                            &PyFloat_Type, n);
        CAPTURE(y->getError());
        REQUIRE(y->isValid() == true);
        REQUIRE(y->currentValue() != nullptr);
        REQUIRE(PyFloat_AsDouble(y->currentValue()) == 1.0);
    }

    SECTION("Not allowed")
    {
        auto y = new Datum("y", "1.0", &PyFloat_Type, n);
        auto z = new Datum("z", "__0.__1", &PyFloat_Type, n);
        REQUIRE(z->isValid() == false);
        CAPTURE(z->getError());
        REQUIRE(z->getError().find("Name '__0' is not defined")
                != std::string::npos);
    }

    delete g;
}

TEST_CASE("UID changes")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    auto x = new Datum("x", "1.0", &PyFloat_Type, n);
    auto y = new Datum("y", Datum::SIGIL_CONNECTION + std::string("[__0.__0]"),
                       &PyFloat_Type, n);
    x->setText("2.0");

    CAPTURE(y->getError());
    REQUIRE(y->isValid() == true);
    REQUIRE(y->currentValue() != nullptr);
    REQUIRE(PyFloat_AsDouble(y->currentValue()) == 2.0);
    delete g;
}

TEST_CASE("Failed datum type conversion")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    auto x = new Datum("x", "'hi there'", &PyFloat_Type, n);

    CAPTURE(x->getError());
    REQUIRE(x->getError().find("Could not convert from <class 'str'> to <class 'float'>")
            != std::string::npos);
    REQUIRE(x->isValid() == false);
    delete g;
}

TEST_CASE("Successful datum type conversion")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    auto x = new Datum("x", "2", &PyFloat_Type, n);

    CAPTURE(x->getError());
    REQUIRE(x->isValid() == true);
    REQUIRE(x->currentValue() != nullptr);
    REQUIRE(PyFloat_AsDouble(x->currentValue()) == 2.0);
    delete g;
}

TEST_CASE("Mutable node proxies")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    auto x = new Datum("x", "1.0", &PyFloat_Type, n);

    auto d = Py_BuildValue("{sNsO}", "this", n->mutableProxy(),
                           "__builtins__", PyEval_GetBuiltins());

    PyRun_String("this.x = 2.0", Py_file_input, d, d);
    REQUIRE(!PyErr_Occurred());
    REQUIRE(x->isValid() == true);
    REQUIRE(x->currentValue() != nullptr);
    REQUIRE(PyFloat_AsDouble(x->currentValue()) == 2.0);

    PyRun_String("this.x = 3", Py_file_input, d, d);
    if (PyErr_Occurred())
    {
        CAPTURE(getPyError().first);
        REQUIRE(false);
    }
    REQUIRE(x->isValid() == true);
    REQUIRE(x->currentValue() != nullptr);
    REQUIRE(PyFloat_AsDouble(x->currentValue()) == 3.0);

    PyRun_String("this.x = 's'", Py_file_input, d, d);
    REQUIRE(PyErr_Occurred());
    REQUIRE(getPyError().first.find("Assignment failed due to invalid type") !=
            std::string::npos);

    Py_DECREF(d);
    delete g;
}

TEST_CASE("Un-assignable mutable node proxies")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    auto x = new Datum("x", "1.0", &PyFloat_Type, n);
    auto y = new Datum("y", "n.x", &PyFloat_Type, n);

    auto d = Py_BuildValue("{sNsO}", "this", n->mutableProxy(),
                           "__builtins__", PyEval_GetBuiltins());

    PyRun_String("this.y = 2.0", Py_file_input, d, d);
    if (PyErr_Occurred())
    {
        CAPTURE(getPyError().first);
        REQUIRE(false);
    }
    REQUIRE(y->isValid() == true);
    REQUIRE(y->currentValue() != nullptr);
    REQUIRE(PyFloat_AsDouble(y->currentValue()) == 1.0);

    Py_DECREF(d);
    delete g;
}

TEST_CASE("Datum output detection")
{
    auto g = new Graph();
    auto n = new Node("n", g);
    auto x = new Datum("x", "1.0", &PyFloat_Type, n);
    auto y = new Datum("y", Datum::SIGIL_CONNECTION +
                            std::string("[__0.__0]"),
                        &PyFloat_Type, n);
    auto out = x->outgoingLinks();
    REQUIRE(out.size() == 1);
    REQUIRE(out.count(y) == 1);
    delete g;
}

TEST_CASE("Looking using 'self'")
{
    auto g = new Graph();
    auto n = new Node("a", g);
    auto x = new Datum("x", "123.0", &PyFloat_Type, n);
    auto from_self = new Datum("from_self", "self.x", &PyFloat_Type, n);

    // Sanity-checking of x datum
    REQUIRE(x->isValid() == true);
    REQUIRE(x->currentValue() != nullptr);
    REQUIRE(PyFloat_AsDouble(x->currentValue()) == 123.0);

    SECTION("Use of 'self'")
    {
        CAPTURE(from_self->getError());
        REQUIRE(from_self->isValid() == true);
        REQUIRE(from_self->currentValue() != nullptr);
        REQUIRE(PyFloat_AsDouble(from_self->currentValue()) == 123.0);
    }

    SECTION("Change tracking with 'self'")
    {
        x->setText("231.0");
        REQUIRE(PyFloat_AsDouble(from_self->currentValue()) == 231.0);
    }

    delete g;
}

TEST_CASE("Constructing a datum with empty link list")
{
    auto g = new Graph();
    auto n = new Node("a", g);
    auto x = new Datum("x", Datum::SIGIL_CONNECTION + std::string("[]"),
                       &PyFloat_Type, n);

    REQUIRE(PyFloat_AsDouble(x->currentValue()) == 0.0);
}

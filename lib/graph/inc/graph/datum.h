#pragma once

#include <Python.h>

#include <string>
#include <memory>

#include "graph/types/downstream.h"

class Source;
class Node;

class Datum : public Downstream
{
public:
    explicit Datum(std::string name, std::string value,
                   PyTypeObject* type, Node* parent);

    ~Datum();


    /*
    static Datum* OutputDatum(std::string name, PyObject* obj,
                              Node* parent);
                              */
protected:
    /*
     *  When an upstream changes, call update.
     */
    void update() override;

    /*
     *  Evaluates our expression, giving a value.
     */
    PyObject* getValue();

    std::string name;
    std::string expr;

    PyObject* value;
    bool valid;
    PyTypeObject* type;

    Node* parent;

    friend class Node;
    friend class Proxy;
    friend class Root;
};


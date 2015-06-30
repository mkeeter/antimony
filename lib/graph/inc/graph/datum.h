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

    void setText(std::string s);

    bool isValid() const { return valid; }
    std::string getError() const { return error; }

    /*
     *  Returns a borrowed reference to the current value.
     */
    PyObject* currentValue() const { return value; }

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
    std::string error;
    PyTypeObject* type;

    Node* parent;

    /*
     *  This set represents any source whose modification could cause
     *  this datum to be activated.  It is used to detect recursive loops.
     */
    std::unordered_set<Datum*> sources;

    friend class Node;
    friend class Proxy;
    friend class Root;
};


#pragma once

#include <Python.h>

#include <string>
#include <memory>
#include <unordered_set>
#include <list>

#include "graph/types/downstream.h"
#include "graph/watchers.h"

class Source;
class Node;
class DatumWatcher;

class Datum : public Downstream
{
public:
    explicit Datum(std::string name, std::string value,
                   PyTypeObject* type, Node* parent);

    ~Datum();

    void setText(std::string s);
    std::string getText() const { return expr; }

    Node* parentNode() const { return parent; }

    /*
     *  Returns a borrowed reference to the type object.
     */
    PyTypeObject* getType() const { return type; }

    bool isValid() const { return valid; }
    std::string getError() const { return error; }

    std::string getName() const { return name; }

    /*
     *  Return the state (passed into callbacks)
     */
    DatumState getState() const;

    /*
     *  Returns a borrowed reference to the current value.
     */
    PyObject* currentValue() const { return value; }

    /*
     *  Sets the callback object.
     */
    void installWatcher(DatumWatcher* w) { watchers.push_back(w); }

    /*
     *  Returns true unless the leading character is an OUTPUT sigil.
     */
    bool hasInput() const;

    static const char SIGIL_CONNECTION = '$';
    static const char SIGIL_OUTPUT = '#';
protected:
    /*
     *  When an upstream changes, call update.
     */
    void update() override;

    /*
     *  Trims a leading sigil (if present) and returns the string
     *  and a boolean indicating whether a sigil was trimmed.
     */
    static std::pair<std::string, bool> trimSigil(std::string e);

    /*
     *  Evaluates our expression, giving a value.
     */
    PyObject* getValue();

    /*
     *  If this datum has a connection sigil, allow UID lookups.
     */
    bool allowLookupByUID() const override;

    const std::string name;
    const uint32_t uid;

    std::string expr;

    PyObject* value;
    bool valid;
    std::string error;
    PyTypeObject* type;

    Node* parent;

    std::list<DatumWatcher*> watchers;

    /*
     *  This set represents any source whose modification could cause
     *  this datum to be activated.  It is used to detect recursive loops.
     */
    std::unordered_set<Datum*> sources;

    /*
     *  Sigils are single characters at the beginning of an expression
     *  that mark it as special in some way (connection, output, etc).
     */
    static std::unordered_set<char> sigils;

    friend class Node;
    friend class Proxy;
    friend class Root;
};


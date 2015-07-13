#pragma once

#include <Python.h>

#include <string>
#include <memory>
#include <unordered_set>
#include <unordered_map>
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

    /*
     *  Registers d as an upstream datum, loading it into sources
     *
     *  Returns true on success, false on recursive lookup failure.
     */
    bool addUpstream(Datum* d);

    /*
     *  Returns the set of incoming links
     *  (found by parsing the expression)
     */
    std::list<const Datum*> getLinks() const;

    /*
     *  Checks to see if we can accept the given link.
     */
    bool acceptsLink(Datum* upstream) const;

    static const char SIGIL_CONNECTION;
    static const char SIGIL_OUTPUT;

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
     *  Attempts to cast the given value to this Datum's type.
     *  Returns NULL and sets error on failure.
     */
    PyObject* castToType(PyObject* value);

    /*
     *  If this datum has a connection sigil, allow UID lookups.
     */
    bool allowLookupByUID() const override;

    /*
     *  Returns true if this expression is a connection.
     */
    bool isLink() const;

    /*
     *  Handles post-processing of a link value.
     *
     *  Returns a new value (from extraction or reduction)
     */
    PyObject* checkLinkResult(PyObject* obj);

    /*
     *  Updates the expression by pruning invalid links and
     *  collapsing to a single value if the list ends up empty.
     *
     *  The expression must begin with SIGIL_CONNECTION.
     */
    void checkLinkExpression();

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
    std::unordered_set<const Datum*> sources;

    /*
     *  Sigils are single characters at the beginning of an expression
     *  that mark it as special in some way (connection, output, etc).
     */
    static std::unordered_set<char> sigils;

    /*
     *  Functions used to reduce a list of linked inputs into a single value
     *  (e.g. operator.or_ to combine a list of bitfields)
     */
    static std::unordered_map<PyTypeObject*, PyObject*> reducers;
    static void installReducer(PyTypeObject* t, PyObject* f);

    friend class Node;
    friend class Proxy;
    friend class Root;
};


#pragma once

#include <Python.h>

#include <string>
#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <list>

#include "graph/types/downstream.h"
#include "graph/types/watched.h"
#include "graph/watchers.h"

class Source;
class Node;
class DatumWatcher;

class Datum : public Downstream, public Watched<DatumWatcher, DatumState>
{
public:
    explicit Datum(std::string name, std::string value,
                   PyTypeObject* type, Node* parent);
    explicit Datum(std::string name, uint64_t uid,
                   std::string expr, PyTypeObject* type,
                   Node* parent);

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

    /*
     *  Look up the datum's name.
     */
    std::string getName() const { return name; }

    /*
     *  Look up the datum's UID.
     */
    uint64_t getUID() const { return uid; }

    /*
     *  Return the state (passed into callbacks)
     */
    DatumState getState() const override;

    /*
     *  Returns a borrowed reference to the current value.
     */
    PyObject* currentValue() const { return value; }

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
     *
     *  The list skips any incoming link that is no longer valid.
     */
    std::unordered_set<const Datum*> getLinks() const;

    /*
     *  Returns all links that are outgoingg from this datum.
     */
    std::unordered_set<Datum*> outgoingLinks() const;

    /*
     *  Checks to see if we can accept the given link.
     */
    bool acceptsLink(const Datum* upstream) const;

    /*
     *  Installs the given datum as an upstream link.
     */
    void installLink(const Datum* upstream);

    /*
     *  Removes the given datum as an upstream link.
     */
    void uninstallLink(const Datum* upstream);

    /*
     *  Returns true if this expression is a connection.
     */
    bool isLink() const;

    /*
     *  Returns true if this expression is an output.
     */
    bool isOutput() const;

    /*
     *  Sets up a global reducer function
     *  (used to reduce multiple inputs into a single value)
     *
     *  This should only be called once, in application setup.
     */
    static void installReducer(PyTypeObject* t, PyObject* f);
    static void clearReducers() { reducers.clear(); }

    static const char SIGIL_NONE;
    static const char SIGIL_CONNECTION;
    static const char SIGIL_OUTPUT;

protected:
    /*
     *  Calls parent->changed and parent->triggerWatchers
     */
    void init();

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
     *  Handles post-processing of a link value.
     *
     *  Returns a new value (from extraction or reduction)
     */
    PyObject* checkLinkResult(PyObject* obj);

    /*
     *  Updates the expression with the given set of links
     *  (turning it back into a value if the list is empty)
     *
     *  Sets the expression directly (rather than calling setText);
     *  call trigger() afterwards if a re-evaluation is needed.
     */
    void writeLinkExpression(const std::unordered_set<const Datum*> links);

    /*
     *  Returns a formatted link expression in the form
     *      __nodeID.__datumID
     *      __parent.__datumID
     *      __nodeID.__subgraph.__nodeID.__datumID
     *  (depending on whether the datum shares a parent with us or not)
     */
    std::string formatLink(const Datum* upstream) const;

    const std::string name;
    const uint32_t uid;

    std::string expr;

    PyObject* value;
    bool valid;
    std::string error;
    PyTypeObject* type;

    Node* parent;

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

    friend class Node;
    friend class ScriptNode;
    friend class Root;
    friend struct Script;
};


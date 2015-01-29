#ifndef HOOK_OUTPUT_H
#define HOOK_OUTPUT_H

#include <boost/python.hpp>

class ScriptDatum;

struct ScriptOutputHook
{
    ScriptOutputHook() : datum(NULL) {}
    ScriptOutputHook(ScriptDatum* d) : datum(d) {}

    void call(std::string name, boost::python::object obj);

    ScriptDatum* datum;
};

#endif

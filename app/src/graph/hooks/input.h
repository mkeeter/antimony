#ifndef HOOK_INPUT_H
#define HOOK_INPUT_H

#include <boost/python.hpp>

class ScriptDatum;

struct ScriptInputHook
{
    ScriptInputHook() : datum(NULL) {}
    ScriptInputHook(ScriptDatum* d) : datum(d) {}

    void call(std::string name, boost::python::object type);
    void call_with_default(std::string name, boost::python::object type,
                           boost::python::object default_value);

    ScriptDatum* datum;
};

#endif

#include "graph/hooks/input.h"
#include "graph/hooks/hooks.h"

#include "graph/datum/datums/script_datum.h"

void ScriptInputHook::call(std::string name, boost::python::object type)
{
    if (!PyType_Check(type.ptr()))
        throw hooks::HookException("Invalid second argument (must be a type)");

    if (datum)
        datum->makeInput(QString::fromStdString(name), (PyTypeObject*)type.ptr());
}

void ScriptInputHook::call_with_default(
        std::string name, boost::python::object type,
        boost::python::object default_value)
{
    if (!PyType_Check(type.ptr()))
        throw hooks::HookException("Invalid second argument (must be a type)");

    boost::python::extract<std::string> s(default_value);

    // Extract a string from the third argument.
    std::string txt = s.check()
        ? s()
        : boost::python::extract<std::string>(
                  boost::python::str(default_value))();

    if (datum)
        datum->makeInput(QString::fromStdString(name), (PyTypeObject*)type.ptr(),
                         QString::fromStdString(txt));
}

#include "VSFinder.h"
#include "Python.h"

#include <vector>
#include <map>
#include <string>

static PyObject *PyCFunc_GetVSInfo(PyObject *self, PyObject *args)
{
    PyObject *pyDict_VSInfo = PyDict_New();

    std::vector<std::map<std::string, val>> vsInfo;

    if (!GetVsInfo(vsInfo))
    {
        return Py_None;
    }

    for (auto &&info : vsInfo)
    {
        for (auto &&entry : info)
        {
            if (entry.first.rfind("__", 0) == 0)
            {
                continue;
            }

            switch (entry.second.valType)
            {
            case STRING_TYPE:
                PyDict_SetItemString(pyDict_VSInfo, entry.first.data(), PyUnicode_FromString(entry.second.stringVal.data()));
                break;
            case NUMBER_TYPE:
                PyDict_SetItemString(pyDict_VSInfo, entry.first.data(), PyLong_FromDouble(entry.second.numberVal));
                break;
            case BOOLEAN_TYPE:
                PyDict_SetItemString(pyDict_VSInfo, entry.first.data(), PyBool_FromLong(static_cast<long>(entry.second.boolVal)));
                break;
            case NULL_TYPE:
                PyDict_SetItemString(pyDict_VSInfo, entry.first.data(), Py_None);
                break;
            }
        }
    }

    return pyDict_VSInfo;
}

static PyMethodDef methods[] = {
    {"GetVSInfo", PyCFunc_GetVSInfo, METH_VARARGS, NULL},
    {NULL, NULL, 0, NULL}};

static struct PyModuleDef module = {
    PyModuleDef_HEAD_INIT,
    "PyVSFinder",
    NULL,
    -1,
    methods};

PyMODINIT_FUNC
PyInit_PyVSFinder(void)
{
    return PyModule_Create(&module);
}
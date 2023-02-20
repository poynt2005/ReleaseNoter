#include "pyglober.h"
#include <string>
#include <Python.h>

#define PY_RT_NOT_INITIALIZE 0xf0
#define PY_RT_INITIALIZED 0xf1
#define PY_RT_INITIALIZED_BEFORE 0xf2

static int pythonRuntimeInitializeDetail = PY_RT_NOT_INITIALIZE;

int PyRTInit()
{
    if (pythonRuntimeInitializeDetail != PY_RT_INITIALIZED_BEFORE)
    {
        Py_SetPythonHome(L"");
        Py_Initialize();
        pythonRuntimeInitializeDetail = PY_RT_INITIALIZED;
        return 1;
    }
    else
    {
        return 0;
    }
}

int PYRTFinal()
{
    if (pythonRuntimeInitializeDetail == PY_RT_INITIALIZED)
    {
        Py_Finalize();
        pythonRuntimeInitializeDetail = PY_RT_INITIALIZED_BEFORE;
        return 1;
    }
    return 0;
}

void PyPrint(PyObject *o)
{
    PyObject *PyModule_Builitins = PyImport_ImportModule("builtins");
    PyObject *PyPrint_Result = PyObject_CallMethod(PyModule_Builitins, "print", "O", o);
    Py_DECREF(PyPrint_Result);
    Py_DECREF(PyModule_Builitins);
}

int PyGlobber(const char *pathname, const char *root_dir, void *dir_fd, const int recursive, const int include_hidden, char ***paths, int *paths_num)
{
    if (pythonRuntimeInitializeDetail != PY_RT_INITIALIZED)
    {
        return 0;
    }

    PyObject *PyModule_Glob = PyImport_ImportModule("glob");

    if (PyModule_Glob == NULL)
    {
        return 0;
    }

    PyObject *PyFunc_GlobFunc = PyObject_GetAttrString(PyModule_Glob, "glob");

    if (PyFunc_GlobFunc == NULL)
    {
        Py_DECREF(PyModule_Glob);
        return 0;
    }

    PyObject *PyGlobFunc_Args = Py_BuildValue("(s)", pathname);
    PyObject *PyGlobFunc_XArgs = PyDict_New();

    if (root_dir != NULL)
    {
        PyDict_SetItemString(PyGlobFunc_XArgs, "root_dir", PyUnicode_FromString(root_dir));
    }

    if (dir_fd != NULL)
    {
        PyDict_SetItemString(PyGlobFunc_XArgs, "dir_fd", PyLong_FromLong(*(reinterpret_cast<long *>(dir_fd))));
    }

    if (recursive)
    {
        PyDict_SetItemString(PyGlobFunc_XArgs, "recursive", PyBool_FromLong(static_cast<const long>(recursive)));
    }

    if (include_hidden)
    {
        PyDict_SetItemString(PyGlobFunc_XArgs, "include_hidden", PyBool_FromLong(static_cast<const long>(include_hidden)));
    }

    PyObject *PyGlobFunc_ExecResult = PyObject_Call(PyFunc_GlobFunc, PyGlobFunc_Args, PyGlobFunc_XArgs);

    Py_DECREF(PyGlobFunc_XArgs);
    Py_DECREF(PyGlobFunc_Args);

    if (PyGlobFunc_ExecResult == NULL)
    {
        Py_DECREF(PyFunc_GlobFunc);
        Py_DECREF(PyModule_Glob);
        return 0;
    }

    if (!PyList_Check(PyGlobFunc_ExecResult))
    {
        Py_DECREF(PyGlobFunc_ExecResult);
        Py_DECREF(PyFunc_GlobFunc);
        Py_DECREF(PyModule_Glob);
        return 0;
    }

    *paths_num = PyList_Size(PyGlobFunc_ExecResult);

    if (*paths_num)
    {
        *paths = new char *[*paths_num];

        for (int i = 0; i < *paths_num; i++)
        {

            PyObject *PyGlobResult_String = PyList_GetItem(PyGlobFunc_ExecResult, i);

            /** borrow ref */
            Py_INCREF(PyGlobResult_String);
            PyObject *PyGlobResult_UnicodeByte = PyUnicode_AsEncodedString(PyGlobResult_String, "utf-8", NULL);
            std::string tmpResultStr(PyBytes_AsString(PyGlobResult_UnicodeByte));
            (*paths)[i] = new char[tmpResultStr.length() + 1];
            strcpy((*paths)[i], tmpResultStr.data());
            Py_DECREF(PyGlobResult_UnicodeByte);
            Py_DECREF(PyGlobResult_String);
        }
    }

    Py_DECREF(PyGlobFunc_ExecResult);
    Py_DECREF(PyFunc_GlobFunc);
    Py_DECREF(PyModule_Glob);

    return 1;
}
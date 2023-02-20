#include "utilities.h"
#include <string>
#include <Python.h>

static PyObject *PyCFunc_GetScreenResolution(PyObject *self, PyObject *args)
{
    int screenX, screenY;
    Utilities::GetScreenSize(screenX, screenY);

    PyObject *pyScreenTuple = PyTuple_New(2);

    PyTuple_SetItem(pyScreenTuple, 0, PyLong_FromLong(screenX));
    PyTuple_SetItem(pyScreenTuple, 1, PyLong_FromLong(screenY));

    return pyScreenTuple;
}

static PyObject *PyCFunc_ShowMsgBox(PyObject *self, PyObject *args)
{
    const char *title = NULL;
    const char *msg = NULL;
    if (!PyArg_ParseTuple(args, "ss", &title, &msg))
    {
        PyErr_SetString(PyExc_ValueError, "Wrong Arguments, expected two strings");
        return NULL;
    }

    Utilities::ShowMsgBox(std::string(title), std::string(msg));
    return Py_None;
}

static PyMethodDef methods[] = {
    {"GetScreenResolution", PyCFunc_GetScreenResolution, METH_VARARGS, NULL},
    {"ShowMsgBox", PyCFunc_ShowMsgBox, METH_VARARGS, NULL},
    {NULL, NULL, 0, NULL}};

static PyModuleDef module = {
    PyModuleDef_HEAD_INIT,
    "PyUsefulModule",
    NULL,
    -1,
    methods,
    NULL,
    NULL,
    NULL,
    NULL};

PyMODINIT_FUNC
PyInit_PyUsefulModule(void)
{
    return PyModule_Create(&module);
}
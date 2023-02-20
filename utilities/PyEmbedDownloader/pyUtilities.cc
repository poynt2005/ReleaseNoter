#include <Python.h>
#include <windows.h>

static PyObject *PyCFunc_DownloadFile(PyObject *self, PyObject *args)
{
    const char *filename = NULL;
    const char *url = NULL;

    if (!PyArg_ParseTuple(args, "ss", &filename, &url))
    {
        PyErr_SetString(PyExc_ValueError, "Wrong Arguments, excepted two strings");
        return NULL;
    }

    HRESULT hr = URLDownloadToFile(0, url, filename, 0, 0);

    if (hr == S_OK)
    {
        return PyBool_FromLong(1);
    }
    else
    {
        return PyBool_FromLong(0);
    }
}

static PyMethodDef methods[] = {
    {"DownloadFile", PyCFunc_DownloadFile, METH_VARARGS, NULL},
    {NULL, NULL, 0, NULL}};

static struct PyModuleDef module = {
    PyModuleDef_HEAD_INIT,
    "PyBuildUtilities",
    NULL,
    -1,
    methods};

PyMODINIT_FUNC
PyInit_PyBuildUtilities(void)
{
    return PyModule_Create(&module);
}
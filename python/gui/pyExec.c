#include <Python.h>
#include <windows.h>
#include <stdio.h>
int main(int argc, char **argv)
{
    Py_SetPythonHome(L"");
    Py_Initialize();
    PyObject *pyMainExecObject = PyImport_ImportModule("main");

    if (pyMainExecObject == NULL)
    {
        printf("Get python main execute bytecode failed");
        return 1;
    }

    PyObject *pyRunMainResult;

    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE);

    if (argc > 1)
    {
        pyRunMainResult = PyObject_CallMethod(pyMainExecObject, "run_main", "s", argv[1]);
    }
    else
    {
        pyRunMainResult = PyObject_CallMethod(pyMainExecObject, "run_main", "i", 1);
    }

    PyErr_Print();

    if (pyRunMainResult == NULL)
    {
        printf("Run python main execute bytecode failed");
        return 1;
    }

    Py_DECREF(pyRunMainResult);
    Py_DECREF(pyMainExecObject);
    Py_Finalize();
    return 0;
}
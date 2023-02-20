#include <Python.h>
#include "../ReleaseNoter.h"
#include <map>
#include <windows.h>
#include <memory>
#include <string>

static std::map<std::string, std::shared_ptr<ReleaseNoter>> noterMapping;

static PyObject *PyCFunc_NewNoter(PyObject *self, PyObject *args)
{

    int oldestPeriodDate;

    if (!PyArg_ParseTuple(args, "i", &oldestPeriodDate))
    {
        PyErr_SetString(PyExc_ValueError, "Wrong Argument, excepted one int");
        return NULL;
    }

    static auto createUUID = []() -> std::string
    {
        UUID uuid;
        UuidCreate(&uuid);
        char *str;
        UuidToStringA(&uuid, reinterpret_cast<RPC_CSTR *>(&str));
        std::string uuidStr(str);
        RpcStringFreeA(reinterpret_cast<RPC_CSTR *>(&str));
        return uuidStr;
    };

    std::shared_ptr<ReleaseNoter> noterPtr = std::make_shared<ReleaseNoter>(oldestPeriodDate);
    std::string noterId = createUUID();

    noterMapping.insert(std::pair<std::string, std::shared_ptr<ReleaseNoter>>(noterId, noterPtr));

    return PyUnicode_FromString(noterId.data());
}

static PyObject *PyCFunc_SetCopierDllPath(PyObject *self, PyObject *args)
{
    const char *noterId = NULL;
    const char *dllPath = NULL;

    if (!PyArg_ParseTuple(args, "ss", &noterId, &dllPath))
    {
        PyErr_SetString(PyExc_ValueError, "Wrong Arguments, excepted two string");
        return NULL;
    }

    auto noterInstance = noterMapping.at(std::string(noterId));

    noterInstance->SetClipboardCopierDllPath(std::string(dllPath));

    return Py_None;
}

static PyObject *PyCFunc_FindGitRepository(PyObject *self, PyObject *args)
{
    const char *noterId = NULL;
    PyObject *pyCallbackFn = NULL;

    if (!PyArg_ParseTuple(args, "s|O", &noterId, &pyCallbackFn))
    {
        PyErr_SetString(PyExc_ValueError, "Wrong Arguments, excepted string and callback addition");
        return NULL;
    }

    if (pyCallbackFn != NULL && !PyCallable_Check(pyCallbackFn))
    {
        PyErr_SetString(PyExc_ValueError, "Wrong Argument, position 1 excepted callable");
        return NULL;
    }

    auto noterInstance = noterMapping.at(std::string(noterId));
    std::string gitFolder;
    bool isRepositoryFound = noterInstance->FindGitRepository(gitFolder);

    PyObject *pyCallCBResult;
    if (isRepositoryFound)
    {
        pyCallCBResult = PyObject_CallFunction(pyCallbackFn, "s", gitFolder.data());
    }
    else
    {
        PyObject *tmpPyNone = Py_None;
        pyCallCBResult = PyObject_CallFunction(pyCallbackFn, "O", tmpPyNone);
        Py_DECREF(tmpPyNone);
    }

    Py_DECREF(pyCallCBResult);

    return PyBool_FromLong(static_cast<long>(isRepositoryFound));
}

static PyObject *PyCFunc_ListCommit(PyObject *self, PyObject *args)
{
    const char *noterId = NULL;

    if (!PyArg_ParseTuple(args, "s", &noterId))
    {
        PyErr_SetString(PyExc_ValueError, "Wrong Argument, excepted one string");
        return NULL;
    }
    auto noterInstance = noterMapping.at(std::string(noterId));

    bool isCommitList = noterInstance->ListCommit();

    if (!isCommitList)
    {
        return Py_False;
    }

    return Py_True;
}

static PyObject *PyCFunc_RemoveCommitByIndex(PyObject *self, PyObject *args)
{
    const char *noterId = NULL;
    PyObject *pyToRemoveList = NULL;

    if (!PyArg_ParseTuple(args, "sO", &noterId, &pyToRemoveList))
    {
        PyErr_SetString(PyExc_ValueError, "Wrong Arguments, excepted string and List");
        return NULL;
    }

    if (!PyList_Check(pyToRemoveList))
    {
        PyErr_SetString(PyExc_ValueError, "Wrong Argument, position 1 expected one List");
        return NULL;
    }

    std::vector<int> toRemove;

    for (int i = 0; i < PyList_Size(pyToRemoveList); i++)
    {
        PyObject *pyToRemoveIdx = PyList_GetItem(pyToRemoveList, i);
        if (!PyLong_Check(pyToRemoveIdx))
        {
            continue;
        }
        toRemove.emplace_back(static_cast<int>(PyLong_AsLong(pyToRemoveIdx)));
    }

    auto noterInstance = noterMapping.at(std::string(noterId));

    noterInstance->RemoveCommitByIndex(toRemove);

    return Py_None;
}

static PyObject *PyCFunc_GetCurrentListCommits(PyObject *self, PyObject *args)
{
    const char *noterId = NULL;

    if (!PyArg_ParseTuple(args, "s", &noterId))
    {
        PyErr_SetString(PyExc_ValueError, "Wrong Argument, excepted one string");
        return NULL;
    }

    auto noterInstance = noterMapping.at(std::string(noterId));

    std::vector<CommitInfo> commitInfos;
    noterInstance->GetCurrentListCommits(commitInfos);

    PyObject *pyCommitInfoList = PyList_New(commitInfos.size());

    for (int i = 0; i < commitInfos.size(); i++)
    {
        auto commitInfo = std::move(commitInfos[i]);

        PyObject *pyCommitInfos = PyDict_New();

        PyDict_SetItemString(pyCommitInfos, "asanaURL", PyUnicode_FromString(commitInfo.asanaURL.data()));
        PyDict_SetItemString(pyCommitInfos, "asanaID", PyUnicode_FromString(commitInfo.asanaID.data()));
        PyDict_SetItemString(pyCommitInfos, "mantisURL", PyUnicode_FromString(commitInfo.mantisURL.data()));
        PyDict_SetItemString(pyCommitInfos, "mantisID", PyUnicode_FromString(commitInfo.mantisID.data()));
        PyDict_SetItemString(pyCommitInfos, "title", PyUnicode_FromString(commitInfo.title.data()));
        PyDict_SetItemString(pyCommitInfos, "author", PyUnicode_FromString(commitInfo.author.data()));
        PyDict_SetItemString(pyCommitInfos, "authorMail", PyUnicode_FromString(commitInfo.authorMail.data()));
        PyDict_SetItemString(pyCommitInfos, "difftime", PyLong_FromLong(commitInfo.difftime));

        PyList_SetItem(pyCommitInfoList, i, pyCommitInfos);
    }

    return pyCommitInfoList;
}

static PyObject *PyCFunc_CopyReleaseNoteToClipboard(PyObject *self, PyObject *args)
{
    const char *noterId = NULL;

    if (!PyArg_ParseTuple(args, "s", &noterId))
    {
        PyErr_SetString(PyExc_ValueError, "Wrong Argument, excepted one string");
        return NULL;
    }

    auto noterInstance = noterMapping.at(std::string(noterId));

    bool isClipboardCopyied = noterInstance->CopyReleaseNoteToClipboard();

    return PyBool_FromLong(static_cast<long>(isClipboardCopyied));
}

static PyMethodDef methods[] = {
    {"NewNoter", PyCFunc_NewNoter, METH_VARARGS, NULL},
    {"SetCopierDllPath", PyCFunc_SetCopierDllPath, METH_VARARGS, NULL},
    {"FindGitRepository", PyCFunc_FindGitRepository, METH_VARARGS, NULL},
    {"ListCommit", PyCFunc_ListCommit, METH_VARARGS, NULL},
    {"RemoveCommitByIndex", PyCFunc_RemoveCommitByIndex, METH_VARARGS, NULL},
    {"GetCurrentListCommits", PyCFunc_GetCurrentListCommits, METH_VARARGS, NULL},
    {"CopyReleaseNoteToClipboard", PyCFunc_CopyReleaseNoteToClipboard, METH_VARARGS, NULL},
    {NULL, NULL, 0, NULL}};

static struct PyModuleDef module = {
    PyModuleDef_HEAD_INIT,
    "PyReleaseNoter",
    NULL,
    -1,
    methods};

PyMODINIT_FUNC
PyInit_PyReleaseNoter(void)
{
    return PyModule_Create(&module);
}

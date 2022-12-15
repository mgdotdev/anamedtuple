#include <Python.h>

static PyObject* _anamedtuple(PyObject* self, PyObject* args) {

    PyObject* __dict__ = PyObject_GetAttrString(self, "__dict__");
    PyObject* _cache = PyDict_GetItemString(__dict__, "_cache");

    if (!_cache) {
        _cache = PyDict_New();
        PyDict_SetItemString(__dict__, "_cache", _cache);
    }
    Py_DECREF(__dict__);

    PyObject* _type = PyDict_GetItem(_cache, args);

    if (!_type) {

        PyObject* _builtins = PyImport_ImportModule(
                "builtins"
        );
        PyObject* _collections = PyImport_ImportModule(
                "_collections"
        );
        PyObject* _tuplegetter = PyObject_GetAttrString(
                _collections, "_tuplegetter"
        );
        PyObject* tuple = PyObject_GetAttrString(
                _builtins, "tuple"
        );
        PyObject* type = PyObject_GetAttrString(
                _builtins, "type"
        );

        PyObject* methods = PyDict_New();

        PyObject* key;
        for (long i = 0; i < PyTuple_Size(args); i++) {
            key = PyTuple_GetItem(args, i);
            Py_INCREF(key);
            PyObject* tg_args = PyTuple_Pack(2, PyLong_FromLong(i), key);
            PyDict_SetItem(
                methods, key, PyObject_CallObject(_tuplegetter, tg_args)
            );
        }

        PyObject* bases = PyTuple_Pack(1, tuple);

        PyObject* name = PyUnicode_FromString("anamedtuple");

        PyObject* t_args = PyTuple_Pack(3, name, bases, methods);

        _type = PyObject_CallObject(type, t_args);

        PyDict_SetItem(_cache, args, _type);
        Py_INCREF(args);

        Py_DECREF(_builtins);
        Py_DECREF(_collections);
        Py_DECREF(tuple);
        Py_DECREF(type);
    }

    return _type;
}

static PyObject* anamedtuple(PyObject* self, PyObject* args, PyObject* kwargs) {

    if (PyTuple_Size(args)) {
        // due to caching, anamedtuple(0, x=1) would have the same type as
        // anamedtuple(x=1), throwing off the cached _tuplegetter. So just make
        // function kwd only.
        PyErr_SetString(
            PyExc_TypeError,
            "anamedtuple() takes no positional arguments"
        );
        return NULL;
    }

    long kwd_len = PyDict_Size(kwargs);

    PyObject *keys = PyTuple_New(kwd_len);
    PyObject *vals = PyTuple_New(kwd_len);

    PyObject *key, *val;
    Py_ssize_t pos = 0;
    while (PyDict_Next(kwargs, &pos, &key, &val)) {
        PyTuple_SET_ITEM(keys, pos-1, key);
        PyTuple_SET_ITEM(vals, pos-1, val);
        Py_INCREF(key);
        Py_INCREF(val);
    }

    PyObject* _type = _anamedtuple(self, keys);

    PyObject* instance = PyObject_CallOneArg(_type, vals);

    return instance;
}

static PyMethodDef anamedtuple_methods[] = {
    {
        "anamedtuple",
        (PyCFunctionWithKeywords)anamedtuple,
        METH_VARARGS|METH_KEYWORDS,
        "docs",
    },
    {
        "_anamedtuple",
        (PyCFunction)_anamedtuple,
        METH_VARARGS,
        "docs",
    },
    {
        NULL,
        NULL,
        0,
        NULL
    }
};

static struct PyModuleDef anamedtuple_module = {
    PyModuleDef_HEAD_INIT,
    "anamedtuple",
    NULL,
    -1,
    anamedtuple_methods,
};

PyMODINIT_FUNC PyInit_anamedtuple(void) {
    return PyModule_Create(&anamedtuple_module);
};


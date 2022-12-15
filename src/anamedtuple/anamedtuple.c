#include <Python.h>

void PyDebug() {return;}

static PyObject* _anamedtuple(PyObject* self, PyObject* _args) {

    if (PyTuple_Size(_args) != 2) {
        PyErr_SetString(
            PyExc_TypeError,
            "_anamedtuple() requires two positional arguments"
        );
        return NULL;
    }
    PyObject *name = PyTuple_GetItem(_args, 0);

    if (!name || !PyUnicode_Check(name)) {
        PyErr_SetString(
            PyExc_TypeError,
            "name must be unicode string"
        );
        return NULL;
    }

    Py_INCREF(name);

    PyObject *args = PyTuple_GetItem(_args, 1);

    PyObject* __dict__ = PyObject_GetAttrString(self, "__dict__");
    PyObject* _cache = PyDict_GetItemString(__dict__, "_cache");

    if (!_cache) {
        _cache = PyDict_New();
        PyDict_SetItemString(__dict__, "_cache", _cache);
    }
    else {
        Py_INCREF(_cache);
    }

    Py_DECREF(__dict__);

    PyObject* _type = PyDict_GetItem(_cache, args);

    if (!_type) {

        PyObject* _builtins = PyImport_ImportModule("builtins");
        PyObject* tuple = PyObject_GetAttrString(_builtins, "tuple");
        PyObject* type = PyObject_GetAttrString(_builtins, "type");

        PyObject* _collections = PyImport_ImportModule("_collections");
        PyObject* _tuplegetter = PyObject_GetAttrString(
                _collections, "_tuplegetter"
        );

        PyObject* methods = PyDict_New();

        PyObject* key;
        for (long i = 0; i < PyTuple_Size(args); i++) {
            key = PyTuple_GetItem(args, i);
            Py_INCREF(key);
            PyObject* index = PyLong_FromLong(i);
            PyObject* tg_args = PyTuple_Pack(2, index, key);
            PyDict_SetItem(
                methods, key, PyObject_CallObject(_tuplegetter, tg_args)
            );
            Py_DECREF(index);
            Py_DECREF(tg_args);
        }

        PyObject* bases = PyTuple_Pack(1, tuple);
        PyObject* t_args = PyTuple_Pack(3, name, bases, methods);
        _type = PyObject_CallObject(type, t_args);

        PyDict_SetItem(_cache, args, _type);
        Py_INCREF(args);

        Py_DECREF(_builtins);
        Py_DECREF(_collections);
        Py_DECREF(tuple);
        Py_DECREF(type);
    }
    else {
        Py_INCREF(_type);
    }

    Py_DECREF(_cache);

    return _type;
}

static PyObject* anamedtuple(PyObject* self, PyObject* args, PyObject* kwargs) {

    long arg_size = PyTuple_Size(args);
    if (arg_size > 1) {
        PyErr_SetString(
            PyExc_TypeError,
            "anamedtuple() takes only one positional argument"
        );
        return NULL;
    }

    if (!kwargs || !PyDict_Size(kwargs)) {
        PyErr_SetString(
            PyExc_TypeError,
            "anamedtuple() expects keyword arguments"
        );
        return NULL;
    }

    PyObject* name;
    if (arg_size) {
        name = PyTuple_GetItem(args, 0);
        Py_INCREF(name);
    }
    else {
        // name = PyUnicode_FromString("anamedtuple");
        name = PyModule_GetNameObject(self);
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

    PyObject* _args = PyTuple_Pack(2, name, keys);
    PyObject* _type = _anamedtuple(self, _args);

    PyObject* instance = PyObject_CallOneArg(_type, vals);

    Py_DECREF(_args);
    Py_DECREF(vals);
    Py_DECREF(keys);
    Py_DECREF(_type);
    Py_DECREF(name);

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


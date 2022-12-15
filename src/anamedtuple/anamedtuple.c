#include <Python.h>

void PyDebug() {return;}

static PyObject* _anamedtuple(PyObject* self, PyObject* args) {

    if (PyTuple_Size(args) != 2) {
        PyErr_SetString(
            PyExc_TypeError,
            "_anamedtuple() requires two positional args"
        );
        return NULL;
    }

    PyObject *name = PyTuple_GetItem(args, 0);

    if (!name || !PyUnicode_Check(name)) {
        PyErr_SetString(
            PyExc_TypeError,
            "name must be unicode string"
        );
        return NULL;
    }

    PyObject *attrs = PyTuple_GetItem(args, 1);

    PyObject* _builtins = PyImport_ImportModule("builtins");
    PyObject* tuple = PyObject_GetAttrString(_builtins, "tuple");
    PyObject* type = PyObject_GetAttrString(_builtins, "type");

    PyObject* _collections = PyImport_ImportModule("_collections");
    PyObject* _tuplegetter = PyObject_GetAttrString(
            _collections, "_tuplegetter"
    );

    PyObject* methods = PyDict_New();

    PyObject* key;
    for (long i = 0; i < PyTuple_Size(attrs); i++) {
        key = PyTuple_GetItem(attrs, i);

        PyObject* index = PyLong_FromLong(i);
        PyObject* _tg_args = PyTuple_Pack(2, index, key);
        PyObject* val = PyObject_CallObject(_tuplegetter, _tg_args);
        PyDict_SetItem(methods, key, val);

        Py_DECREF(val);
        Py_DECREF(index);
        Py_DECREF(_tg_args);
    }

    PyObject* bases = PyTuple_Pack(1, tuple);
    PyObject* t_args = PyTuple_Pack(3, name, bases, methods);
    PyObject* _type = PyObject_CallObject(type, t_args);

    Py_DECREF(methods);

    Py_DECREF(bases);
    Py_DECREF(t_args);

    Py_DECREF(_builtins);
    Py_DECREF(tuple);
    Py_DECREF(type);
    Py_DECREF(_collections);
    Py_DECREF(_tuplegetter);

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
        Py_INCREF(key); Py_INCREF(val);
        PyTuple_SET_ITEM(keys, pos-1, key);
        PyTuple_SET_ITEM(vals, pos-1, val);
    }

    PyObject* __dict__ = PyObject_GetAttrString(self, "__dict__");
    PyObject* _cache = PyDict_GetItemString(__dict__, "_cache");

    if (!_cache) {
        _cache = PyDict_New();
        PyDict_SetItemString(__dict__, "_cache", _cache);
    }
    else {
        Py_INCREF(_cache);
    }

    PyObject* _type = PyDict_GetItem(_cache, keys);

    if (!_type) {
        PyObject* _args = PyTuple_Pack(2, name, keys);
        _type = _anamedtuple(self, _args);
        PyDict_SetItem(_cache, keys, _type);
        Py_DECREF(_args);
    }
    else {
        Py_INCREF(_type);
    }

    PyObject* instance = PyObject_CallOneArg(_type, vals);

    Py_DECREF(__dict__);
    Py_DECREF(_cache);

    Py_DECREF(name);
    Py_DECREF(vals);
    Py_DECREF(keys);

    Py_DECREF(_type);

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


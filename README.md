Anonymous NamedTuples
---

Python's standard library provides a tuple-type factory function which
generates NamedTuple types.

```python
>>> from collections import namedtuple

>>> _type = namedtuple("MyTuple", ('x', 'y'))
>>> item = _type(1, 2)
>>> item
>>> MyTuple(x=1, y=2)
```

these types are defined explicitly in two steps with ordered collections, as
earlier implementations of python lacked ordered dictionaries.

With however the introduction of the python's ordered dict type as the default
dictionary implementation, it became possible to implement namedtuples with
only the keyword arguments of a function signature.

This library is an implementaiton of that.

```python
>>> from anamedtuple import anamedtuple

>>> instance = anamedtuple("MyTuple", x=1, y=2)
>>> instance
(1, 2)
```

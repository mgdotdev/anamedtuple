import pytest

from anamedtuple import anamedtuple, _anamedtuple

class TestANamedTuple:
    def test_kwargs(self):
        instance = anamedtuple(x=5, y=6)
        assert instance.x == 5
        assert instance.y == 6

    def test_named_kwargs(self):
        instance = anamedtuple("MyType", x=5, y=6)
        assert instance.x == 5
        assert instance.y == 6

    def test_args(self):
        with pytest.raises(TypeError) as err:
            anamedtuple(1)

        assert isinstance(err.value, TypeError)

    def test_type_generation(self):
        _type = _anamedtuple("MyType", ('x', 'y'))
        instance = _type((5, 6))
        assert instance.x == 5
        assert instance.y == 6

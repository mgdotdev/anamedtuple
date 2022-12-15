import pytest

from anamedtuple import anamedtuple

class TestANamedTuple:
    def test_kwargs(self):
        at = anamedtuple(x=5, y=6)
        assert at.x == 5
        assert at.y == 6


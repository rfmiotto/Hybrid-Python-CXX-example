from myproject import core
import numpy as np


def test_multiply_in_place():
    arr = np.array([1.0, 2.0, 3.0])
    core.multiply_in_place(arr, 2.0)
    assert np.array_equal(arr, np.array([2.0, 4.0, 6.0]))

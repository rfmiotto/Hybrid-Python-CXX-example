from myproject import core
import numpy as np

def test_fast_sum():
    arr = np.array([1.0, 2.0, 3.0])
    assert core.fast_sum(arr) == 6.0

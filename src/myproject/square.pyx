# cython: language_level=3
import numpy as np
cimport numpy as np

# Function to square the NumPy array element-wise
def square(np.ndarray[np.float64_t] x):
    # Ensure x is a 1D array
    print(x.shape)
    if x.ndim != 1:
        raise ValueError("Input must be a 1D NumPy array")

    # Element-wise operation
    return x * x

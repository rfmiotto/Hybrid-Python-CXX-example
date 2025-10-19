import numpy as np
from myproject import core

# Create a NumPy vector
vec = np.arange(10, dtype=np.float64)
print("Original vector:", vec)

# Multiply by scalar in C++ (in-place)
core.multiply_in_place(vec, 2.0)
print("After multiply_in_place:", vec)

# Compute dot product (C++)
result = core.dot_product(vec, vec)
print("Dot product:", result)

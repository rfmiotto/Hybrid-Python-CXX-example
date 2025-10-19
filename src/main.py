import myproject
import numpy as np

def main():
    data = np.random.rand(1_000_000)  # NumPy array, should be float64
    print("Data type:", type(data))  # This will tell you the type of data
    print("Data shape:", data.shape)  # Check the shape of the array
    print("Sum =", myproject.square(data).sum())

if __name__ == "__main__":
    main()

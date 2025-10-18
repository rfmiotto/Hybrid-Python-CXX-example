from myproject import core
import numpy as np

def main():
    data = np.random.rand(1_000_000)
    print("Sum =", core.fast_sum(data))

if __name__ == "__main__":
    main()

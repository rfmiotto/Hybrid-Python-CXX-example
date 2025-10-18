#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>

namespace py = pybind11;

// Example: fast sum of array elements
double fast_sum(py::array_t<double> arr) {
  auto buf = arr.request();
  const double *ptr = static_cast<double *>(buf.ptr);
  double sum = 0.0;
  for (ssize_t i = 0; i < buf.size; ++i) {
    sum += ptr[i];
  }
  return sum;
}

PYBIND11_MODULE(core, m) {
  m.doc() = "High-performance C++ extensions for myproject";
  m.def("fast_sum", &fast_sum, "Sum elements of a NumPy array");
}

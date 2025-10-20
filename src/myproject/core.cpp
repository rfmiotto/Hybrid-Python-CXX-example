#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>

#include <Eigen/Dense>

namespace py = pybind11;
using Eigen::VectorXd;

// Multiply a NumPy vector by a scalar (in-place)
void multiply_in_place(py::array_t<double> arr, double scalar) {
  py::buffer_info buf = arr.request();
  double *ptr = static_cast<double *>(buf.ptr);
  size_t n = buf.size;

  // Map NumPy memory to an Eigen vector (no copy!)
  Eigen::Map<VectorXd> vec(ptr, n);
  vec *= scalar;  // Fast Eigen operation
}

// Compute dot product between two vectors
double dot_product(py::array_t<double> a, py::array_t<double> b) {
  py::buffer_info buf_a = a.request(), buf_b = b.request();
  if (buf_a.size != buf_b.size) throw std::runtime_error("Vectors must have the same size");

  Eigen::Map<VectorXd> va(static_cast<double *>(buf_a.ptr), buf_a.size);
  Eigen::Map<VectorXd> vb(static_cast<double *>(buf_b.ptr), buf_b.size);

  return va.dot(vb);
}

PYBIND11_MODULE(core, m) {
  m.doc() = "Example hybrid C++ module using pybind11 and Eigen";
  m.def("multiply_in_place", &multiply_in_place, "Multiply vector by scalar in place");
  m.def("dot_product", &dot_product, "Compute dot product between two vectors");
}

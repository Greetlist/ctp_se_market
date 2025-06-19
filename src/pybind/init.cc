#include <pybind11/pybind11.h>

namespace py = pybind11;

void init_reader(py::module&);
void init_struct(py::module&);

PYBIND11_MODULE(libmmap_reader, m) {
  m.doc() = "mmap python reader";
  init_reader(m);
  init_struct(m);
}

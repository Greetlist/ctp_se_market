#include <pybind11/pybind11.h>
#include <string>

#include "mmap_reader.h"
#include "market_data.h"

namespace py = pybind11;

void init_reader(py::module& m) {
  py::class_<MMapReader<FutureMarketData>>(m, "FutureMarketMMapReader")
    .def(py::init<std::string&>())
    .def("Init", &MMapReader<FutureMarketData>::Init)
    .def("GetStructSize", &MMapReader<FutureMarketData>::GetStructSize)
    .def("GetDataCount", &MMapReader<FutureMarketData>::GetDataCount)
    .def("ReadData", &MMapReader<FutureMarketData>::ReadData);
}

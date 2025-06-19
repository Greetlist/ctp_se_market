#include <pybind11/pybind11.h>

#include "market_data.h"

namespace py = pybind11;

void init_struct(py::module& m) {
  m.doc() = "future market struct";
  py::class_<FutureMarketData>(m, "FutureMarketData")
    .def(py::init<>())
    .def_readonly("last_price", &FutureMarketData::last_price)
    .def_readonly("settlement_price", &FutureMarketData::settlement_price)
    .def_readonly("pre_close_price", &FutureMarketData::pre_close_price)
    .def_readonly("high", &FutureMarketData::high)
    .def_readonly("open", &FutureMarketData::open)
    .def_readonly("low", &FutureMarketData::low)
    .def_readonly("close", &FutureMarketData::close)
    .def_readonly("hold_volume", &FutureMarketData::hold_volume)
    .def_readonly("uid", &FutureMarketData::uid)
    .def_readonly("vendor_update_time", &FutureMarketData::vendor_update_time);
}

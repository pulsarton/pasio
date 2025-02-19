#include <pybind11/pybind11.h>

#include <string>

#include <pasio/async/async_serial_port.hpp>
#include <pasio/sync/sync_serial_port.hpp> // IWYU pragma: keep{}

namespace py = pybind11;

PYBIND11_MODULE(pasio, async) {
    // py::class_<pasio::sync::serial_port_wrapper>(sync, "SyncSerialPort")
    //     .def(py::init<const std::string&, unsigned int>())
    //     .def("write", &pasio::sync::serial_port_wrapper::write)
    //     .def("read", &pasio::sync::serial_port_wrapper::read);
    py::class_<pasio::async::async_serial_port>(async, "AsyncSerialPort")
        .def(py::init<const std::string&, unsigned int>())
        .def("write", &pasio::async::async_serial_port::write)
        .def("read", &pasio::async::async_serial_port::read)
        .def("close", &pasio::async::async_serial_port::close);
}
#include <pybind11/pybind11.h>

#include <memory>

#include <pybind11/cast.h>
#include <pybind11/functional.h>

#include <string>

#include <pasio/async/async_serial_port.hpp>
#include <pasio/sync/sync_serial_port.hpp> // IWYU pragma: keep{}

namespace py = pybind11;

using py::literals::operator""_a;

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

    py::class_<pasio::async::callback_port, std::shared_ptr<pasio::async::callback_port>>(async, "CallbackSerialPort")
        .def(py::init(&pasio::async::callback_port::make_callback_port))
        .def("async_read", &pasio::async::callback_port::async_read, "bytes"_a, "callback"_a)
        .def("async_write", &pasio::async::callback_port::async_write, "data"_a, "callback"_a);
}
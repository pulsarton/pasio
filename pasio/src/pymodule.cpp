#include <pybind11/attr.h>
#include <pybind11/gil.h>
#include <pybind11/pybind11.h>

#include <memory>

#include <pybind11/cast.h>
#include <pybind11/functional.h>

#include <pybind11/pytypes.h>
#include <string>

#include <pasio/async/async_serial_port.hpp>
#include <pasio/sync/sync_serial_port.hpp> // IWYU pragma: keep{}

namespace py = pybind11;

using py::literals::operator""_a;

std::string async_task(int seconds) {
    std::this_thread::sleep_for(std::chrono::seconds(seconds)); // Simulate delay
    return "Task completed!";
}

// Wrapper function that returns a Python coroutine (async def function)
py::object async_function(int seconds) {
    py::gil_scoped_acquire gil;
    py::object loop = py::module_::import("asyncio").attr("get_event_loop")();
    return loop.attr("run_in_executor")(py::none(), py::cpp_function(async_task), seconds);
}

PYBIND11_MODULE(pasio, async) {
    async.def("async_function", &async_function, "An asynchronous function", py::arg("seconds"));

    py::class_<pasio::sync::serial_port_wrapper>(async, "SyncSerialPort")
        .def(py::init<const std::string&, unsigned int>())
        .def("write", &pasio::sync::serial_port_wrapper::write)
        .def("read", &pasio::sync::serial_port_wrapper::read);

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
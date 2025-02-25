#include <asio/serial_port.hpp>
#include <pybind11/attr.h>
#include <pybind11/detail/common.h>
#include <pybind11/gil.h>
#include <pybind11/pybind11.h>

#include <memory>

#include <pybind11/cast.h>
#include <pybind11/functional.h>

#include <pybind11/pytypes.h>
#include <pyerrors.h>
#include <string>

#include <pasio/errors.hpp>
#include <pasio/serial_port.hpp>

namespace py = pybind11;

using py::literals::operator""_a;

PYBIND11_MODULE(pasio, async) {
    async.doc() = "Python bindings to asio::serial_port";

    /** SP - serial port */
    py::register_exception<pasio::os_error>(async, "SPError", PyExc_OSError);

    py::class_<pasio::async::serial_port, std::shared_ptr<pasio::async::serial_port>>(async, "SerialPort")
        .def(py::init(&pasio::async::serial_port::make))
        .def("write", &pasio::async::serial_port::write, "data"_a,
             R"pbdoc(
               Write data to serial port synchronously

               :param data: data to write
               :type data: str
               )pbdoc")
        .def("read", &pasio::async::serial_port::read, "bytes"_a,
             R"pbdoc(
               Read data from serial port synchronously

               :param bytes: bytes amount to read
               :type bytes: int
               :rtype: str
               )pbdoc")
        .def("async_write", py::overload_cast<py::object, std::string>(&pasio::async::serial_port::async_write),
             "executor"_a, "data"_a,
             R"pbdoc(
               Writes data to serial port asynchronously on passed executor

               :param executor: executor to run on
               :param data: data to write
               :type data: str
               )pbdoc")
        .def("async_read", py::overload_cast<py::object, std::size_t>(&pasio::async::serial_port::async_read),
             "executor"_a, "bytes"_a,
             R"pbdoc(
               Reads data from serial port asynchronously on passed executor

               :param executor: executor to run on
               :type executor: 
               :param bytes: bytes amount to read
               :type data: int
               :rtype: str
               )pbdoc")
        .def("async_write", py::overload_cast<std::string>(&pasio::async::serial_port::async_write), "data"_a,
             R"pbdoc(
               Writes data to serial port asynchronously using default executor

               :param data: bytes to write
               :type data: 
               )pbdoc")
        .def("async_read", py::overload_cast<std::size_t>(&pasio::async::serial_port::async_read), "bytes"_a,
             R"pbdoc(
               Reads data from serial port asynchronously using default executor

               :param bytes: bytes amount to read
               :type data: int
               :rtype: str
               )pbdoc")
        .def("close", &pasio::async::serial_port::close, R"pbdoc(
               Closes serial port    
               )pbdoc")
        .def("set_baud_rate", &pasio::async::serial_port::set_option<asio::serial_port::baud_rate, unsigned int>,
             "baud_rate"_a,
             R"pbdoc(
               Sets baud rate

               :param baud_rate: baud rate
               :type baud_rate: int
               )pbdoc");
}
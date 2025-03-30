#include <pybind11/pybind11.h> // IWYU pragma: keep

#include <asio/serial_port.hpp>
#include <memory>
#include <pasio/serial_port.hpp>
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
    py::enum_<asio::serial_port::flow_control::type>(async, "FlowControl")
        .value("none", asio::serial_port::flow_control::type::none)
        .value("software", asio::serial_port::flow_control::type::software)
        .value("hardware", asio::serial_port::flow_control::type::hardware);

    py::enum_<asio::serial_port::stop_bits::type>(async, "StopBits")
        .value("one", asio::serial_port::stop_bits::type::one)
        .value("onepointfive", asio::serial_port::stop_bits::type::onepointfive)
        .value("two", asio::serial_port::stop_bits::type::two);

    py::enum_<asio::serial_port::parity::type>(async, "Parity")
        .value("none", asio::serial_port::parity::type::none)
        .value("even", asio::serial_port::parity::type::even)
        .value("odd", asio::serial_port::parity::type::odd);

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
               )pbdoc")
        .def("set_character_size",
             &pasio::async::serial_port::set_option<asio::serial_port::character_size, unsigned int>, "char_size"_a,
             R"pbdoc(
                Sets character size

                :param char_size: char size
                :type char_size: int
                )pbdoc")
        .def("set_flow_control",
             &pasio::async::serial_port::set_option<asio::serial_port::flow_control,
                                                    asio::serial_port::flow_control::type>,
             "flow_control"_a,
             R"pbdoc(
                Sets flow control

                :param flow_control: flow control
                :type flow_control: FlowControl
                )pbdoc")
        .def("set_stop_bits",
             &pasio::async::serial_port::set_option<asio::serial_port::stop_bits, asio::serial_port::stop_bits::type>,
             "stop_bits"_a,
             R"pbdoc(
                Sets stop bits

                :param stop_bits: stop bits 
                :type stop_bits: StopBits
                )pbdoc")
        .def("set_parity",
             &pasio::async::serial_port::set_option<asio::serial_port::parity, asio::serial_port::parity::type>,
             "parity"_a,
             R"pbdoc(
                Sets parity

                :param parity: parity 
                :type parity: Parity
                )pbdoc")
        .def("get_baud_rate", &pasio::async::serial_port::get_option<asio::serial_port::baud_rate, unsigned int>,
             R"pbdoc(
                Gets baud rate

                :rtype: int
                )pbdoc")
        .def("get_character_size",
             &pasio::async::serial_port::get_option<asio::serial_port::character_size, unsigned int>,
             R"pbdoc(
                Gets char size

                :rtype: int
                )pbdoc")
        .def("get_flow_control",
             &pasio::async::serial_port::get_option<asio::serial_port::flow_control,
                                                    asio::serial_port::flow_control::type>,
             R"pbdoc(
                Gets baud rate

                :rtype: FlowControl
                )pbdoc")
        .def("get_stop_bits",
             &pasio::async::serial_port::get_option<asio::serial_port::stop_bits, asio::serial_port::stop_bits::type>,
             R"pbdoc(
               Gets baud rate

               :rtype: StopBits
               )pbdoc")
        .def("get_parity",
             &pasio::async::serial_port::get_option<asio::serial_port::parity, asio::serial_port::parity::type>,
             R"pbdoc(
               Gets baud rate

               :rtype: Parity
               )pbdoc");
}
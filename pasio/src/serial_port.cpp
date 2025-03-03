#include <asio/read.hpp>
#include <asio/serial_port.hpp>
#include <asio/write.hpp>
#include <pasio/serial_port.hpp>
#include <pybind11/cast.h>
#include <pybind11/gil.h>
#include <pybind11/pytypes.h>
#include <pybind11/stl_bind.h>
#include <string>

namespace pasio::async {

    serial_port::serial_port(const std::string& port, unsigned int baud_rate, private_constructor_t /*unused*/)
        : m_serial(m_io, port) {
        set_option<asio::serial_port::baud_rate>(baud_rate);
    }

    auto serial_port::make(const std::string& port, unsigned int baud_rate) -> std::shared_ptr<serial_port> {
        return std::make_shared<serial_port>(port, baud_rate, private_constructor);
    }

    auto serial_port::async_read(py::object executor, std::size_t bytes) -> py::object {
        py::object loop = py::module_::import("asyncio").attr("get_event_loop")();
        return loop.attr("run_in_executor")(std::move(executor),
                                            py::cpp_function([self = shared_from_this()](std::size_t bytes_read) {
                                                return self->read_impl(bytes_read);
                                            }),
                                            bytes);
    }

    auto serial_port::async_write(py::object executor, std::string data) -> py::object {
        py::object loop = py::module_::import("asyncio").attr("get_event_loop")();
        return loop.attr("run_in_executor")(std::move(executor),
                                            py::cpp_function([self = shared_from_this()](std::string data_write) {
                                                self->write_impl(std::move(data_write));
                                            }),
                                            std::move(data));
    }

    auto serial_port::async_read(std::size_t bytes) -> py::object {
        py::object loop = py::module_::import("asyncio").attr("get_event_loop")();
        return loop.attr("run_in_executor")(py::none(),
                                            py::cpp_function([self = shared_from_this()](std::size_t bytes_read) {
                                                return self->read_impl(bytes_read);
                                            }),
                                            bytes);
    }

    auto serial_port::async_write(std::string data) -> py::object {
        py::object loop = py::module_::import("asyncio").attr("get_event_loop")();
        return loop.attr("run_in_executor")(py::none(),
                                            py::cpp_function([self = shared_from_this()](std::string data_write) {
                                                self->write_impl(std::move(data_write));
                                            }),
                                            std::move(data));
    }

    auto serial_port::read(std::size_t bytes) -> std::string { return read_impl(bytes); }

    auto serial_port::write(std::string data) -> void { write_impl(std::move(data)); }

    auto serial_port::read_impl(std::size_t bytes) -> std::string {
        std::vector<char> buffer(bytes);
        asio::read(m_serial, asio::buffer(buffer, bytes));
        return {buffer.begin(), buffer.end()};
    }

    void serial_port::write_impl(std::string data) { asio::write(m_serial, asio::buffer(data)); }

    auto serial_port::is_open() const -> bool { return m_serial.is_open(); }

    void serial_port::close() {
        if (m_serial.is_open()) {
            m_serial.close();
        }
    }

    serial_port::~serial_port() { close(); }

    template<> void serial_port::set_option<asio::serial_port::baud_rate>(unsigned int value) {
        m_serial.set_option(asio::serial_port::baud_rate{value});
    }

    template<> void serial_port::set_option<asio::serial_port::character_size>(unsigned int value) {
        m_serial.set_option(asio::serial_port::character_size{value});
    }

    template<>
    void serial_port::set_option<asio::serial_port::flow_control>(asio::serial_port::flow_control::type value) {
        m_serial.set_option(asio::serial_port::flow_control{value});
    }

    template<> void serial_port::set_option<asio::serial_port::stop_bits>(asio::serial_port::stop_bits::type bits) {
        m_serial.set_option(asio::serial_port::stop_bits{bits});
    }

    template<> void serial_port::set_option<asio::serial_port::parity>(asio::serial_port::parity::type parity) {
        m_serial.set_option(asio::serial_port::parity{parity});
    }

    template<> [[nodiscard]] auto serial_port::get_option<asio::serial_port::baud_rate>() const -> unsigned int {
        asio::serial_port::baud_rate result;
        m_serial.get_option(result);
        return result.value();
    }

    template<> auto serial_port::get_option<asio::serial_port::character_size>() const -> unsigned int {
        asio::serial_port::character_size result;
        m_serial.get_option(result);
        return result.value();
    }

    template<>
    auto serial_port::get_option<asio::serial_port::flow_control>() const -> asio::serial_port::flow_control::type {
        asio::serial_port::flow_control result;
        m_serial.get_option(result);
        return result.value();
    }

    template<>
    auto serial_port::get_option<asio::serial_port::stop_bits>() const -> asio::serial_port::stop_bits::type {
        asio::serial_port::stop_bits result;
        m_serial.get_option(result);
        return result.value();
    }

    template<> auto serial_port::get_option<asio::serial_port::parity>() const -> asio::serial_port::parity::type {
        asio::serial_port::parity result;
        m_serial.get_option(result);
        return result.value();
    }
} // namespace pasio::async

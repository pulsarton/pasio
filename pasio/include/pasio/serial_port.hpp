#ifndef PASIO_ASYNC_serial_port_HPP
#define PASIO_ASYNC_serial_port_HPP

#include <asio/io_context.hpp>
#include <asio/serial_port.hpp>
#include <cstddef>
#include <memory>
#include <pasio/export/export.hpp>
#include <pybind11/pytypes.h>
#include <string>

namespace pasio::async {

    namespace py = pybind11;

    class PASIO_EXPORT serial_port : public std::enable_shared_from_this<serial_port> {
        static constexpr struct private_constructor_t {
        } private_constructor{};

    public:
        serial_port(const std::string& port, unsigned int baud_rate, private_constructor_t);

        static auto make(const std::string& port, unsigned int baud_rate) -> std::shared_ptr<serial_port>;

        auto read(std::size_t bytes) -> std::string;
        auto write(std::string data) -> void;

        auto async_read(py::object executor, std::size_t bytes) -> py::object;
        auto async_write(py::object executor, std::string data) -> py::object;

        auto async_read(std::size_t bytes) -> py::object;
        auto async_write(std::string data) -> py::object;

        auto is_open() const -> bool;

        void close();

        template<typename Option, typename Value> void set_option(Value);

        template<typename Option, typename R> auto get_option() const -> R;

        ~serial_port();

    private:
        asio::io_context m_io;
        asio::serial_port m_serial;

        auto read_impl(std::size_t bytes) -> std::string;
        void write_impl(std::string data);
    };

    template<> void serial_port::set_option<asio::serial_port::baud_rate>(unsigned int value);

    template<> void serial_port::set_option<asio::serial_port::character_size>(unsigned int value);

    template<>
    void serial_port::set_option<asio::serial_port::flow_control>(asio::serial_port::flow_control::type value);

    template<> void serial_port::set_option<asio::serial_port::stop_bits>(asio::serial_port::stop_bits::type bits);

    template<> void serial_port::set_option<asio::serial_port::parity>(asio::serial_port::parity::type parity);

    template<> auto serial_port::get_option<asio::serial_port::baud_rate>() const -> unsigned int;

    template<> auto serial_port::get_option<asio::serial_port::character_size>() const -> unsigned int;

    template<>
    auto serial_port::get_option<asio::serial_port::flow_control>() const -> asio::serial_port::flow_control::type;

    template<> auto serial_port::get_option<asio::serial_port::stop_bits>() const -> asio::serial_port::stop_bits::type;

    template<> auto serial_port::get_option<asio::serial_port::parity>() const -> asio::serial_port::parity::type;
} // namespace pasio::async

#endif // PASIO_ASYNC_serial_port_HPP
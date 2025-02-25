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

        template<typename Option, typename Value> void set_option(Value value);

        template<typename Option> auto get_option() const;

        ~serial_port();

    private:
        asio::io_context m_io;
        asio::serial_port m_serial;

        auto read_impl(std::size_t bytes) -> std::string;
        void write_impl(std::string data);
    };

    template<> void serial_port::set_option<asio::serial_port::baud_rate>(unsigned int value);

    template<> [[nodiscard]] auto serial_port::get_option<asio::serial_port::baud_rate>() const;

    /*

        not implemented

        template<> void serial_port::set_option<asio::serial_port::flow_control>(int value);
        template<> void serial_port::set_option<asio::serial_port::character_size>(int value);
        template<> void serial_port::set_option<asio::serial_port::stop_bits>(int value);
        template<> void serial_port::set_option<asio::serial_port::parity>(int value);
    */
} // namespace pasio::async

#endif // PASIO_ASYNC_serial_port_HPP
#ifndef PASIO_ASYNC_ASYNC_SERIAL_PORT_HPP
#define PASIO_ASYNC_ASYNC_SERIAL_PORT_HPP

#include <asio/io_context.hpp>
#include <asio/serial_port.hpp>
#include <pybind11/pytypes.h>
#include <string>
#include <thread>

namespace pasio::async {

    namespace py = pybind11;

    class async_serial_port {
    public:
        async_serial_port(const async_serial_port&) = delete;
        async_serial_port(async_serial_port&&) = delete;
        auto operator=(const async_serial_port&) -> async_serial_port& = delete;
        auto operator=(async_serial_port&&) -> async_serial_port& = delete;

        async_serial_port(const std::string& port, unsigned int baud_rate);

        // Асинхронная запись
        auto write(std::string data) -> py::object;

        // Асинхронное чтение
        auto read(size_t size) -> py::object;

        void close();

        ~async_serial_port();

    private:
        asio::io_context m_io_context;
        asio::serial_port m_serial;
        std::jthread m_thread{[&] {
            auto worker = asio::make_work_guard(m_io_context);
            m_io_context.run();
        }};
    };
} // namespace pasio::async

#endif // PASIO_ASYNC_ASYNC_SERIAL_PORT_HPP
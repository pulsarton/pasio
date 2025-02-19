#ifndef PASIO_ASYNC_ASYNC_SERIAL_PORT_HPP
#define PASIO_ASYNC_ASYNC_SERIAL_PORT_HPP

#include "pasio/export/export.hpp"
#include <asio/error_code.hpp>
#include <asio/io_context.hpp>
#include <asio/serial_port.hpp>
#include <cstddef>
#include <functional>
#include <memory>
#include <pybind11/pytypes.h>
#include <string>
#include <thread>
#include <vector>

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

    class PASIO_EXPORT callback_port : public std::enable_shared_from_this<callback_port> {
        static constexpr struct private_constructor_t {
        } private_constructor;

    public:
        callback_port(const std::string& port, unsigned int baud_rate, private_constructor_t);

        static std::shared_ptr<callback_port> make_callback_port(const std::string& port, unsigned int baud_rate) {
            return std::make_shared<callback_port>(port, baud_rate, private_constructor_t{});
        }

        void async_read(std::size_t bytes, std::function<void(std::string)>);
        void async_write(std::string data, std::function<void(int)>);

        ~callback_port();

    private:
        asio::io_context m_io_context;
        asio::serial_port m_serial;
        std::jthread m_thread{[&] {
            auto worker = asio::make_work_guard(m_io_context);
            m_io_context.run();
        }};

        std::vector<char> m_buffer;
    };
} // namespace pasio::async

#endif // PASIO_ASYNC_ASYNC_SERIAL_PORT_HPP
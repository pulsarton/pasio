#ifndef pasio_HPP
#define pasio_HPP

#include <asio.hpp>
#include <pasio/export/export.hpp>

namespace pasio::sync {
    class PASIO_EXPORT serial_port_wrapper {
    public:
        serial_port_wrapper(const serial_port_wrapper&) = delete;
        serial_port_wrapper(serial_port_wrapper&&) = delete;
        auto operator=(const serial_port_wrapper&) -> serial_port_wrapper& = delete;
        auto operator=(serial_port_wrapper&&) -> serial_port_wrapper& = delete;
        serial_port_wrapper(const std::string& port, unsigned int baud_rate);

        void write(std::string data);

        auto read(size_t size) -> std::string;

        ~serial_port_wrapper();

    private:
        asio::io_service m_io;
        asio::serial_port m_serial;
    };
} // namespace pasio::sync

#endif
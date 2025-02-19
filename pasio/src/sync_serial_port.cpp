#include <asio/buffer.hpp>
#include <asio/error_code.hpp>
#include <asio/serial_port.hpp>
#include <cstddef>
#include <exception>
#include <iostream>
#include <pasio/sync/sync_serial_port.hpp>
#include <string>
#include <tuple>
#include <vector>

namespace pasio::sync {
    serial_port_wrapper::serial_port_wrapper(const std::string& port, unsigned int baud_rate) : m_serial(m_io) {
        try {
            m_serial.open(port);
            m_serial.set_option(asio::serial_port::baud_rate(baud_rate));
        } catch (const std::exception& e) {
            std::cout << e.what();
        }
    }

    void serial_port_wrapper::write(std::string data) { asio::write(m_serial, asio::buffer(data)); }

    auto serial_port_wrapper::read(size_t size) -> std::string {
        std::vector<char> buf(size);
        asio::read(m_serial, asio::buffer(buf, size));
        return {buf.begin(), buf.end()};
    }

    serial_port_wrapper::~serial_port_wrapper() {
        asio::error_code error;
        std::ignore = m_serial.close(error);
    }
} // namespace pasio::sync

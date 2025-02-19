#include <iostream>
#include <pasio/sync/sync_serial_port.hpp>
#include <pybind11/pybind11.h>

namespace pasio::sync {
    serial_port_wrapper::serial_port_wrapper(const std::string& port, unsigned int baud_rate) : m_serial(m_io) {
        try {
            m_serial.open(port);
            m_serial.set_option(asio::serial_port_base::baud_rate(baud_rate));
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

    serial_port_wrapper::~serial_port_wrapper() { m_serial.close(); }
} // namespace pasio::sync

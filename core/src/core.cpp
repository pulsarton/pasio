#include <codecvt>
#include <iostream>
#include <locale>
#include <pasio/core/core.hpp>

namespace py = pybind11;

class SerialPortWrapper {
public:
    SerialPortWrapper(const std::string& port, unsigned int baud_rate) : io(), serial(io) {
        try {
            serial.open(std::move(port));
            serial.set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
        } catch (const std::exception& e) {
            std::cout << e.what();
        }
    }

    void write(std::string data) { boost::asio::write(serial, boost::asio::buffer(data)); }

    std::string read(size_t size) {
        std::vector<char> buf(size);
        boost::asio::read(serial, boost::asio::buffer(buf, size));
        return std::string(buf.begin(), buf.end());
    }

    ~SerialPortWrapper() { serial.close(); }

private:
    boost::asio::io_service io;
    boost::asio::serial_port serial;
};

PYBIND11_MODULE(core, m) {
    py::class_<SerialPortWrapper>(m, "SerialPort")
        .def(py::init<const std::string&, unsigned int>())
        .def("write", &SerialPortWrapper::write)
        .def("read", &SerialPortWrapper::read);
}

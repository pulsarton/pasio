
#include <asio.hpp>
#include <asio/consign.hpp>
#include <asio/executor_work_guard.hpp>
#include <asio/steady_timer.hpp>
#include <asio/use_future.hpp>
#include <asio/write.hpp>
#include <future>
#include <memory>
#include <pasio/async/async_serial_port.hpp>
#include <pybind11/cast.h>
#include <pybind11/pybind11.h>

namespace pasio::async {

    /*
from pasio import AsyncSerialPort
import asyncio

async def main():
    port = AsyncSerialPort("/dev/pts/4", 9600)
    await port.write("hello world")
    port.close()

asyncio.run(main())
    */

    namespace {
        template<typename T> auto to_asyncio_future(std::shared_ptr<std::future<T>> future) -> py::object {
            py::gil_scoped_acquire gil;
            py::module_ asyncio = py::module_::import("asyncio");
            py::object loop = asyncio.attr("get_running_loop")();
            py::object py_future = loop.attr("create_future")();

            std::thread([future = std::move(future), py_future]() {
                try {
                    T result = future->get(); 
                    py_future.attr("set_result")(result);
                } catch (const std::exception& e) {
                    py::gil_scoped_acquire gil;
                    py_future.attr("set_exception")(py::value_error(e.what()));
                }
            }).detach();

            return py_future;
        }
    } // namespace

    async_serial_port::async_serial_port(const std::string& port, unsigned int baud_rate)
        : m_serial(m_io_context, port) {
        m_serial.set_option(asio::serial_port_base::baud_rate(baud_rate));
    }
    auto pasio::async::async_serial_port::write(std::string data) -> py::object {
        auto promise = std::make_shared<std::promise<int>>();
        auto future = std::make_shared<std::future<int>>(promise->get_future());

        async_write(m_serial, asio::buffer(data), [promise](const asio::error_code& error, std::size_t written) {
            if (!error) {
                promise->set_value(static_cast<int>(written));
            } else {
                promise->set_exception(std::make_exception_ptr(std::runtime_error("Write error: " + error.message())));
            }
        });

        return to_asyncio_future(future);
    }

    auto pasio::async::async_serial_port::read(size_t size) -> py::object {
        auto promise = std::make_shared<std::promise<std::string>>();
        auto future = std::make_shared<std::future<std::string>>(promise->get_future());
        auto buffer = std::make_shared<std::vector<char>>(size);

        async_read(m_serial, asio::mutable_buffer(buffer->data(), size),
                   asio::consign(
                       [promise, buffer](const asio::error_code& error, std::size_t length) {
                           if (!error) {
                               promise->set_value(std::string(buffer->data(), length));
                           } else {
                               promise->set_exception(
                                   std::make_exception_ptr(std::runtime_error("Read error: " + error.message())));
                           }
                       },
                       buffer));

        return to_asyncio_future(future);
    }

    void async_serial_port::close() { m_serial.close(); }

    async_serial_port::~async_serial_port() {
        m_io_context.stop();
        close();
    }
} // namespace pasio::async

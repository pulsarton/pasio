#include <asio/bind_executor.hpp>
#include <asio/consign.hpp>
#include <asio/dispatch.hpp>
#include <asio/executor_work_guard.hpp>
#include <asio/read.hpp>
#include <asio/read_at.hpp>
#include <asio/read_until.hpp>
#include <asio/serial_port_base.hpp>
#include <asio/steady_timer.hpp>
#include <asio/use_future.hpp>
#include <asio/write.hpp>
#include <future>
#include <memory>
#include <pasio/async/async_serial_port.hpp>
#include <pybind11/cast.h>
#include <pybind11/gil.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>
#include <string>
#include <utility>

namespace pasio::async {

    /*
from pasio import AsyncSerialPort
import asyncio

port = AsyncSerialPort("/dev/pts/4", 9600)

async def main():
    await port.write("hello world")
    port.close()

asyncio.run(main())
    */

    namespace {
        template<typename T> auto to_asyncio_future(auto& executor, std::future<T> future) -> py::object {
            const py::module_ asyncio = py::module_::import("asyncio");
            const py::object loop = asyncio.attr("get_running_loop")();
            py::object py_future = loop.attr("create_future")();
            // py_future.attr("set_result")(future.get());

            asio::dispatch(executor, [future = std::move(future), py_future]() mutable {
                try {
                    T result = std::move(future).get();
                    const py::gil_scoped_acquire gil;
                    py_future.attr("set_result")(std::move(result)); // py::none() for void
                } catch (const std::exception& e) {
                    const py::gil_scoped_acquire gil;
                    py_future.attr("set_exception")(py::value_error(e.what()));
                }
            });

            return py_future;
        }
    } // namespace

    async_serial_port::async_serial_port(const std::string& port, unsigned int baud_rate)
        : m_serial(m_io_context, port) {
        m_serial.set_option(asio::serial_port::baud_rate(baud_rate));
    }
    auto pasio::async::async_serial_port::write(std::string data) -> py::object {
        auto promise = std::promise<int>();
        auto future = std::future<int>(promise.get_future());
        auto buffer = std::make_shared<std::string>(std::move(data));

        asio::async_write(
            m_serial, asio::buffer(*buffer),
            asio::consign(
                [promise = std::move(promise)](const asio::error_code& error, std::size_t written) mutable {
                    if (!error) {
                        promise.set_value(static_cast<int>(written));
                    } else {
                        promise.set_exception(
                            std::make_exception_ptr(std::runtime_error("Write error: " + error.message())));
                    }
                },
                buffer));

        return to_asyncio_future(m_serial.get_executor(), std::move(future));
    }

    auto pasio::async::async_serial_port::read(std::size_t size) -> py::object {
        auto promise = std::promise<std::string>();
        auto future = std::future<std::string>(promise.get_future());
        auto buffer = std::make_shared<std::vector<char>>(size);

        asio::async_read(
            m_serial, asio::mutable_buffer(buffer->data(), size),
            [promise = std::move(promise), buffer](const asio::error_code& error, std::size_t length) mutable {
                if (!error) {
                    promise.set_value(std::string(buffer->data(), length));
                } else {
                    promise.set_exception(
                        std::make_exception_ptr(std::runtime_error("Read error: " + error.message())));
                }
            });

        return to_asyncio_future(m_serial.get_executor(), std::move(future));
    }

    void async_serial_port::close() { m_serial.close(); }

    async_serial_port::~async_serial_port() {
        m_io_context.stop();
        close();
    }

    callback_port::callback_port(const std::string& port, unsigned int baud_rate, private_constructor_t)
        : m_serial(m_io_context, port) {
        m_serial.set_option(asio::serial_port::baud_rate(baud_rate));
    }

    void callback_port::async_read(std::size_t bytes, std::function<void(std::string)> callback) {
        m_buffer.resize(bytes);
        asio::async_read(m_serial, asio::buffer(m_buffer),
                         [self = shared_from_this(), callback = std::move(callback)](auto, auto read) {
                             callback(std::string{self->m_buffer.begin(), self->m_buffer.begin() + read});
                         });
    }

    void callback_port::async_write(std::string data, std::function<void(int)> callback) {
        asio::async_write(m_serial, asio::buffer(data),
                          [self = shared_from_this(), callback = std::move(callback)](auto, auto written) {
                              callback(static_cast<int>(written));
                          });
    }

    callback_port::~callback_port() {
        m_serial.close();
        m_io_context.stop();
    }
} // namespace pasio::async
